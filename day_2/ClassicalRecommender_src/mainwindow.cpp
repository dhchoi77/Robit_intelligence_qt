#include "mainwindow.h"
#include "keywordpage.h"
#include "choicepage.h"
#include "playlistpage.h"
#include "audiopreviewplayer.h"
#include "addmusicdialog.h"
#include "keyword.h"

#include <QStackedWidget>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("오늘의 클래식 추천기"));
    resize(720, 560);

    m_player = new AudioPreviewPlayer(this);
    connect(m_player, &AudioPreviewPlayer::errorOccurred, this, [this](const QString &msg) {
        QMessageBox::warning(this, QStringLiteral("오디오 오류"), msg);
    });

    // 곡 목록 로드 (프로젝트 루트의 tracks.json) — 실패 시 사용자에게 알린다.
    const QString dataPath = QStringLiteral(PROJECT_ROOT_DIR "/tracks.json");
    if (!m_library.loadFromFile(dataPath)) {
        QMessageBox::critical(this, QStringLiteral("불러오기 실패"),
                              m_library.errorString()
                                  + QStringLiteral("\n\n경로: ") + dataPath);
    }

    m_recommender = new Recommender(&m_library);

    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    m_keywordPage  = new KeywordPage(defaultKeywords(), m_library.count(), this);
    m_choicePage   = new ChoicePage(m_player, this);
    m_playlistPage = new PlaylistPage(m_player, this);

    m_stack->addWidget(m_keywordPage);  // index 0
    m_stack->addWidget(m_choicePage);   // index 1
    m_stack->addWidget(m_playlistPage); // index 2

    connect(m_keywordPage,  &KeywordPage::keywordChosen,   this, &MainWindow::onKeywordChosen);
    connect(m_keywordPage,  &KeywordPage::addMusicRequested, this, &MainWindow::onAddMusic);
    connect(m_choicePage,   &ChoicePage::trackChosen,      this, &MainWindow::onTrackChosen);
    connect(m_playlistPage, &PlaylistPage::restartRequested, this, &MainWindow::restart);

    m_stack->setCurrentWidget(m_keywordPage);
}

void MainWindow::onKeywordChosen(const Features &seed)
{
    if (m_library.count() < 3) {
        QMessageBox::warning(this, QStringLiteral("곡 부족"),
                             QStringLiteral("추천을 시작하려면 곡이 최소 3개 필요합니다."));
        return;
    }
    m_recommender->start(seed);
    m_playlistSize = m_keywordPage->playlistSize(); // 시작 화면에서 정한 곡 수
    m_currentStage = 1;
    m_choicePage->setChoices(m_recommender->nextChoices(3), m_currentStage, kTotalStages);
    m_stack->setCurrentWidget(m_choicePage);
}

void MainWindow::onTrackChosen(const Track &track)
{
    m_recommender->choose(track);

    if (m_currentStage < kTotalStages) {
        ++m_currentStage;
        m_choicePage->setChoices(m_recommender->nextChoices(3), m_currentStage, kTotalStages);
        m_stack->setCurrentWidget(m_choicePage);
    } else {
        // 3단계 완료 → 최종 플레이리스트 화면으로
        m_playlistPage->setPlaylist(m_recommender->finalPlaylist(m_playlistSize),
                                    m_recommender->explainProfile());
        m_stack->setCurrentWidget(m_playlistPage);
    }
}

void MainWindow::restart()
{
    m_currentStage = 0;
    m_stack->setCurrentWidget(m_keywordPage);
}

QString MainWindow::copyIntoMusic(const QString &src, QString &errorOut)
{
    const QFileInfo srcInfo(src);
    if (src.isEmpty() || !srcInfo.exists()) {
        errorOut = QStringLiteral("원본 파일을 찾을 수 없습니다.");
        return QString();
    }

    const QString musicDir = QStringLiteral(PROJECT_ROOT_DIR "/music");
    QDir().mkpath(musicDir); // 폴더가 없으면 만든다

    // 이름이 겹치면 뒤에 (1), (2) ... 를 붙여 덮어쓰지 않는다.
    const QString base = srcInfo.completeBaseName();
    const QString ext  = srcInfo.suffix();
    QString name = srcInfo.fileName();
    QString dest = musicDir + QStringLiteral("/") + name;
    int n = 1;
    while (QFileInfo::exists(dest)) {
        name = QStringLiteral("%1(%2).%3").arg(base).arg(n).arg(ext);
        dest = musicDir + QStringLiteral("/") + name;
        ++n;
    }

    if (!QFile::copy(srcInfo.absoluteFilePath(), dest)) {
        errorOut = QStringLiteral("파일 복사에 실패했습니다:\n%1").arg(dest);
        return QString();
    }
    return QStringLiteral("music/") + name; // tracks.json 에 저장할 상대경로
}

void MainWindow::onAddMusic()
{
    AddMusicDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    // 1) 파일을 music/ 로 복사
    QString err;
    const QString relPath = copyIntoMusic(dlg.sourceFilePath(), err);
    if (relPath.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("추가 실패"), err);
        return;
    }

    // 2) 입력값으로 Track 만들기 (id 는 자동 부여)
    QJsonObject obj;
    obj["id"]           = m_library.nextId();
    obj["title"]        = dlg.title();
    obj["composer"]     = dlg.composer();
    obj["file"]         = relPath;
    obj["previewStart"] = static_cast<double>(dlg.previewStartMs());
    obj["features"]     = dlg.features().toJson();
    m_library.addTrack(Track::fromJson(obj));

    // 3) tracks.json 에 저장 (다음 실행에도 남도록)
    const QString dataPath = QStringLiteral(PROJECT_ROOT_DIR "/tracks.json");
    if (!m_library.saveToFile(dataPath)) {
        QMessageBox::warning(this, QStringLiteral("저장 실패"), m_library.errorString());
        return;
    }

    // 4) UI 갱신 및 안내
    m_keywordPage->setMaxPlaylistSize(m_library.count());
    QMessageBox::information(this, QStringLiteral("추가 완료"),
        QStringLiteral("'%1' 곡이 추가되었습니다. (총 %2곡)")
            .arg(dlg.title()).arg(m_library.count()));
}
