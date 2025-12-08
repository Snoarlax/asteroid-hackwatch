#include "CommandRunner.h"
#include <QDir>
#include <QDebug>

CommandRunner::CommandRunner(QObject *parent)
    : QObject(parent)
    , m_output("")
    , m_isRunning(false)
    , m_process(nullptr)
{
    refreshScripts();
    refreshKeyboardScripts();
    qDebug() << "kscripts: " << kscripts();
}

QString CommandRunner::output() const
{
    return m_output;
}

QStringList CommandRunner::scripts() const
{
    return m_scripts;
}

QStringList CommandRunner::kscripts() const
{
    return m_kscripts;
}

bool CommandRunner::isRunning() const
{
    return m_isRunning;
}




void CommandRunner::setOutput(const QString &output)
{
    if (m_output != output) {
        m_output = output;
        emit outputChanged();
    }
}

void CommandRunner::setScripts(const QStringList &scripts)
{
    if (m_scripts != scripts) {
        m_scripts = scripts;
        emit scriptsChanged();
    }
}

void CommandRunner::setKeyboardScripts(const QStringList &scripts)
{
    if (m_kscripts != scripts) {
        m_kscripts = scripts;
        emit kScriptsChanged();
    }
}

void CommandRunner::setIsRunning(bool running)
{
    if (m_isRunning != running) {
        m_isRunning = running;
        emit isRunningChanged();
    }
}

void CommandRunner::refreshScripts()
{
    QString scriptsPath = QDir::homePath() + "/hack_scripts";
    QDir scriptsDir(scriptsPath);

    if (!scriptsDir.exists()) {
        qWarning() << "Scripts directory does not exist:" << scriptsPath;
        setScripts(QStringList());
        return;
    }

    // Get all .sh files
    QStringList filters;
    filters << "*.sh";
    QStringList scriptFiles = scriptsDir.entryList(filters, QDir::Files, QDir::Name);

    qDebug() << "Found scripts:" << scriptFiles;
    setScripts(scriptFiles);
}

void CommandRunner::refreshKeyboardScripts()
{
    QString scriptsPath = QDir::homePath() + "/keyboard_scripts";
    QDir keyboardScriptsDir(scriptsPath);

    if (!keyboardScriptsDir.exists()) {
        qWarning() << "Scripts directory does not exist:" << scriptsPath;
        setKeyboardScripts(QStringList());
        return;
    }

    // Get all files
    QStringList filters;
    filters << "*";
    QStringList keyboardScriptFiles = keyboardScriptsDir.entryList(filters, QDir::Files, QDir::Name);

    qDebug() << "Found keyboard scripts:" << keyboardScriptFiles;
    setKeyboardScripts(keyboardScriptFiles);
}

void CommandRunner::runScript(const QString &scriptName)
{
    if (m_isRunning) {
        qWarning() << "Already running a script";
        return;
    }

    if (scriptName.isEmpty()) {
        setOutput("Error: No script specified");
        emit commandFinished(false);
        return;
    }

    QString scriptPath = QDir::homePath() + "/hack_scripts/" + scriptName;

    // Clean up old process
    if (m_process) {
        m_process->deleteLater();
    }

    m_process = new QProcess(this);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &CommandRunner::onProcessFinished);

    setIsRunning(true);
    setOutput("Running " + scriptName + "...");

    // Run the script
    m_process->start("/bin/sh", QStringList() << scriptPath);

    if (!m_process->waitForStarted(1000)) {
        setOutput("Error: Failed to start script");
        setIsRunning(false);
        emit commandFinished(false);
    }
}

void CommandRunner::runKeyboardScript(const QString &scriptName)
{
    if (m_isRunning) {
        qWarning() << "Already running a keyboard script";
        return;
    }

    if (scriptName.isEmpty()) {
        setOutput("Error: No keyboard script specified");
        emit commandFinished(false);
        return;
    }

    QString scriptPath = QDir::homePath() + "/keyboard_scripts/" + scriptName;
    QString deviceInterfacePath = QDir::homePath() + "/utilities/keyboard_interface";
    QString runasRootPath = QDir::homePath() + "/utilities/runas";

    // Clean up old process
    if (m_process) {
        m_process->deleteLater();
    }

    m_process = new QProcess(this);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &CommandRunner::onProcessFinished);

    setIsRunning(true);
    setOutput("Running " + scriptName + "...");

    // Run the keyboard script
    m_process->start(runasRootPath, QStringList() << deviceInterfacePath << scriptPath);

    if (!m_process->waitForStarted(1000)) {
        setOutput("Error: Failed to start script");
        setIsRunning(false);
        emit commandFinished(false);
    }
}

void CommandRunner::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (!m_process) {
        return;
    }

    QString output = m_process->readAllStandardOutput().trimmed();
    QString errorOutput = m_process->readAllStandardError().trimmed();

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        setOutput(output.isEmpty() ? "Success (no output)" : output);
        emit commandFinished(true);
    } else {
        QString errorMsg = errorOutput.isEmpty() ?
            QString("Exit code: %1").arg(exitCode) : errorOutput;
        setOutput("Error: " + errorMsg);
        emit commandFinished(false);
    }

    setIsRunning(false);
}
