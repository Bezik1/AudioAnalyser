#include "SpectrumChartWidget.hpp"

SpectrumChartWidget::SpectrumChartWidget(const std::vector<AudioAnalyser::FrequencyData> &spectrum, QWidget *parent)
    : QWidget(parent)
{
    auto *chart = new QChart();
    chart->setBackgroundBrush(QBrush(QColor(53, 53, 53)));
    auto *series = new QLineSeries();

    int sizeLimit = std::min(5512, static_cast<int>(spectrum.size()));
    for (int i = 0; i < sizeLimit; ++i)
        series->append(spectrum[i].frequency, spectrum[i].amplitude);

    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->legend()->hide();
    chart->setTitle("Frequency Spectrum");
    chart->setTitleBrush(QBrush(Qt::white));

    auto *axisX = qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).at(0));
    auto *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));
    if (axisX)
    {
        axisX->setLabelsColor(Qt::white);
        axisX->setTitleBrush(QBrush(Qt::white));
        axisX->setTitleText("Frequency (Hz)");
    }
    if (axisY)
    {
        axisY->setLabelsColor(Qt::white);
        axisY->setTitleBrush(QBrush(Qt::white));
        axisY->setTitleText("Amplitude");
    }

    auto *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(view);
}