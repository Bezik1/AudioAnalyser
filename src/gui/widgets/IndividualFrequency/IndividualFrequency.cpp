#include "IndividualFrequency.hpp"

#include "../../styles/GlobalStyles.cpp"

IndividualFrequency::IndividualFrequency(const AudioAnalyser::FrequencyData &data, int numSamples, int sampleRate, QWidget *parent)
    : QWidget(parent), data(data), numSamples(numSamples), sampleRate(sampleRate)
{
    setupUi();
    updateChart();
}

AudioAnalyser::FrequencyData IndividualFrequency::getData() const
{
    return data;
}

void IndividualFrequency::setData(const AudioAnalyser::FrequencyData &newData)
{
    data = newData;

    frequencyLabel->setText(FREQUENCY_LABEL_TEXT.arg(data.frequency));

    ampSpin->blockSignals(true);
    phaseSpin->blockSignals(true);

    ampSpin->setValue(data.amplitude);
    phaseSpin->setValue(data.phase);

    ampSpin->blockSignals(false);
    phaseSpin->blockSignals(false);

    updateChart();
}

void IndividualFrequency::setupUi()
{
    auto *layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignVCenter);

    auto *controlsCol = new QVBoxLayout();
    controlsCol->setSpacing(5);

    frequencyLabel = new QLabel(FREQUENCY_LABEL_TEXT.arg(data.frequency));
    frequencyLabel->setStyleSheet(GlobalStyles::FontBasicColor);

    ampSpin = new QDoubleSpinBox();
    ampSpin->setRange(0.0, 0.1);
    ampSpin->setSingleStep(0.0001);
    ampSpin->setDecimals(4);
    ampSpin->setValue(data.amplitude);
    ampSpin->setPrefix(AMPLITUDE_TEXT);

    phaseSpin = new QDoubleSpinBox();
    phaseSpin->setRange(-M_PI, M_PI);
    phaseSpin->setSingleStep(0.05);
    phaseSpin->setDecimals(4);
    phaseSpin->setValue(data.phase);
    phaseSpin->setPrefix(PHASE_TEXT);

    controlsCol->addWidget(frequencyLabel);
    controlsCol->addWidget(ampSpin);
    controlsCol->addWidget(phaseSpin);
    controlsCol->addStretch();

    chartView = createChart();
    layout->addLayout(controlsCol);
    layout->addWidget(chartView, 1);
    layout->setContentsMargins(5, 2, 5, 2);

    auto onValueChanged = [this]()
    {
        data.amplitude = static_cast<float>(ampSpin->value());
        data.phase = static_cast<float>(phaseSpin->value());
        updateChart();
        emit frequencyUpdated();
    };

    connect(ampSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onValueChanged);
    connect(phaseSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onValueChanged);
}

QChartView *IndividualFrequency::createChart()
{
    auto *chart = new QChart();
    series = new QLineSeries();
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setMargins(QMargins(2, 2, 2, 2));
    chart->setBackgroundBrush(GlobalStyles::BackgroundColor);

    auto *axisX = new QValueAxis();
    axisX->setRange(0, numSamples);
    axisX->hide();
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    auto *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setMinimumHeight(250);
    return view;
}

void IndividualFrequency::updateChart()
{
    std::vector<AudioAnalyser::FrequencyData> single = {data};
    auto samples = AudioAnalyser::reconstructDFT(single, numSamples, sampleRate);

    QList<QPointF> points;
    int stepSize = std::max(1, (int)(samples.size() * 0.001));
    for (int j = 0; j < (int)samples.size(); j += stepSize)
        points.append(QPointF(j, samples[j]));

    series->replace(points);
    float amp = std::abs(data.amplitude);
    chartView->chart()->axes(Qt::Vertical).back()->setRange(-amp * 1.1 - 0.01, amp * 1.1 + 0.01);
}