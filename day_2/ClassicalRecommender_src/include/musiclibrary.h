#ifndef MUSICLIBRARY_H
#define MUSICLIBRARY_H

#include <QString>
#include <QVector>
#include "track.h"

// tracks.json 에서 곡 목록을 읽어와 보관하는 클래스.
class MusicLibrary
{
public:
    MusicLibrary() = default;

    // JSON 파일을 읽어 곡 목록을 채운다.
    // 성공하면 true, 실패하면 false 를 반환하고 errorString() 에 이유를 남긴다.
    bool loadFromFile(const QString &path);

    const QVector<Track> &tracks() const { return m_tracks; }
    int count() const { return m_tracks.size(); }
    QString errorString() const { return m_error; }

    // id 로 곡을 찾는다. 없으면 유효하지 않은 Track 을 돌려준다.
    Track trackById(int id) const;

    // 새 곡을 부여할 id(현재 최대 id + 1).
    int nextId() const;

    // 메모리상의 곡 목록에 곡을 추가한다.
    void addTrack(const Track &t);

    // 현재 곡 목록 전체를 JSON 파일로 저장한다. 실패 시 false + errorString().
    bool saveToFile(const QString &path);

private:
    QVector<Track> m_tracks;
    QString m_error;
};

#endif // MUSICLIBRARY_H
