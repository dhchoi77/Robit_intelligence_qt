#include "playlistpage.h"
#include "audiopreviewplayer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <algorithm>
#include <random>

PlaylistPage::PlaylistPage(AudioPreviewPlayer *player, QWidget *parent)
    : QWidget(parent), m_player(player)
{
    auto *root = new QVBoxLayout(this);

    auto *title = new QLabel(QStringLiteral("오늘의 추천 플레이리스트 🎧"), this);
    title->setObjectName(QStringLiteral("title"));
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    m_explainLabel = new QLabel(this);
    m_explainLabel->setWordWrap(true);
    m_explainLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_explainLabel);

    m_nowPlaying = new QLabel(this);
    m_nowPlaying->setAlignment(Qt::AlignCenter);
    root->addWidget(m_nowPlaying);

    m_list = new QListWidget(this);
    root->addWidget(m_list);
    connect(m_list, &QListWidget::itemDoubleClicked, this, [this]() {
        playIndex(m_list->currentRow());
    });

    auto *controls = new QHBoxLayout();

    m_playButton = new QPushButton(QStringLiteral("▶ 선택 곡 재생"), this);
    connect(m_playButton, &QPushButton::clicked, this, [this]() {
        playIndex(m_list->currentRow());
    });
    controls->addWidget(m_playButton);

    m_shuffleButton = new QPushButton(QStringLiteral("🔀 셔플"), this);
    connect(m_shuffleButton, &QPushButton::clicked, this, [this]() { shuffle(); });
    controls->addWidget(m_shuffleButton);

    auto *stopBtn = new QPushButton(QStringLiteral("■ 정지"), this);
    connect(stopBtn, &QPushButton::clicked, this, [this]() { m_player->stop(); });
    controls->addWidget(stopBtn);

    m_restartButton = new QPushButton(QStringLiteral("↺ 처음부터 다시"), this);
    connect(m_restartButton, &QPushButton::clicked, this, [this]() {
        m_player->stop();
        emit restartRequested();
    });
    controls->addWidget(m_restartButton);

    root->addLayout(controls);

    // 현재 곡이 끝까지 재생되면 자동으로 다음 곡으로 넘어간다.
    connect(m_player, &AudioPreviewPlayer::finished, this, &PlaylistPage::advanceToNext);
}

void PlaylistPage::setPlaylist(const QVector<Track> &tracks, const QString &explanation)
{
    m_tracks = tracks;
    m_current = -1;
    m_explainLabel->setText(explanation);
    m_nowPlaying->clear();
    refreshList();
    if (!m_tracks.isEmpty())
        m_list->setCurrentRow(0);
}

void PlaylistPage::refreshList()
{
    m_list->clear();
    for (const Track &t : m_tracks)
        m_list->addItem(QStringLiteral("%1 — %2").arg(t.composer(), t.title()));
}

void PlaylistPage::playIndex(int index)
{
    if (index < 0 || index >= m_tracks.size())
        return;
    m_current = index;
    m_list->setCurrentRow(index); // 재생 중인 곡을 목록에서 강조
    const Track &t = m_tracks[index];
    m_nowPlaying->setText(QStringLiteral("재생 중: %1 — %2").arg(t.composer(), t.title()));
    m_player->playFull(t);
}

void PlaylistPage::advanceToNext()
{
    if (m_current < 0)
        return; // 재생 중이 아니면 무시
    const int next = m_current + 1;
    if (next < m_tracks.size()) {
        playIndex(next); // 다음 곡 재생
    } else {
        m_current = -1;  // 플레이리스트 끝
        m_nowPlaying->setText(QStringLiteral("플레이리스트 재생 완료"));
    }
}

void PlaylistPage::shuffle()
{
    if (m_tracks.size() < 2)
        return;

    // 무작위로 순서 섞기 (매 실행마다 다른 순서)
    static std::mt19937 rng(std::random_device{}());
    std::shuffle(m_tracks.begin(), m_tracks.end(), rng);

    refreshList();      // 섞인 순서대로 목록 갱신
    m_current = -1;
    playIndex(0);       // 섞인 순서의 첫 곡부터 재생 (이후 자동 넘김도 이 순서를 따름)
}
