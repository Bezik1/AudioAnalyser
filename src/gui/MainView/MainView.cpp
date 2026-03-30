#include "MainView.hpp"
#include "../../core/AudioAnalyser/AudioAnalysis.hpp"
#include <QtCharts>

MainView::MainView(const std::string &wavPath, QWidget *parent)
    : QWidget(parent)
{
    AudioUtils::AudioData audioData = AudioUtils::readWav(wavPath);

    layout = new QHBoxLayout(this);

    int displayWidth = this->screen()->geometry().width();
    int displayHeight = this->screen()->geometry().height();

    int numSamples = audioData.getNumSamples();
    int step = numSamples / displayWidth;
    if (step < 1)
        step = 1;

    double durationInSeconds = static_cast<double>(numSamples) / audioData.fmt.sampleRate;

    auto *originalChart = new QChart();
    originalChart->setBackgroundBrush(QBrush(QColor(53, 53, 53)));
    auto *originalSeries = new QLineSeries();

    for (int i = 0; i < numSamples; i += step)
    {
        double time = static_cast<double>(i) / audioData.fmt.sampleRate;
        float y = audioData.data.samples[i];
        originalSeries->append(time, y);
    }

    originalChart->addSeries(originalSeries);
    originalChart->createDefaultAxes();
    originalChart->setTitle("Original Audio Waveform");
    originalChart->setTitleBrush(QBrush(Qt::white));

    originalChart->axisX()->setRange(0, durationInSeconds);
    originalChart->axisY()->setRange(-1.0, 1.0);

    originalChart->axisX()->setLabelsColor(Qt::white);
    originalChart->axisX()->setTitleText("Time (s)");
    originalChart->axisX()->setTitleBrush(QBrush(Qt::white));
    originalChart->axisY()->setLabelsColor(Qt::white);
    originalChart->axisY()->setTitleText("Amplitude");
    originalChart->axisY()->setTitleBrush(QBrush(Qt::white));

    auto *originalChartView = new QChartView(originalChart);
    originalChartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(originalChartView, 1);

    // audioData.data.samples.resize(audioData.getNumSamples() * 0.01);
    auto spectrum = AudioAnalyser::discreteFourierTransform(audioData.data.samples, audioData.fmt.sampleRate);
    auto reconstructed = AudioAnalyser::reconstruct(spectrum, numSamples * 0.5, audioData.fmt.sampleRate);

    auto *reconstructedChart = new QChart();
    reconstructedChart->setBackgroundBrush(QBrush(QColor(53, 53, 53)));
    auto *reconstructedSeries = new QLineSeries();

    for (int i = 0; i < numSamples; i += step)
    {
        double time = static_cast<double>(i) / audioData.fmt.sampleRate;
        float y = reconstructed[i];
        reconstructedSeries->append(time, y);
    }

    reconstructedChart->addSeries(reconstructedSeries);
    reconstructedChart->createDefaultAxes();
    reconstructedChart->setTitle("Reconstructed Audio Waveform");
    reconstructedChart->setTitleBrush(QBrush(Qt::white));

    reconstructedChart->axisX()->setRange(0, durationInSeconds);
    reconstructedChart->axisY()->setRange(-1.0, 1.0);

    reconstructedChart->axisX()->setLabelsColor(Qt::white);
    reconstructedChart->axisX()->setTitleText("Time (s)");
    reconstructedChart->axisX()->setTitleBrush(QBrush(Qt::white));
    reconstructedChart->axisY()->setLabelsColor(Qt::white);
    reconstructedChart->axisY()->setTitleText("Amplitude");
    reconstructedChart->axisY()->setTitleBrush(QBrush(Qt::white));

    auto *reconstructedChartView = new QChartView(reconstructedChart);
    reconstructedChartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(reconstructedChartView, 1);
}