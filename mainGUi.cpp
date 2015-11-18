#include <QtCore>
#include <QtWidgets/QApplication>

#include "mainWindow.h"


int main(int argc, char **argv){
  
  QApplication app(argc,argv);
  MainWindow Spiedo;
  Spiedo.show();
  
  return app.exec();
}