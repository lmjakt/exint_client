#include "psDialog.h"
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qstring.h>
#include <qtabwidget.h>
#include <iostream>
#include <set>

using namespace std;

PSDialog::PSDialog(std::set<ExpressionPlotter::PlotType> ptypes, QWidget* parent, const char* name, bool modal)
    : QDialog(parent, name, modal)
{

    fileDialog = new QFileDialog(this);
    QString filters("PS files (*.ps, *.PS);;Any files (*)");
    fileDialog->setFilters(filters);
    fileDialog->setMode(QFileDialog::AnyFile);
//    fileDialog->setModal(true);  // Borland compiler can't cope with this?

    exportWidget = new ExportWidget(ptypes, this);
    fileName = new QLineEdit(this);
    QPushButton* getNameButton = new QPushButton("File Name", this);
    QPushButton* exportButton = new QPushButton("Export", this);

    connect(getNameButton, SIGNAL(clicked()), this, SLOT(getSaveFileName()) );
    connect(exportButton, SIGNAL(clicked()), this, SLOT(emitExport()) );

    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->addWidget(exportWidget);
    QHBoxLayout* buttonBox = new QHBoxLayout();
    vbox->addWidget(fileName);
    vbox->addLayout(buttonBox);
    buttonBox->addWidget(getNameButton);
    buttonBox->addWidget(exportButton);

}

void PSDialog::getSaveFileName(){
    QString file;
    if(fileDialog->exec() == QDialog::Accepted)
	file = fileDialog->selectedFile();

    fileName->setText(file);
}

void PSDialog::emitExport(){
    map<ExpressionPlotter::PlotType, QString> suffixes;
    suffixes.insert(make_pair(ExpressionPlotter::RAW, "r_"));
    suffixes.insert(make_pair(ExpressionPlotter::MODEL, "m_"));
    set<ExpressionPlotter::PlotType> types = exportWidget->plotTypes();
    QString coreName = fileName->text();
    for(set<ExpressionPlotter::PlotType>::iterator it=types.begin(); it != types.end(); it++){
	QString fname = coreName;
	if(types.size() > 1 && suffixes.count(*it))
	    fname.prepend(suffixes[*it]);
	emit exportPS(fname, *it, exportWidget->imWidth(), exportWidget->imHeight());
    }
    // ok..  -- that will do, but is ugly.. 
}

void PSDialog::prepareExport(ExpressionPlotter::PlotType ptype){
    exportWidget->prepareExport(ptype);
}
