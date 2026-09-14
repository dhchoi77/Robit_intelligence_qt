#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "musiclibrary.h"
#include "recommender.h"
#include "musicfeatures.h"
#include "track.h"

class QStackedWidget;
class KeywordPage;
class ChoicePage;
class PlaylistPage;
class AudioPreviewPlayer;

// 전체 흐름을 조율하는 메인 윈도우.
// 키워드 → 1단계 → 2단계 → 3단계 → 플레이리스트 를 QStackedWidget 으로 넘긴다.
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onKeywordChosen(const Features &seed);
    void onTrackChosen(const Track &track);
    void onAddMusic(); // "음악 추가" 처리
    void restart();

private:
    // 원본 파일을 프로젝트 루트의 music/ 로 복사하고 상대경로("music/이름")를 돌려준다.
    // 실패하면 빈 문자열을 돌려주고 errorOut 에 이유를 담는다.
    QString copyIntoMusic(const QString &src, QString &errorOut);

    static const int kTotalStages = 3;

    MusicLibrary m_library;
    Recommender *m_recommender = nullptr;
    AudioPreviewPlayer *m_player = nullptr;

    QStackedWidget *m_stack = nullptr;
    KeywordPage *m_keywordPage = nullptr;
    ChoicePage *m_choicePage = nullptr;
    PlaylistPage *m_playlistPage = nullptr;

    int m_currentStage = 0; // 0 = 시작 전, 1~3 = 진행 단계
    int m_playlistSize = 6; // 사용자가 시작 화면에서 정한 최종 곡 수
};

#endif // MAINWINDOW_H
