#include <string>
#include "sampleMemoryWidget.h"
#include <qstring.h>
#include <qscrollview.h>
#include <qfiledialog.h>
#include <qpushbutton.h>
#include <iostream>
#include <sstream>

using namespace std;

SampleMemoryWidget::SampleMemoryWidget(QWidget* parent, const char* name) :
    QWidget(parent, name)
{
    // set up the layout and that's about it really
    QVBoxLayout* mainBox = new QVBoxLayout(this);
    scroll = new QScrollView(this);
    scroll->setVScrollBarMode(QScrollView::AlwaysOn);
    frame = new QVBox(scroll->viewport());
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scroll->addChild(frame); 

    QPushButton* readButton = new QPushButton("Read from file", this, "readButton");
    connect(readButton, SIGNAL(clicked()), this, SLOT(readSetFromFile()) );

    vbox = new QVBoxLayout(frame);
    mainBox->addWidget(scroll);
    memSize=1;
    setCaption("Sample Selection History");
    mainBox->addWidget(readButton);
}

SampleMemoryWidget::~SampleMemoryWidget(){
    cout << "Sample Memory Widget destructor. Doing nothing" << endl;
}

void SampleMemoryWidget::rememberStates(std::set<int> activities, std::set<int> marks, QString& label){
    frame->setFixedWidth(scroll->viewport()->width());
    SampleSelectionWidget* sw = new SampleSelectionWidget(label, activities, marks, frame, "sw");
    connect(sw, SIGNAL(displayThese(std::set<int>, std::set<int>)), this, SIGNAL(displayThese(std::set<int>, std::set<int>)));
    connect(sw, SIGNAL(deleteMe(SampleSelectionWidget*)), this, SLOT(deleteSelection(SampleSelectionWidget*)) );
    connect(sw, SIGNAL(saveMeToFile(SampleSelectionWidget*)), this, SLOT(saveSetToFile(SampleSelectionWidget*)) );
    sw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    vbox->addWidget(sw);
    sw->show();
    scroll->updateScrollBars();
    memSize++;
}
    
void SampleMemoryWidget::rememberStates(std::set<int> activities, std::set<int> marks){
    QString labString;
    labString.setNum(memSize);
    rememberStates(activities, marks, labString);
}


void SampleMemoryWidget::resizeEvent(QResizeEvent* e){
    frame->setFixedWidth(scroll->viewport()->width());
}

void SampleMemoryWidget::deleteSelection(SampleSelectionWidget* sw){
    cout << "Deleting a sampleselection widget with address : " << sw << endl;
    delete(sw);
}

void SampleMemoryWidget::saveSetToFile(SampleSelectionWidget* sw){
    QString outfile = QFileDialog::getSaveFileName();
    ofstream out(outfile.ascii());
    if(!out){
	cerr << "Unable to open file for saving selection" << endl;
	return;
    }
    out << "Sample_selection_set" << endl;
    QString lab = sw->labelString();
    if(!(lab.length()))
	lab = "undef";
    out << lab << endl;
    set<int> s = sw->activeIndex();
    for(set<int>::iterator it=s.begin(); it !=s.end(); it++)
	out << (*it) << " ";
    out << endl;
    s = sw->markedIndex();
    for(set<int>::iterator it=s.begin(); it !=s.end(); it++)
	out << (*it) << " ";
    out << endl;
    out.close();
}

// this used to take a reference to the ifstream, rather than the string.
// However, the braindead version of Borland (5.2 yeah I know it's old) 
// can't hack a reference to a stream.
set<int> SampleMemoryWidget::readInts(string& line){
//    string line;
//    getline(in, line);
//    cout << "Reading ints from : " << endl << line << endl;
    istringstream iss(line);
    set<int> s;
    int i;
    while(iss >> i)
	s.insert(i);
    return(s);
}

void SampleMemoryWidget::readSetFromFile(){
    QString infile = QFileDialog::getOpenFileName();
    ifstream in(infile.ascii());
    if(!in){
	cerr << "Unable to open " << infile.ascii() << " for reading a sample set" << endl;
	return;
    }
    string word;
    getline(in, word);
    if(word != "Sample_selection_set"){
	cerr << "unable to read file id or file id wrong: " << word << endl;
    }
    getline(in, word);  // the label
    cout << "got a label " << word << endl;
    string line;
    getline(in, line);
    set<int> active = readInts(line);
    getline(in, line);
    set<int> marks = readInts(line);  // sending a ref to the ifstream is better but Borland can't cope
    QString label(word.c_str());
    rememberStates(active, marks, label);
}
   
