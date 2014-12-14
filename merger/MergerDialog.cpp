#include "common.hpp"
#include "MergerDialog.hpp"
#include <iostream>
#include <sstream>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
using namespace std;

MergerDialog::MergerDialog():
    file_filter(tr("Facebook message databases (*.db)"))
{
    fileList = new QListWidget;

    QHBoxLayout* hLayout = new QHBoxLayout;
    QPushButton* addFileBtn = new QPushButton("Add files", this);
    QPushButton* clearFilesBtn = new QPushButton("Clear", this);
    QPushButton* mergeBtn = new QPushButton("Merge", this);
    hLayout->addWidget(addFileBtn);
    hLayout->addWidget(clearFilesBtn);
    hLayout->addWidget(mergeBtn);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(fileList);
    layout->addLayout(hLayout);

    setLayout(layout);

    connect(addFileBtn, &QPushButton::released, this, &MergerDialog::addFile);
    connect(clearFilesBtn, &QPushButton::released, this, &MergerDialog::clearFiles);
    connect(mergeBtn, &QPushButton::released, this, &MergerDialog::merge);
}

void MergerDialog::showEvent(QShowEvent* /*event*/)
{
    if(fileList->count() > 0)
    {
        merge();
    }
}

void MergerDialog::setFiles(vector<string> files, string of)
{
    clearFiles();
    for(string file : files)
    {
        fileList->addItem(QDir().absoluteFilePath(QString::fromStdString(file)));
    }
    output_file = QString::fromStdString(of);
}

void MergerDialog::addFile()
{
    // Qt Note: If you want to iterate over the list of files, you should iterate over a copy.
    QStringList files = QFileDialog::getOpenFileNames(this, QString(), QString(), file_filter);
    for(QString file : files)
    {
        string filename = file.toStdString();
        if(file_set.find(filename) == file_set.end())
        {
            file_set.insert(filename);
            fileList->addItem(file);
        }
        else
        {
            cerr << "Warning: " << filename << " already selected" << endl;
        }
    }
}

void MergerDialog::clearFiles()
{
    fileList->clear();
}

void MergerDialog::merge()
{
    if(output_file.isNull()) // it may be set in setFiles
    {
        output_file = QFileDialog::getSaveFileName(this, QString(), QString(), file_filter);
    }

    if(output_file.isNull())
    {
        return;
    }

    QSqlDatabase output_db = QSqlDatabase::addDatabase("QSQLITE", "output_db");
    output_db.setDatabaseName(output_file);
    output_db.open();

    if(!openDatabases())
    {
        return;
    }

    QSqlQuery query;
    query.exec(QString::fromStdString("CREATE TABLE IF NOT EXISTS output_db.messages "+messages_schema));

    stringstream insertQuery;
    insertQuery << "INSERT INTO output_db.messages " << messages_columns << " ";
    for(int i = 0; i < fileList->count(); i++)
    {
        insertQuery << "SELECT * FROM db" << i << ".messages";
        if(i != fileList->count() - 1)
        {
            insertQuery << " UNION ";
        }
    }

    query.exec(QString::fromStdString(insertQuery.str()));
    query.exec("SELECT INTO output_db.meta FROM db0.meta"); // copy meta data

    closeDatabases();
}

bool MergerDialog::openDatabases()
{
    if(fileList->count() <= 0)
    {
        return false;
    }

    for(int i = 0; i < fileList->count(); i++)
    {
        QString filename = fileList->item(0)->text();
        stringstream ss;
        ss << "db" << i;
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", ss.str().c_str());
        db.setDatabaseName(filename);
        db.open();
    }
    return true;
}

void MergerDialog::closeDatabases()
{
    for(QString connectionName : QSqlDatabase::connectionNames())
    {
        QSqlDatabase::database(connectionName).close();
    }
}
