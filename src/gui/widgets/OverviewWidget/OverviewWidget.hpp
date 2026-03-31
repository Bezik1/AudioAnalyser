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
    void displayData(const AudioUtils::AudioData &audioData,
                     const std::vector<AudioAnalyser::FrequencyData> &spectrum,
                     const std::vector<float> &reconstructedSamples);

    void showAnalyzingStatus();

private:
    QVBoxLayout *layout = nullptr;
    QLabel *statusLabel = nullptr;
};