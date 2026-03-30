#include "MainWindow.hpp"
#include <QLabel>
#include "../widgets/MainWidget/MainWidget.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupLayout();
    startAsyncAnalysis(TEST_PATH);
}

MainWindow::~MainWindow() {}

void MainWindow::setupLayout()
{
    centralWidget = new QWidget(this);
    mainLayout = new QVBoxLayout(centralWidget);

    menuLayout = new QHBoxLayout();

    btnView1 = new QPushButton("Overview", this);
    btnView2 = new QPushButton("Frequency Components", this);

    btnView1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    btnView2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    menuLayout->addStretch(1);
    menuLayout->addWidget(btnView1, 1);
    menuLayout->addSpacing(30);
    menuLayout->addWidget(btnView2, 1);
    menuLayout->addStretch(1);

    menuLayout->setSpacing(20);

    stackedWidget = new QStackedWidget(this);
    mainWidget = new MainWidget(this);
    freqWidget = new FrequencyComponentsWidget(this);

    stackedWidget->addWidget(mainWidget);
    stackedWidget->addWidget(freqWidget);

    mainLayout->addLayout(menuLayout);
    mainLayout->addWidget(stackedWidget);

    setCentralWidget(centralWidget);

    connect(btnView1, &QPushButton::clicked, this, &MainWindow::showFirstView);
    connect(btnView2, &QPushButton::clicked, this, &MainWindow::showSecondView);
    connect(freqWidget, &FrequencyComponentsWidget::topXChanged, this, [this](int newX)
    {
        this->onAnalysisFinished();
    });

}

void MainWindow::startAsyncAnalysis(const std::string &wavPath)
{
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

        for (int N : THRESHOLDS) {
            int count = std::min(N, static_cast<int>(sortedSpectrum.size()));
            std::vector<AudioAnalyser::FrequencyData> topN(sortedSpectrum.begin(), sortedSpectrum.begin() + count);
            auto recN = AudioAnalyser::reconstruct(topN, numSamples, audioData.fmt.sampleRate);
            AudioUtils::AudioData nData = AudioUtils::prepareSamplesToBeSaved(
                recN, audioData.fmt.numChannels, audioData.fmt.sampleRate, audioData.fmt.bitsPerSample);
            AudioUtils::saveWav(nData, std::string(SPECTRUM_FILE_DIR) + std::to_string(N) + ".wav");
        }

        this->reconstructedSamples = AudioAnalyser::reconstruct(this->spectrum, numSamples, audioData.fmt.sampleRate);
        AudioUtils::AudioData fullData = AudioUtils::prepareSamplesToBeSaved(
            this->reconstructedSamples, audioData.fmt.numChannels, audioData.fmt.sampleRate, audioData.fmt.bitsPerSample);
        AudioUtils::saveWav(fullData, OUTPUT_FILE_DIR); });

    watcher.setFuture(future);
}

void MainWindow::onAnalysisFinished()
{
    mainWidget->displayData(audioData, spectrum, reconstructedSamples);

    int xCount = freqWidget->findChild<QSpinBox *>()->value();

    auto topX = spectrum;
    std::sort(topX.begin(), topX.end(), [](const auto &a, const auto &b)
              { return a.amplitude > b.amplitude; });

    if (topX.size() > (size_t)xCount)
        topX.resize(xCount);

    freqWidget->setFrequencyData(topX, audioData.getNumSamples(), audioData.fmt.sampleRate);
}

void MainWindow::showFirstView()
{
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::showSecondView()
{
    stackedWidget->setCurrentIndex(1);
}