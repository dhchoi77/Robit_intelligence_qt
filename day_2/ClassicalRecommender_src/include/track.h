#ifndef TRACK_H
#define TRACK_H

#include <QString>
#include <QJsonObject>
#include "musicfeatures.h"

// 곡 한 개의 정보를 담는 클래스.
class Track
{
public:
    Track() = default;

    int id() const { return m_id; }
    QString title() const { return m_title; }
    QString composer() const { return m_composer; }
    QString filePath() const { return m_filePath; }
    qint64 previewStartMs() const { return m_previewStartMs; }
    Features features() const { return m_features; }

    bool isValid() const { return m_id >= 0 && !m_filePath.isEmpty(); }

    // JSON 객체 하나를 Track 으로 변환한다.
    static Track fromJson(const QJsonObject &obj);

    // Track 을 JSON 객체로 변환한다(저장용).
    QJsonObject toJson() const;

private:
    int m_id = -1;
    QString m_title;
    QString m_composer;
    QString m_filePath;          // 오디오 파일 경로 (mp3/wav)
    qint64 m_previewStartMs = 0; // 미리듣기를 시작할 지점(밀리초)
    Features m_features;
};

#endif // TRACK_H
