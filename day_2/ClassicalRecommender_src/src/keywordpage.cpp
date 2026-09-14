#include "keywordpage.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <algorithm>

KeywordPage::KeywordPage(const QVector<Keyword> &keywords,
                         int maxPlaylistSize, QWidget *parent)
    : QWidget(parent)
{
    auto *root = new QVBoxLayout(this);

    auto *title = new QLabel(QStringLiteral("오늘은 어떤 클래식이 듣고 싶나요?"), this);
    title->setObjectName(QStringLiteral("title"));
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    auto *sub = new QLabel(QStringLiteral("느낌 하나를 골라주세요."), this);
    sub->setAlignment(Qt::AlignCenter);
    root->addWidget(sub);

    // 플레이리스트 곡 수 선택
    const int maxSize = std::max(1, maxPlaylistSize);
    auto *sizeRow = new QHBoxLayout();
    sizeRow->addStretch();
    sizeRow->addWidget(new QLabel(QStringLiteral("플레이리스트 곡 수:"), this));
    m_sizeSpin = new QSpinBox(this);
    m_sizeSpin->setRange(1, maxSize);
    m_sizeSpin->setValue(std::min(6, maxSize)); // 기본 6곡(라이브러리가 작으면 그만큼)
    sizeRow->addWidget(m_sizeSpin);
    sizeRow->addStretch();
    root->addLayout(sizeRow);

    auto *grid = new QGridLayout();
    grid->setSpacing(12);
    for (int i = 0; i < keywords.size(); ++i) {
        const Keyword kw = keywords[i];
        auto *btn = new QPushButton(kw.name + QStringLiteral("\n") + kw.description, this);
        btn->setMinimumHeight(72);
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, [this, kw]() {
            emit keywordChosen(kw.seed);
        });
        grid->addWidget(btn, i / 4, i % 4); // 2행 4열
    }
    root->addLayout(grid);

    // 음악 추가 버튼
    auto *addBtn = new QPushButton(QStringLiteral("➕ 음악 추가"), this);
    addBtn->setCursor(Qt::PointingHandCursor);
    connect(addBtn, &QPushButton::clicked, this, [this]() {
        emit addMusicRequested();
    });
    root->addWidget(addBtn);

    root->addStretch();
}

int KeywordPage::playlistSize() const
{
    return m_sizeSpin->value();
}

void KeywordPage::setMaxPlaylistSize(int maxSize)
{
    m_sizeSpin->setMaximum(std::max(1, maxSize));
}
