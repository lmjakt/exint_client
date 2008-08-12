#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <qfiledialog.h>

class FileDialog : public QFileDialog
{
    Q_OBJECT
	public :
	FileDialog(QWidget* parent=0, const char* name=0);

    protected slots :

	void accept();
    void reject();
    void done();

    private slots :
	void okClicked();  // but we won't get the connection anyway.. 

};

#endif
