#include "fileDialog.h"
#include <iostream>

using namespace std;

FileDialog::FileDialog(QWidget* parent, const char* name)
    : QFileDialog(parent, name)
{
}

void FileDialog::accept(){
    cout << "Accept call received" << endl;
}

void FileDialog::reject(){
    cout << "Reject call received" << endl;
}

void FileDialog::done(){
    cout << "Done call received" << endl;
}

void FileDialog::okClicked(){
    cout << "OK clicked" << endl;
}
