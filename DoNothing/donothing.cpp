#include "donothing.h"

#include <QtPlugin>
#include <QStringList>
#include <QMessageBox>
#include <qdebug.h>

#include <coreplugin/coreconstants.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/icore.h>

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#include <QUiLoader>
#include <QSettings>
#include <QXmlStreamReader>
#include "settingsdialog.h"

DoNothingPlugin * DoNothingPlugin::createdInstance;

DoNothingPlugin::DoNothingPlugin() :
        mime_type("application/x-designer")
{
    // Do nothing
    createdInstance = this;
    fm = Core::ICore::instance()->fileManager();
    timer.start();
}

DoNothingPlugin::~DoNothingPlugin()
{
    // Do notning
}

bool DoNothingPlugin::initialize(const QStringList& args, QString *errMsg)
{
    Q_UNUSED(args);
    Q_UNUSED(errMsg);

    //print_trace();

    createMenuItems();
    connect(fm, SIGNAL(currentFileChanged(QString)), this, SLOT(changeWatchedFile(QString)));
    connect(&fsw, SIGNAL(fileChanged(QString)), this, SLOT(handleFileChange(QString)));

    return true;
}

void DoNothingPlugin::extensionsInitialized()
{
    // Do nothing
}

void DoNothingPlugin::nothingChanged(QString str, QVariant val)
{
    qDebug() << "nothingChanged";
    qDebug() << str << val;
}

void DoNothingPlugin::changeWatchedFile(QString fileName)
{
    if (!oldFileName.isEmpty())
        fsw.removePath(oldFileName);

    if (!fsw.files().contains(fileName))
        fsw.addPath(fileName);
    oldFileName = fileName;
}

void DoNothingPlugin::shutdown()
{
    // Do nothing
}

void DoNothingPlugin::foo()
{
    qDebug() << "foo::Donothing";
}

void DoNothingPlugin::about()
{
    QMessageBox::information(reinterpret_cast<QWidget *>(Core::ICore::instance()->mainWindow()),
                             "About Bilkon Plugin",
                             "Seriously dude, this plugin does nothing");
}

void DoNothingPlugin::sendUi()
{
    qDebug() << "Send ui";
    printModifiedFiles();

    parseResource("/home/cihangir/newmy.qrc");
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
    if (timer.elapsed() < 50)
        return;
    timer.restart();
    qDebug() << "Following file changed" << path;

    QWidget *widget = load(path);

    if (!checkNames(widget))
        QMessageBox::warning(reinterpret_cast<QWidget *>(Core::ICore::instance()->mainWindow()),
                                                         "Error",
                                                         "Check widget names!");

    delete widget;
}

void DoNothingPlugin::settings()
{
    QSettings set("Bilkon", "DoNothing");
    settingsDialog d(reinterpret_cast<QWidget *>(Core::ICore::instance()->mainWindow()));
    d.setIpAddress(set.value("ipAddress").toString());
    d.setPortNumber(set.value("portNumber").toString());

    if (d.exec()) {
        QString ipAddress = d.ipAddress();
        QString portNumber = d.portNumber();

        if (!ipAddress.isEmpty() && !portNumber.isEmpty()) {
            set.setValue("ipAddress", ipAddress);
            set.setValue("portNumber", portNumber);
        }
    }
}

void DoNothingPlugin::createMenuItems()
{
    Core::ActionManager* am = Core::ICore::instance()->actionManager();
    Core::ActionContainer *ac = am->createMenu("BilkonPlugin.BilkonMenu");
    ac->menu()->setTitle("Bilko&n");

    Core::Command *cmd = am->registerAction(new QAction(this),
                                            "DoNothingPlugin.DoNothingMenu",
                                            QList<int>() << 0);
    cmd->action()->setText("About Bilkon Plugin");
    am->actionContainer(Core::Constants::MENU_BAR)->addMenu(ac);
    ac->addAction(cmd);
    connect(cmd->action(), SIGNAL(triggered(bool)), this, SLOT(about()));

    QAction *sendUiAction = ac->menu()->addAction("Send UI");
    connect(sendUiAction, SIGNAL(triggered(bool)), this, SLOT(sendUi()));

    QAction *settingsAction = ac->menu()->addAction("Settings");
    connect(settingsAction, SIGNAL(triggered(bool)), this, SLOT(settings()));
}

bool DoNothingPlugin::isValid(const QString & objName) const
{
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
    file.open(QFile::ReadOnly);

    return loader.load(&file, NULL);
}

bool DoNothingPlugin::checkNames(const QWidget *widget) const
{

    if (widget == NULL)
        return false;

    qDebug() << "checkNames" << widget;
    QList<QWidget *> list = widget->findChildren<QWidget *>();
    qDebug() << "checkNames" << list.size() << list.first()->objectName();

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

void
print_trace (void)
{
  void *array[10];
  size_t size;
  char **strings;
  size_t i;

  size = backtrace (array, 10);
  strings = backtrace_symbols (array, size);

  printf ("Obtained %zd stack frames.\n", size);

  for (i = 0; i < size; i++)
     printf ("%s\n", strings[i]);

  free (strings);
}


Q_EXPORT_PLUGIN(DoNothingPlugin)
