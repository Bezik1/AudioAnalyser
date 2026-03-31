#include "WaveformChartWidget.hpp"

WaveformChartWidget::WaveformChartWidget(const std::vector<float> &samples, int sampleRate, const std::string &title, QWidget *parent)
    : QWidget(parent)
{
    chart = new QChart();
    chart->setBackgroundBrush(QBrush(QColor(53, 53, 53)));
    series = new QLineSeries();

    int screenWidth = 1200;
    if (this->screen())
    {
        screenWidth = this->screen()->geometry().width();
    }

    int step = std::max(1, static_cast<int>(samples.size() / screenWidth));
    for (int i = 0; i < samples.size(); i += step)
        series->append(static_cast<double>(i) / sampleRate, samples[i]);

    chart->setTitle(QString::fromStdString(title));
    chart->setTitleBrush(QBrush(Qt::white));

    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->legend()->hide();

    auto *axisX = qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).at(0));
    auto *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));
    if (axisY)
    {
        axisX->setLabelsColor(Qt::white);
        axisX->setTitleText("Time (s)");
        axisX->setTitleBrush(QBrush(Qt::white));
        axisX->setRange(0, static_cast<double>(samples.size()) / sampleRate);
        axisX->setTitleBrush(QBrush(Qt::white));
    }
    if (axisX)
    {
        axisY->setLabelsColor(Qt::white);
        axisY->setRange(-1.0, 1.0);
        axisY->setTitleText("Amplitude");
        axisY->setTitleBrush(QBrush(Qt::white));
    }

    auto *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(view);
}