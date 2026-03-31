#pragma once

#include <QtWidgets>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>

class FileManagmentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileManagmentWidget(QWidget *parent = nullptr);

signals:
    void startAnalysisRequested(const std::string &path);

private slots:
    void onBrowse();

private:
    QLineEdit *pathEdit;
    QPushButton *browseBtn;
    QPushButton *analyzeBtn;
};