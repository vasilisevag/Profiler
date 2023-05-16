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
#include <vector>
#include <string>
#include <QHorizontalBarSeries>
#include <QValueAxis>
#include <QProgressBar>
#include <QGraphicsTextItem>
#include <QHeaderView>
#include <QTableWidgetItem>

CodeWindow::CodeWindow(QWidget *parent) : QMainWindow(parent), actionOpen("open...") {
    this->setGeometry(100, 100, 900, 580);
    fileMenu.setTitle("File");
    fileMenu.addAction(&actionOpen);
    this->menuBar()->addMenu(&fileMenu);
    this->menuBar()->setFixedHeight(24);
    connect(&actionOpen, &QAction::triggered, this, &CodeWindow::Open);
    codeScrollArea.setWidget(&code);
    //resultsScrollArea.setWidget(&resultsWidget);
    pages.addWidget(&code);
    //pages.addWidget(&resultsWidget);
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

    //ffffffffffffffffffffffffffffff
    QVBoxLayout* bl = new QVBoxLayout();
    bl->addWidget(&inclusiveTimesBarSeries);
    bl->addWidget(&exclusiveTimesBarSeries); // if you want to add more widgets add them here (for the results page)!
    bl->addWidget(&callsTable);
    resultsWidget.setLayout(bl);
    resultsScrollArea.setWidget(&resultsWidget);
    resultsScrollArea.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    pages.addWidget(&resultsScrollArea);
    resultsScrollArea.setWidgetResizable(true);
    this->statusBar()->setHidden(false);
    this->statusBar()->setStyleSheet("background-color: #414141;");

    callsTable.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    callsTable.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
    pages.setCurrentWidget(&resultsScrollArea);
}

void CodeWindow::ShowCode(){
    auto a = toolBar.actions();
    for(auto& ac : a) ac->setVisible(true);
    pages.setCurrentWidget(&code);
}

void CodeWindow::StartProfiling(){
    //this->statusBar()->showMessage("running profiled program...");
    //QProgressBar progressBar;
    //progressBar.setRange(0, 0);  // Indeterminate progress bar
    //progressBar.setWindowTitle("running profiled program");
    //progressBar.show();

    // find function names
    std::vector<std::string> sourceCode;
    for(const auto& filename : filenames){
        sourceCode.push_back(filename.toStdString());
    }
    auto functionNames = pc.FindFunctionNames(sourceCode);
    //for(auto functionName : functionNames)
        //std::cout << functionName << std::endl; // this is for debugging purpose

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

    ida.CreateFunctionCallTree("profilingResults");
    auto inclusiveFunctionTimes = ida.FunctionsInclusiveTimes();
    auto exclusiveFunctionTimes = ida.FunctionsExclusiveTimes();

    // here we continue with the graphs (gui)
    std::vector<QtCharts::QBarSet*> bars;
    for(int i = 1; i < inclusiveFunctionTimes.size(); i++){
        bars.push_back(new QtCharts::QBarSet(QString(inclusiveFunctionTimes[i].first.c_str())));
        *bars.back() << inclusiveFunctionTimes[i].second;
    }
    QtCharts::QHorizontalBarSeries *series = new QtCharts::QHorizontalBarSeries();
    for(auto i : bars)
        series->append(i);
    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle("Inclusive Execution Times");
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);

    QtCharts::QValueAxis *axisX = new QtCharts::QValueAxis();
    axisX->setTitleText(QString("time (ms)"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    axisX->applyNiceNumbers();

    inclusiveTimesBarSeries.setChart(chart);

    std::vector<QtCharts::QBarSet*> barsx;
    for(int i = 1; i < exclusiveFunctionTimes.size(); i++){
        barsx.push_back(new QtCharts::QBarSet(QString(exclusiveFunctionTimes[i].first.c_str())));
        *barsx.back() << exclusiveFunctionTimes[i].second;
    }
    QtCharts::QHorizontalBarSeries *seriesx = new QtCharts::QHorizontalBarSeries();
    for(auto i : barsx)
        seriesx->append(i);
    QtCharts::QChart *chartx = new QtCharts::QChart();
    chartx->addSeries(seriesx);
    chartx->setTitle("Exclusive Execution Times");
    chartx->setAnimationOptions(QtCharts::QChart::SeriesAnimations);

    QtCharts::QValueAxis *axisXx = new QtCharts::QValueAxis();
    axisXx->setTitleText(QString("time (ms)"));
    chartx->addAxis(axisXx, Qt::AlignBottom);
    seriesx->attachAxis(axisXx);
    axisXx->applyNiceNumbers();
    exclusiveTimesBarSeries.setChart(chartx);

    inclusiveTimesBarSeries.setMinimumHeight(250);
    exclusiveTimesBarSeries.setMinimumHeight(250);

    //progressBar.close();

    //this->statusBar()->clearMessage();

    callsTable.setRowCount(inclusiveFunctionTimes.size() - 1);
    callsTable.setColumnCount(3);
    callsTable.setMinimumHeight(inclusiveFunctionTimes.size() * callsTable.rowHeight(0) - 8);

    for(int i = 1; i < exclusiveFunctionTimes.size(); i++){
        callsTable.setVerticalHeaderItem(i-1, new QTableWidgetItem(exclusiveFunctionTimes[i].first.c_str()));
    }
    callsTable.setHorizontalHeaderItem(0, new QTableWidgetItem("Total Calls"));
    callsTable.setHorizontalHeaderItem(1, new QTableWidgetItem("Average Inclusive Exeuction Time"));
    callsTable.setHorizontalHeaderItem(2, new QTableWidgetItem("Average Exclusive Exeuction Time"));
    callsTable.setColumnWidth(1, 300);
    callsTable.setColumnWidth(2, 312);

    ida.traverseTree();
    std::vector<std::pair<std::string, int>> totalCalls(ida.callsTotal.begin(), ida.callsTotal.end());
    for(int i = 1; i < exclusiveFunctionTimes.size(); i++){
        for(int j = 1; j < exclusiveFunctionTimes.size(); j++){
            if(callsTable.verticalHeaderItem(j-1)->text().toStdString() == totalCalls[i-1].first){
                callsTable.setItem(j-1, 0, new QTableWidgetItem(QString::number(totalCalls[i-1].second)));
                callsTable.setItem(j-1, 1, new QTableWidgetItem(QString::number(inclusiveFunctionTimes[j].second/totalCalls[i-1].second, 'f', 3)));
                callsTable.setItem(j-1, 2, new QTableWidgetItem(QString::number(exclusiveFunctionTimes[j].second/totalCalls[i-1].second, 'f', 3)));
            }
        }
    }

    ShowResults();

    //inclusiveTimesBarSeries.setFixedSize(300, 300);
    //exclusiveTimesBarSeries.setFixedSize(400, 400);
}
