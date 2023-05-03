#ifndef CODELABEL_H
#define CODELABEL_H

#include <QAction>
#include <QString>

class CodeLabel : public QAction {
    Q_OBJECT
public:
    CodeLabel(unsigned int codeID, const QString& name);
signals:
    void OnClick(unsigned int codeID);
    void CloseClick(unsigned int codeID);
public slots:
    void OnClickCallBack();
private:
    unsigned int codeID;
};

#endif // CODELABEL_H
