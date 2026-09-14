#include "choicepage.h"
#include "audiopreviewplayer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QFrame>
#include <QStringList>
#include <QLayoutItem>

ChoicePage::ChoicePage(AudioPreviewPlayer *player, QWidget *parent)
    : QWidget(parent), m_player(player)
{
    auto *root = new QVBoxLayout(this);

    m_stageLabel = new QLabel(this);
    m_stageLabel->setObjectName(QStringLiteral("title"));
    m_stageLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_stageLabel);

    auto *hint = new QLabel(
        QStringLiteral("5초씩 들어보고, 가장 끌리는 곡을 골라주세요. (제목은 마지막에 공개돼요)"), this);
    hint->setAlignment(Qt::AlignCenter);
    hint->setWordWrap(true);
    root->addWidget(hint);

    auto *rowsHost = new QWidget(this);
    m_rowsLayout = new QVBoxLayout(rowsHost);
    m_rowsLayout->setSpacing(10);
    root->addWidget(rowsHost);

    m_selectGroup = new QButtonGroup(this);
    m_selectGroup->setExclusive(true);

    root->addStretch();

    m_nextButton = new QPushButton(QStringLiteral("다음 →"), this);
    m_nextButton->setEnabled(false);
    m_nextButton->setMinimumHeight(44);
    connect(m_nextButton, &QPushButton::clicked, this, [this]() {
        if (m_selectedIndex >= 0 && m_selectedIndex < m_tracks.size()) {
            m_player->stop();
            emit trackChosen(m_tracks[m_selectedIndex]);
        }
    });
    root->addWidget(m_nextButton);
}

void ChoicePage::clearRows()
{
    // 버튼 그룹에서 기존 버튼 제거
    const auto buttons = m_selectGroup->buttons();
    for (auto *b : buttons)
        m_selectGroup->removeButton(b);

    // 레이아웃의 위젯 제거
    QLayoutItem *item;
    while ((item = m_rowsLayout->takeAt(0)) != nullptr) {
        if (QWidget *w = item->widget())
            w->deleteLater();
        delete item;
    }
}

void ChoicePage::setChoices(const QVector<Track> &tracks, int stage, int totalStages)
{
    m_tracks = tracks;
    m_selectedIndex = -1;
    m_stageLabel->setText(QStringLiteral("%1단계 / 총 %2단계").arg(stage).arg(totalStages));
    clearRows();
    m_nextButton->setEnabled(false);

    const QStringList labels = { "A", "B", "C", "D", "E" };
    for (int i = 0; i < m_tracks.size(); ++i) {
        auto *row = new QFrame();
        row->setObjectName(QStringLiteral("card"));
        auto *h = new QHBoxLayout(row);

        auto *name = new QLabel(
            QStringLiteral("곡 %1").arg(labels.value(i, QString::number(i + 1))));
        name->setMinimumWidth(60);
        h->addWidget(name);

        const Track t = m_tracks[i];

        auto *preview = new QPushButton(QStringLiteral("▶ 5초 듣기"));
        preview->setCursor(Qt::PointingHandCursor);
        connect(preview, &QPushButton::clicked, this, [this, t]() {
            m_player->playPreview(t, 5000);
        });
        h->addWidget(preview);

        auto *select = new QPushButton(QStringLiteral("이 곡 선택"));
        select->setCheckable(true);
        select->setCursor(Qt::PointingHandCursor);
        m_selectGroup->addButton(select, i);
        connect(select, &QPushButton::clicked, this, [this, i]() {
            m_selectedIndex = i;
            updateNextEnabled();
        });
        h->addWidget(select);

        m_rowsLayout->addWidget(row);
    }
}

void ChoicePage::updateNextEnabled()
{
    m_nextButton->setEnabled(m_selectedIndex >= 0);
}
