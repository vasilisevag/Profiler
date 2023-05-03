#ifndef CODEWINDOW_H
#define CODEWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QAction>
#include <QMenu>
#include <QScrollArea>
#include <QToolBar>
#include <QDockWidget>
#include <vector>
#include "codelabel.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QGraphicsView>
#include <QStackedWidget>
#include "profilingcontroller.h"
#include "InstrumentationDataAnalyzer.h"

class CodeWindow : public QMainWindow {
  Q_OBJECT

  public:
    explicit CodeWindow(QWidget *parent = 0);
    //~CodeWindow();

  private slots:
    void ChangeCode(unsigned int codeID);
    void Open();
    void ShowResults();
    void ShowCode();
    void StartProfiling();
    //void save();
    //void saveAs();
    //void exit();
    //void copy();
    //void cut();
    //void paste();
    //void undo();
    //void redo();

  private:
    QTextEdit code;
    QMenu fileMenu;
    QToolBar toolBar;
    QToolBar leftToolBar;
    QAction actionOpen;
    QScrollArea codeScrollArea;
    std::vector<QString> codes;
    std::vector<CodeLabel*> codeLabels;
    QPushButton runButton;
    QPushButton textEditorButton;
    QPushButton profilingResultsButton;
    QStackedWidget pages;
    std::vector<QString> filenames; // temporary solution
    QLabel resultsLabel;
    QString folderPath;
    ProfilingController pc;
    InstrumentationDataAnalyzer ida;
};

#endif // CODEWINDOW_H
