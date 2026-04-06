#include "FrequencyComponentsWidget.hpp"
#include "../../styles/GlobalStyles.cpp"

FrequencyComponentsWidget::FrequencyComponentsWidget(QWidget *parent) : QWidget(parent)
{
    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    mediaPlayer->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.7);

    mainLayout = new QVBoxLayout(this);

    statusLabel = new QLabel(STATUS_LABEL_TEXT, this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet(GlobalStyles::StatusFontColor);
    mainLayout->addWidget(statusLabel);

    contentWidget = new QWidget(this);
    auto *contentLayout = new QVBoxLayout(contentWidget);

    auto *controlsLayout = new QHBoxLayout();

    controlsLayout->addWidget(new QLabel(COUNT_LABEL_TEXT, this));
    countSpinner = new QSpinBox(this);
    countSpinner->setRange(1, 10000);
    countSpinner->setValue(100);
    controlsLayout->addWidget(countSpinner);

    controlsLayout->addWidget(new QLabel(VISIBLE_COUNT_LABEL_TEXT, this));
    visibleCountSpinner = new QSpinBox(this);
    visibleCountSpinner->setRange(1, 5000);
    visibleCountSpinner->setValue(5000);
    controlsLayout->addWidget(visibleCountSpinner);

    controlsLayout->addStretch();
    playButton = new QPushButton(PLAY_BTN_TEXT, this);
    controlsLayout->addWidget(playButton);

    saveButton = new QPushButton(SAVE_BTN_TEXT, this);
    controlsLayout->addWidget(saveButton);

    contentLayout->addLayout(controlsLayout);

    summaryChartView = new QChartView(this);
    summaryChartView->setMinimumHeight(300);
    summaryChartView->setRenderHint(QPainter::Antialiasing);

    auto *summaryChart = new QChart();
    auto *summarySeries = new QLineSeries();
    summaryChart->addSeries(summarySeries);
    summaryChart->legend()->hide();
    summaryChart->setBackgroundBrush(GlobalStyles::BackgroundColor);
    summaryChart->setTitleBrush(QBrush(Qt::white));
    summaryChart->setTitle(SUMMARY_CHART_TEXT);

    auto *axisX = new QValueAxis();
    axisX->setLabelsColor(Qt::white);
    axisX->setLabelFormat("%d");
    summaryChart->addAxis(axisX, Qt::AlignBottom);
    summarySeries->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setLabelsColor(Qt::white);
    summaryChart->addAxis(axisY, Qt::AlignLeft);
    summarySeries->attachAxis(axisY);

    summaryChartView->setChart(summaryChart);
    contentLayout->addWidget(summaryChartView);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollContainer = new QWidget();
    scrollLayout = new QVBoxLayout(scrollContainer);
    scrollArea->setWidget(scrollContainer);
    contentLayout->addWidget(scrollArea);

    mainLayout->addWidget(contentWidget);
    contentWidget->setVisible(false);

    connect(countSpinner, QOverload<int>::of(&QSpinBox::valueChanged), this, &FrequencyComponentsWidget::topXChanged);

    connect(saveButton, &QPushButton::clicked, this, [this]()
            { emit saveRequested(localFrequencies); });

    connect(playButton, &QPushButton::clicked, this, [this]()
            {
                QString modificationFilePath = QDir::currentPath() + "/data/eval/modified_output.wav";
                mediaPlayer->setSource(modificationFilePath);
                mediaPlayer->play(); });

    connect(visibleCountSpinner, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &FrequencyComponentsWidget::updateVisibleWidgets);
}

void FrequencyComponentsWidget::updateVisibleWidgets()
{
    int maxVisible = visibleCountSpinner->value();
    for (int i = 0; i < (int)freqWidgets.size(); i++)
        freqWidgets[i]->setVisible(i < maxVisible);
}

void FrequencyComponentsWidget::showAnalyzingStatus()
{
    contentWidget->setVisible(false);
    statusLabel->setVisible(true);
    statusLabel->setText(ANALYZING_TEXT);
    statusLabel->setStyleSheet(GlobalStyles::StatusFontColor);
}

void FrequencyComponentsWidget::setFrequencyData(const std::vector<AudioAnalyser::FrequencyData> &topFrequencies, int numSamples, int sampleRate)
{
    localFrequencies = topFrequencies;
    currentNumSamples = numSamples;
    currentSampleRate = sampleRate;

    if (localFrequencies.empty())
    {
        contentWidget->setVisible(false);
        statusLabel->setVisible(true);
        statusLabel->setText(STATUS_LABEL_TEXT);
        return;
    }

    statusLabel->setVisible(false);
    contentWidget->setVisible(true);

    while (freqWidgets.size() < localFrequencies.size())
    {
        int index = static_cast<int>(freqWidgets.size());
        auto *freqWidget = new IndividualFrequency(AudioAnalyser::FrequencyData{}, currentNumSamples, currentSampleRate);
        freqWidgets.push_back(freqWidget);
        scrollLayout->addWidget(freqWidget);

        connect(freqWidget, &IndividualFrequency::frequencyUpdated, this, [this, index, freqWidget]()
                {
            if (index < (int)this->localFrequencies.size()) {
                this->localFrequencies[index] = freqWidget->getData();
                this->updateSummaryChart();
            } });
    }

    for (size_t i = 0; i < freqWidgets.size(); i++)
    {
        if (i < localFrequencies.size())
            freqWidgets[i]->setData(localFrequencies[i]);
        else
            freqWidgets[i]->hide();
    }

    updateVisibleWidgets();
    updateSummaryChart();
}

void FrequencyComponentsWidget::updateSummaryChart()
{
    if (localFrequencies.empty())
        return;

    auto sumSamples = AudioAnalyser::reconstructFFT(localFrequencies, currentNumSamples, currentSampleRate);

    auto *chart = summaryChartView->chart();
    auto *series = qobject_cast<QLineSeries *>(chart->series().at(0));

    QList<QPointF> points;
    int previewLimit = std::min((int)sumSamples.size(), 10000);
    int stepSize = std::max(1, previewLimit / 1000);

    float maxAmp = 0.01f;
    for (int i = 0; i < previewLimit; i += stepSize)
    {
        float val = sumSamples[i];
        points.append(QPointF(i, val));
        if (std::abs(val) > maxAmp)
            maxAmp = std::abs(val);
    }

    series->replace(points);
    chart->axes(Qt::Horizontal).back()->setRange(0, static_cast<qreal>(previewLimit));
    chart->axes(Qt::Vertical).back()->setRange(-maxAmp * 1.2, maxAmp * 1.2);
}