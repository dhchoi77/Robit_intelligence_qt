#include "audiopreviewplayer.h"
#include <QCoreApplication>
#include <QUrl>

AudioPreviewPlayer::AudioPreviewPlayer(QObject *parent)
    : QObject(parent)
{
    m_player = new QMediaPlayer(this);
    m_output = new QAudioOutput(this);
    m_player->setAudioOutput(m_output);
    m_output->setVolume(0.8f);

    m_previewTimer.setSingleShot(true);
    connect(&m_previewTimer, &QTimer::timeout, this, [this]() {
        m_player->stop();
    });

    connect(m_player, &QMediaPlayer::errorOccurred,
            this, &AudioPreviewPlayer::handleError);
    connect(m_player, &QMediaPlayer::mediaStatusChanged,
            this, &AudioPreviewPlayer::handleMediaStatus);
    connect(m_player, &QMediaPlayer::positionChanged,
            this, &AudioPreviewPlayer::handlePosition);
}

QFileInfo AudioPreviewPlayer::resolveFile(const Track &track) const
{
    QFileInfo info(track.filePath());
    if (info.isRelative())
        info = QFileInfo(QStringLiteral(PROJECT_ROOT_DIR "/") + track.filePath());
    return info;
}

void AudioPreviewPlayer::playPreview(const Track &track, int durationMs)
{
    stop();

    const QFileInfo info = resolveFile(track);
    if (!info.exists()) { // 파일 존재 여부 확인 (예외 처리)
        emit errorOccurred(QStringLiteral("오디오 파일이 없습니다: %1").arg(info.filePath()));
        return;
    }

    m_previewMode = true;
    m_pendingSeekMs = track.previewStartMs();
    m_previewDurationMs = durationMs;
    m_playIssued = false;
    m_seekDone = false;
    m_timingStarted = false;

    // 이동(seek) 전 소리가 새어 나오지 않도록 잠깐 음소거.
    m_output->setMuted(true);

    // play() 는 곧바로 부르지 않고, 로딩이 끝난 뒤 handleMediaStatus 에서 부른다.
    // (앱 시작 직후 첫 재생이 씹히는 콜드 스타트 문제를 막기 위함)
    m_player->setSource(QUrl::fromLocalFile(info.absoluteFilePath()));

    // 안전장치: 위치 신호를 못 받는 경우를 대비해 일정 시간 뒤 강제로 카운트 시작.
    const int gen = m_previewGen;
    QTimer::singleShot(2000, this, [this, gen]() {
        if (gen == m_previewGen)
            startPreviewTiming();
    });
}

void AudioPreviewPlayer::playFull(const Track &track)
{
    stop();

    const QFileInfo info = resolveFile(track);
    if (!info.exists()) {
        emit errorOccurred(QStringLiteral("오디오 파일이 없습니다: %1").arg(info.filePath()));
        return;
    }

    m_previewMode = false;
    m_pendingSeekMs = 0;
    m_output->setMuted(false);

    m_player->setSource(QUrl::fromLocalFile(info.absoluteFilePath()));
    m_player->play();
}

void AudioPreviewPlayer::stop()
{
    ++m_previewGen; // 대기 중인 지연 작업 무효화
    m_previewTimer.stop();
    m_player->stop();
    m_output->setMuted(false);
}

void AudioPreviewPlayer::startPreviewTiming()
{
    if (m_timingStarted)
        return;
    m_timingStarted = true;
    m_output->setMuted(false);
    m_previewTimer.start(m_previewDurationMs);
}

void AudioPreviewPlayer::handleError(QMediaPlayer::Error error, const QString &errorString)
{
    if (error == QMediaPlayer::NoError)
        return;
    emit errorOccurred(QStringLiteral("재생 오류: %1").arg(errorString));
}

void AudioPreviewPlayer::handleMediaStatus(QMediaPlayer::MediaStatus status)
{
    // 미리듣기: 미디어가 로드/버퍼링되면 그때 재생을 시작한다(첫 재생 유실 방지).
    if (m_previewMode && !m_playIssued
        && (status == QMediaPlayer::LoadedMedia || status == QMediaPlayer::BufferedMedia)) {
        m_playIssued = true;
        m_player->play();
    }

    // 전체 재생이 곡 끝까지 도달하면 다음 곡으로 넘어가도록 알린다.
    if (status == QMediaPlayer::EndOfMedia && !m_previewMode)
        emit finished();
}

void AudioPreviewPlayer::handlePosition(qint64 pos)
{
    if (!m_previewMode)
        return;

    // 재생이 실제로 시작되어 위치가 움직이기 시작한 순간에 목표 지점으로 이동한다.
    if (!m_seekDone) {
        if (m_pendingSeekMs > 0)
            m_player->setPosition(m_pendingSeekMs);
        m_seekDone = true;
        return; // 다음 위치 신호에서 목표 도달을 확인
    }

    // 목표 지점에 도달하면(=실제로 소리가 나기 시작하면) 음소거를 풀고 5초 카운트 시작.
    if (!m_timingStarted && pos + 60 >= m_pendingSeekMs)
        startPreviewTiming();
}
