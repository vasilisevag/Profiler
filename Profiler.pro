TEMPLATE = app

QT = core gui widgets charts

CONFIG += c++17

SOURCES += \
    FunctionData.cpp \
    FunctionNode.cpp \
    Graph.cpp \
    InstrumentationDataAnalyzer.cpp \
    InstrumentationDataParser.cpp \
    Tree.cpp \
    codelabel.cpp \
    codewindow.cpp \
    main.cpp \
    profilingcontroller.cpp

HEADERS += codewindow.h \
    FunctionData.h \
    FunctionNode.h \
    Graph.h \
    InstrumentationDataAnalyzer.h \
    InstrumentationDataParser.h \
    Tree.h \
    codelabel.h \
    profilingcontroller.h


