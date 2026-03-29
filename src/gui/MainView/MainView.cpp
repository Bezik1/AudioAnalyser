#include "MainView.hpp"

MainView::MainView(const std::string &wavPath, QWidget *parent)
    : QWidget(parent)
{
    AudioUtils::AudioData audioData;
    audioData = AudioUtils::readWav(wavPath);

    layout = new QHBoxLayout(this);

    chart = new QChart();
    series = new QLineSeries();

    int displayWidth = this->screen()->geometry().width();
    int displayHeight = this->screen()->geometry().height();

    int numSamples = audioData.getNumSamples();
    int step = numSamples / displayWidth;
    if (step < 1)
        step = 1;

    double durationInSeconds = (double)numSamples / audioData.fmt.sampleRate;

    for (int i = 0; i < numSamples; i += step)
    {
        double timeInSeconds = (double)i / audioData.fmt.sampleRate;
        float y = audioData.data.samples[i];
        series->append(timeInSeconds, y);
    }

    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Audio Waveform");
    chart->setTitleBrush(QBrush(Qt::white));
    chart->setTitleFont(QFont("Arial", 24, QFont::Bold));
    chart->axisX()->setRange(0, durationInSeconds);
    chart->axisX()->setTitleText("Time (s)");
    chart->axisX()->setLabelsColor(Qt::white);
    chart->axisX()->setTitleBrush(QBrush(Qt::white));
    chart->axisY()->setRange(-1.0, 1.0);
    chart->axisY()->setTitleText("Amplitude");
    chart->axisY()->setLabelsColor(Qt::white);
    chart->axisY()->setTitleBrush(QBrush(Qt::white));
    chart->setBackgroundBrush(QBrush(QColor(53, 53, 53)));
    chart->legend()->hide();

    auto *lineChartView = new QChartView(chart);
    lineChartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(lineChartView, 1);

    auto *radialSeries = new QLineSeries();
    
    double centerX = 0.0;
    double centerY = 0.0;
    int size = std::min(displayWidth, displayHeight);

    for (int i = 0; i < numSamples; i += step)
    {
        double time = i / (double) numSamples;
        double angle = time * 2 * M_PI;

        double x = audioData.data.samples[i] *  cos(angle);
        double y = audioData.data.samples[i] * sin(angle);
        radialSeries->append(x, y);
    }
}