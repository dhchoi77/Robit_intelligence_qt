#ifndef ADDMUSICDIALOG_H
#define ADDMUSICDIALOG_H

#include <QDialog>
#include "features.h"

class QLineEdit;
class QSpinBox;
class QSlider;
class QVBoxLayout;

// "음악 추가" 창: 파일 선택 + 제목/작곡가/미리듣기 지점 입력 + 느낌 슬라이더 5개.
// 입력만 모으고, 실제 복사/저장은 MainWindow 가 담당한다.
class AddMusicDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddMusicDialog(QWidget *parent = nullptr);

    QString sourceFilePath() const; // 사용자가 고른 원본 파일 경로
    QString title() const;
    QString composer() const;
    qint64 previewStartMs() const;  // 미리듣기 시작 지점(밀리초)
    Features features() const;      // 슬라이더로 정한 느낌 벡터

private slots:
    void browse();
    void validateAndAccept();

private:
    // 라벨 + 슬라이더(0~100) 한 줄을 만들어 레이아웃에 넣고 슬라이더를 돌려준다.
    QSlider *makeAxis(const QString &labelText, QVBoxLayout *into);

    QLineEdit *m_fileEdit;
    QLineEdit *m_titleEdit;
    QLineEdit *m_composerEdit;
    QSpinBox *m_previewSpin;
    QSlider *m_tempo;
    QSlider *m_mood;
    QSlider *m_energy;
    QSlider *m_weight;
    QSlider *m_era;
};

#endif // ADDMUSICDIALOG_H
