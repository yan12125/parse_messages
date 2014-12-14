#include "MergerDialog.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>

MergerDialog::MergerDialog()
{
    fileList = new QListWidget;

    QHBoxLayout* hLayout = new QHBoxLayout;
    QPushButton* addFileBtn = new QPushButton("Add files", this);
    QPushButton* mergeBtn = new QPushButton("Merge", this);
    hLayout->addWidget(addFileBtn);
    hLayout->addWidget(mergeBtn);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(fileList);
    layout->addLayout(hLayout);

    setLayout(layout);

    connect(addFileBtn, &QPushButton::released, this, &MergerDialog::addFile);
}

void MergerDialog::addFile()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("Facebook message databases (*.db)"));
    if(dialog.exec())
    {
        for(QString file : dialog.selectedFiles())
        {
            fileList->addItem(file);
        }
    }
}
