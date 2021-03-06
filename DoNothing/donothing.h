#ifndef DONOTHING_H
#define DONOTHING_H

#include <extensionsystem/iplugin.h>
#include <coreplugin/ifile.h>
#include <coreplugin/filemanager.h>

#include <QVariant>
#include <QFileSystemWatcher>
#include <QtNetwork/QTcpSocket>

typedef QPair<QString, QPair<QString, QStringList> > ActionData;

class QProgressBar;
class uiBinderDialog;
class /*__attribute__ ((visibility("default")))*/ DoNothingPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    DoNothingPlugin();
    ~DoNothingPlugin();

    enum MessageType {
        FILE = 0,
        MAP = 1,
        COMMAND = 2,
        FILE_LIST = 3,
        UPGRADE = 4
    };

    void extensionsInitialized();
    bool initialize(const QStringList & arguments, QString * errorString);
    void shutdown();
    //static DoNothingPlugin * instance() { return created; }

private slots:
    void about();
    void printModifiedFiles();
    void handleFileChange(const QString &);
    void insertFile(const QString &);
    void settings();

    void connectedSlot();
    void disconnectedSlot();

    void changeWatchedFile(QString);
    void readMessage();
    void sendAllFiles();
    void upgrade();
    void deleteAllFiles();
    void applyClickedSlot();
    void showActionManager();

private:
    void createMenuItems();
    bool isValid(const QString & objName) const;
    QList<Core::IFile *> modifiedFiles() const;
    QWidget* load(const QString &);
    bool checkNames(const QWidget*) const;
    QStringList parseResource(const QString & fileName) const;
    void sendMessage(const QString &, const QByteArray &, DoNothingPlugin::MessageType mType = DoNothingPlugin::FILE);
    void sendMessage(const QString & string);
    void sendMessage(const QMap<QString, QStringList> & classMap);
    void sendDirectory(const QString &);
    void centerWidget(QWidget *widget) const;
    void setProgressBarValue();

    const QString mime_type;
    QFileSystemWatcher uiWatcher;
    QFileSystemWatcher imageWatcher;
    QString oldFileName;
    Core::FileManager *fm;
    QMap<QString, QStringList> classMap;
    QMap<QString, ActionData> actionMap;
    QTime timer;
    QTcpSocket socket;
    bool connected;
    quint32 blocksize;
    quint16 portNumber;
    const QString artDirectory;
    QByteArray ba;
    QStringList filesOnServer;
    QStringList imagesToSend;
    QProgressBar *progressBar;
    uiBinderDialog *uiBinder;
    QWidget *widget;
};

#endif // DONOTHING_H
