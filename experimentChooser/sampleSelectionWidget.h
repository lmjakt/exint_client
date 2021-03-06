#ifndef SAMPLESELECTIONWIDGET_H
#define SAMPLESELECTIONWIDGET_H

#include <set>
#include <qlineedit.h>
#include <qstring.h>

class SampleSelectionWidget : public QWidget 
{
    Q_OBJECT
	
	public:
    SampleSelectionWidget(QString lab, std::set<int> a, std::set<int> m, QWidget* parent=0, const char* name=0);
    //QSize sizeHint() const;
    std::set<int> activeIndex();
    std::set<int> markedIndex();
    QString labelString();

    signals :
	void displayThese(std::set<int>, std::set<int>); // activities, marks
    void deleteMe(SampleSelectionWidget*);
    void saveMeToFile(SampleSelectionWidget*);

    private slots :
	void setMarks();
    void requestDelete();
    void requestSave();

    private :
	QLineEdit* label;  // user editable label
    std::set<int> activity;
    std::set<int> marks;

};


#endif
