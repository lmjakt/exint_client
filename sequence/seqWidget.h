#ifndef SEQWIDGET_H
#define SEQWIDGET_H

#include <qwidget.h>
#include <qtextedit.h>
#include <qstring.h>

class SeqWidget : public QWidget
{
    Q_OBJECT
	public :
	SeqWidget(QWidget* parent=0, const char* name=0);
    void setText(QString text);
    
    private :
	QTextEdit* textArea;
};


#endif
