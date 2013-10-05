/*

Copyright (C) 2011-2012 Jacob Dawid

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

#include "welcome-wizard.h"
#include "resource-manager.h"
#include "ui-welcome-wizard.h"

welcome_wizard::welcome_wizard (QWidget *p)
  : QDialog (p), _ui (new Ui::welcome_wizard)
{
  _ui->setupUi (this);
  QString label_text = _ui->label_config_file->text ();
  label_text.replace (QString ("__%1__"),
                      resource_manager::get_settings_file ());
  _ui->label_config_file->setText (label_text);
}

welcome_wizard::~welcome_wizard()
{
  delete _ui;
}
