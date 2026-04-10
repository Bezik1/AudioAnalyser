#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <vector>
#include <string>

#include "../../core/AudioAnalyser/AudioAnalyser.hpp"
#include "../../utils/AudioUtils/AudioUtils.hpp"

#include "../widgets/FileStreamWidget/FileStreamWidget.hpp"
#include "../widgets/OverviewWidget/OverviewWidget.hpp"
#include "../widgets/FileManagmentWidget/FileManagmentWidget.hpp"
#include "../widgets/FrequencyComponentsWidget/FrequencyComponentsWidget.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAnalysisFinished();
    void saveModifiedFrequency(const std::vector<AudioAnalyser::FrequencyData> &modifiedData);

private:
    struct IndexedFreq
    {
        int index;
        double amp;
    };

    void setupLayout();
    void startAsyncAnalysis(const QString &wavPath);

    QStackedWidget *stackedWidget;
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *menuLayout;

    QPushButton *fileManagmentBtn;
    QPushButton *overviewBtn;
    QPushButton *fileStreamBtn;
    QPushButton *componentsBtn;

    OverviewWidget *overviewWidget = nullptr;
    FileManagmentWidget *fileManagmentWidget = nullptr;
    FileStreamWidget *fileStreamWidget = nullptr;
    FrequencyComponentsWidget *freqWidget = nullptr;

    QFutureWatcher<void> watcher;

    AudioUtils::AudioData audioData;
    std::vector<AudioAnalyser::FrequencyData> spectrum;
    std::vector<double> reconstructedSamples;

    inline static const QString FILE_MANAGING_BTN_TXT = "File Managment";
    inline static const QString OVERVIEW_BTN_TXT = "Overview";
    inline static const QString FILE_STREAM_BTN_TXT = "File Streaming";
    inline static const QString COMPONENTS_BTN_TXT = "Frequency Components";

    inline static const std::string SPECTRUM_FILE_DIR = "/spectrum_top_";
    inline static const std::string OUTPUT_FILE_DIR = "/output_file.wav";
    inline static const std::vector<int> THRESHOLDS = {8, 128, 1024, 2028, 4056, 9112};
};