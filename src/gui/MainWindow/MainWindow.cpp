#include "MainWindow.hpp"
#include <QLabel>

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

    fileManagmentBtn = new QPushButton("File Managment", this);
    overviewBtn = new QPushButton("Overview", this);
    componentsBtn = new QPushButton("Frequency Components", this);

    fileManagmentBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    overviewBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    componentsBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    menuLayout->addStretch(1);
    menuLayout->addWidget(fileManagmentBtn, 1);
    menuLayout->addSpacing(30);
    menuLayout->addWidget(overviewBtn, 1);
    menuLayout->addSpacing(30);
    menuLayout->addWidget(componentsBtn, 1);
    menuLayout->addStretch(1);

    menuLayout->setSpacing(20);

    stackedWidget = new QStackedWidget(this);

    fileManagmentWidget = new FileManagmentWidget(this);

    connect(fileManagmentWidget, &FileManagmentWidget::startAnalysisRequested,
            this, &MainWindow::startAsyncAnalysis);

    overviewWidget = new OverviewWidget(this);
    freqWidget = new FrequencyComponentsWidget(this);

    stackedWidget->addWidget(fileManagmentWidget);
    stackedWidget->addWidget(overviewWidget);
    stackedWidget->addWidget(freqWidget);

    mainLayout->addLayout(menuLayout);
    mainLayout->addWidget(stackedWidget);

    setCentralWidget(centralWidget);

    connect(fileManagmentBtn, &QPushButton::clicked, this, &MainWindow::showFirstView);
    connect(overviewBtn, &QPushButton::clicked, this, &MainWindow::showSecondView);
    connect(componentsBtn, &QPushButton::clicked, this, &MainWindow::showThirdView);
    connect(freqWidget, &FrequencyComponentsWidget::topXChanged, this, [this](int newX)
    {
        this->onAnalysisFinished();
    });

}

void MainWindow::startAsyncAnalysis(const std::string &wavPath)
{
    overviewWidget->showAnalyzingStatus();
    freqWidget->showAnalyzingStatus();

    showSecondView();

    disconnect(&watcher, &QFutureWatcher<void>::finished, this, &MainWindow::onAnalysisFinished);
    connect(&watcher, &QFutureWatcher<void>::finished, this, &MainWindow::onAnalysisFinished);

    QFuture<void> future = QtConcurrent::run([this, wavPath]()
    {
        this->audioData = AudioUtils::readWav(wavPath);
        int numSamples = audioData.getNumSamples();

        this->spectrum = AudioAnalyser::discreteFourierTransform(audioData.data.samples, audioData.fmt.sampleRate);

        auto sortedSpectrum = this->spectrum;
        std::sort(sortedSpectrum.begin(), sortedSpectrum.end(), [](const auto &a, const auto &b) 
        {
            return a.amplitude > b.amplitude;
        });

        for (int threshold : THRESHOLDS)
        {
            int count = std::min(threshold, static_cast<int>(sortedSpectrum.size()));

            std::vector<AudioAnalyser::FrequencyData> topN(sortedSpectrum.begin(), sortedSpectrum.begin() + count);
            auto reconstructedThresholdSample = AudioAnalyser::reconstruct(
                topN, numSamples, audioData.fmt.sampleRate);

            AudioUtils::AudioData thresholdData = AudioUtils::prepareSamplesToBeSaved(
                reconstructedThresholdSample,
                audioData.fmt.numChannels,
                audioData.fmt.sampleRate,
                audioData.fmt.bitsPerSample);

            AudioUtils::saveWav(thresholdData, std::string(SPECTRUM_FILE_DIR) + std::to_string(threshold) + ".wav");
        }

        this->reconstructedSamples = AudioAnalyser::reconstruct(this->spectrum, numSamples, audioData.fmt.sampleRate);
        AudioUtils::AudioData fullData = AudioUtils::prepareSamplesToBeSaved(
            this->reconstructedSamples, audioData.fmt.numChannels, audioData.fmt.sampleRate, audioData.fmt.bitsPerSample);
        
        AudioUtils::saveWav(fullData, OUTPUT_FILE_DIR); });

    watcher.setFuture(future);
}

void MainWindow::onAnalysisFinished()
{
    overviewWidget->displayData(audioData, spectrum, reconstructedSamples);

    int thesholdDataCount = freqWidget->findChild<QSpinBox *>()->value();

    auto thesholdData = spectrum;
    std::sort(thesholdData.begin(), thesholdData.end(), [](const auto &a, const auto &b)
              { return a.amplitude > b.amplitude; });

    if (thesholdData.size() > (size_t)thesholdDataCount)
        thesholdData.resize(thesholdDataCount);

    freqWidget->setFrequencyData(thesholdData, audioData.getNumSamples(), audioData.fmt.sampleRate);
}

void MainWindow::showFirstView()
{
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::showSecondView()
{
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::showThirdView()
{
    stackedWidget->setCurrentIndex(2);
}