#ifndef EXTENSION_H
#define EXTENSION_H

#include <QObject>
#include <QScriptEngine>

class Extension : public QObject
{
    Q_OBJECT
public:
    explicit Extension(QString path, QObject *parent = 0);
    ~Extension();

signals:

public slots:

private:
    QScriptEngine *m_uiScriptEngine;
    void failedToLoadMessage(QString path);
};

#endif // EXTENSION_H
