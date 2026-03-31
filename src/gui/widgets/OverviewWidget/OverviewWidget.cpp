#include "OverviewWidget.hpp"
#include "../SpectrumChartWidget/SpectrumChartWidget.hpp"
#include "../WaveformChartWidget/WaveformChartWidget.hpp"

OverviewWidget::OverviewWidget(QWidget *parent) : QWidget(parent)
{
    layout = new QVBoxLayout(this);
    statusLabel = new QLabel("Please select an audio file first.", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("color: #888;");

    layout->addWidget(statusLabel);
}

void OverviewWidget::displayData(const AudioUtils::AudioData &audioData,
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

void OverviewWidget::showAnalyzingStatus()
{
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr)
    {
        if (item->widget())
            delete item->widget();
        delete item;
    }

    statusLabel = new QLabel("Analyzing Audio Data...", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("color: #888;");
    layout->addWidget(statusLabel);
}