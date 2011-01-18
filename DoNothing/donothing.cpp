#include "donothing.h"

#include <QtPlugin>
#include <QStringList>
#include <QMessageBox>
#include <qdebug.h>

#include <coreplugin/coreconstants.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/icore.h>


DoNothingPlugin::DoNothingPlugin()
{
    // Do nothing
}

DoNothingPlugin::~DoNothingPlugin()
{
    // Do notning
}

bool DoNothingPlugin::initialize(const QStringList& args, QString *errMsg)
{
    Q_UNUSED(args);
    Q_UNUSED(errMsg);

     //Fetch the action manager
//    Core::ActionManager* am = Core::ICore::instance()->actionManager();
//    Core::ActionContainer *ac = am->createMenu("BilkonPlugin.BilkonMenu");
//    ac->menu()->setTitle("Bilkon");

//    Core::Command *cmd = am->registerAction(new QAction(this),
//                                            "DoNothingPlugin.DoNothingMenu",
//                                            QList<int>() << 0);
//    cmd->action()->setText("About Bilkon Plugin");
//    am->actionContainer(Core::Constants::MENU_BAR)->addMenu(ac);
//    ac->addAction(cmd);
//    connect(cmd->action(), SIGNAL(triggered(bool)), this, SLOT(about()));

//    QAction *action = ac->menu()->addAction("Send UI");
//    connect(action, SIGNAL(triggered(bool)), this, SLOT(sendUi()));

    createMenuItems();

    return true;
}

void DoNothingPlugin::extensionsInitialized()
{
    // Do nothing
}

void DoNothingPlugin::shutdown()
{
    // Do nothing
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
}

void DoNothingPlugin::createMenuItems()
{
//    Core::ActionManager *am = Core::ICore::instance()->actionManager();
//    Core::ActionContainer *ac = am->createMenu("BilkonPlugin.BilkonMenu");
//    ac->menu()->setTitle("Bilkon");

//    QAction *sendAction = ac->menu()->addAction("Send Ui");
//    QAction *aboutAction = ac->menu()->addAction("About Bilkon Plugin");

//    connect(sendAction, SIGNAL(triggered(bool)), this, SLOT(sendUi()));
//    connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(about()));
//    am->actionContainer(Core::Constants::MENU_BAR)->addMenu(ac);

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

    QAction *action = ac->menu()->addAction("Send UI");
    connect(action, SIGNAL(triggered(bool)), this, SLOT(sendUi()));
}

Q_EXPORT_PLUGIN(DoNothingPlugin)
