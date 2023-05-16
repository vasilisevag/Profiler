#ifndef PROFILINGCONTROLLER_H
#define PROFILINGCONTROLLER_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <QString>
#include <QRegExp>

class ProfilingController {
public:
    ProfilingController();
    std::vector<std::string> FindFunctionNames(const std::vector<std::string>& filenames){
        std::vector<std::string> functionNames;
        std::ifstream in;
        for(auto filename : filenames){
            in.open(filename);
            char line[1000];
            //std::cout << "what is going on!";
            while(!in.eof()){
                in.getline(line, 1000, '\n');
                QString qline(line);
                QRegExp re(QString("[a-z]* *(*)*{*"));
                re.setPatternSyntax(QRegExp::Wildcard);
                if(re.exactMatch(line)){
                    std::string stdline(line);
                    int names = stdline.find_first_of(' ');
                    int namee = stdline.find_first_of('(');
                    functionNames.push_back(stdline.substr(names+1, namee - names - 1));
                }
            }
            in.close();
        }
        return functionNames;
    }
    void ProfileCode(const std::vector<std::string>& filenames, const std::vector<std::string>& functionNames){
        std::ifstream in;
        std::ofstream out;
        bool first = true;
        for(auto filename : filenames){ // the first filename should be a c++ file
            bool chronotime = false;
            bool inserta = false;
            in.open(filename);
            int idx = filename.find('.');
            filename = filename.substr(0, idx) + "Pr" + filename.substr(idx, 100);
            out.open(filename);
            char line[1000];

            bool firstTimeInFunction = true;
            while(!in.eof()){
                in.getline(line, 1000, '\n');
                if(!inserta){
                    if(line[0] != '#' && first){
                        first = false;
                        inserta = true;
                        out << "\nstd::ofstream outProf(\"profilingResults\");\n";
                    }
                    else if(line[0] != '#' && !first){
                        inserta = true;
                        out << "\nextern std::ofstream outProf;\n";
                    }
                }
                QString qline(line);
                QRegExp re(QString("[a-z]* *(*)*{*"));
                std::string stdline = qline.toStdString();
                re.setPatternSyntax(QRegExp::Wildcard);
                if(re.exactMatch(line))
                    firstTimeInFunction = true;
                for(auto functionName : functionNames){
                    if(!re.exactMatch(line) && stdline.find(functionName) != std::string::npos){
                        out << "outProf << \" " << functionName << " start \";\n";
                        if(firstTimeInFunction){
                            out << "std::clock_t c_start = std::clock();\n";
                        }
                        else
                            out << "c_start = std::clock();\n";
                    }
                }
                out << line << '\n';
                if(!chronotime){
                    chronotime = true;
                    out << "#include <ctime>\n#include <chrono>\n";
                }
                for(auto functionName : functionNames){
                    if(!re.exactMatch(line) && stdline.find(functionName) != std::string::npos){
                        out << "outProf << \" " << functionName << " end \";\n";
                        if(firstTimeInFunction)
                            out << "std::clock_t c_end = std::clock();\n";
                        else
                            out << "c_end = std::clock();\n";
                        if(firstTimeInFunction){
                            out << "double time_elapsed_ms = 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC;\n";
                            firstTimeInFunction = false;
                        }
                        else
                            out << "time_elapsed_ms = 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC;\n";
                        out << "outProf << " << "time_elapsed_ms" << ";\n";
                    }
                }
            }
            in.close();
            out.close();
        }
    }
};

#endif // PROFILINGCONTROLLER_H
