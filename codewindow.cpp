#include "codewindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QFont>
#include <QMenuBar>
#include <QFontDialog>
#include <QScrollArea>
#include <QStatusBar>
#include <algorithm>
#include "codelabel.h"
#include <iostream>
#include <QStyle>
#include <QCommonStyle>
#include <QPalette>
#include <QColor>
#include <QToolBar>
#include <QScrollBar>
#include <QIcon>
#include <QLabel>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

CodeWindow::CodeWindow(QWidget *parent) : QMainWindow(parent), actionOpen("open..."), resultsLabel("NO RESULTS YET!") {
    this->setGeometry(100, 100, 900, 480);
    fileMenu.setTitle("File");
    fileMenu.addAction(&actionOpen);
    this->menuBar()->addMenu(&fileMenu);
    this->menuBar()->setFixedHeight(24);
    connect(&actionOpen, &QAction::triggered, this, &CodeWindow::Open);
    codeScrollArea.setWidget(&code);
    pages.addWidget(&code);
    pages.addWidget(&resultsLabel);
    this->setCentralWidget(&pages);
    this->addToolBar(&toolBar);
    this->addToolBar(Qt::LeftToolBarArea, &leftToolBar);
    toolBar.setMovable(false);
    toolBar.setFixedHeight(24);
    leftToolBar.setMovable(false);
    leftToolBar.setFixedWidth(70);
    leftToolBar.setStyle(new QCommonStyle());
    leftToolBar.setPalette(QPalette(QColor(65, 65, 65)));
    runButton.setIcon(QIcon(tr("/home/vasilis/Programming/C++ Projects/Qt Projects/Profiler/runButton.png")));
    runButton.setIconSize(QSize(50, 50));
    runButton.setGeometry(10, 10, 50, 50);
    runButton.setFlat(true);
    runButton.setStyle(new QCommonStyle());
    textEditorButton.setGeometry(0, 50, 20, 20);
    textEditorButton.setFlat(true);
    textEditorButton.setIcon(QIcon(tr("/home/vasilis/Programming/C++ Projects/Qt Projects/Profiler/text.png")));
    textEditorButton.setIconSize(QSize(50, 50));
    textEditorButton.setStyle(new QCommonStyle());
    profilingResultsButton.setGeometry(0, 100, 20, 20);
    profilingResultsButton.setFlat(true);
    profilingResultsButton.setIcon(QIcon(tr("/home/vasilis/Programming/C++ Projects/Qt Projects/Profiler/results.png")));
    profilingResultsButton.setIconSize(QSize(50, 50));
    profilingResultsButton.setStyle(new QCommonStyle());
    leftToolBar.addWidget(&runButton);
    leftToolBar.addWidget(&textEditorButton);
    leftToolBar.addWidget(&profilingResultsButton);

    toolBar.setStyle(new QCommonStyle());
    toolBar.setBackgroundRole(QPalette::Dark);
    toolBar.setPalette(QPalette(QColor(65, 65, 65)));
    this->menuBar()->setPalette(QPalette(QColor(210, 210, 210)));
    auto p = new QLabel();
    p->setFixedWidth(61);
    toolBar.addWidget(p);
    toolBar.addSeparator();

    connect(&profilingResultsButton, &QPushButton::clicked, this, &CodeWindow::ShowResults);
    connect(&textEditorButton, &QPushButton::clicked, this, &CodeWindow::ShowCode);
    connect(&runButton, &QPushButton::clicked, this, &CodeWindow::StartProfiling);

    resultsLabel.setAlignment(Qt::AlignCenter);
}

void CodeWindow::ChangeCode(unsigned int codeID){
    code.setText(codes[codeID]);
}

void CodeWindow::Open(){
    QString fileName = QFileDialog::getOpenFileName(this, "Open file");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }
    QTextStream in(&file);
    QString text = in.readAll();

    filenames.push_back(fileName);

    codes.push_back(text);
    code.setText(text);
    file.close();

    int begin = fileName.lastIndexOf("/");
    auto temp = fileName;
    temp.remove(begin+1, 100);
    folderPath = temp;
    fileName.remove(0, begin + 1);
    codeLabels.push_back(new CodeLabel(codeLabels.size(), fileName));
    connect(codeLabels.back(), &CodeLabel::OnClick, this, &CodeWindow::ChangeCode);
    toolBar.addAction(codeLabels.back());
    toolBar.addSeparator();
}

void CodeWindow::ShowResults(){
    auto a = toolBar.actions();
    for(auto& ac : a) ac->setVisible(false);
    pages.setCurrentWidget(&resultsLabel);
}

void CodeWindow::ShowCode(){
    auto a = toolBar.actions();
    for(auto& ac : a) ac->setVisible(true);
    pages.setCurrentWidget(&code);
}

void CodeWindow::StartProfiling(){
    // find function names

    std::vector<std::string> sourceCode;
    for(const auto& filename : filenames){
        sourceCode.push_back(filename.toStdString());
    }
    auto functionNames = pc.FindFunctionNames(sourceCode);
    for(auto functionName : functionNames)
        std::cout << functionName << std::endl;

    // profile the code
    pc.ProfileCode(sourceCode, functionNames);

    //int status; we will not be using fork, at least for now.
    //int id = fork();
    std::string compilerCmd = "g++ -o ";
    compilerCmd += folderPath.toStdString() + "profiling.out ";
    for(const auto& filename : filenames){
        auto stdfilename = filename.toStdString();
        if(stdfilename[stdfilename.size()-1] == 'p'){
            int idx = stdfilename.find('.');
            stdfilename = stdfilename.substr(0, idx) + "Pr" + stdfilename.substr(idx, 100);
            compilerCmd += stdfilename + " ";
        }
    }
    system(compilerCmd.c_str());
    std::string runCmd = folderPath.toStdString();
    runCmd += "profiling.out";
    system(runCmd.c_str());

    //ida.CreateFunctionCallTree(argument);
    //auto inclusiveFunctionTimes = ida.FunctionsInclusiveTimes();
    //auto exclusiveFunctionTimes = ida.FunctionsExclusiveTimes();

    // here we continue with the graphs (gui)
}
