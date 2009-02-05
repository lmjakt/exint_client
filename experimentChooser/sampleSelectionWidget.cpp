#include "sampleSelectionWidget.h"
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>

SampleSelectionWidget::SampleSelectionWidget(QString lab, std::set<int> a, std::set<int> m, QWidget* parent, const char* name) :
    QWidget(parent, name)
{
    activity = a;
    marks = m;
    
    //and some interface elments
    label = new QLineEdit(lab, this, "label");
    QPushButton* dispButton = new QPushButton("set", this, "dispButton");
    connect(dispButton, SIGNAL(clicked()), this, SLOT(setMarks()));

    QPushButton* delButton = new QPushButton("del", this, "delButton");
    connect(delButton, SIGNAL(clicked()), this, SLOT(requestDelete()) );

    QPushButton* saveButton = new QPushButton("save", this, "saveButton");
    connect(saveButton, SIGNAL(clicked()), this, SLOT(requestSave()) );

    // A couple of labels for the size of the set
    QString labstring;
    labstring.setNum(a.size());
    QLabel* set_size = new QLabel(labstring, this);
    labstring.setNum(m.size());
    QLabel* mark_size = new QLabel(labstring, this);

    QHBoxLayout* hbox = new QHBoxLayout(this);
    hbox->addWidget(dispButton);
    hbox->addWidget(label);
    hbox->addWidget(set_size);
    hbox->addSpacing(5);
    hbox->addWidget(mark_size);
    hbox->addWidget(delButton);
    hbox->addWidget(saveButton);
}

void SampleSelectionWidget::setMarks(){
    emit displayThese(activity, marks);
}

// QSize SampleSelectionWidget::sizeHint() const
// {
//     QWidget* pw = parentWidget();
//     if(pw)
// 	return(QSize(pw->width(), label->height()));
//     return( QSize(label->width() + 30, label->height()) );
// }

void SampleSelectionWidget::requestDelete(){
    emit deleteMe(this);
}

void SampleSelectionWidget::requestSave(){
    emit saveMeToFile(this);
}

std::set<int> SampleSelectionWidget::activeIndex(){
    return(activity);
}

std::set<int> SampleSelectionWidget::markedIndex(){
    return(marks);
}

QString SampleSelectionWidget::labelString(){
    return(label->text());
}
