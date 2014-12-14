#pragma once

#include <QWidget>
#include <QListWidget>
#include <QLabel>

class MergerDialog : public QWidget
{
    Q_OBJECT

public:
    MergerDialog();

private:
    QListWidget* fileList;

public slots:
    void addFile();
};
