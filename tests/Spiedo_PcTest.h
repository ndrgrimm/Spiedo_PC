#ifndef Spiedo_PcTEST_H
#define Spiedo_PcTEST_H

#include <QtCore/QObject>
#include <QtTest/QTest>

class Spiedo_PcTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();
    void someTest();
};

#endif // Spiedo_PcTEST_H
