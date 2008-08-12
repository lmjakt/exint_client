#include "exportMeanDialog.h"
#include <qfiledialog.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qbuttongroup.h>

ExportMeanDialog::ExportMeanDialog(QWidget* parent, const char* name)
    : QWidget(parent, name)
{
    // just need some buttons and a qline edit..
    setCaption("Export Means");
    QPushButton* requestButton = new QPushButton("Export Means", this, "requestButton");
    connect(requestButton, SIGNAL(clicked()), this, SLOT(requestMeans()) );

    QPushButton* cancelButton = new QPushButton("Cancel", this, "cancelButton");
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelRequest()) );

    QPushButton* fileButton = new QPushButton("Specify File", this, "fileButton");
    connect(fileButton, SIGNAL(clicked()), this, SLOT(getFileName()) );

    fileName = new QLineEdit(this, "fileName");

    // a button Group and a number of buttons..
    QLabel* normLabel = new QLabel("Normalisation Method", this, "normLabel");
    QButtonGroup* normGroup = new QButtonGroup("normGroup");
    zscore = new QRadioButton("z-score", this, "zscore");
    mscore = new QRadioButton("m-score", this, "mscore");
    raw = new QRadioButton("raw", this, "raw");
    // this is very ugly, but I can't seem to get it to work.. so well otherwise.
    normGroup->insert(zscore);
    normGroup->insert(mscore);
    normGroup->insert(raw);

    // and set up some sort of layout .. 
    QVBoxLayout* vbox = new QVBoxLayout(this, 2, 2, "vbox");
    vbox->addWidget(normLabel);
    vbox->addWidget(zscore);
    vbox->addWidget(mscore);
    vbox->addWidget(raw);

    QHBoxLayout* fileBox = new QHBoxLayout(vbox);
    fileBox->addWidget(fileName);
    fileBox->addWidget(fileButton);
    vbox->addStretch();
    QHBoxLayout* buttonBox = new QHBoxLayout(vbox);
    buttonBox->addWidget(cancelButton);
    buttonBox->addStretch();
    buttonBox->addWidget(requestButton);
}

void ExportMeanDialog::getFileName(){
    QString file = QFileDialog::getSaveFileName();
    fileName->setText(file);
}

void ExportMeanDialog::requestMeans(){
    // find out which button is requested... hmm,
    // this is ugly, but ..
    int norm = 0;
    if(zscore->isChecked()){ norm = 1; }
    if(mscore->isChecked()){ norm = 2; }
    if(raw->isChecked()){ norm = 3; }
    if(!norm){
	QMessageBox::information(this, "Error", "You must select one normalisation method", QMessageBox::Ok);
	return;
    }
    QString file = fileName->text();
    if(file.isNull() || file == ""){
	QMessageBox::information(this, "Error", "You must specify the file name", QMessageBox::Ok);
	return;
    }
    emit getMeans(file, norm);
    hide();
}

void ExportMeanDialog::cancelRequest(){
    hide();
}

