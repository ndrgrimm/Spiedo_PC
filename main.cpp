#include <QtCore/QtCore>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "Spiedo_Pc.h"
#include "Funzioni.h"

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QTextStream standardOutput(stdout);
    QSerialPort* SerialSelected = Collegamento();
    
    
    if (!SerialSelected.open(QIODevice::ReadWrite)) {
        standardOutput << QObject::tr("Failed to open port %1, error: %2").arg(SerialSelected->portName()).arg(SerialSelected->errorString()) << endl;
        return 1;
    }
    
    Ser
    
    return app.exec();
}
