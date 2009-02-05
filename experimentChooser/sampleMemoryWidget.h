#ifndef SAMPLEMEMORYWIDGET_H
#define SAMPLEMEMORYWIDGET_H

#include "sampleSelectionWidget.h"
#include <qwidget.h>
#include <set>
#include <fstream>
#include <qlayout.h>
#include <qvbox.h>
#include <qscrollview.h>
#include <qstring.h>


class SampleMemoryWidget : public QWidget
{
    Q_OBJECT
	
	public :
	SampleMemoryWidget(QWidget* parent=0, const char* name=0); 
    ~SampleMemoryWidget();
    
    void rememberStates(std::set<int> activities, std::set<int> marks);
    void rememberStates(std::set<int> activities, std::set<int> marks, QString& label);

    public slots :
	void readSetFromFile();

    signals :
	void displayThese(std::set<int>, std::set<int>); // activities, marks
    
    private slots:
	void deleteSelection(SampleSelectionWidget* sw);
    void saveSetToFile(SampleSelectionWidget* sw);

    private :
	void resizeEvent(QResizeEvent* e);
//    std::set<int> readInts(std::ifstream& in); // replaced with below to allow for problems with Borland compiler
    std::set<int> readInts(std::string& line);

	QVBoxLayout* vbox;
    QVBox* frame;
    QScrollView* scroll;
    int memSize;
};

#endif
