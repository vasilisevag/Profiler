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
                QRegExp re(QString("* *(*)*{*"));
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
        for(auto filename : filenames){
            in.open(filename);
            int idx = filename.find('.');
            filename = filename.substr(0, idx) + "Pr" + filename.substr(idx, 100);
            out.open(filename);
            char line[1000];

            while(!in.eof()){
                in.getline(line, 1000, '\n');
                QString qline(line);
                QRegExp re(QString("* *(*)*{*"));
                std::string stdline = qline.toStdString();
                re.setPatternSyntax(QRegExp::Wildcard);
                for(auto functionName : functionNames){
                    if(!re.exactMatch(line) && stdline.find(functionName) != std::string::npos){
                        out << "// " << functionName << " start\n";
                    }
                }
                out << line << '\n';
                for(auto functionName : functionNames){
                    if(!re.exactMatch(line) && stdline.find(functionName) != std::string::npos){
                        out << "// " << functionName << " end\n";
                    }
                }
            }
            in.close();
            out.close();
        }
    }
};

#endif // PROFILINGCONTROLLER_H
