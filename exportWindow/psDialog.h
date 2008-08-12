#ifndef PSDIALOG_H
#define PSDIALOG_H

#include <set>
#include <qfiledialog.h>
#include <qdialog.h>
#include <qstring.h>
#include <qlineedit.h>
#include "exportWidget.h"
#include "fileDialog.h"
#include "../expressionplotter.h"

class PSDialog : public QDialog
{
    Q_OBJECT
	public :
	PSDialog(std::set<ExpressionPlotter::PlotType> ptypes, QWidget* parent=0, const char* name=0, bool modal=FALSE);


    ExportWidget* expWidget(){
	return(exportWidget);
    }
    void prepareExport(ExpressionPlotter::PlotType ptype);

    private :
	ExportWidget* exportWidget;
    QFileDialog* fileDialog;
    QLineEdit* fileName;

    private slots :
	void getSaveFileName();
    void emitExport();

    signals :
	void exportPS(QString, ExpressionPlotter::PlotType, int, int);
};


#endif
