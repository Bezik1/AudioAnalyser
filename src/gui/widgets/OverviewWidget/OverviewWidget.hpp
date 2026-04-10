#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <vector>
#include <string>

#include "../../../core/AudioAnalyser/AudioAnalyser.hpp"
#include "../../../utils/AudioUtils/AudioUtils.hpp"

class OverviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OverviewWidget(QWidget *parent = nullptr);
    void displayData(
        const AudioUtils::AudioData &audioData,
        const std::vector<AudioAnalyser::FrequencyData> &spectrum,
        const std::vector<double> &reconstructedSamples);

    void showAnalyzingStatus();

private:
    inline static const QString ANALYZING_STATUS_TEXT = "Analyzing Audio Data...";
    inline static const QString SELECT_FILE_TEXT = "Please select an audio file first.";
    inline static const QString INPUT_CHART_TEXT = "Input Audio File Spectrum";
    inline static const QString OUTPUT_CHART_TEXT = "Output Audio File Spectrum";

    QVBoxLayout *layout = nullptr;
    QLabel *statusLabel = nullptr;
};