/*
Copyright 2017 SillyLossy.
This program is free software; you can redistribute it and/or
modify it under the terms of the Lesser GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the Lesser GNU General Public License for more details.
You should have received a copy of the Lesser GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QStandardPaths>
#include <QImageWriter>
#include <QCheckBox>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDockWidget>

#include "newimagedialog.h"
#include "paintarea.h"
#include "zoomslider.h"

#include <retroshare/rspeers.h>
#include <retroshare/rsmsgs.h>
#include <retroshare/rsidentity.h>
#include <unistd.h>
#include <util/imageutil.h>
#include <util/HandleRichText.h>

static const QString FORMATS_FILTER = QStringLiteral("Images (*.png *.bmp *.jpg *.jpeg *.gif);;All files (*.*)");
static const QString PICTURES_PATH = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
static const QString DEFAULT_FILE_NAME = QStringLiteral("Unnamed");
static const int MESSAGE_TIMEOUT = 5000; // 5 sec


MainWindow::MainWindow(QWidget *parent, ChatId chatId, ChatWidget *chatWidget) :
	QMainWindow(parent), chatId(chatId), chatType(ChatWidget::CHATTYPE_UNKNOWN), chatWidget(chatWidget),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_paintArea = ui->paintArea;
    m_fileName = DEFAULT_FILE_NAME;
    m_layersList = new LayersList;
    m_dock.setVisible(false);
    setStatusBar();
    setToolBox();
    setDock();

    m_strokeColorLabel.setToolTip(tr("Stroke color"));
    m_fillColorLabel.setToolTip(tr("Fill color"));  

    //connect(m_paintArea,SIGNAL(haveUpdate()),SLOT(on_haveUpdate()));

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),SLOT(on_haveUpdate()));
    timer->start(500);

    chatType = chatWidget->chatType();

    if(chatType == ChatWidget::CHATTYPE_LOBBY)
    {
        ui->progressBar->setRange(0,100);
        ui->progressBar->setValue(0);

        ChatLobbyId id;
        id = chatId.toLobbyId();
        std::string nick, lobby;

        RsGxsId gxsid;
        RsIdentityDetails details;
        rsMsgs->getIdentityForChatLobby(id, gxsid);
        for(int i=0;i<3;++i)
            if(rsIdentity->getIdDetails(gxsid,details))
                break ;
            else
                usleep(1000*300) ;
        nick = details.mNickname;

        ChatLobbyInfo info;
        rsMsgs->getChatLobbyInfo(id, info);
        lobby = info.lobby_name;

        std::string label = nick + "@" + lobby;
        setWindowTitle(QString::fromStdString(label + " - QtPaint"));
    }

    ui->lblWarning->setVisible(false);
    MAX_LOBBY_MSG_SIZE = maxMessageSize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_haveUpdate(){
    std::cerr<<"PaintWindow::on_haveUpdate()"<<std::endl;

    /*if(chatType == ChatWidget::CHATTYPE_PRIVATE)
    {
        updateImage();
    }*/
    if(chatType == ChatWidget::CHATTYPE_LOBBY)
    {
        QImage img = m_paintArea->getImage();
        QString html;
        QImage opt;
        ImageUtil::optimizeSizeHtml(html, img, opt, -1, MAX_LOBBY_MSG_SIZE);

        ui->progressBar->setValue((html.size()*100)/MAX_LOBBY_MSG_SIZE);

        if(html.size()>MAX_LOBBY_MSG_SIZE)
        {
            ui->progressBar->setValue(100);
        }
        ui->lblImageSize->setText(QString("Image size: %1 bytes, maximum: %2 bytes").arg(html.size()).arg(MAX_LOBBY_MSG_SIZE));
        ui->lblWarning->setVisible(opt.width() < img.width());
    }
}

uint32_t MainWindow::maxMessageSize()
{
    uint32_t maxMessageSize = 1;
    switch (chatWidget->chatType()) {
    case ChatWidget::CHATTYPE_UNKNOWN:
        break;
    case ChatWidget::CHATTYPE_PRIVATE:
        maxMessageSize = rsMsgs->getMaxMessageSecuritySize(RS_CHAT_TYPE_PRIVATE);
        break;
    case ChatWidget::CHATTYPE_LOBBY:
        maxMessageSize = rsMsgs->getMaxMessageSecuritySize(RS_CHAT_TYPE_LOBBY);
        break;
    case ChatWidget::CHATTYPE_DISTANT:
        maxMessageSize = rsMsgs->getMaxMessageSecuritySize(RS_CHAT_TYPE_DISTANT);
        break;
    }
    return maxMessageSize;
}

void MainWindow::on_actionClear_triggered()
{
    m_paintArea->setBlankLayer(400, 400, Qt::white);
    on_haveUpdate();
}

void MainWindow::on_actionOpen_file_triggered()
{
    auto file = QFileDialog::getOpenFileName(this, QStringLiteral("Open image"), PICTURES_PATH, FORMATS_FILTER);
    if (file.isNull()) {
        return;
    }
    if (m_paintArea->loadImage(file)) {
        ui->statusBar->showMessage(QStringLiteral("Opened ") + file, MESSAGE_TIMEOUT);
        m_filePath = file;
        m_fileName = QFileInfo(file).fileName();
    }
    else {
        ui->statusBar->showMessage(QStringLiteral("Can't open ") + file, MESSAGE_TIMEOUT);
    }
}

void MainWindow::on_actionSave_file_triggered()
{
    static const QString SAVE_SUCCESS = QStringLiteral("Saved image to ");
    static const QString SAVE_FAILED = QStringLiteral("Can't save to ");
    bool result = QFileInfo(m_filePath).exists() ? saveExistingFile() : saveNewFile();
    ui->statusBar->showMessage((result ? SAVE_SUCCESS : SAVE_FAILED) + m_fileName, MESSAGE_TIMEOUT);
}

void MainWindow::on_actionSelect_color_triggered()
{
    m_strokeColorLabel.openColorSelectionDialog();
}

void MainWindow::setToolBox()
{
    auto tools = m_paintArea->tools();
    for (int i = 0; i < tools->size(); i++) {
        auto tool = tools->at(i);
        QIcon icon;
        icon.addFile(":/icons/" + tool->iconName() + ".png");
        m_toolbox.addItem(icon, tool->toolName());
    }
    connect(&m_toolbox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), m_paintArea, &PaintArea::setSelectedTool);
}

void MainWindow::setStatusBar()
{
    m_widthButton = new WidthButton(this);
    auto checkButton = new QCheckBox(QStringLiteral("Antialiasing"));
    checkButton->setChecked(m_paintArea->antialiasingEnabled());
    connect(checkButton, &QCheckBox::toggled, m_paintArea, &PaintArea::setAntialiasingEnabled);
    ui->statusBar->addPermanentWidget(m_widthButton);
    //ui->statusBar->addPermanentWidget(new QLabel(QStringLiteral("Stroke color:")));
    ui->statusBar->addPermanentWidget(&m_strokeColorLabel);
    //ui->statusBar->addPermanentWidget(new QLabel(QStringLiteral("Fill color:")));
    ui->statusBar->addPermanentWidget(&m_fillColorLabel);
    ui->statusBar->addPermanentWidget(new QLabel(QStringLiteral("Tool:")));
    ui->statusBar->addPermanentWidget(&m_toolbox);
    ui->statusBar->addPermanentWidget(checkButton);
    connect(m_widthButton->widthSlider(), &QSlider::valueChanged, m_paintArea, &PaintArea::setPenWidth);
    m_strokeColorLabel.setColor(m_paintArea->pen().color());
    connect(m_strokeColorLabel.dialog(), &QColorDialog::colorSelected, m_paintArea, &PaintArea::setPenColor);
    m_fillColorLabel.setColor(m_paintArea->brush().color());
    connect(m_fillColorLabel.dialog(), &QColorDialog::colorSelected, m_paintArea, &PaintArea::setBrushColor);
    auto zoom = new ZoomSlider();
    connect(zoom->slider(), &QSlider::valueChanged, m_paintArea, &PaintArea::setZoom);
    ui->statusBar->addPermanentWidget(zoom);
}

void MainWindow::setDock()
{
    static QLabel* title = new QLabel(QStringLiteral("Layers"));
    title->setAlignment(Qt::AlignHCenter);
    m_dock.setTitleBarWidget(title);
    m_dock.setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_dock.setWidget(m_layersList);
    addDockWidget(Qt::RightDockWidgetArea, &m_dock);
}

bool MainWindow::saveExistingFile()
{
    return (m_paintArea->saveImage().save(m_filePath));
}

bool MainWindow::saveNewFile()
{
    QString filter = QStringLiteral("All files (*.*);;");
    foreach (const auto& x, QImageWriter::supportedImageFormats()) {
        filter += QStringLiteral(".") += QString::fromLatin1(x) += QStringLiteral(";;");
    }
    filter.chop(2);
    m_filePath = QFileDialog::getSaveFileName(this, QStringLiteral("Save image"), PICTURES_PATH, filter);
    if (m_filePath.isNull()) {
        return false;
    }
    m_fileName = QFileInfo(m_filePath).fileName();
    return m_paintArea->saveImage().save(m_filePath);
}

int MainWindow::confirmSaveAction()
{
    return QMessageBox::question(this, QStringLiteral(""), QStringLiteral("Save changes to ") + m_fileName + QStringLiteral("?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
}

void MainWindow::on_actionNew_image_triggered()
{
    switch (confirmSaveAction()) {
    case QMessageBox::Yes:
        on_actionSave_file_triggered();
        break;
    case QMessageBox::Cancel:
        return;
    }
    auto dlg = new NewImageDialog(this);
    dlg->setWindowModality(Qt::WindowModal);
    if (dlg->exec()) {
        m_paintArea->setBlankLayer(dlg->width(), dlg->height(), dlg->color());
        m_fileName = DEFAULT_FILE_NAME;
        m_filePath = DEFAULT_FILE_NAME;
    }
}

void MainWindow::on_actionPaste_triggered()
{
    switch (confirmSaveAction()) {
    case QMessageBox::Yes:
        on_actionSave_file_triggered();
        break;
    case QMessageBox::Cancel:
        return;
    }
    if (!m_paintArea->pasteImage()) {
        ui->statusBar->showMessage(QStringLiteral("Clipboard doesn't contain acceptable image"), MESSAGE_TIMEOUT);
    }
}


void MainWindow::on_actionAbout_triggered()
{
    static const QString copyright = QStringLiteral(
                "<p>Copyright 2017 <a href='https://github.com/SillyLossy/'>SillyLossy</a></p>"
                 "<p>QtPaint is free software: you can redistribute it and/or modify "
                 "it under the terms of the Lesser GNU General Public License as published by "
                 "the Free Software Foundation, either version 3 of the License, or "
                 "(at your option) any later version.</p>"
                 "<p>QtPaint is distributed in the hope that it will be useful, "
                 "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                 "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                 "Lesser GNU General Public License for more details</p>"
                 "<p>You should have received a copy of the Lesser GNU General Public License "
                 "along with QtPaint. If not, see <a href='http://www.gnu.org/licenses/'>GNU.org</a>.</p>"
                 "<p>Uses icons from Oxygen theme by The Oxygen Team (LGPL v3): "
                 "<a href='https://github.com/KDE/oxygen-icons5'>their GitHub</a>.</p>");
    QMessageBox msgBox(this);
    QIcon icon;
    icon.addFile(QStringLiteral(":/icons/app.png"));
    msgBox.setWindowTitle(QStringLiteral("About"));
    msgBox.setIconPixmap(icon.pixmap(QSize(64, 64)));
    msgBox.setTextFormat(Qt::RichText);   //this is what makes the links clickable
    msgBox.setText(copyright);
    msgBox.exec();
}

void MainWindow::on_actionCopy_triggered()
{
    m_paintArea->copyImage();
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    switch (confirmSaveAction()) {
    case QMessageBox::Yes:
        on_actionSave_file_triggered();
        break;
    case QMessageBox::Cancel:
        e->setAccepted(false);
        return;
    }
}

PaintArea* MainWindow::paintArea() const
{
    return m_paintArea;
}

void MainWindow::on_actionFlip_horizontal_triggered()
{
    m_paintArea->transform(PaintArea::FLIP_HORIZONTAL);
}

void MainWindow::on_actionFlip_vertical_triggered()
{
    m_paintArea->transform(PaintArea::FLIP_VERTICAL);
}

void MainWindow::on_actionRotate_left_triggered()
{
    m_paintArea->transform(PaintArea::ROTATE_LEFT);
}

void MainWindow::on_actionRotate_right_triggered()
{
    m_paintArea->transform(PaintArea::ROTATE_RIGHT);
}

void MainWindow::on_actionLayers_triggered(bool checked)
{
    m_dock.setVisible(checked);
}

void MainWindow::on_actionSend_triggered()
{
    QImage img = m_paintArea->getImage();
    QString html;
    QImage opt;
    if(ImageUtil::optimizeSizeHtml(html, img, opt, -1, MAX_LOBBY_MSG_SIZE)){
        rsMsgs->sendChat(ChatId(chatId), html.toUtf8().constData());
    }
}