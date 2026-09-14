#ifndef CHOICEPAGE_H
#define CHOICEPAGE_H

#include <QWidget>
#include <QVector>
#include "track.h"

class QLabel;
class QPushButton;
class QButtonGroup;
class QVBoxLayout;
class AudioPreviewPlayer;

// 3곡 중 하나를 고르는 화면. 3단계에 걸쳐 재사용된다.
// 곡의 제목/작곡가는 숨기고, 오직 5초 미리듣기로만 고르게 한다(블라인드 선택).
class ChoicePage : public QWidget
{
    Q_OBJECT
public:
    explicit ChoicePage(AudioPreviewPlayer *player, QWidget *parent = nullptr);

    // 이번 단계에 보여줄 곡들과 단계 번호(1~totalStages)를 설정한다.
    void setChoices(const QVector<Track> &tracks, int stage, int totalStages);

signals:
    void trackChosen(const Track &track);

private:
    void clearRows();
    void updateNextEnabled();

    AudioPreviewPlayer *m_player;
    QLabel *m_stageLabel;
    QVBoxLayout *m_rowsLayout;
    QPushButton *m_nextButton;
    QButtonGroup *m_selectGroup;
    QVector<Track> m_tracks;
    int m_selectedIndex = -1;
};

#endif // CHOICEPAGE_H
