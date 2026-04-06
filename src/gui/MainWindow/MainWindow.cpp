#include "MainWindow.hpp"
#include <QLabel>
#include <QDir>
#include <QCoreApplication>
#include <QtConcurrent/QtConcurrent>
#include <algorithm>
#include <complex>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupLayout();
}

MainWindow::~MainWindow() {}

void MainWindow::setupLayout()
{
    centralWidget = new QWidget(this);
    mainLayout = new QVBoxLayout(centralWidget);

    menuLayout = new QHBoxLayout();

    fileManagmentBtn = new QPushButton(FILE_MANAGING_BTN_TXT, this);
    overviewBtn = new QPushButton(OVERVIEW_BTN_TXT, this);
    fileStreamBtn = new QPushButton(FILE_STREAM_BTN_TXT, this);
    componentsBtn = new QPushButton(COMPONENTS_BTN_TXT, this);

    fileManagmentBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    overviewBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    fileStreamBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    componentsBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    menuLayout->addStretch(1);
    menuLayout->addWidget(fileManagmentBtn, 1);
    menuLayout->addSpacing(30);
    menuLayout->addWidget(overviewBtn, 1);
    menuLayout->addSpacing(30);
    menuLayout->addWidget(fileStreamBtn, 1);
    menuLayout->addSpacing(30);
    menuLayout->addWidget(componentsBtn, 1);
    menuLayout->addStretch(1);

    menuLayout->setSpacing(20);

    stackedWidget = new QStackedWidget(this);

    fileManagmentWidget = new FileManagmentWidget(this);
    overviewWidget = new OverviewWidget(this);
    fileStreamWidget = new FileStreamWidget(this);
    freqWidget = new FrequencyComponentsWidget(this);

    stackedWidget->addWidget(fileManagmentWidget);
    stackedWidget->addWidget(overviewWidget);
    stackedWidget->addWidget(fileStreamWidget);
    stackedWidget->addWidget(freqWidget);

    mainLayout->addLayout(menuLayout);
    mainLayout->addWidget(stackedWidget);

    setCentralWidget(centralWidget);

    connect(fileManagmentWidget, &FileManagmentWidget::startAnalysisRequested,
            this, &MainWindow::startAsyncAnalysis);

    connect(fileManagmentWidget, &FileManagmentWidget::pathChoosen,
            fileStreamWidget, &FileStreamWidget::refreshFileList);

    connect(fileManagmentBtn, &QPushButton::clicked, this, [this]()
            { stackedWidget->setCurrentIndex(0); });

    connect(overviewBtn, &QPushButton::clicked, this, [this]()
            { stackedWidget->setCurrentIndex(1); });

    connect(fileStreamBtn, &QPushButton::clicked, this, [this]()
            { stackedWidget->setCurrentIndex(2); });

    connect(componentsBtn, &QPushButton::clicked, this, [this]()
            { stackedWidget->setCurrentIndex(3); });

    connect(freqWidget, &FrequencyComponentsWidget::topXChanged, this, [this](int)
            { this->onAnalysisFinished(); });

    connect(freqWidget, &FrequencyComponentsWidget::saveRequested, this, &MainWindow::saveModifiedFrequency);
}

void MainWindow::saveModifiedFrequency(const std::vector<AudioAnalyser::FrequencyData> &modifiedData)
{
    int originalNumSamples = audioData.getNumSamples();
    auto samples = AudioAnalyser::reconstructFFT(modifiedData, originalNumSamples, audioData.fmt.sampleRate);

    AudioUtils::AudioData outputData = AudioUtils::prepareSamplesToBeSaved(
        samples, audioData.fmt.numChannels, audioData.fmt.sampleRate, audioData.fmt.bitsPerSample);

    QString savePath = QDir::currentPath() + "/data/eval/modified_output.wav";
    AudioUtils::saveWav(outputData, savePath.toStdString());
    fileStreamWidget->refreshFileList(savePath);
}

void MainWindow::startAsyncAnalysis(const QString &wavPath)
{
    overviewWidget->showAnalyzingStatus();
    freqWidget->showAnalyzingStatus();
    stackedWidget->setCurrentIndex(1);

    QString appDir = QDir::currentPath();
    QString evalDirPath = appDir + "/data/eval";
    QString spectrumsDirPath = evalDirPath + "/spectrums";

    QDir().mkpath(spectrumsDirPath);

    std::string wavPathStr = wavPath.toStdString();
    std::string evalPathStr = evalDirPath.toStdString();
    std::string spectrumPathStr = spectrumsDirPath.toStdString();

    disconnect(&watcher, &QFutureWatcher<void>::finished, this, &MainWindow::onAnalysisFinished);
    connect(&watcher, &QFutureWatcher<void>::finished, this, &MainWindow::onAnalysisFinished);

    QFuture<void> future = QtConcurrent::run([this, wavPathStr, evalPathStr, spectrumPathStr]()
                                             {
        this->audioData = AudioUtils::readWav(wavPathStr);
        int originalNumSamples = audioData.getNumSamples();

        this->spectrum = AudioAnalyser::fastFourierTransform(audioData.data.samples, audioData.fmt.sampleRate);

        std::vector<IndexedFreq> indexed;
        for (int i = 0; i < (int)this->spectrum.size(); ++i)
            indexed.push_back({i, this->spectrum[i].amplitude});

        std::sort(indexed.begin(), indexed.end(), [](const auto &a, const auto &b)
                  { return a.amp > b.amp; });

        for (int threshold : THRESHOLDS)
        {
            std::vector<AudioAnalyser::FrequencyData> thresholdDataList;
            int count = std::min(threshold, static_cast<int>(indexed.size()));

            for (int i = 0; i < count; ++i)
                thresholdDataList.push_back(this->spectrum[indexed[i].index]);

            auto reconstructedThresholdSample = AudioAnalyser::reconstructFFT(
                thresholdDataList, originalNumSamples, audioData.fmt.sampleRate);

            AudioUtils::AudioData thresholdData = AudioUtils::prepareSamplesToBeSaved(
                reconstructedThresholdSample,
                audioData.fmt.numChannels,
                audioData.fmt.sampleRate,
                audioData.fmt.bitsPerSample);

            std::string savePath = spectrumPathStr + SPECTRUM_FILE_DIR + std::to_string(threshold) + ".wav";
            AudioUtils::saveWav(thresholdData, savePath);
        }

        this->reconstructedSamples = AudioAnalyser::reconstructFFT(this->spectrum, originalNumSamples, audioData.fmt.sampleRate);

        AudioUtils::AudioData fullData = AudioUtils::prepareSamplesToBeSaved(
            this->reconstructedSamples,
            audioData.fmt.numChannels,
            audioData.fmt.sampleRate,
            audioData.fmt.bitsPerSample);

        std::string finalOutputPath = evalPathStr + OUTPUT_FILE_DIR;
        AudioUtils::saveWav(fullData, finalOutputPath); });

    watcher.setFuture(future);
}

void MainWindow::onAnalysisFinished()
{
    QString evalDirPath = QDir::currentPath() + "/data/eval/input_file.wav";
    fileStreamWidget->refreshFileList(evalDirPath);

    overviewWidget->displayData(audioData, spectrum, reconstructedSamples);

    QSpinBox *spinner = freqWidget->findChild<QSpinBox *>();
    int thresholdDataCount = spinner ? spinner->value() : 20;

    auto thresholdData = spectrum;
    std::sort(thresholdData.begin(), thresholdData.end(), [](const auto &a, const auto &b)
              { return a.amplitude > b.amplitude; });

    if (thresholdData.size() > (size_t)thresholdDataCount)
        thresholdData.resize(thresholdDataCount);

    freqWidget->setFrequencyData(thresholdData, audioData.getNumSamples(), audioData.fmt.sampleRate);
}