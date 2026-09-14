#ifndef KEYWORDPAGE_H
#define KEYWORDPAGE_H

#include <QWidget>
#include "keyword.h"
#include "musicfeatures.h"

class QSpinBox;

// 시작 화면: 플레이리스트 곡 수를 정하고, 8개의 키워드 중 하나를 고른다.
// "음악 추가" 버튼으로 새 곡을 등록할 수도 있다.
class KeywordPage : public QWidget
{
    Q_OBJECT
public:
    // maxPlaylistSize: 곡 수 선택의 최댓값(보통 라이브러리의 곡 수).
    explicit KeywordPage(const QVector<Keyword> &keywords,
                         int maxPlaylistSize,
                         QWidget *parent = nullptr);

    // 사용자가 정한 플레이리스트 곡 수.
    int playlistSize() const;

    // 곡이 추가되면 곡 수 선택의 최댓값을 갱신한다.
    void setMaxPlaylistSize(int maxSize);

signals:
    void keywordChosen(const Features &seed);
    void addMusicRequested(); // "음악 추가" 버튼

private:
    QSpinBox *m_sizeSpin;
};

#endif // KEYWORDPAGE_H
