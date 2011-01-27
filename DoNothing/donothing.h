#ifndef DONOTHING_H
#define DONOTHING_H

#include <extensionsystem/iplugin.h>
#include <coreplugin/ifile.h>
#include <coreplugin/filemanager.h>

#include <QVariant>
#include <QFileSystemWatcher>
#include <QtNetwork/QTcpSocket>

class /*__attribute__ ((visibility("default")))*/ DoNothingPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    DoNothingPlugin();
    ~DoNothingPlugin();

    enum MessageType {
        FILE = 0,
        MAP = 1,
        ERROR = 2,
        FILE_LIST = 3
    };

    void extensionsInitialized();
    bool initialize(const QStringList & arguments, QString * errorString);
    void shutdown();
    //static DoNothingPlugin * instance() { return created; }

    void foo();

private slots:
    void about();
    void sendUi();
    void printModifiedFiles();
    void handleFileChange(const QString &);
    void settings();

    void showConnected();
    void disconnectedSlot();

public slots:
    void nothingChanged(QString, QVariant);
    void changeWatchedFile(QString);

private:
    void createMenuItems();
    bool isValid(const QString & objName) const;
    QList<Core::IFile *> modifiedFiles() const;
    QWidget* load(const QString &);
    bool checkNames(const QWidget*) const;
    QStringList parseResource(const QString & fileName) const;
    void sendMessage(const QString &, const QByteArray &);
    void sendMessage(const QString & string);
    void sendMessage(const QMap<QString, QStringList> & classMap);
    void sendImages(const QString &);

    const QString mime_type;
    QFileSystemWatcher watcher;
    QString oldFileName;
    Core::FileManager *fm;
    QMap<QString, QStringList> classMap;
    QTime timer;
    QTcpSocket socket;
    bool connected;
};

void print_trace (void);

#endif // DONOTHING_H
