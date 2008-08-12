#include "seqWidget.h"
#include <qlayout.h>

SeqWidget::SeqWidget(QWidget* parent, const char* name)
    : QWidget(parent, name)
{
    textArea = new QTextEdit(this, "textArea");
    textArea->setReadOnly(true);
    textArea->setWrapPolicy(QTextEdit::Anywhere);
    textArea->setFont(QFont("Monospace", 9));
    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->addWidget(textArea);
}

void SeqWidget::setText(QString text){
    textArea->setText(text);
    if(!isVisible()){
	show();
    }
}
