/*******************************************************************************
 * PaintChatPopupChatDialog.h                                                  *
 *                                                                             *
 * Copyright (C) 2025 Retroshare Team <retroshare.project@gmail.com>           *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify        *
 * it under the terms of the GNU Affero General Public License as              *
 * published by the Free Software Foundation, either version 3 of the          *
 * License, or (at your option) any later version.                             *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                *
 * GNU Affero General Public License for more details.                         *
 *                                                                             *
 * You should have received a copy of the GNU Affero General Public License    *
 * along with this program. If not, see <https://www.gnu.org/licenses/>.       *
 *                                                                             *
 *******************************************************************************/

#ifndef PAINTCHATPOPUPCHATDIALOG_H
#define PAINTCHATPOPUPCHATDIALOG_H

#include <gui/chat/ChatWidget.h>
#include <QToolButton>
#include "mainwindow.h"

class PaintChatPopupChatDialog : public QObject, public ChatWidgetHolder
{
    Q_OBJECT
public:
    PaintChatPopupChatDialog(ChatWidget *chatWidget);

private slots:
    void togglePaintChatWindow();

protected:

private:
    QToolButton *paintChatWindowToggleButton;
    MainWindow *paintChatWindow;
};

#endif // PAINTCHATPOPUPCHATDIALOG_H
