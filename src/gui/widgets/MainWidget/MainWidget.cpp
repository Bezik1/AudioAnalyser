#include "MainWidget.hpp"
#include "../SpectrumChartWidget/SpectrumChartWidget.hpp"
#include "../WaveformChartWidget/WaveformChartWidget.hpp"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
    layout = new QVBoxLayout(this);
    statusLabel = new QLabel("Analyzing Audio Data...", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel);
}

void MainWidget::displayData(const AudioUtils::AudioData &audioData,
                             const std::vector<AudioAnalyser::FrequencyData> &spectrum,
                             const std::vector<float> &reconstructedSamples)
{
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr)
    {
        if (item->widget())
            delete item->widget();
        delete item;
    }

    QHBoxLayout *topChartsLayout = new QHBoxLayout();
    auto *originalChart = new WaveformChartWidget(
        audioData.data.samples, audioData.fmt.sampleRate, "Input Audio File Spectrum");
    auto *reconstructedChart = new WaveformChartWidget(
        reconstructedSamples, audioData.fmt.sampleRate, "Output Audio File Spectrum");

    topChartsLayout->addWidget(originalChart, 1);
    topChartsLayout->addWidget(reconstructedChart, 1);

    layout->addLayout(topChartsLayout, 1);
    layout->addWidget(new SpectrumChartWidget(spectrum), 1);
}