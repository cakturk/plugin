#ifndef DONOTHING_H
#define DONOTHING_H

#include <extensionsystem/iplugin.h>

class DoNothingPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    DoNothingPlugin();
    ~DoNothingPlugin();
    void extensionsInitialized();
    bool initialize(const QStringList & arguments, QString * errorString);
    void shutdown();

private slots:
    void about();
    void sendUi();

private:
    void createMenuItems();
};

#endif // DONOTHING_H
