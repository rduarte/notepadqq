#include "include/notepadqq.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include <QCheckBox>
#include <QSettings>
#include <QDirIterator>

const QString Notepadqq::version = POINTVERSION;
const QString Notepadqq::contributorsUrl = "https://github.com/notepadqq/notepadqq/blob/master/CONTRIBUTORS.md";
const QString Notepadqq::website = "http://notepadqq.altervista.org";
bool Notepadqq::m_oldQt = false;
QList<Extension*> Notepadqq::m_extensions;

QString Notepadqq::copyright()
{
    return QObject::trUtf8("Copyright © 2010-2014, Daniele Di Sarli");
}

QString Notepadqq::editorPath()
{
    QString def = QString("%1/../appdata/editor/index.html").
            arg(qApp->applicationDirPath());

    if(!QFile(def).exists())
        def = QString("%1/../../share/%2/editor/index.html").
                arg(qApp->applicationDirPath()).
                arg(qApp->applicationName().toLower());

    return def;
}

QString Notepadqq::fileNameFromUrl(const QUrl &url)
{
    return QFileInfo(url.toDisplayString(
                         QUrl::RemoveScheme |
                         QUrl::RemovePassword |
                         QUrl::RemoveUserInfo |
                         QUrl::RemovePort |
                         QUrl::RemoveAuthority |
                         QUrl::RemoveQuery |
                         QUrl::RemoveFragment |
                         QUrl::PreferLocalFile )
                     ).fileName();
}

QCommandLineParser *Notepadqq::getCommandLineArgumentsParser(const QStringList &arguments)
{
    QCommandLineParser *parser = new QCommandLineParser();
    parser->setApplicationDescription("Text editor for developers");
    parser->addHelpOption();
    parser->addVersionOption();

    QCommandLineOption newWindowOption("new-window",
                                         QObject::tr("Open a new window in an existing instance of %1.")
                                         .arg(QCoreApplication::applicationName()));
    parser->addOption(newWindowOption);

    parser->addPositionalArgument("urls",
                                 QObject::tr("Files to open."),
                                 "[urls...]");

    parser->process(arguments);

    return parser;
}

bool Notepadqq::oldQt()
{
    return m_oldQt;
}

void Notepadqq::setOldQt(bool oldQt)
{
    m_oldQt = oldQt;
}

void Notepadqq::showQtVersionWarning(bool showCheckBox, QWidget *parent)
{
    QSettings settings;
    QString dir = QDir::toNativeSeparators(QDir::homePath() + "/Qt");
    QString altDir = "/opt/Qt";

    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(QCoreApplication::applicationName());
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("<h3>" + QObject::tr("You are using an old version of Qt (%1)").arg(qVersion()) + "</h3>");
    msgBox.setInformativeText("<html><body>"
        "<p>" + QObject::tr("Notepadqq will try to do its best, but <b>some things will not work properly</b>.") + "</p>" +
        QObject::tr(
            "Install a newer Qt version (&ge; %1) from the official repositories "
            "of your distribution.<br><br>"
            "If it's not available, download Qt (&ge; %1) from %2 and install it to \"%3\" or to \"%4\".").
                  arg("5.3").
                  arg("<nobr><a href=\"http://qt-project.org/\">http://qt-project.org/</a></nobr>").
                  arg("<nobr>" + dir + "</nobr>").
                  arg("<nobr>" + altDir + "</nobr>") +
        "</body></html>");

    QCheckBox *chkDontShowAgain;

    if (showCheckBox) {
        chkDontShowAgain = new QCheckBox();
        chkDontShowAgain->setText(QObject::tr("Don't show me this warning again"));
        msgBox.setCheckBox(chkDontShowAgain);
    }

    msgBox.exec();

    if (showCheckBox) {
        settings.setValue("checkQtVersionAtStartup", !chkDontShowAgain->isChecked());
        chkDontShowAgain->deleteLater();
    }
}

QString Notepadqq::extensionsPath()
{
    QSettings settings;

    QFileInfo f = QFileInfo(settings.fileName());
    return f.absoluteDir().absoluteFilePath("extensions");
}

void Notepadqq::loadExtensions()
{
    qRegisterMetaType<MainWindow*>("MainWindow*");
    qRegisterMetaType<Editor*>("Editor*");

    QDirIterator it(extensionsPath(), QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable, QDirIterator::NoIteratorFlags);
    while (it.hasNext()) {
        QString ext = it.next();
        m_extensions.append(new Extension(ext));
    }
}
