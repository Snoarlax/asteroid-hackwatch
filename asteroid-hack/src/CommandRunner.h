#ifndef COMMANDRUNNER_H
#define COMMANDRUNNER_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>

class CommandRunner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString output READ output NOTIFY outputChanged)
    Q_PROPERTY(QStringList scripts READ scripts NOTIFY scriptsChanged)
    Q_PROPERTY(QStringList kscripts READ kscripts NOTIFY kScriptsChanged)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)

public:
    explicit CommandRunner(QObject *parent = nullptr);

    QString output() const;
    QStringList scripts() const;
    QStringList kscripts() const;
    bool isRunning() const;

    Q_INVOKABLE void runScript(const QString &scriptName);
    Q_INVOKABLE void runKeyboardScript(const QString &keyboardScriptName);
    Q_INVOKABLE void refreshScripts();
    Q_INVOKABLE void refreshKeyboardScripts();

signals:
    void outputChanged();
    void kScriptsChanged();
    void scriptsChanged();
    void isRunningChanged();
    void commandFinished(bool success);

private:
    void setOutput(const QString &output);
    void setScripts(const QStringList &scripts);
    void setKeyboardScripts(const QStringList &scripts);
    void setIsRunning(bool running);

    QString m_output;
    QStringList m_scripts;
    QStringList m_kscripts;
    bool m_isRunning;
    QProcess *m_process;

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // COMMANDRUNNER_H
