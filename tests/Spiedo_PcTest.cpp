#include "Spiedo_PcTest.h"

void Spiedo_PcTest::initTestCase()
{}

void Spiedo_PcTest::init()
{}

void Spiedo_PcTest::cleanup()
{}

void Spiedo_PcTest::cleanupTestCase()
{}

void Spiedo_PcTest::someTest()
{
    QCOMPARE(1,2);
}

QTEST_MAIN(Spiedo_PcTest)

#include "Spiedo_PcTest.moc"
