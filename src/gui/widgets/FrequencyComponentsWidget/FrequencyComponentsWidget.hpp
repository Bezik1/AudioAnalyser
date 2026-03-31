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

    void showAnalyzingStatus();
    void setFrequencyData(const std::vector<AudioAnalyser::FrequencyData> &topFrequencies,
                          int numSamples, int sampleRate);

signals:
    void topXChanged(int newX);

private:
    QVBoxLayout *mainLayout = nullptr;
    QScrollArea *scrollArea = nullptr;
    QWidget *scrollContainer = nullptr;

    QWidget *contentWidget = nullptr;
    QLabel *statusLabel = nullptr;

    QVBoxLayout *scrollLayout = nullptr;
    QChartView *summaryChartView = nullptr;
    QSpinBox *countSpinner = nullptr;

    QChartView *createIndividualChart(const std::vector<float> &samples, const QString &title, float ampLimit = 0.0f);
};