#ifndef DONOTHING_H
#define DONOTHING_H

#include <extensionsystem/iplugin.h>
#include <coreplugin/ifile.h>
#include <coreplugin/filemanager.h>

#include <QVariant>
#include <QFileSystemWatcher>

class /*__attribute__ ((visibility("default")))*/ DoNothingPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    DoNothingPlugin();
    ~DoNothingPlugin();
    void extensionsInitialized();
    bool initialize(const QStringList & arguments, QString * errorString);
    void shutdown();
    //static DoNothingPlugin * instance() { return created; }

    void foo();

    static DoNothingPlugin *createdInstance;

private slots:
    void about();
    void sendUi();
    void printModifiedFiles();
    void handleFileChange(const QString &);

public slots:
    void nothingChanged(QString, QVariant);
    void changeWatchedFile(QString);

private:
    void createMenuItems();
    bool isValid(const QString objName) const;
    QList<Core::IFile *> modifiedFiles() const;
    QWidget* load(const QString);
    bool checkNames(const QWidget*) const;

    const QString mime_type;
    QFileSystemWatcher fsw;
    QString oldFileName;
    Core::FileManager *fm;
};

void print_trace (void);

#endif // DONOTHING_H
