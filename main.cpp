#include "codewindow.h"
#include <QApplication>
#include <iostream>
#include <QMessageBox>
#include "codelabel.h"

int main(int argc, char *argv[]) {
    QApplication profilerApplication(argc, argv);
    //profilerApplication.setStyle(QStyleFactory::create("Fusion"));
    CodeWindow codeWindow;
    codeWindow.show();

    return profilerApplication.exec();
}
