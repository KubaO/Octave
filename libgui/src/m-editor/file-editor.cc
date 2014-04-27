/*

Copyright (C) 2011-2013 Jacob Dawid

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.

*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_QSCINTILLA

#include "file-editor.h"
#include "resource-manager.h"
#include "shortcut-manager.h"

#include <QVBoxLayout>
#include <QApplication>
#include <QFile>
#include <QFont>
#include <QFileDialog>
#include <QMessageBox>
#include <QStyle>
#include <QTextStream>
#include <QTabBar>
#include <QProcess>
#include <QInputDialog>
#include <Qsci/qscicommandset.h>

#include "octave-link.h"
#include "utils.h"
#include "main-window.h"

file_editor::file_editor (QWidget *p)
  : file_editor_interface (p)
{
  // Set current editing directory before construct because loaded
  // files will change ced accordingly.
  ced = QDir::currentPath ();

  construct ();

  setVisible (false);

  setAcceptDrops(true);
}

file_editor::~file_editor (void)
{
  QSettings *settings = resource_manager::get_settings ();

  // Have all file editor tabs signal what their file names are.
  editor_tab_map.clear ();
  emit fetab_file_name_query (0);

  // save file names (even if last session will not be restored next time)
  QStringList fetFileNames;
  for (editor_tab_map_const_iterator p = editor_tab_map.begin ();
       p != editor_tab_map.end (); p++)
    {
      QString file_name = p->first;
      if (!file_name.isEmpty () && file_name.at (file_name.size () - 1) != '/')
        fetFileNames.append (p->first);  // do not append unnamed files
    }

  settings->setValue ("editor/savedSessionTabs", fetFileNames);
  settings->sync ();

  for (int index = _tab_widget->count ()-1; index >= 0; index--)
    {
      // true: app closing
      emit fetab_close_request (_tab_widget->widget (index), true);
    }

  if (_mru_file_menu)
    delete _mru_file_menu;
}

void
file_editor::focus (void)
{
  set_focus ();
}

// set focus to editor and its current tab
void
file_editor::set_focus (void)
{
  if (!isVisible ())
    setVisible (true);
  setFocus ();
  activateWindow ();
  raise ();
  QWidget *fileEditorTab = _tab_widget->currentWidget ();
  if (fileEditorTab)
    emit fetab_set_focus (fileEditorTab);
}

QMenu *
file_editor::debug_menu (void)
{
  return _debug_menu;
}

QToolBar *
file_editor::toolbar (void)
{
  return _tool_bar;
}

void
file_editor::handle_enter_debug_mode (void)
{
  _run_action->setEnabled (false);
}

void
file_editor::handle_exit_debug_mode (void)
{
  _run_action->setEnabled (true);
}

void
file_editor::request_new_file (const QString& commands)
{
  // Custom editor? If yes, we can only call the editor without passing
  // some initial contents and even without being sure a new file is opened
  if (call_custom_editor ())
    return;

  // New file isn't a file_editor_tab function since the file
  // editor tab has yet to be created and there is no object to
  // pass a signal to.  Hence, functionality is here.

  file_editor_tab *fileEditorTab = new file_editor_tab (ced);
  if (fileEditorTab)
    {
      add_file_editor_tab (fileEditorTab, "");  // new tab with empty title
      fileEditorTab->new_file (commands);       // title is updated here
      set_focus ();                             // focus editor and new tab
    }
}

void
file_editor::request_new_script (const QString& commands)
{
  request_new_file (commands);
}

void
file_editor::request_new_function (bool)
{
  bool ok;
  // get the name of the new function
  QString new_name  = QInputDialog::getText (this, tr ("New Function"),
                      tr ("New function name:\n"), QLineEdit::Normal, "", &ok);
  if (ok && new_name.length () > 0)
    {
      // append suffix if it not already exists
      if (new_name.rightRef (2) != ".m")
        new_name.append (".m");
      // check whether new files are created without prompt
      QSettings *settings = resource_manager::get_settings ();
      if (! settings->value ("editor/create_new_file",false).toBool ())
        {
          // no, so enable this settings and wait for end of new file loading
          settings->setValue ("editor/create_new_file",true);
          connect (this, SIGNAL (file_loaded_signal ()),
                   this, SLOT (restore_create_file_setting ()));
        }
      // start the edit command
      emit execute_command_in_terminal_signal ("edit " + new_name);
    }
}

void
file_editor::restore_create_file_setting ()
{
  // restore the new files creation setting
  QSettings *settings = resource_manager::get_settings ();
  settings->setValue ("editor/create_new_file",false);
  disconnect (this, SIGNAL (file_loaded_signal ()),
              this, SLOT (restore_create_file_setting ()));
}

void
file_editor::request_open_file (void)
{
  // Open file isn't a file_editor_tab function since the file
  // editor tab has yet to be created and there is no object to
  // pass a signal to.  Hence, functionality is here.

  // Create a NonModal message.
  QFileDialog *fileDialog = new QFileDialog (this);
  fileDialog->setNameFilter (tr ("Octave Files (*.m);;All Files (*)"));

  // Giving trouble under KDE (problem is related to Qt signal handling on unix,
  // see https://bugs.kde.org/show_bug.cgi?id=260719 ,
  // it had/has no effect on Windows, though)
  fileDialog->setOption(QFileDialog::DontUseNativeDialog, true);

  fileDialog->setAcceptMode (QFileDialog::AcceptOpen);
  fileDialog->setViewMode (QFileDialog::Detail);
  fileDialog->setFileMode (QFileDialog::ExistingFiles);
  fileDialog->setDirectory (ced);

  connect (fileDialog, SIGNAL (filesSelected (const QStringList&)),
           this, SLOT (request_open_files (const QStringList&)));

  fileDialog->setWindowModality (Qt::NonModal);
  fileDialog->setAttribute (Qt::WA_DeleteOnClose);
  fileDialog->show ();
}

// Check whether this file is already open in the editor.
QWidget *
file_editor::find_tab_widget (const QString& file) const
{
  QWidget *retval = 0;

  for (editor_tab_map_const_iterator p = editor_tab_map.begin ();
       p != editor_tab_map.end (); p++)
    {
      QString tab_file = p->first;

      if (same_file (file.toStdString (), tab_file.toStdString ()))
        {
          retval = p->second;
          break;
        }
    }

  return retval;
}

bool
file_editor::call_custom_editor (const QString& file_name, int line)
{
  // Check if the user wants to use a custom file editor.
  QSettings *settings = resource_manager::get_settings ();

  if (settings->value ("useCustomFileEditor").toBool ())
    {
      QString editor = settings->value ("customFileEditor").toString ();
      editor.replace ("%f", file_name);
      editor.replace ("%l", QString::number (line));

      QProcess::startDetached (editor);

      if (line < 0 && ! file_name.isEmpty ())
        handle_mru_add_file (QFileInfo (file_name).canonicalFilePath ());

      return true;
    }

  return false;
}

bool
file_editor::is_editor_console_tabbed ()
{
  main_window *w = static_cast<main_window *>(main_win ());
  QList<QDockWidget *> w_list = w->tabifiedDockWidgets (this);
  QDockWidget *console =
    static_cast<QDockWidget *> (w->get_dock_widget_list ().at (0));

  for (int i = 0; i < w_list.count (); i++)
    {
      if (w_list.at (i) == console)
        return true;
    }

  return false;
}

void
file_editor::request_open_files (const QStringList& open_file_names)
{
  for (int i = 0; i < open_file_names.count (); i++)
    request_open_file (open_file_names.at (i));
}

void
file_editor::request_open_file (const QString& openFileName, int line,
                                bool debug_pointer,
                                bool breakpoint_marker, bool insert)
{
  if (call_custom_editor (openFileName, line))
    return;   // custom editor called

  if (openFileName.isEmpty ())
    {
      // This happens if edit is calles without an argument
      // Open eitor with empty edit area instead (as new file would do)
      request_new_file ("");
    }
  else
    {
      // Have all file editor tabs signal what their file names are.
      editor_tab_map.clear ();
      emit fetab_file_name_query (0);

      // Check whether this file is already open in the editor.
      QWidget *tab = find_tab_widget (openFileName);

      if (tab)
        {
          _tab_widget->setCurrentWidget (tab);

          if (line > 0)
            {
              emit fetab_goto_line (tab, line);

              if (debug_pointer)
                emit fetab_insert_debugger_pointer (tab, line);

              if (breakpoint_marker)
                emit fetab_do_breakpoint_marker (insert, tab, line);
            }

          if (! ((breakpoint_marker || debug_pointer) && is_editor_console_tabbed ()))
            {
              emit fetab_set_focus (tab);
              set_focus ();
            }
        }
      else
        {
          file_editor_tab *fileEditorTab = new file_editor_tab ();
          if (fileEditorTab)
            {
              QString result = fileEditorTab->load_file (openFileName);
              if (result == "")
                {
                  // Supply empty title then have the file_editor_tab update
                  // with full or short name.
                  add_file_editor_tab (fileEditorTab, "");
                  fileEditorTab->update_window_title (false);
                  // file already loaded, add file to mru list here
                  QFileInfo file_info = QFileInfo (openFileName);
                  handle_mru_add_file (file_info.canonicalFilePath ());

                  if (line > 0)
                    {
                      emit fetab_goto_line (fileEditorTab, line);

                      if (debug_pointer)
                        emit fetab_insert_debugger_pointer (fileEditorTab,
                                                            line);
                      if (breakpoint_marker)
                        emit fetab_do_breakpoint_marker (insert, fileEditorTab,
                                                         line);
                    }
                }
              else
                {
                  delete fileEditorTab;

                  if (QFile::exists (openFileName))
                    {
                      // File not readable:
                      // create a NonModal message about error.
                      QMessageBox *msgBox
                        = new QMessageBox (QMessageBox::Critical,
                                           tr ("Octave Editor"),
                                           tr ("Could not open file\n%1\nfor read: %2.").
                                           arg (openFileName).arg (result),
                                           QMessageBox::Ok, this);

                      msgBox->setWindowModality (Qt::NonModal);
                      msgBox->setAttribute (Qt::WA_DeleteOnClose);
                      msgBox->show ();
                    }
                  else
                    {
                      // File does not exist, should it be crated?
                      QMessageBox *msgBox;
                      int answer;
                      QSettings *settings = resource_manager::get_settings ();
                      if (settings->value ("editor/create_new_file", false).toBool ())
                        {
                          answer = QMessageBox::Yes;
                        }
                      else
                        {
                          msgBox = new QMessageBox (QMessageBox::Question,
                                                    tr ("Octave Editor"),
                                                    tr ("File\n%1\ndoes not exist. "
                                                        "Do you want to create it?").arg (openFileName),
                                                    QMessageBox::Yes
                                                    | QMessageBox::No, 0);

                          msgBox->setAttribute (Qt::WA_DeleteOnClose);
                          answer = msgBox->exec ();
                        }

                      if (answer == QMessageBox::Yes)
                        {
                          // create the file and call the editor again
                          QFile file (openFileName);
                          if (!file.open (QIODevice::WriteOnly))
                            {
                              // error opening the file
                              msgBox = new QMessageBox (QMessageBox::Critical,
                                                        tr ("Octave Editor"),
                                                        tr ("Could not open file\n%1\nfor write: %2.").
                                                        arg (openFileName).arg (file.errorString ()),
                                                        QMessageBox::Ok, this);

                              msgBox->setWindowModality (Qt::NonModal);
                              msgBox->setAttribute (Qt::WA_DeleteOnClose);
                              msgBox->show ();
                            }
                          else
                            {
                              file.close ();
                              request_open_file (openFileName);
                            }
                        }
                    }
                }
            }

          if (! ((breakpoint_marker || debug_pointer) && is_editor_console_tabbed ()))
            {
              // really show editor and the current editor tab
              set_focus ();
              emit file_loaded_signal ();
            }
        }
    }
}

// open a file from the mru list
void
file_editor::request_mru_open_file (QAction *action)
{
  if (action)
    {
      request_open_file (action->data ().toString ());
    }
}


void
file_editor::check_conflict_save (const QString& saveFileName,
                                  bool remove_on_success)
{
  // Have all file editor tabs signal what their file names are.
  editor_tab_map.clear ();
  emit fetab_file_name_query (0);

  // Check whether this file is already open in the editor.
  QWidget *tab = find_tab_widget (saveFileName);

  if (tab)
    {
      // Note: to overwrite the contents of some other file editor tab
      // with the same name requires identifying which file editor tab
      // that is (not too difficult) then close that tab.  Of course,
      // that could trigger another dialog box if the file editor tab
      // with the same name has modifications in it.  This could become
      // somewhat confusing to the user.  For now, opt to do nothing.

      // Create a NonModal message about error.
      QMessageBox *msgBox
        = new QMessageBox (QMessageBox::Critical, tr ("Octave Editor"),
                           tr ("File not saved! A file with the selected name\n%1\n"
                               "is already open in the editor").
                           arg (saveFileName),
                           QMessageBox::Ok, 0);

      msgBox->setWindowModality (Qt::NonModal);
      msgBox->setAttribute (Qt::WA_DeleteOnClose);
      msgBox->show ();

      return;
    }

  QObject *saveFileObject = sender ();
  QWidget *saveFileWidget = 0;

  for (int i = 0; i < _tab_widget->count (); i++)
    {
      if (_tab_widget->widget (i) == saveFileObject)
        {
          saveFileWidget = _tab_widget->widget (i);
          break;
        }
    }
  if (!saveFileWidget)
    {
      // Create a NonModal message about error.
      QMessageBox *msgBox
        = new QMessageBox (QMessageBox::Critical, tr ("Octave Editor"),
                           tr ("The associated file editor tab has disappeared."),
                           QMessageBox::Ok, 0);

      msgBox->setWindowModality (Qt::NonModal);
      msgBox->setAttribute (Qt::WA_DeleteOnClose);
      msgBox->show ();

      return;
    }

  // Can save without conflict, have the file editor tab do so.
  emit fetab_save_file (saveFileWidget, saveFileName, remove_on_success);
}

void
file_editor::handle_insert_debugger_pointer_request (const QString& file,
                                                     int line)
{
  request_open_file (file, line, true);
}

void
file_editor::handle_delete_debugger_pointer_request (const QString& file,
                                                     int line)
{
  if (! file.isEmpty ())
    {
      // Have all file editor tabs signal what their file names are.
      editor_tab_map.clear ();
      emit fetab_file_name_query (0);

      // Check whether this file is already open in the editor.
      QWidget *tab = find_tab_widget (file);

      if (tab)
        {
          _tab_widget->setCurrentWidget (tab);

          if (line > 0)
            emit fetab_delete_debugger_pointer (tab, line);

          emit fetab_set_focus (tab);
        }
    }
}

void
file_editor::handle_update_breakpoint_marker_request (bool insert,
                                                      const QString& file,
                                                      int line)
{
  request_open_file (file, line, false, true, insert);
}

void
file_editor::handle_edit_file_request (const QString& file)
{
  request_open_file (file);
}

void
file_editor::request_undo (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_UNDO);
}

void
file_editor::request_redo (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_REDO);
}

void
file_editor::request_copy (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_COPY);
}

void
file_editor::request_cut (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_CUT);
}

void
file_editor::request_paste (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_PASTE);
}

void
file_editor::request_selectall (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_SELECTALL);
}

void
file_editor::request_context_help (bool)
{
  emit fetab_context_help (_tab_widget->currentWidget (), false);
}
void
file_editor::request_context_doc (bool)
{
  emit fetab_context_help (_tab_widget->currentWidget (), true);
}

void
file_editor::request_context_edit (bool)
{
  emit fetab_context_edit (_tab_widget->currentWidget ());
}

void
file_editor::request_save_file (bool)
{
  emit fetab_save_file (_tab_widget->currentWidget ());
}

void
file_editor::request_save_file_as (bool)
{
  emit fetab_save_file_as (_tab_widget->currentWidget ());
}

void
file_editor::request_print_file (bool)
{
  emit fetab_print_file (_tab_widget->currentWidget ());
}


void
file_editor::request_run_file (bool)
{
  emit fetab_run_file (_tab_widget->currentWidget ());
}

void
file_editor::request_context_run (bool)
{
  emit fetab_context_run (_tab_widget->currentWidget ());
}

void
file_editor::request_toggle_bookmark (bool)
{
  emit fetab_toggle_bookmark (_tab_widget->currentWidget ());
}

void
file_editor::request_next_bookmark (bool)
{
  emit fetab_next_bookmark (_tab_widget->currentWidget ());
}

void
file_editor::request_previous_bookmark (bool)
{
  emit fetab_previous_bookmark (_tab_widget->currentWidget ());
}

void
file_editor::request_remove_bookmark (bool)
{
  emit fetab_remove_bookmark (_tab_widget->currentWidget ());
}

void
file_editor::request_toggle_breakpoint (bool)
{
  emit fetab_toggle_breakpoint (_tab_widget->currentWidget ());
}

void
file_editor::request_next_breakpoint (bool)
{
  emit fetab_next_breakpoint (_tab_widget->currentWidget ());
}

void
file_editor::request_previous_breakpoint (bool)
{
  emit fetab_previous_breakpoint (_tab_widget->currentWidget ());
}

void
file_editor::request_remove_breakpoint (bool)
{
  emit fetab_remove_all_breakpoints (_tab_widget->currentWidget ());
}

// slots for Edit->Commands actions
void
file_editor::request_delete_start_word (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_DELWORDLEFT);
}
void
file_editor::request_delete_end_word (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_DELWORDRIGHT);
}
void
file_editor::request_delete_start_line (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_DELLINELEFT);
}
void
file_editor::request_delete_end_line (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_DELLINERIGHT);
}
void
file_editor::request_delete_line (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_LINEDELETE);
}
void
file_editor::request_copy_line (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_LINECOPY);
}
void
file_editor::request_cut_line (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_LINECUT);
}
void
file_editor::request_duplicate_selection (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_SELECTIONDUPLICATE);
}
void
file_editor::request_transpose_line (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_LINETRANSPOSE);
}
void
file_editor::request_comment_selected_text (bool)
{
  emit fetab_comment_selected_text (_tab_widget->currentWidget ());
}
void
file_editor::request_uncomment_selected_text (bool)
{
  emit fetab_uncomment_selected_text (_tab_widget->currentWidget ());
}

// slots for Edit->Format actions
void
file_editor::request_upper_case (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_UPPERCASE);
}
void
file_editor::request_lower_case (bool)
{
  emit fetab_scintilla_command (_tab_widget->currentWidget (),
                                QsciScintillaBase::SCI_LOWERCASE);
}
void
file_editor::request_indent_selected_text (bool)
{
  emit fetab_indent_selected_text (_tab_widget->currentWidget ());
}

void
file_editor::request_unindent_selected_text (bool)
{
  emit fetab_unindent_selected_text (_tab_widget->currentWidget ());
}


void
file_editor::request_find (bool)
{
  emit fetab_find (_tab_widget->currentWidget ());
}

void
file_editor::request_goto_line (bool)
{
  emit fetab_goto_line (_tab_widget->currentWidget ());
}


void
file_editor::request_completion (bool)
{
  emit fetab_completion (_tab_widget->currentWidget ());
}

void
file_editor::handle_mru_add_file (const QString& file_name)
{
  _mru_files.removeAll (file_name);
  _mru_files.prepend (file_name);
  mru_menu_update ();
}

void
file_editor::mru_menu_update (void)
{
  int num_files = qMin (_mru_files.size (), int (MaxMRUFiles));

  // configure and show active actions of mru-menu
  for (int i = 0; i < num_files; ++i)
    {
      QString text = tr ("&%1 %2").
                     arg ((i+1) % int (MaxMRUFiles)).arg (_mru_files.at (i));
      _mru_file_actions[i]->setText (text);
      _mru_file_actions[i]->setData (_mru_files.at (i));
      _mru_file_actions[i]->setVisible (true);
    }

  // hide unused mru-menu entries
  for (int j = num_files; j < MaxMRUFiles; ++j)
    _mru_file_actions[j]->setVisible (false);

  // delete entries in string-list beyond MaxMRUFiles
  while (_mru_files.size () > MaxMRUFiles)
    _mru_files.removeLast ();

  // save actual mru-list in settings
  QSettings *settings = resource_manager::get_settings ();

  // FIXME: what should happen if settings is 0?
  settings->setValue ("editor/mru_file_list", _mru_files);
  settings->sync ();
}

void
file_editor::handle_file_name_changed (const QString& fname,
                                       const QString& tip)
{
  QObject *fileEditorTab = sender ();
  if (fileEditorTab)
    {
      for (int i = 0; i < _tab_widget->count (); i++)
        {
          if (_tab_widget->widget (i) == fileEditorTab)
            {
              _tab_widget->setTabText (i, fname);
              _tab_widget->setTabToolTip (i, tip);
            }
        }
    }
}

void
file_editor::request_close_file (bool)
{
  emit fetab_close_request (_tab_widget->currentWidget ());
}

void
file_editor::request_close_all_files (bool)
{
  // loop over all tabs starting from last one otherwise deletion changes index
  for (int index = _tab_widget->count ()-1; index >= 0; index--)
    emit fetab_close_request (_tab_widget->widget (index));
}

void
file_editor::request_close_other_files (bool)
{
  QWidget *tabID = _tab_widget->currentWidget ();
  // loop over all tabs starting from last one otherwise deletion changes index
  for (int index = _tab_widget->count ()-1; index >= 0; index--)
    {
      if (tabID != _tab_widget->widget (index))
        emit fetab_close_request (_tab_widget->widget (index));
    }
}


void
file_editor::handle_tab_close_request (int index)
{
  // Signal to the tabs a request to close whomever matches the identifying
  // tag (i.e., unique widget pointer).  The reason for this indirection is
  // that it will enable a file editor widget to toss up a non-static
  // dialog box and later signal that it wants to be removed.
  QWidget *tabID = _tab_widget->widget (index);
  emit fetab_close_request (tabID);
}

void
file_editor::handle_tab_remove_request (void)
{
  QObject *fileEditorTab = sender ();
  if (fileEditorTab)
    {
      for (int i = 0; i < _tab_widget->count (); i++)
        {
          if (_tab_widget->widget (i) == fileEditorTab)
            {
              _tab_widget->removeTab (i);
              delete fileEditorTab;
              break;
            }
        }
    }
  check_actions ();
}

void
file_editor::handle_add_filename_to_list (const QString& fileName, QWidget *ID)
{
  // Should we allow multiple tabs for a single file?

  editor_tab_map[fileName] = ID;
}

// context menu of edit area
void
file_editor::active_tab_changed (int index)
{
  emit fetab_change_request (_tab_widget->widget (index));
}

void file_editor::create_context_menu (QMenu *menu)
{
  // remove all standard actions from scintilla
  QList<QAction *> all_actions = menu->actions ();
  QAction* a;

  foreach (a, all_actions)
    menu->removeAction (a);

  // add editor's actions with icons and customized shortcuts
  menu->addAction (_undo_action);
  menu->addAction (_redo_action);
  menu->addSeparator ();
  menu->addAction (_cut_action);
  menu->addAction (_copy_action);
  menu->addAction (_paste_action);
  menu->addSeparator ();
  menu->addAction (_selectall_action);
}

void
file_editor::zoom_in (bool)
{
  emit fetab_zoom_in (_tab_widget->currentWidget ());
}

void
file_editor::zoom_out (bool)
{
  emit fetab_zoom_out (_tab_widget->currentWidget ());
}

void
file_editor::zoom_normal (bool)
{
  emit fetab_zoom_normal (_tab_widget->currentWidget ());
}

void
file_editor::handle_editor_state_changed (bool copy_available,
                                          const QString& file_name)
{
  // In case there is some scenario where traffic could be coming from
  // all the file editor tabs, just process info from the current active tab.
  if (sender () == _tab_widget->currentWidget ())
    {
      _copy_action->setEnabled (copy_available);
      _cut_action->setEnabled (copy_available);
      _run_selection_action->setEnabled (copy_available);

      if (!file_name.isEmpty ())
        {
          ced = QDir::cleanPath (file_name);
          int lastslash = ced.lastIndexOf ('/');

          // Test against > 0 because if somehow the directory is "/" the
          // slash should be retained.  Otherwise, last slash is removed.
          if (lastslash > 0 && lastslash != ced.count ())
            ced = ced.left (lastslash);
        }

      setFocusProxy (_tab_widget->currentWidget ());
    }
}

void
file_editor::notice_settings (const QSettings *settings)
{
  int icon_size = settings->value ("toolbar_icon_size", 16).toInt ();
  _tool_bar->setIconSize (QSize (icon_size, icon_size));

  int tab_width_min = settings->value ("editor/notebook_tab_width_min", 160)
                                      .toInt ();
  int tab_width_max = settings->value ("editor/notebook_tab_width_max", 300)
                                      .toInt ();

  QString style_sheet;
  if (settings->value ("editor/longWindowTitle", false).toBool ())
    {
      style_sheet = QString ("QTabBar::tab {max-height: 4ex; "
                             "min-width: %1px; max-width: %2px;}")
                             .arg (tab_width_min).arg (tab_width_max);
      _tab_widget->setElideMode (Qt::ElideLeft);
    }
  else
    {
      style_sheet = QString ("QTabBar::tab {max-height: 4ex;}");
      _tab_widget->setElideMode (Qt::ElideNone);
    }

  _tab_widget->setUsesScrollButtons (true);
  _tab_widget->setStyleSheet (style_sheet);

  set_shortcuts ();

  // Relay signal to file editor tabs.
  emit fetab_settings_changed (settings);
}

void
file_editor::request_preferences (bool)
{
  emit request_settings_dialog ("editor");
}

void
file_editor::request_styles_preferences (bool)
{
  emit request_settings_dialog ("editor_styles");
}

void
file_editor::insert_new_open_actions (QAction *new_action,
                                      QAction *new_fcn_action,
                                      QAction *open_action)
{
  _fileMenu->insertAction (_mru_file_menu->menuAction (), open_action);
  _fileMenu->insertAction (open_action, new_fcn_action);
  _fileMenu->insertAction (new_fcn_action, new_action);
  _tool_bar->insertAction (_save_action, open_action);
  _tool_bar->insertAction (open_action, new_action);
}

QAction*
file_editor::add_action (QMenu *menu, const QIcon &icon, const QString &text,
                         const char *member)
{
  QAction *a = menu->addAction (icon, text, this, member);
  addAction (a);  // important for shortcut context
  a->setShortcutContext (Qt::WidgetWithChildrenShortcut);
  return a;
}

void
file_editor::construct (void)
{
  QWidget *editor_widget = new QWidget (this);

  // FIXME: what was the intended purpose of this unused variable?
  // QStyle *editor_style = QApplication::style ();
  _menu_bar = new QMenuBar (editor_widget);
#if defined (Q_OS_MAC)
  _menu_bar->setNativeMenuBar (false);
#endif
  _tool_bar = new QToolBar (editor_widget);
  _tool_bar->setMovable (true);
  _tab_widget = new QTabWidget (editor_widget);
  _tab_widget->setTabsClosable (true);
#ifdef HAVE_QTABWIDGET_SETMOVABLE
  _tab_widget->setMovable (true);
#endif


  // the mru-list and an empty array of actions
  QSettings *settings = resource_manager::get_settings ();
  _mru_files = settings->value ("editor/mru_file_list").toStringList ();
  for (int i = 0; i < MaxMRUFiles; ++i)
    {
      _mru_file_actions[i] = new QAction (this);
      _mru_file_actions[i]->setVisible (false);
    }

  // menu bar

  // file menu

  _fileMenu = new QMenu (tr ("&File"), _menu_bar);

  // new and open menus are inserted later by the main window
  _mru_file_menu = new QMenu (tr ("&Recent Editor Files"), _fileMenu);
  for (int i = 0; i < MaxMRUFiles; ++i)
    _mru_file_menu->addAction (_mru_file_actions[i]);
  _fileMenu->addMenu (_mru_file_menu);

  _fileMenu->addSeparator ();

  _edit_function_action = add_action (_fileMenu, QIcon (),
          tr ("&Edit Function"), SLOT (request_context_edit (bool)));

  _fileMenu->addSeparator ();

  _save_action = add_action (_fileMenu, QIcon (":/actions/icons/filesave.png"),
          tr ("&Save File"), SLOT (request_save_file (bool)));
  _save_as_action = add_action (_fileMenu, QIcon (":/actions/icons/filesaveas.png"),
          tr ("Save File &As..."), SLOT (request_save_file_as (bool)));

  _fileMenu->addSeparator ();

  _close_action = add_action (_fileMenu,
          QIcon::fromTheme("window-close",QIcon (":/actions/icons/fileclose.png")),
          tr ("&Close"), SLOT (request_close_file (bool)));
  _close_all_action = add_action (_fileMenu,
          QIcon::fromTheme("window-close", QIcon (":/actions/icons/fileclose.png")),
          tr ("Close All"), SLOT (request_close_all_files (bool)));
  _close_others_action = add_action (_fileMenu,
          QIcon::fromTheme("window-close", QIcon (":/actions/icons/fileclose.png")),
          tr ("Close Other Files"), SLOT (request_close_other_files (bool)));

  _fileMenu->addSeparator ();

  _print_action = add_action (_fileMenu, QIcon (":/actions/icons/fileprint.png"),
          tr ("Print..."), SLOT (request_print_file (bool)));

  _menu_bar->addMenu (_fileMenu);

  // edit menu

  QMenu *editMenu = new QMenu (tr ("&Edit"), _menu_bar);

  _undo_action = add_action (editMenu, QIcon (":/actions/icons/undo.png"),
          tr ("&Undo"), SLOT (request_undo (bool)));
  _redo_action = add_action (editMenu, QIcon (":/actions/icons/redo.png"),
          tr ("&Redo"), SLOT (request_redo (bool)));

  editMenu->addSeparator ();

  _copy_action = add_action (editMenu, QIcon (":/actions/icons/editcopy.png"),
          tr ("&Copy"), SLOT (request_copy (bool)));
  _copy_action->setEnabled (false);
  _cut_action = add_action (editMenu, QIcon (":/actions/icons/editcut.png"),
          tr ("Cu&t"), SLOT (request_cut (bool)));
  _cut_action->setEnabled (false);
  _paste_action = add_action (editMenu, QIcon (":/actions/icons/editpaste.png"),
          tr ("Paste"), SLOT (request_paste (bool)));

  editMenu->addSeparator ();

  _selectall_action = add_action (editMenu, QIcon (), tr ("Select All"),
          SLOT (request_selectall (bool)));

  editMenu->addSeparator ();

  _find_action = add_action (editMenu, QIcon (":/actions/icons/find.png"),
          tr ("&Find and Replace..."), SLOT (request_find (bool)));

  editMenu->addSeparator ();

  _edit_cmd_menu = editMenu->addMenu (tr ("&Commands"));

  _delete_line_action = add_action (_edit_cmd_menu, QIcon (),
          tr ("Delete Line"), SLOT (request_delete_line (bool)));
  _copy_line_action = add_action (_edit_cmd_menu, QIcon (),
          tr ("Copy Line"), SLOT (request_copy_line (bool)));
  _cut_line_action = add_action (_edit_cmd_menu, QIcon (),
          tr ("Cut Line"), SLOT (request_cut_line (bool)));

  _edit_cmd_menu->addSeparator ();

  _delete_start_word_action = add_action (_edit_cmd_menu, QIcon (),
          tr ("Delete to Start of Word"), SLOT (request_delete_start_word (bool)));
  _delete_end_word_action = add_action (_edit_cmd_menu, QIcon (),
          tr ("Delete to End of Word"), SLOT (request_delete_end_word (bool)));
  _delete_start_line_action = add_action (_edit_cmd_menu, QIcon (),
          tr ("Delete to Start of Line"), SLOT (request_delete_start_line (bool)));
  _delete_end_line_action = add_action (_edit_cmd_menu, QIcon (),
          tr ("Delete to End of Line"), SLOT (request_delete_end_line (bool)));

  _edit_cmd_menu->addSeparator ();

  _duplicate_selection_action = add_action (_edit_cmd_menu, QIcon (),
          tr ("Duplicate Selection/Line"), SLOT (request_duplicate_selection (bool)));
  _transpose_line_action = add_action (_edit_cmd_menu, QIcon (),
          tr ("Transpose Line"), SLOT (request_transpose_line (bool)));

  _edit_cmd_menu->addSeparator ();

  _completion_action = add_action (_edit_cmd_menu, QIcon (),
          tr ("&Show Completion List"), SLOT (request_completion (bool)));

  _edit_fmt_menu = editMenu->addMenu (tr ("&Format"));

  _upper_case_action = add_action (_edit_fmt_menu, QIcon (),
          tr ("&Uppercase Selection"), SLOT (request_upper_case (bool)));
  _lower_case_action = add_action (_edit_fmt_menu, QIcon (),
          tr ("&Lowercase Selection"), SLOT (request_lower_case (bool)));
  _comment_selection_action = add_action (_edit_fmt_menu, QIcon (),
          tr ("&Comment"), SLOT (request_comment_selected_text (bool)));
  _uncomment_selection_action = add_action (_edit_fmt_menu, QIcon (),
          tr ("&Uncomment"), SLOT (request_uncomment_selected_text (bool)));
  _indent_selection_action = add_action (_edit_fmt_menu, QIcon (),
          tr ("&Indent"), SLOT (request_indent_selected_text (bool)));
  _unindent_selection_action = add_action (_edit_fmt_menu, QIcon (),
          tr ("&Unindent"), SLOT (request_unindent_selected_text (bool)));

  _edit_nav_menu = editMenu->addMenu (tr ("Navi&gation"));

  _goto_line_action = add_action (_edit_nav_menu, QIcon (),
          tr ("Go &to Line..."), SLOT (request_goto_line (bool)));

  _edit_nav_menu->addSeparator ();

  _next_bookmark_action =  add_action (_edit_nav_menu, QIcon (),
          tr ("&Next Bookmark"), SLOT (request_next_bookmark (bool)));
  _previous_bookmark_action =  add_action (_edit_nav_menu, QIcon (),
          tr ("Pre&vious Bookmark"), SLOT (request_previous_bookmark (bool)));
  _toggle_bookmark_action =  add_action (_edit_nav_menu, QIcon (),
          tr ("Toggle &Bookmark"), SLOT (request_toggle_bookmark (bool)));
  _remove_bookmark_action = add_action (_edit_nav_menu, QIcon (),
          tr ("&Remove All Bookmarks"), SLOT (request_remove_bookmark (bool)));

  editMenu->addSeparator ();

  _preferences_action = add_action (editMenu,
          QIcon (":/actions/icons/configure.png"),
          tr ("&Preferences..."), SLOT (request_preferences (bool)));
  _styles_preferences_action = add_action (editMenu,
          QIcon (":/actions/icons/configure.png"),
          tr ("&Styles Preferences..."), SLOT (request_styles_preferences (bool)));

  _menu_bar->addMenu (editMenu);

  // view menu

  QMenu *view_menu = new QMenu (tr ("&View"), _menu_bar);

  _zoom_in_action = add_action (view_menu, QIcon (),
          tr ("Zoom &In"), SLOT (zoom_in (bool)));
  _zoom_out_action = add_action (view_menu, QIcon (),
          tr ("Zoom &Out"), SLOT (zoom_out (bool)));
  _zoom_normal_action = add_action (view_menu, QIcon (),
          tr ("&Normal Size"), SLOT (zoom_normal (bool)));

  _menu_bar->addMenu (view_menu);

  // debug menu

  _debug_menu = new QMenu (tr ("&Debug"), _menu_bar);

  _toggle_breakpoint_action = add_action (_debug_menu,
          QIcon (":/actions/icons/bp_toggle.png"), tr ("Toggle &Breakpoint"),
          SLOT (request_toggle_breakpoint (bool)));
  _next_breakpoint_action = add_action (_debug_menu,
          QIcon (":/actions/icons/bp_next.png"), tr ("&Next Breakpoint"),
          SLOT (request_next_breakpoint (bool)));
  _previous_breakpoint_action = add_action (_debug_menu,
          QIcon (":/actions/icons/bp_prev.png"), tr ("Pre&vious Breakpoint"),
          SLOT (request_previous_breakpoint (bool)));
  _remove_all_breakpoints_action = add_action (_debug_menu,
          QIcon (":/actions/icons/bp_rm_all.png"), tr ("&Remove All Breakpoints"),
          SLOT (request_remove_breakpoint (bool)));

  _debug_menu->addSeparator ();

  // The other debug actions will be added by the main window.

  _menu_bar->addMenu (_debug_menu);

  // run menu

  QMenu *_run_menu = new QMenu (tr ("&Run"), _menu_bar);

  _run_action = add_action (_run_menu, QIcon (":/actions/icons/artsbuilderexecute.png"),
          tr ("Save File and Run"), SLOT (request_run_file (bool)));
  _run_selection_action = add_action (_run_menu, QIcon (),
          tr ("Run &Selection"), SLOT (request_context_run (bool)));
  _run_selection_action->setEnabled (false);

  _menu_bar->addMenu (_run_menu);

  // help menu

  QMenu *_help_menu = new QMenu (tr ("&Help"), _menu_bar);

  _context_help_action = add_action (_help_menu, QIcon (),
          tr ("&Help on Keyword"), SLOT (request_context_help (bool)));
  _context_doc_action = add_action (_help_menu, QIcon (),
          tr ("&Documentation on Keyword"), SLOT (request_context_doc (bool)));

  _menu_bar->addMenu (_help_menu);

  // toolbar

  // new and open actions are inserted later from main window
  _tool_bar->addAction (_save_action);
  _tool_bar->addAction (_save_as_action);
  _tool_bar->addSeparator ();
  _tool_bar->addAction (_print_action);
  _tool_bar->addSeparator ();
  _tool_bar->addAction (_undo_action);
  _tool_bar->addAction (_redo_action);
  _tool_bar->addAction (_copy_action);
  _tool_bar->addAction (_cut_action);
  _tool_bar->addAction (_paste_action);
  _tool_bar->addSeparator ();
  _tool_bar->addAction (_find_action);
  _tool_bar->addAction (_run_action);
  _tool_bar->addSeparator ();
  _tool_bar->addAction (_toggle_breakpoint_action);
  _tool_bar->addAction (_next_breakpoint_action);
  _tool_bar->addAction (_previous_breakpoint_action);
  _tool_bar->addAction (_remove_all_breakpoints_action);

  // layout
  QVBoxLayout *vbox_layout = new QVBoxLayout ();
  vbox_layout->addWidget (_menu_bar);
  vbox_layout->addWidget (_tool_bar);
  vbox_layout->addWidget (_tab_widget);
  vbox_layout->setMargin (0);
  editor_widget->setLayout (vbox_layout);
  setWidget (editor_widget);

  // signals
  connect (this, SIGNAL (request_settings_dialog (const QString&)),
           main_win (),
           SLOT (process_settings_dialog_request (const QString&)));

  connect (main_win (), SIGNAL (new_file_signal (const QString&)),
           this, SLOT (request_new_file (const QString&)));

  connect (main_win (), SIGNAL (open_file_signal (const QString&)),
           this, SLOT (request_open_file (const QString&)));

  connect (_mru_file_menu, SIGNAL (triggered (QAction *)),
           this, SLOT (request_mru_open_file (QAction *)));

  mru_menu_update ();

  connect (_tab_widget, SIGNAL (tabCloseRequested (int)),
           this, SLOT (handle_tab_close_request (int)));

  connect (_tab_widget, SIGNAL (currentChanged (int)),
           this, SLOT (active_tab_changed (int)));

  connect (this, SIGNAL (execute_command_in_terminal_signal (const QString&)),
           main_win (), SLOT (execute_command_in_terminal (const QString&)));

  resize (500, 400);
  setWindowIcon (QIcon (":/actions/icons/logo.png"));
  set_title ("Editor");

  //restore previous session
  if (settings->value ("editor/restoreSession", true).toBool ())
    {
      QStringList sessionFileNames
        = settings->value ("editor/savedSessionTabs",
                           QStringList ()).toStringList ();

      for (int n = 0; n < sessionFileNames.count (); ++n)
        request_open_file (sessionFileNames.at (n));
    }

  check_actions ();
}

void
file_editor::add_file_editor_tab (file_editor_tab *f, const QString& fn)
{
  _tab_widget->addTab (f, fn);

  // Signals from the file editor_tab
  connect (f, SIGNAL (file_name_changed (const QString&, const QString&)),
           this, SLOT (handle_file_name_changed (const QString&,
                                                 const QString&)));

  connect (f, SIGNAL (editor_state_changed (bool, const QString&)),
           this, SLOT (handle_editor_state_changed (bool, const QString&)));

  connect (f, SIGNAL (tab_remove_request ()),
           this, SLOT (handle_tab_remove_request ()));

  connect (f, SIGNAL (add_filename_to_list (const QString&, QWidget*)),
           this, SLOT (handle_add_filename_to_list (const QString&, QWidget*)));

  connect (f, SIGNAL (editor_check_conflict_save (const QString&, bool)),
           this, SLOT (check_conflict_save (const QString&, bool)));

  connect (f, SIGNAL (mru_add_file (const QString&)),
           this, SLOT (handle_mru_add_file (const QString&)));

  connect (f, SIGNAL (create_context_menu_tab_signal (QMenu *)),
           this, SLOT (create_context_menu (QMenu *)));

  connect (f, SIGNAL (run_file_signal (const QFileInfo&)),
           main_win (), SLOT (run_file_in_terminal (const QFileInfo&)));

  connect (f, SIGNAL (execute_command_in_terminal_signal (const QString&)),
           main_win (), SLOT (execute_command_in_terminal (const QString&)));

  connect (f, SIGNAL (set_global_edit_shortcuts_signal (bool)),
           main_win (), SLOT (set_global_edit_shortcuts (bool)));

  // Signals from the file_editor non-trivial operations
  connect (this, SIGNAL (fetab_settings_changed (const QSettings *)),
           f, SLOT (notice_settings (const QSettings *)));

  connect (this, SIGNAL (fetab_close_request (const QWidget*,bool)),
           f, SLOT (conditional_close (const QWidget*,bool)));

  connect (this, SIGNAL (fetab_change_request (const QWidget*)),
           f, SLOT (change_editor_state (const QWidget*)));

  connect (this, SIGNAL (fetab_file_name_query (const QWidget*)),
           f, SLOT (file_name_query (const QWidget*)));

  connect (this, SIGNAL (fetab_save_file (const QWidget*, const QString&,
                                          bool)),
           f, SLOT (save_file (const QWidget*, const QString&, bool)));

  // Signals from the file_editor trivial operations
  connect (this, SIGNAL (fetab_zoom_in (const QWidget*)),
           f, SLOT (zoom_in (const QWidget*)));
  connect (this, SIGNAL (fetab_zoom_out (const QWidget*)),
           f, SLOT (zoom_out (const QWidget*)));
  connect (this, SIGNAL (fetab_zoom_normal (const QWidget*)),
           f, SLOT (zoom_normal (const QWidget*)));

  connect (this, SIGNAL (fetab_context_help (const QWidget*, bool)),
           f, SLOT (context_help (const QWidget*, bool)));

  connect (this, SIGNAL (fetab_context_edit (const QWidget*)),
           f, SLOT (context_edit (const QWidget*)));

  connect (this, SIGNAL (fetab_save_file (const QWidget*)),
           f, SLOT (save_file (const QWidget*)));

  connect (this, SIGNAL (fetab_save_file_as (const QWidget*)),
           f, SLOT (save_file_as (const QWidget*)));

  connect (this, SIGNAL (fetab_print_file (const QWidget*)),
           f, SLOT (print_file (const QWidget*)));

  connect (this, SIGNAL (fetab_run_file (const QWidget*)),
           f, SLOT (run_file (const QWidget*)));

  connect (this, SIGNAL (fetab_context_run (const QWidget*)),
           f, SLOT (context_run (const QWidget*)));

  connect (this, SIGNAL (fetab_toggle_bookmark (const QWidget*)),
           f, SLOT (toggle_bookmark (const QWidget*)));

  connect (this, SIGNAL (fetab_next_bookmark (const QWidget*)),
           f, SLOT (next_bookmark (const QWidget*)));

  connect (this, SIGNAL (fetab_previous_bookmark (const QWidget*)),
           f, SLOT (previous_bookmark (const QWidget*)));

  connect (this, SIGNAL (fetab_remove_bookmark (const QWidget*)),
           f, SLOT (remove_bookmark (const QWidget*)));

  connect (this, SIGNAL (fetab_toggle_breakpoint (const QWidget*)),
           f, SLOT (toggle_breakpoint (const QWidget*)));

  connect (this, SIGNAL (fetab_next_breakpoint (const QWidget*)),
           f, SLOT (next_breakpoint (const QWidget*)));

  connect (this, SIGNAL (fetab_previous_breakpoint (const QWidget*)),
           f, SLOT (previous_breakpoint (const QWidget*)));

  connect (this, SIGNAL (fetab_remove_all_breakpoints (const QWidget*)),
           f, SLOT (remove_all_breakpoints (const QWidget*)));

  connect (this, SIGNAL (fetab_scintilla_command (const QWidget *, unsigned int)),
           f, SLOT (scintilla_command (const QWidget *, unsigned int)));

  connect (this, SIGNAL (fetab_comment_selected_text (const QWidget*)),
           f, SLOT (comment_selected_text (const QWidget*)));

  connect (this, SIGNAL (fetab_uncomment_selected_text (const QWidget*)),
           f, SLOT (uncomment_selected_text (const QWidget*)));

  connect (this, SIGNAL (fetab_indent_selected_text (const QWidget*)),
           f, SLOT (indent_selected_text (const QWidget*)));

  connect (this, SIGNAL (fetab_unindent_selected_text (const QWidget*)),
           f, SLOT (unindent_selected_text (const QWidget*)));

  connect (this, SIGNAL (fetab_find (const QWidget*)),
           f, SLOT (find (const QWidget*)));

  connect (this, SIGNAL (fetab_goto_line (const QWidget*, int)),
           f, SLOT (goto_line (const QWidget*, int)));

  connect (this, SIGNAL (fetab_completion (const QWidget*)),
           f, SLOT (show_auto_completion (const QWidget*)));

  connect (this, SIGNAL (fetab_set_focus (const QWidget*)),
           f, SLOT (set_focus (const QWidget*)));

  connect (this, SIGNAL (fetab_insert_debugger_pointer (const QWidget*, int)),
           f, SLOT (insert_debugger_pointer (const QWidget*, int)));

  connect (this, SIGNAL (fetab_delete_debugger_pointer (const QWidget*, int)),
           f, SLOT (delete_debugger_pointer (const QWidget*, int)));

  connect (this, SIGNAL (fetab_do_breakpoint_marker (bool, const QWidget*,
                                                     int)),
           f, SLOT (do_breakpoint_marker (bool, const QWidget*, int)));

  _tab_widget->setCurrentWidget (f);

  check_actions ();
}

void
file_editor::set_shortcuts ()
{
  // File menu
  shortcut_manager::set_shortcut (_edit_function_action, "editor_file:edit_function");
  shortcut_manager::set_shortcut (_save_action, "editor_file:save");
  shortcut_manager::set_shortcut (_save_as_action, "editor_file:save_as");
  shortcut_manager::set_shortcut (_close_action, "editor_file:close");
  shortcut_manager::set_shortcut (_close_all_action, "editor_file:close_all");
  shortcut_manager::set_shortcut (_close_others_action, "editor_file:close_other");
  shortcut_manager::set_shortcut (_print_action, "editor_file:print");

  // Edit menu
  shortcut_manager::set_shortcut (_undo_action, "editor_edit:undo");
  shortcut_manager::set_shortcut (_redo_action, "editor_edit:redo");
  shortcut_manager::set_shortcut (_copy_action, "editor_edit:copy");
  shortcut_manager::set_shortcut (_cut_action, "editor_edit:cut");
  shortcut_manager::set_shortcut (_paste_action, "editor_edit:paste");
  shortcut_manager::set_shortcut (_selectall_action, "editor_edit:select_all");
  shortcut_manager::set_shortcut (_find_action, "editor_edit:find_replace");

  shortcut_manager::set_shortcut (_delete_start_word_action, "editor_edit:delete_start_word");
  shortcut_manager::set_shortcut (_delete_end_word_action, "editor_edit:delete_end_word");
  shortcut_manager::set_shortcut (_delete_start_line_action, "editor_edit:delete_start_line");
  shortcut_manager::set_shortcut (_delete_end_line_action, "editor_edit:delete_end_line");
  shortcut_manager::set_shortcut (_delete_line_action, "editor_edit:delete_line");
  shortcut_manager::set_shortcut (_copy_line_action, "editor_edit:copy_line");
  shortcut_manager::set_shortcut (_cut_line_action, "editor_edit:cut_line");
  shortcut_manager::set_shortcut (_duplicate_selection_action, "editor_edit:duplicate_selection");
  shortcut_manager::set_shortcut (_transpose_line_action, "editor_edit:transpose_line");
  shortcut_manager::set_shortcut (_comment_selection_action, "editor_edit:comment_selection");
  shortcut_manager::set_shortcut (_uncomment_selection_action, "editor_edit:uncomment_selection");

  shortcut_manager::set_shortcut (_upper_case_action, "editor_edit:upper_case");
  shortcut_manager::set_shortcut (_lower_case_action, "editor_edit:lower_case");
  shortcut_manager::set_shortcut (_indent_selection_action, "editor_edit:indent_selection");
  shortcut_manager::set_shortcut (_unindent_selection_action, "editor_edit:unindent_selection");
  shortcut_manager::set_shortcut (_completion_action, "editor_edit:completion_list");
  shortcut_manager::set_shortcut (_goto_line_action, "editor_edit:goto_line");
  shortcut_manager::set_shortcut (_toggle_bookmark_action, "editor_edit:toggle_bookmark");
  shortcut_manager::set_shortcut (_next_bookmark_action, "editor_edit:next_bookmark");
  shortcut_manager::set_shortcut (_previous_bookmark_action, "editor_edit:previous_bookmark");
  shortcut_manager::set_shortcut (_remove_bookmark_action, "editor_edit:remove_bookmark");
  shortcut_manager::set_shortcut (_preferences_action, "editor_edit:preferences");
  shortcut_manager::set_shortcut (_styles_preferences_action, "editor_edit:styles_preferences");

  // View menu
  shortcut_manager::set_shortcut (_zoom_in_action, "editor_view:zoom_in");
  shortcut_manager::set_shortcut (_zoom_out_action, "editor_view:zoom_out");
  shortcut_manager::set_shortcut (_zoom_normal_action, "editor_view:zoom_normal");

  // Debug menu
  shortcut_manager::set_shortcut (_toggle_breakpoint_action, "editor_debug:toggle_breakpoint");
  shortcut_manager::set_shortcut (_next_breakpoint_action, "editor_debug:next_breakpoint");
  shortcut_manager::set_shortcut (_previous_bookmark_action, "editor_debug:previous_breakpoint");
  shortcut_manager::set_shortcut (_remove_all_breakpoints_action, "editor_debug:remove_breakpoints");

  // Run menu
  shortcut_manager::set_shortcut (_run_action, "editor_run:run_file");
  shortcut_manager::set_shortcut (_run_selection_action, "editor_run:run_selection");

  // Help menu
  shortcut_manager::set_shortcut (_context_help_action, "editor_help:help_keyword");
  shortcut_manager::set_shortcut (_context_doc_action,  "editor_help:doc_keyword");

}

void
file_editor::check_actions ()
{
  bool  have_tabs = _tab_widget->count () > 0;

  _edit_cmd_menu->setEnabled (have_tabs);
  _edit_fmt_menu->setEnabled (have_tabs);
  _edit_nav_menu->setEnabled (have_tabs);

  _comment_selection_action->setEnabled (have_tabs);
  _uncomment_selection_action->setEnabled (have_tabs);
  _indent_selection_action->setEnabled (have_tabs);
  _unindent_selection_action->setEnabled (have_tabs);

  _paste_action->setEnabled (have_tabs);
  _context_help_action->setEnabled (have_tabs);
  _context_doc_action->setEnabled (have_tabs);

  _zoom_in_action->setEnabled (have_tabs);
  _zoom_out_action->setEnabled (have_tabs);
  _zoom_normal_action->setEnabled (have_tabs);

  _find_action->setEnabled (have_tabs);
  _print_action->setEnabled (have_tabs);
  _run_action->setEnabled (have_tabs);

  _edit_function_action->setEnabled (have_tabs);
  _save_action->setEnabled (have_tabs);
  _save_as_action->setEnabled (have_tabs);
  _close_action->setEnabled (have_tabs);
  _close_all_action->setEnabled (have_tabs);
  _close_others_action->setEnabled (have_tabs && _tab_widget->count () > 1);

  _undo_action->setEnabled (have_tabs);
  _redo_action->setEnabled (have_tabs);
  _selectall_action->setEnabled (have_tabs);
}

// empty_script determines whether we have to create an empty script
// 1. At startup, when the editor has to be (really) visible
//    (Here we can not use the visibility changed signal)
// 2. When the editor becomes visible when octave is running
void
file_editor::empty_script (bool startup, bool visible)
{
  bool real_visible;

  if (startup)
    real_visible = isVisible ();
  else
    real_visible = visible;

  if (! real_visible || _tab_widget->count () > 0)
    return;

  if (startup && ! isFloating ())
    {
      // check is editor is really visible or hidden between tabbed widgets
      QList<QTabBar *> tab_list = main_win ()->findChildren<QTabBar *>();

      bool in_tab = false;
      int i = 0;
      while ((i < tab_list.count ()) && (! in_tab))
        {
          QTabBar *tab = tab_list.at (i);
          i++;

          int j = 0;
          while ((j < tab->count ()) && (! in_tab))
            {
              // check all tabs for the editor
              if (tab->tabText (j) == windowTitle ())
                {
                  // editor is in this tab widget
                  in_tab = true;
                  int top = tab->currentIndex ();
                  if (top > -1 && tab->tabText (top) == windowTitle ())
                    real_visible = true;  // and is the current tab
                  else
                    return; // not current tab -> not visible
                }
              j++;
            }
        }
    }

  request_new_file ("");
}

// This slot is a reimplementation of the virtual slot in octave_dock_widget.
// We need this for creating an empty script when the editor has no open files
// and is made visible
void
file_editor::handle_visibility (bool visible)
  {
    empty_script (false, visible);

    if (visible && ! isFloating ())
      focus ();
  }

void 
file_editor::dragEnterEvent (QDragEnterEvent *event)
  {
    if (event->mimeData ()->hasUrls ())
      {
        event->acceptProposedAction();
      }
  }

void
file_editor::dropEvent (QDropEvent *event)
  {
    if (event->mimeData ()->hasUrls ())
      {
        foreach (QUrl url, event->mimeData ()->urls ())
        {
          request_open_file (url.toLocalFile ());
        }
      }
  }

#endif
