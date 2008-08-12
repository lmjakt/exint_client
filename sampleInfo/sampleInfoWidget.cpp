#include "sampleInfoWidget.h"
#include <sstream>
#include <qlayout.h>

using namespace std;

SampleInfoWidget::SampleInfoWidget(QWidget* parent, const char* name)
    : QWidget(parent, name)
{
    textView = new QTextEdit(this);
    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->addWidget(textView);
}

void SampleInfoWidget::setInfo(exptInfo& eInfo){
    currentInfo = eInfo;
    ostringstream ts;
    
    const char* fontd = "<font color=\"#ffffff\" size=+1>";
    const char* fend = "</font>";

    ts << "<qt bgcolor=\"#ffff90\">";
    ts << "<table bgcolor=\"#0000f0\">";
    ts << "<tr><td width=20%>" << fontd << "Short" << fend << "</td><td width=80%>" << fontd << eInfo.shortName << fend << "</td>" << "</tr>";
    ts << "<tr><td width=20%>" << fontd << "Description" << fend << "</td><td width=80%>" << fontd << eInfo.description << fend <<  "</td></tr>";
    ts << "<tr><td width=20%>" << fontd << "index" << fend << "</td><td width=80%>" << fontd << eInfo.index << fend << "</td></tr>";
    ts << "<tr><td width=20%>" << fontd << "db index" << fend << "</td><td width=80%>" << fontd << eInfo.dbaseIndex << fend << "</td></tr>";
    ts << "</table>";

    int chipWidth = 10;
    ts << "<p><table  bgcolor=\"#0000f0\">";
    ts << "<tr><th colspan=" << eInfo.chips.size() << ">" << fontd << "Chips" << fend << "</th>" << "</tr>";  
    ts << "<tr>";
    for(map<int, bool>::iterator it=eInfo.chips.begin(); it !=eInfo.chips.end(); it++){
	ts << "<td width=" << chipWidth << " align=center>" << fontd << it->first << fend << "</td>";
    }
    ts << "</tr><tr>";
    for(map<int, bool>::iterator it=eInfo.chips.begin(); it !=eInfo.chips.end(); it++){
	ts << "<td width=" << chipWidth << " align=center>" << fontd;
	if(it->second){
	    ts << "O";
	}else{
	    ts << "X";
	}
	ts << fend << "</td>";
    }
    ts << "</tr></table>";

    textView->setText(ts.str().c_str());
}
    
