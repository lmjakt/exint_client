#include "exportWidget.h"
#include <qlabel.h>
#include <qlayout.h>
#include <iostream>

ExportWidget::ExportWidget(std::set<ExpressionPlotter::PlotType> plotTypes, QWidget* parent, const char* name)
    : QWidget(parent, name)
{
    for(set<ExpressionPlotter::PlotType>::iterator it=plotTypes.begin(); it != plotTypes.end(); it++){
	switch(*it){
	    case ExpressionPlotter::RAW:
		typeBoxes.insert(make_pair(*it, new QCheckBox("Raw", this, "raw")));
		break;
	    case ExpressionPlotter::MODEL:
		typeBoxes.insert(make_pair(*it, new QCheckBox("Model", this, "model")));
		break;
	    default :
		std::cerr << "ExportWidget unknown plotType : " << (*it) << std::endl;
	}
    }
    // the following max Sizes are based on the assumption that the QPrinter
    // will be set up at screen resolution (~72 dpi) and that a paper size of B0
    // will be selected with a landscape orientation. This will give a max width 
    // of 1425 mm and max height of 1030 mm. This isn't a particularly smart way of
    // doing it.. - but I my experience is that if we don't do it like this we'll get
    // some issues with the font sizes (since the font size may be set in points).
    // However, we should probably play around with this a bit and see what happens.
    widthBox = new QSpinBox(100, 10000, 10, this, "widthBox");
    heightBox = new QSpinBox(100, 2000, 10, this, "heightBox");
    
    widthBox->setValue(600);
    heightBox->setValue(300);
    QLabel* widthLabel = new QLabel("Width", this);
    QLabel* heightLabel = new QLabel("Height", this);
    
    QLabel* mainLabel = new QLabel("EPS properties", this);
    
    // and some reasonable layout..
    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->addWidget(mainLabel);
    QGridLayout* grid = new QGridLayout();
    vbox->addLayout(grid);
    grid->addWidget(widthLabel, 0, 0);
    grid->addWidget(widthBox, 0, 1);
    grid->addWidget(heightLabel, 1, 0);
    grid->addWidget(heightBox, 1, 1);
    for(map<ExpressionPlotter::PlotType, QCheckBox*>::iterator it=typeBoxes.begin(); it != typeBoxes.end(); it++){
	vbox->addWidget(it->second);
    }
    vbox->addStretch();
}

void ExportWidget::prepareExport(ExpressionPlotter::PlotType type){
    if(typeBoxes.count(type)){
	typeBoxes[type]->setChecked(true);
    }
}

int ExportWidget::imWidth(){
    return(widthBox->value());
}

int ExportWidget::imHeight(){
    return(heightBox->value());
}

std::set<ExpressionPlotter::PlotType> ExportWidget::plotTypes(){
    std::set<ExpressionPlotter::PlotType> types;
    for(map<ExpressionPlotter::PlotType, QCheckBox*>::iterator it=typeBoxes.begin(); it != typeBoxes.end(); it++){
	if(it->second->isChecked())
	    types.insert(it->first);
    }
    return(types);
}

	
