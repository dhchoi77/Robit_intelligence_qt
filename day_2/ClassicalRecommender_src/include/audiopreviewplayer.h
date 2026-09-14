#ifndef AUDIOPREVIEWPLAYER_H
#define AUDIOPREVIEWPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTimer>
#include <QFileInfo>
#include "track.h"

// QMediaPlayer 를 감싸서 '5초 미리듣기'와 전체 재생을 담당하는 클래스.
class AudioPreviewPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AudioPreviewPlayer(QObject *parent = nullptr);

    // 곡의 previewStart 지점부터 durationMs(기본 5초)만큼만 재생.
    void playPreview(const Track &track, int durationMs = 5000);

    // 곡을 처음부터 재생(최종 플레이리스트용).
    void playFull(const Track &track);

    void stop();

signals:
    // 오디오 오류가 났을 때 UI 에 알린다.
    void errorOccurred(const QString &message);

    // 전체 재생(playFull)이 곡 끝까지 도달했을 때 알린다.
    // 미리듣기(playPreview)에서는 발생하지 않는다.
    void finished();

private slots:
    void handleError(QMediaPlayer::Error error, const QString &errorString);
    void handleMediaStatus(QMediaPlayer::MediaStatus status);
    void handlePosition(qint64 pos);

private:
    // 상대 경로면 실행 파일 폴더 기준으로 바꿔서 실제 파일 위치를 찾는다.
    QFileInfo resolveFile(const Track &track) const;

    // 5초 미리듣기 카운트다운을 시작한다(음소거 해제 + 타이머 시작).
    void startPreviewTiming();

    QMediaPlayer *m_player;
    QAudioOutput *m_output;
    QTimer m_previewTimer;          // 미리듣기 정지용 타이머
    qint64 m_pendingSeekMs = 0;     // 이동할 지점(밀리초)
    int m_previewDurationMs = 5000;
    bool m_previewMode = false;
    bool m_playIssued = false;      // 로딩 후 play() 를 이미 호출했는지
    bool m_seekDone = false;        // 이번 미리듣기에서 이동을 이미 처리했는지
    bool m_timingStarted = false;   // 5초 카운트다운을 이미 시작했는지
    int m_previewGen = 0;           // 미리듣기 세대(빠른 재클릭 시 옛 작업 무효화)
};

#endif // AUDIOPREVIEWPLAYER_H
