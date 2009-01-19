#include "sampleMemoryWidget.h"
#include <qstring.h>
#include <qscrollview.h>
#include <iostream>

using namespace std;

SampleMemoryWidget::SampleMemoryWidget(QWidget* parent, const char* name) :
    QWidget(parent, name)
{
    // set up the layout and that's about it really
    QVBoxLayout* mainBox = new QVBoxLayout(this);
    scroll = new QScrollView(this);
    scroll->setVScrollBarMode(QScrollView::AlwaysOn);
    frame = new QVBox(scroll->viewport());
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scroll->addChild(frame); 

    vbox = new QVBoxLayout(frame);
    mainBox->addWidget(scroll);
    memSize=0;
    setCaption("Sample Selection History");
}

SampleMemoryWidget::~SampleMemoryWidget(){
    cout << "Sample Memory Widget destructor. Doing nothing" << endl;
}

void SampleMemoryWidget::rememberStates(std::set<int> activities, std::set<int> marks){
    QString labString;
    memSize++;
    labString.setNum(memSize);
    frame->setFixedWidth(scroll->viewport()->width());
    SampleSelectionWidget* sw = new SampleSelectionWidget(labString, activities, marks, frame, "sw");
    //scroll->addChild(sw);
    connect(sw, SIGNAL(displayThese(std::set<int>, std::set<int>)), this, SIGNAL(displayThese(std::set<int>, std::set<int>)));
    sw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    vbox->addWidget(sw);
    sw->show();
    scroll->updateScrollBars();
}


void SampleMemoryWidget::resizeEvent(QResizeEvent* e){
    frame->setFixedWidth(scroll->viewport()->width());
}
