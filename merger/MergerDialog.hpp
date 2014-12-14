#pragma once

#include <QWidget>
#include <QListWidget>
#include <QString>
#include <QLabel>
#include <string>
#include <vector>
#include <unordered_set>
using namespace std;

class MergerDialog : public QWidget
{
    Q_OBJECT

public:
    MergerDialog();
    void setFiles(vector<string> files, string of);

protected:
    void showEvent(QShowEvent*);

private:
    QListWidget* fileList;
    unordered_set<string> file_set;
    const QString file_filter;
    QString output_file;

    bool openDatabases();
    void closeDatabases();

public slots:
    void addFile();
    void clearFiles();
    void merge();
};
