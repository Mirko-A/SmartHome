#include "speakers_panel.h"

#include <QSignalBlocker>

#include "ui_speakers_panel.h"
SpeakersPanel::SpeakersPanel(QWidget *parent)
    : QWidget(parent), m_ui(std::make_unique<Ui::SpeakersPanel>()) {
    m_ui->setupUi(this);
    m_ui->volumeSlider->setRange(0, 100);
    connect(m_ui->volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        emit valueRequested(smart_home::gui::SpeakerControl::VOLUME, value);
    });
    m_ui->bassSlider->setRange(0, 100);
    connect(m_ui->bassSlider, &QSlider::valueChanged, this, [this](int value) {
        emit valueRequested(smart_home::gui::SpeakerControl::BASS, value);
    });
    m_ui->pitchSlider->setRange(0, 100);
    connect(m_ui->pitchSlider, &QSlider::valueChanged, this, [this](int value) {
        emit valueRequested(smart_home::gui::SpeakerControl::PITCH, value);
    });
}
SpeakersPanel::~SpeakersPanel() = default;
void SpeakersPanel::render(const SpeakerSettings &snapshot, bool available) {
    const QSignalBlocker volumeBlocker(m_ui->volumeSlider);
    m_ui->volumeSlider->setEnabled(available);
    m_ui->volumeSlider->setValue(snapshot.volume);
    m_ui->volumeSliderValueLabel->setText(QString::number(snapshot.volume));
    const QSignalBlocker bassBlocker(m_ui->bassSlider);
    m_ui->bassSlider->setEnabled(available);
    m_ui->bassSlider->setValue(snapshot.bass);
    m_ui->bassSliderValueLabel->setText(QString::number(snapshot.bass));
    const QSignalBlocker pitchBlocker(m_ui->pitchSlider);
    m_ui->pitchSlider->setEnabled(available);
    m_ui->pitchSlider->setValue(snapshot.pitch);
    m_ui->pitchSliderValueLabel->setText(QString::number(snapshot.pitch));
}
