#ifndef SAMPLEINFOWIDGET_H
#define SAMPLEINFOWIDGET_H

#include <qwidget.h>
#include <qtextedit.h>
#include "../exptinfo.h"


class SampleInfoWidget : public QWidget
{
    Q_OBJECT
	public :
	SampleInfoWidget(QWidget* parent=0, const char* name=0);
    
    public slots :
	void setInfo(exptInfo& eInfo);
    
    private :
	QTextEdit* textView;
    exptInfo currentInfo;  // if we want to extend it to allow the editing of information
};

#endif
