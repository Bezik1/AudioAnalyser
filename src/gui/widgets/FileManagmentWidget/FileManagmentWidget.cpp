#include "FileManagmentWidget.hpp"

FileManagmentWidget::FileManagmentWidget(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(12);
    layout->setContentsMargins(15, 15, 15, 15);

    pathEdit = new QLineEdit(this);
    pathEdit->setPlaceholderText("Select .wav file...");
    pathEdit->setReadOnly(true);

    browseBtn = new QPushButton("Browse", this);
    analyzeBtn = new QPushButton("Analyse Audio", this);
    analyzeBtn->setEnabled(false);

    browseBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    analyzeBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    pathEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    int screenWidth = 1200;

    browseBtn->setFixedWidth(screenWidth * 0.6);
    analyzeBtn->setFixedWidth(screenWidth * 0.6);
    pathEdit->setFixedWidth(screenWidth * 0.6);

    layout->addStretch();
    layout->addWidget(pathEdit, 0, Qt::AlignCenter);
    layout->addSpacing(10);
    layout->addWidget(browseBtn, 0, Qt::AlignCenter);
    layout->addSpacing(10);
    layout->addWidget(analyzeBtn, 0, Qt::AlignCenter);
    layout->addStretch();

    connect(browseBtn, &QPushButton::clicked, this, &FileManagmentWidget::onBrowse);
    connect(analyzeBtn, &QPushButton::clicked, this, [this]()
            { emit startAnalysisRequested(pathEdit->text().toStdString()); });
}

void FileManagmentWidget::onBrowse()
{
    QString fullPath = QFileDialog::getOpenFileName(
        this,
        "Select Audio File",
        QCoreApplication::applicationDirPath() + "/data",
        "Wav Files (*.wav)");

    if (!fullPath.isEmpty())
    {
        QString searchPattern = "/data/";
        int index = fullPath.lastIndexOf(searchPattern);

        if (index != -1)
        {
            int cutIndex = index + searchPattern.length();

            QString finalPath = fullPath.mid(cutIndex);

            pathEdit->setText(finalPath);
            analyzeBtn->setEnabled(true);
        }
        else
        {
            QMessageBox::warning(this, "Error", "File must be in /data folder!");
        }
    }
}