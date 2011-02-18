#include "donothing.h"

#include <QtPlugin>
#include <QStringList>
#include <QMessageBox>
#include <qdebug.h>

#include <coreplugin/coreconstants.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/icore.h>

#include <QUiLoader>
#include <QSettings>
#include <QXmlStreamReader>
#include <QDir>
#include <QMainWindow>
#include <QFileDialog>
#include <QProgressBar>
#include <QAbstractButton>
#include <QSpinBox>
#include <QLineEdit>
#include "uibinderdialog.h"
#include "settingsdialog.h"

DoNothingPlugin::DoNothingPlugin() :
        mime_type("application/x-designer"),
        connected(false),
        blocksize(0),
        portNumber(33666),
        artDirectory("art"),
        progressBar(new QProgressBar(static_cast<QWidget *>(Core::ICore::instance()->mainWindow()))),
        uiBinder(new uiBinderDialog(static_cast<QWidget *>(Core::ICore::instance()->mainWindow()))),
        widget(0)
{
    // Do nothing
    fm = Core::ICore::instance()->fileManager();
    timer.start();
    
    progressBar->resize(250, 10);
    progressBar->hide();
    centerWidget(progressBar);
    
    uiBinder->hide();
}

DoNothingPlugin::~DoNothingPlugin()
{
    // Do notning
}

bool DoNothingPlugin::initialize(const QStringList& args, QString *errMsg)
{
    Q_UNUSED(args);
    Q_UNUSED(errMsg);
    
    createMenuItems();
    connect(fm, SIGNAL(currentFileChanged(QString)), this, SLOT(changeWatchedFile(QString)));
    connect(&uiWatcher, SIGNAL(fileChanged(QString)), this, SLOT(handleFileChange(QString)));
    connect(&imageWatcher, SIGNAL(fileChanged(QString)), this, SLOT(insertFile(QString)));
    
    connect(&socket, SIGNAL(connected()), this, SLOT(connectedSlot()));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(disconnectedSlot()));
    connect(&socket, SIGNAL(readyRead()), this, SLOT(readMessage()));
    
    connect(uiBinder, SIGNAL(applyClicked()), this, SLOT(applyClickedSlot()));
    
    QSettings set("Bilkon", "DoNothing");
    QString ipAddress = set.value("ipAddress").toString();
    
    if (!ipAddress.isEmpty() && portNumber != 0)
        socket.connectToHost(ipAddress, portNumber);
    
    return true;
}

void DoNothingPlugin::extensionsInitialized()
{
    // Do nothing
}

void DoNothingPlugin::changeWatchedFile(QString fileName)
{
    if (!oldFileName.isEmpty())
        uiWatcher.removePath(oldFileName);
    
    if (!uiWatcher.files().contains(fileName))
        uiWatcher.addPath(fileName);
    oldFileName = fileName;
}

void DoNothingPlugin::readMessage()
{
    qDebug() << "DoNothingPlugin::readMessage";
    
    again:
    
    int messageType;
    QDataStream in(&socket);
    in.setVersion(QDataStream::Qt_4_6);
    
    if (blocksize == 0) {
        if (socket.bytesAvailable() < sizeof(quint64))
            return;
        
        in >> blocksize;
    }
    
    if (socket.bytesAvailable() < blocksize)
        return;
    
    in >> messageType;
    
    QString fileName, commandString;
    
    switch (messageType) {
    case DoNothingPlugin::FILE:
        ba.clear();
        in >> fileName;
        in >> ba;
        //writeToFile(fileName, ba);
        break;
    case DoNothingPlugin::MAP:
        break;
    case DoNothingPlugin::COMMAND:
        in >> commandString;
        if (commandString == "transfer_completed")
            setProgressBarValue();
        break;
    case DoNothingPlugin::FILE_LIST:
        qDebug() << "File list received from server";
        filesOnServer.clear();
        in >> filesOnServer;
        qDebug() << "List" << filesOnServer;
        break;
    default:
        qDebug() << "Unknown data format!";
    }
    
    blocksize = 0;
    
    if (socket.bytesAvailable())
        goto again;
}

void DoNothingPlugin::shutdown()
{
    // Do nothing
    socket.disconnectFromHost();
    delete progressBar;
}

void DoNothingPlugin::about()
{
    QMessageBox::information(static_cast<QWidget *>(Core::ICore::instance()->mainWindow()),
                             "About Bilkon Plugin v0.1",
                             "Bilkon UI Designer Plugin\n"
                             "Version 0.1");
}

void DoNothingPlugin::printModifiedFiles()
{
    QList<Core::IFile *> list = modifiedFiles();
    qDebug() << "size of list:" << list.size();
    foreach (Core::IFile *file, list) {
        qDebug() << file->fileName() << file->mimeType();
    }
}

void DoNothingPlugin::handleFileChange(const QString & path)
{
    if (timer.elapsed() < 150)
        return;
    timer.restart();
    qDebug() << "Following file changed" << path;
    
    if (widget != NULL)
        delete widget;
    
    widget = load(path);
    
    if (!checkNames(widget)) {
        QMessageBox::warning(static_cast<QWidget *>(Core::ICore::instance()->mainWindow()),
                             "Error",
                             "Check widget names!");
        return;
    }
    
    QFile file(path);
    file.open(QFile::ReadOnly);
    QByteArray array = file.readAll();
    QFileInfo fileInfo(path);
    sendMessage(fileInfo.fileName(), array);
    qDebug() << "File size" << array.size();
    
    QDir dir(fileInfo.absolutePath() + "/" + artDirectory);
    
    foreach (QFileInfo fileName, dir.entryInfoList()) {
        if (fileName.suffix().compare("png", Qt::CaseInsensitive) == 0 ||
            fileName.suffix().compare("jpg", Qt::CaseInsensitive) == 0 ||
            fileName.suffix().compare("jpeg", Qt::CaseInsensitive) == 0) {
            
            QString absoluteFilePath = fileName.absoluteFilePath();
            if (!imageWatcher.files().contains(absoluteFilePath)) {
                imageWatcher.addPath(absoluteFilePath);
                if (!filesOnServer.contains(fileName.fileName()))
                    imagesToSend.append(absoluteFilePath);
                qDebug() << "imagewatcher" << fileName.fileName();
            } else if (!filesOnServer.contains(fileName.fileName())) {
                filesOnServer.append(fileName.fileName());
                imagesToSend.append(absoluteFilePath);
            }
        }
    }
    
    if (imagesToSend.size()) {
        progressBar->setMaximum(imagesToSend.size());
        progressBar->show();
        centerWidget(progressBar);
    }
    
    // Send images to server.
    foreach (QString fileName, imagesToSend) {
        fileInfo.setFile(fileName);
        if (fileInfo.exists()) {
            QFile imFile(fileName);
            imFile.open(QFile::ReadOnly);
            array = imFile.readAll();
            
            sendMessage(fileInfo.fileName(), array);
        }
    }
    imagesToSend.clear();
}

void DoNothingPlugin::insertFile(const QString & path)
{
    if (!imagesToSend.contains(path)) {
        imagesToSend.append(path);
        qDebug() << "inserted file" << path;
    }
}

void DoNothingPlugin::settings()
{
    QSettings set("Bilkon", "DoNothing");
    settingsDialog dialog(static_cast<QWidget *>(Core::ICore::instance()->mainWindow()));
    dialog.setIpAddress(set.value("ipAddress").toString());
    qDebug() << "Connected:" << connected;
    dialog.setStatus(connected ? "Connected" : "Disconnected");
    
    if (dialog.exec()) {
        QString ipAddress = dialog.ipAddress();
        
        if (!ipAddress.isEmpty()) {
            set.setValue("ipAddress", ipAddress);
            
            if (!connected)
                socket.connectToHost(ipAddress, portNumber);
        }
    }
}

void DoNothingPlugin::sendAllFiles()
{
    QFileInfo fileInfo(fm->currentFile());
    
    if (!fileInfo.exists())
        return;
    
    QFile file(fileInfo.absoluteFilePath());
    if (!file.open(QFile::ReadOnly))
        return;
    
    QByteArray array = file.readAll();
    sendMessage(fileInfo.fileName(), array);
    
    QDir dir(fileInfo.absolutePath() + "/" + artDirectory);
    QStringList list;
    
    foreach (QFileInfo fileName, dir.entryInfoList()) {
        if (fileName.suffix().compare("png", Qt::CaseInsensitive) == 0 ||
            fileName.suffix().compare("jpg", Qt::CaseInsensitive) == 0 ||
            fileName.suffix().compare("jpeg", Qt::CaseInsensitive) == 0) {
            
            if (!filesOnServer.contains(fileName.fileName())) {
                filesOnServer.append(fileName.fileName());
                list.append(fileName.absoluteFilePath());
            }
        }
    }
    
    if (list.size()) {
        progressBar->setMaximum(list.size());
        progressBar->show();
        centerWidget(progressBar);
    }
    
    foreach (QString filePath, list) {
        QFileInfo fileInfo(filePath);
        QFile imageFile(filePath);
        imageFile.open(QFile::ReadOnly);
        array = imageFile.readAll();
        sendMessage(fileInfo.fileName(), array);
    }
}

void DoNothingPlugin::upgrade()
{
    QString fileName = QFileDialog::getOpenFileName(0);
    qDebug() << "Recently opened file" << fileName;
    
    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists())
        return;
    
    QFile file(fileInfo.absoluteFilePath());
    if(!file.open(QFile::ReadOnly))
        return;
    
    QByteArray array = file.readAll();
    sendMessage(fileInfo.fileName(), array, DoNothingPlugin::UPGRADE);
}

void DoNothingPlugin::deleteAllFiles()
{
    sendMessage("delete_all_files");
    filesOnServer.clear();
}

void DoNothingPlugin::applyClickedSlot()
{
    qDebug() << "applyClickedSlot";
}

void DoNothingPlugin::showActionManager()
{
    if (widget != NULL)
        delete widget;

    if((widget = load(fm->currentFile())) == NULL)
        return;

    QList<QAbstractButton *> buttonList = widget->findChildren<QAbstractButton *>();
    QList<QSpinBox *> spinList = widget->findChildren<QSpinBox *>();
    QList<QLineEdit *> lineEditList = widget->findChildren<QLineEdit *>();
    QStringList buttonNames, parameterObjNames;
    
    for (int i = 0; i < buttonList.size(); ++i)
        buttonNames.append(buttonList[i]->objectName());
    for (int i = 0; i < spinList.size(); ++i)
        parameterObjNames.append(spinList[i]->objectName());
    for (int i = 0; i < lineEditList.size(); ++i)
        parameterObjNames.append(lineEditList[i]->objectName());

    uiBinder->setClassMap(classMap);
    uiBinder->setButtonNames(buttonNames);
    uiBinder->setFunctionParams(parameterObjNames);
    uiBinder->exec();
}

void DoNothingPlugin::connectedSlot()
{
    qDebug() << "Connected";
    connected = true;
}

void DoNothingPlugin::disconnectedSlot()
{
    qDebug() << "Disconnected";
    connected = false;
}

void DoNothingPlugin::createMenuItems()
{
    Core::ActionManager* am = Core::ICore::instance()->actionManager();
    Core::ActionContainer *ac = am->createMenu("BilkonPlugin.BilkonMenu");
    ac->menu()->setTitle("Bilko&n");
    
    Core::Command *cmd = am->registerAction(new QAction(this),
                                            "DoNothingPlugin.DoNothingMenu",
                                            QList<int>() << 0);
    cmd->action()->setText("&About Bilkon Plugin");
    am->actionContainer(Core::Constants::MENU_BAR)->addMenu(ac);
    ac->addAction(cmd);
    connect(cmd->action(), SIGNAL(triggered(bool)), this, SLOT(about()));
    
    QAction *settingsAction = ac->menu()->addAction("&Settings");
    connect(settingsAction, SIGNAL(triggered(bool)), this, SLOT(settings()));
    
    QAction *sendFilesAction = ac->menu()->addAction("Send All &Files");
    connect(sendFilesAction, SIGNAL(triggered(bool)), this, SLOT(sendAllFiles()));
    
    QAction *upgrade = ac->menu()->addAction("&Upgrade");
    connect(upgrade, SIGNAL(triggered(bool)), this, SLOT(upgrade()));
    
    QAction *deleteAllFiles = ac->menu()->addAction("&Delete All Files");
    connect(deleteAllFiles, SIGNAL(triggered(bool)), this, SLOT(deleteAllFiles()));
    
    QAction *actionManager= ac->menu()->addAction("Show Action Manager");
    connect(actionManager, SIGNAL(triggered(bool)), this, SLOT(showActionManager()));
}

bool DoNothingPlugin::isValid(const QString & objName) const
{
    return true;
    
    QStringList list = objName.split("_");
    if (list.size() >= 1)
        return false;
    
    if (classMap.contains(list[0])) {
        if (!classMap.value(list[0]).contains(list[1]))
            return false;
    }
    
    return true;
}

QList<Core::IFile *> DoNothingPlugin::modifiedFiles() const
{
    QList<Core::IFile *> list;
    //fm = Core::ICore::instance()->fileManager();
    
    foreach (Core::IFile *uiFile, fm->modifiedFiles()) {
        if (uiFile->mimeType() == mime_type)
            list.append(uiFile);
    }
    
    return list;
}

QWidget* DoNothingPlugin::load(const QString & fileName)
{
    QUiLoader loader;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return NULL;
    return loader.load(&file, NULL);
}

bool DoNothingPlugin::checkNames(const QWidget *widget) const
{
    
    if (widget == NULL)
        return false;
    
    QList<QWidget *> list = widget->findChildren<QWidget *>();
    foreach (QWidget *w, list) {
        qDebug() << w->objectName();
        if (!isValid(w->objectName()))
            return false;
    }
    
    return true;
}

QStringList DoNothingPlugin::parseResource(const QString &fileName) const
{
    QStringList resourceMap;
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly))
        return resourceMap;
    
    
    QByteArray ba = file.readAll();
    qDebug() << ba;
    QXmlStreamReader xmlReader(ba);
    qDebug() << "DoNothingPlugin::parseResource" << fileName;
    while (!xmlReader.atEnd()) {
        QXmlStreamReader::TokenType type = xmlReader.readNext();
        if (type == QXmlStreamReader::StartElement) {
            qDebug() << xmlReader.readElementText() << xmlReader.name();
        }
    }
    
    if (xmlReader.hasError())
        qDebug() << "An error occurred" << xmlReader.errorString();
    
    return resourceMap;
}

void DoNothingPlugin::sendMessage(const QString & fileName, const QByteArray & data, DoNothingPlugin::MessageType mType)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    
    out << (quint32) 0;
    out << (quint32) mType;
    out << fileName;
    out << data;
    
    out.device()->seek(0);
    out << (quint32) (block.size() - sizeof(quint32));
    
    socket.write(block);
}

void DoNothingPlugin::sendMessage(const QString & string)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    
    out << (quint32) 0;
    out << (quint32) DoNothingPlugin::COMMAND;
    out << string;
    
    out.device()->seek(0);
    out << (quint32) (block.size() - sizeof(quint32));
    
    socket.write(block);
}

void DoNothingPlugin::sendMessage(const QMap<QString, QStringList> & classMap)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    
    out << (quint32) 0;
    out << (quint32) DoNothingPlugin::MAP;
    out << classMap;
    
    out.device()->seek(0);
    out << (quint32) (block.size() - sizeof(quint32));
    
    socket.write(block);
}

/**
  * Sends all the files in a directory.
  */
void DoNothingPlugin::sendDirectory(const QString & path)
{
    QDir dir(path);
    qDebug() << "Current dir:" << path;
    
    QByteArray array;
    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    
    foreach (QFileInfo fileInfo, list) {
        qDebug() << "Filename:" << fileInfo.fileName();
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QFile::ReadOnly))
            continue;
        
        array = file.readAll();
        qDebug() << "a file in dir =" << array.size();
        sendMessage(fileInfo.fileName(), array);
    }
}

void DoNothingPlugin::centerWidget(QWidget *widget) const
{
    int screenWidth, width;
    int screenHeight, height;
    int x, y;
    QSize widgetSize;
    
    screenWidth = widget->parentWidget()->width();
    screenHeight = widget->parentWidget()->height();
    
    widgetSize = widget->size();
    width = widgetSize.width();
    height = widgetSize.height();
    
    x = (screenWidth - width) / 2;
    y = (screenHeight - height) / 2;
    
    widget->move(x, y);
}

void DoNothingPlugin::setProgressBarValue()
{
    static int counter = 0;
    progressBar->setValue(++counter);
    
    if (counter == progressBar->maximum()) {
        progressBar->hide();
        progressBar->reset();
        counter = 0;
    }
}

Q_EXPORT_PLUGIN(DoNothingPlugin)
