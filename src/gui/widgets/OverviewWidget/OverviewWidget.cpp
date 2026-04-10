#include "OverviewWidget.hpp"

#include "../SpectrumChartWidget/SpectrumChartWidget.hpp"
#include "../WaveformChartWidget/WaveformChartWidget.hpp"

#include "../../styles/GlobalStyles.cpp"

OverviewWidget::OverviewWidget(QWidget *parent) : QWidget(parent)
{
    layout = new QVBoxLayout(this);
    statusLabel = new QLabel(SELECT_FILE_TEXT, this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet(GlobalStyles::StatusFontColor);
    layout->addWidget(statusLabel);
}

void OverviewWidget::displayData(const AudioUtils::AudioData &audioData,
                                 const std::vector<AudioAnalyser::FrequencyData> &spectrum,
                                 const std::vector<double> &reconstructedSamples)
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
        audioData.data.samples, audioData.fmt.sampleRate, INPUT_CHART_TEXT);
    auto *reconstructedChart = new WaveformChartWidget(
        reconstructedSamples, audioData.fmt.sampleRate, OUTPUT_CHART_TEXT);

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

    statusLabel = new QLabel(ANALYZING_STATUS_TEXT, this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet(GlobalStyles::StatusFontColor);
    layout->addWidget(statusLabel);
}