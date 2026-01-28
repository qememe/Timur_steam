#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("Timur Steam");
  QApplication::setOrganizationName("TimurInc");
  QApplication::setWindowIcon(QIcon(":/resources/logo.png"));

  MainWindow window;
  window.show();

  return app.exec();
}
