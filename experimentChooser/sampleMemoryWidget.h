#ifndef SAMPLEMEMORYWIDGET_H
#define SAMPLEMEMORYWIDGET_H

#include "sampleSelectionWidget.h"
#include <qwidget.h>
#include <set>
#include <qlayout.h>
#include <qvbox.h>
#include <qscrollview.h>


class SampleMemoryWidget : public QWidget
{
    Q_OBJECT
	
	public :
	SampleMemoryWidget(QWidget* parent=0, const char* name=0);
    ~SampleMemoryWidget();
    
    void rememberStates(std::set<int> activities, std::set<int> marks);
    
    signals :
	void displayThese(std::set<int>, std::set<int>); // activities, marks

    private :
	void resizeEvent(QResizeEvent* e);

	QVBoxLayout* vbox;
    QVBox* frame;
    QScrollView* scroll;
    int memSize;
};

#endif
