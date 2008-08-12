#ifndef EXPORTMEANS_H
#define EXPORTMEANS_H

#include <qwidget.h>
#include <qstring.h>     // to give the name of the file to the parent...
#include <qlineedit.h>
#include <qradiobutton.h>

class ExportMeanDialog : public QWidget
{
    Q_OBJECT
	
	public:
    ExportMeanDialog(QWidget* parent=0, const char* name=0);
    
    private slots :
	void getFileName();
    void requestMeans();
    void cancelRequest();

    signals :
	void getMeans(QString, int);

    private :
	QLineEdit* fileName;
    // some radio buttons for normalisation choice..
    QRadioButton* zscore;
    QRadioButton* mscore;
    QRadioButton* raw;
};
    

#endif
