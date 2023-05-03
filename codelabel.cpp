#include "codelabel.h"
#include <iostream>
#include <QMessageBox>

CodeLabel::CodeLabel(unsigned int codeID, const QString& name) : QAction(name), codeID(codeID) {
    connect(this, &QAction::triggered, this, &CodeLabel::OnClickCallBack);
}

void CodeLabel::OnClickCallBack(){
    emit OnClick(codeID);
}

