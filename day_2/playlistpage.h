#ifndef PLAYLISTPAGE_H
#define PLAYLISTPAGE_H

#include <QWidget>
#include <QVector>
#include "track.h"

class QLabel;
class QListWidget;
class QPushButton;
class AudioPreviewPlayer;

// 마지막 화면: 추천된 플레이리스트를 보여주고 재생한다.
// 여기서 처음으로 곡의 제목/작곡가가 공개된다.
class PlaylistPage : public QWidget
{
    Q_OBJECT
public:
    explicit PlaylistPage(AudioPreviewPlayer *player, QWidget *parent = nullptr);

    void setPlaylist(const QVector<Track> &tracks, const QString &explanation);

signals:
    void restartRequested();

private:
    void playIndex(int index);
    void advanceToNext(); // 현재 곡이 끝나면 다음 곡으로
    void shuffle();       // 플레이리스트 순서를 무작위로 섞는다
    void refreshList();   // m_tracks 순서대로 목록을 다시 채운다

    AudioPreviewPlayer *m_player;
    QLabel *m_explainLabel;
    QLabel *m_nowPlaying;  // 지금 재생 중인 곡 표시
    QListWidget *m_list;
    QPushButton *m_playButton;
    QPushButton *m_shuffleButton;
    QPushButton *m_restartButton;
    QVector<Track> m_tracks;
    int m_current = -1;
};

#endif // PLAYLISTPAGE_H
