/*******************************************************************************
 * PaintChatPopupChatDialog.cpp                                                *
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

#include "paintchatpopupchatdialog.h"

PaintChatPopupChatDialog::PaintChatPopupChatDialog(ChatWidget *chatWidget)
    : QObject(), ChatWidgetHolder(chatWidget)
{
	paintChatWindow = new MainWindow(chatWidget, mChatWidget->getChatId(), mChatWidget);

    paintChatWindow->hide();

    paintChatWindowToggleButton = new QToolButton ;
    //paintChatWindowToggleButton->setMinimumSize(QSize(28,28)) ;
    //paintChatWindowToggleButton->setMaximumSize(QSize(28,28)) ;
    paintChatWindowToggleButton->setText(QString()) ;
    paintChatWindowToggleButton->setToolTip(tr("QtPaint"));
    paintChatWindowToggleButton->setAutoRaise(true);
    paintChatWindowToggleButton->setIconSize(QSize(28,28));

    QIcon icon ;
    icon.addPixmap(QPixmap(":/icons/app.png"));
    paintChatWindowToggleButton->setIcon(icon);

    connect(paintChatWindowToggleButton,SIGNAL(clicked()),this,SLOT(togglePaintChatWindow()));

    mChatWidget->addChatBarWidget(paintChatWindowToggleButton) ;
}

void PaintChatPopupChatDialog::togglePaintChatWindow(){
    if(paintChatWindow->isHidden()){
        paintChatWindow->show();
    }else{
        paintChatWindow->hide();
    }
}
