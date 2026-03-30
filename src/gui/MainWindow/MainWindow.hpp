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

#include "../widgets/FrequencyComponentsWidget/FrequencyComponentsWidget.hpp"

class MainWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showFirstView();
    void showSecondView();
    void onAnalysisFinished();

private:
    void setupLayout();
    void startAsyncAnalysis(const std::string &wavPath);

    QStackedWidget *stackedWidget;
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *menuLayout;
    QPushButton *btnView1;
    QPushButton *btnView2;

    MainWidget *mainWidget;
    FrequencyComponentsWidget *freqWidget = nullptr;

    QFutureWatcher<void> watcher;

    AudioUtils::AudioData audioData;
    std::vector<AudioAnalyser::FrequencyData> spectrum;
    std::vector<float> reconstructedSamples;

    inline static const std::string TEST_PATH = "eval/input_file.wav";
    inline static const std::string SPECTRUM_FILE_DIR = "eval/spectrums/spectrum_top_";
    inline static const std::string OUTPUT_FILE_DIR = "eval/output_file.wav";
    inline static const std::vector<int> THRESHOLDS = {10, 100, 1000, 2000, 5000, 10000};
};