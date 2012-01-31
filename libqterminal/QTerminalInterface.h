/*

Copyright (C) 2012 Michael Goffioul.
Copyright (C) 2012 Jacob Dawid.

This file is part of QTerminal.

Foobar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

QTerminal is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef QTERMINALINTERFACE_H
#define QTERMINALINTERFACE_H

#include <QWidget>

class QTerminalInterface : public QWidget
{
    Q_OBJECT
public:
    QTerminalInterface(QWidget *parent = 0) : QWidget(parent) { }
    virtual ~QTerminalInterface() { }

    virtual void setTerminalFont(QFont &font) = 0;
    virtual void setSize(int h, int v) = 0;
    virtual void sendText(QString text) = 0;

public slots:
    virtual void copyClipboard() = 0;
    virtual void pasteClipboard() = 0;

protected:
    virtual void focusInEvent(QFocusEvent *focusEvent) = 0;
    virtual void showEvent(QShowEvent *) = 0;
    virtual void resizeEvent(QResizeEvent *) = 0;
};

#endif // QTERMINALINTERFACE_H
