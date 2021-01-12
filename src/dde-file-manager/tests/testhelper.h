#ifndef TESTHELPER_H
#define TESTHELPER_H

#include <QProcess>
#include <QStandardPaths>
#include <QDateTime>
#include <QUrl>
#include <QFile>
#include <QEventLoop>
#include <QTimer>

class TestHelper
{
public:
    TestHelper();

    static QString createTmpFile(QString suffix = "", QStandardPaths::StandardLocation location = QStandardPaths::TempLocation) {
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        int random = qrand()%1000;
        QString fileName = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()) +  QString::number(random) + suffix;
        QString tempFilePath =  QStandardPaths::standardLocations(location).first() + "/" + fileName;
        QProcess::execute("touch " + tempFilePath);
        return tempFilePath;
    }

    static QString createTmpDir() {
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        int random = qrand()%1000;
        QString dirName = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()) + QString::number(random);
        QString tempFilePath =  QStandardPaths::standardLocations(QStandardPaths::TempLocation).first() + "/" + dirName;
        QProcess::execute("mkdir " + tempFilePath);
        return tempFilePath;
    }

    static QString createTmpDir(QString dirName) {
        QString tempFilePath =  QStandardPaths::standardLocations(QStandardPaths::TempLocation).first() + "/" + dirName;
        QProcess::execute("mkdir " + tempFilePath);
        return tempFilePath;
    }

    static void deleteTmpFiles(QStringList paths) {
        for(QString path : paths) {
            deleteTmpFile(path);
        }
    }
    static void deleteTmpFile(QString path) {
         QProcess::execute("rm -rf  " + path);
    }

    template <typename Fun>
    static void runInLoop(Fun fun, int msc = 2000)
    {
        QEventLoop loop;
        QTimer timer;
        timer.start(msc);
        fun();
        QObject::connect(&timer, &QTimer::timeout, [&]{
            timer.stop();
            loop.exit();
        });
        loop.exec();
    }
};

#endif // TESTHELPER_H
