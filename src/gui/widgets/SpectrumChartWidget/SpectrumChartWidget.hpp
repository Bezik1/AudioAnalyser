#pragma once

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QVBoxLayout>
#include <QBrush>
#include <QColor>
#include "../../../core/AudioAnalyser/AudioAnalyser.hpp"

class SpectrumChartWidget : public QWidget
{
    Q_OBJECT
public:
    SpectrumChartWidget(const std::vector<AudioAnalyser::FrequencyData> &spectrum, QWidget *parent = nullptr);
};