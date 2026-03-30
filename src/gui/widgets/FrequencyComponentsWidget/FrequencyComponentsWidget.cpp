#include "FrequencyComponentsWidget.hpp"

FrequencyComponentsWidget::FrequencyComponentsWidget(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);

    auto *controlsLayout = new QHBoxLayout();
    controlsLayout->addWidget(new QLabel("Liczba składowych (X):", this));
    countSpinner = new QSpinBox(this);
    countSpinner->setRange(1, 1000);
    countSpinner->setValue(20);
    controlsLayout->addWidget(countSpinner);
    controlsLayout->addStretch();
    mainLayout->addLayout(controlsLayout);

    connect(countSpinner, &QSpinBox::valueChanged, this, &FrequencyComponentsWidget::topXChanged);

    summaryChartView = new QChartView(this);
    summaryChartView->setMinimumHeight(300);
    mainLayout->addWidget(summaryChartView);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollContainer = new QWidget();
    scrollLayout = new QVBoxLayout(scrollContainer);
    scrollArea->setWidget(scrollContainer);
    mainLayout->addWidget(scrollArea);
}

void FrequencyComponentsWidget::setFrequencyData(const std::vector<AudioAnalyser::FrequencyData> &topFrequencies,
                                                 int numSamples, int sampleRate)
{
    QLayoutItem *child;
    while ((child = scrollLayout->takeAt(0)) != nullptr)
    {
        delete child->widget();
        delete child;
    }

    if (topFrequencies.empty())
        return;

    std::vector<float> sumSamples(numSamples, 0.0f);
    float maxAmp = 0.0f;
    for (const auto &f : topFrequencies)
        if (f.amplitude > maxAmp)
            maxAmp = f.amplitude;

    for (const auto &freq : topFrequencies)
    {
        std::vector<AudioAnalyser::FrequencyData> singleFreqVec = {freq};
        auto samples = AudioAnalyser::reconstruct(singleFreqVec, numSamples, sampleRate);

        for (int i = 0; i < numSamples; ++i)
            sumSamples[i] += samples[i];

        QString title = QString("Freq:uency %1 Hz | Amplitude: %2").arg(freq.frequency).arg(freq.amplitude);
        scrollLayout->addWidget(createIndividualChart(samples, title, freq.amplitude));
    }

    summaryChartView->setChart(createIndividualChart(sumSamples, "Frequency Composition", 0.0f)->chart());
}

QChartView *FrequencyComponentsWidget::createIndividualChart(const std::vector<float> &samples, const QString &title, float ampLimit)
{
    auto *chart = new QChart();
    auto *series = new QLineSeries();

    int stepSize = samples.size() * 0.001;
    for (int i = 0; i < samples.size(); i += stepSize)
    {
        series->append(i, samples[i]);
    }

    chart->addSeries(series);
    chart->setTitle(title);
    chart->legend()->hide();
    chart->setMargins(QMargins(5, 5, 5, 5));

    auto *axisX = new QValueAxis();
    axisX->setRange(0, samples.size());
    axisX->setLabelFormat("%d");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    if (ampLimit > 0)
    {
        axisY->setRange(-ampLimit * 1.1, ampLimit * 1.1);
    }
    else
    {
        float localMax = 0.1f;
        for (float s : samples)
            if (std::abs(s) > localMax)
                localMax = std::abs(s);
        axisY->setRange(-localMax * 1.2, localMax * 1.2);
    }
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->setBackgroundBrush(QBrush(QColor(53, 53, 53)));
    chart->setTitleBrush(QBrush(Qt::white));
    axisX->setTitleText("Amplitude");
    axisX->setLabelsColor(Qt::white);
    axisY->setTitleText("Saple Number");
    axisY->setLabelsColor(Qt::white);

    auto *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(250);
    return chartView;
}