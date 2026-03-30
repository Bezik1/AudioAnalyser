#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QSpinBox>
#include <QLabel>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <vector>
#include "../../../core/AudioAnalyser/AudioAnalyser.hpp"

class FrequencyComponentsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FrequencyComponentsWidget(QWidget *parent = nullptr);
    void setFrequencyData(const std::vector<AudioAnalyser::FrequencyData> &topFrequencies,
                          int numSamples, int sampleRate);

signals:
    void topXChanged(int newX);

private:
    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;
    QWidget *scrollContainer;
    QVBoxLayout *scrollLayout;
    QChartView *summaryChartView;
    QSpinBox *countSpinner;

    QChartView *createIndividualChart(const std::vector<float> &samples, const QString &title, float ampLimit = 0.0f);
};