#include "sampleSelectionWidget.h"
#include <qpushbutton.h>
#include <qlayout.h>

SampleSelectionWidget::SampleSelectionWidget(QString lab, std::set<int> a, std::set<int> m, QWidget* parent, const char* name) :
    QWidget(parent, name)
{
    activity = a;
    marks = m;
    
    //and some interface elments
    label = new QLineEdit(lab, this, "label");
    QPushButton* dispButton = new QPushButton("set", this, "dispButton");
    connect(dispButton, SIGNAL(clicked()), this, SLOT(setMarks()));

    QHBoxLayout* hbox = new QHBoxLayout(this);
    hbox->addWidget(dispButton);
    hbox->addWidget(label);
}

void SampleSelectionWidget::setMarks(){
    emit displayThese(activity, marks);
}

QSize SampleSelectionWidget::sizeHint() const
{
    QWidget* pw = parentWidget();
    if(pw)
	return(QSize(pw->width(), label->height()));
    return( QSize(label->width() + 30, label->height()) );
}
