#include "addmusicdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QSlider>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>

AddMusicDialog::AddMusicDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("음악 추가"));
    resize(460, 540);

    auto *root = new QVBoxLayout(this);

    // 파일 선택
    auto *fileRow = new QHBoxLayout();
    m_fileEdit = new QLineEdit(this);
    m_fileEdit->setReadOnly(true);
    m_fileEdit->setPlaceholderText(QStringLiteral("mp3/wav 파일을 선택하세요"));
    auto *browseBtn = new QPushButton(QStringLiteral("찾아보기"), this);
    connect(browseBtn, &QPushButton::clicked, this, &AddMusicDialog::browse);
    fileRow->addWidget(m_fileEdit);
    fileRow->addWidget(browseBtn);
    root->addLayout(fileRow);

    // 제목 / 작곡가 / 미리듣기 지점
    auto *form = new QFormLayout();
    m_titleEdit = new QLineEdit(this);
    m_composerEdit = new QLineEdit(this);
    m_previewSpin = new QSpinBox(this);
    m_previewSpin->setRange(0, 3600);
    m_previewSpin->setSuffix(QStringLiteral(" 초"));
    form->addRow(QStringLiteral("제목"), m_titleEdit);
    form->addRow(QStringLiteral("작곡가"), m_composerEdit);
    form->addRow(QStringLiteral("미리듣기 시작"), m_previewSpin);
    root->addLayout(form);

    // 느낌 슬라이더 5개
    root->addWidget(new QLabel(QStringLiteral("이 곡의 느낌을 조절하세요:"), this));
    m_tempo  = makeAxis(QStringLiteral("빠르기  (느림 ↔ 빠름)"), root);
    m_mood   = makeAxis(QStringLiteral("분위기  (어두움 ↔ 밝음)"), root);
    m_energy = makeAxis(QStringLiteral("에너지  (차분 ↔ 강렬)"), root);
    m_weight = makeAxis(QStringLiteral("무게감  (가벼움/독주 ↔ 웅장/관현악)"), root);
    m_era    = makeAxis(QStringLiteral("시대    (바로크 ↔ 근현대)"), root);

    // 확인 / 취소
    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &AddMusicDialog::validateAndAccept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    root->addWidget(buttons);
}

QSlider *AddMusicDialog::makeAxis(const QString &labelText, QVBoxLayout *into)
{
    into->addWidget(new QLabel(labelText, this));
    auto *s = new QSlider(Qt::Horizontal, this);
    s->setRange(0, 100);
    s->setValue(50); // 기본값: 가운데
    into->addWidget(s);
    return s;
}

void AddMusicDialog::browse()
{
    const QString path = QFileDialog::getOpenFileName(
        this, QStringLiteral("음악 파일 선택"), QString(),
        QStringLiteral("오디오 파일 (*.mp3 *.wav *.flac *.ogg);;모든 파일 (*.*)"));
    if (!path.isEmpty()) {
        m_fileEdit->setText(path);
        if (m_titleEdit->text().isEmpty())
            m_titleEdit->setText(QFileInfo(path).completeBaseName());
    }
}

void AddMusicDialog::validateAndAccept()
{
    if (m_fileEdit->text().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("입력 필요"),
                             QStringLiteral("음악 파일을 선택하세요."));
        return;
    }
    if (m_titleEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("입력 필요"),
                             QStringLiteral("제목을 입력하세요."));
        return;
    }
    accept();
}

QString AddMusicDialog::sourceFilePath() const
{
    return m_fileEdit->text();
}

QString AddMusicDialog::title() const
{
    return m_titleEdit->text().trimmed();
}

QString AddMusicDialog::composer() const
{
    const QString c = m_composerEdit->text().trimmed();
    return c.isEmpty() ? QStringLiteral("(미상)") : c;
}

qint64 AddMusicDialog::previewStartMs() const
{
    return static_cast<qint64>(m_previewSpin->value()) * 1000;
}

Features AddMusicDialog::features() const
{
    return Features(m_tempo->value()  / 100.0,
                    m_mood->value()   / 100.0,
                    m_energy->value() / 100.0,
                    m_weight->value() / 100.0,
                    m_era->value()    / 100.0);
}
