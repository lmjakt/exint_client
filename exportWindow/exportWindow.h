#ifndef EXPORTWIDGET_H
#define EXPORTWIDGET_H

#include <map>
#include <vector>
#include <set>
#include <qwidget.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include "../expressionplotter.h"   // for ExpressionPlotter::PlotType

class ExportWidget : public QWidget
{
    Q_OBJECT
	public :
	ExportWidget(std::set<ExpressionPlotter::PlotType> plotTypes, QWidget* parent=0, const char* name=0);
    void prepareExport(ExpressionPlotter::PlotType type);

    int imWidth();
    int imHeight();
    std::vector<ExpressionPlotter::PlotType> plotTypes();

    private :
	std::map<ExpressionPlotter::PlotType, QCheckBox*> typeBoxes;
    QSpinBox* widthBox;
    QSpinBox* heightBox;        // set the height of the thingy..

};

#endif
