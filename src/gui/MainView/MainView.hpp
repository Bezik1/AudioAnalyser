#pragma once

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QVBoxLayout>
#include <QLabel>
#include "../../utils/AudioUtils/AudioUtils.hpp"

class MainView : public QWidget
{
    Q_OBJECT
public:
    explicit MainView(const std::string &wavPath, QWidget *parent = nullptr);

private:
    QChart *chart;
    QHBoxLayout *layout;
    QLineSeries *series;
};