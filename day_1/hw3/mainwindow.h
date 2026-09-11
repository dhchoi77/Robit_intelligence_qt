#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QByteArray>
#include <QColor>

class QComboBox;
class QPushButton;
class QLineEdit;
class QTextEdit;
class QCheckBox;
class QLabel;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void refreshPorts();          // 필수: 시리얼 장치 자동 검색
    void onOpenCloseClicked();    // 필수: OPEN/CLOSE 버튼 통일
    void onWriteClicked();        // WRITE 버튼 / Enter
    void onClearClicked();        // 필수: Clear 버튼
    void onReadyRead();           // 데이터 수신

private:
    // ---- UI 생성 ----
    void buildUi();
    QLabel *makeLed() const;

    // ---- 동작 로직 ----
    void appendLog(const QString &text, const QColor &color);
    void flashLed(QLabel *led, const QString &colorHex);
    void resetLed(QLabel *led) const;
    void setPortControlsEnabled(bool enabled);
    void processIncomingBytes(const QByteArray &bytes);

    // ---- 보너스: 패킷 분석/보내기 ----
    // 패킷 포맷: [STX 0x02][LEN][DATA...(LEN bytes)][CHK][ETX 0x03]
    // CHK = DATA 바이트들의 XOR
    QByteArray buildPacket(const QByteArray &payload) const;
    void tryExtractPackets();

    // Device / BaudRate / OPEN
    QComboBox   *deviceCombo   = nullptr;
    QComboBox   *baudCombo     = nullptr;
    QPushButton *openButton    = nullptr;

    // 입력 / WRITE
    QLineEdit   *inputLine     = nullptr;
    QPushButton *writeButton   = nullptr;

    // 보너스: RX 표시 형식 / 패킷 모드
    QComboBox   *rxFormatCombo   = nullptr; // ASCII / HEX
    QCheckBox   *packetModeCheck = nullptr;

    // 로그 표시
    QTextEdit   *logView       = nullptr;

    // 하단 컨트롤
    QPushButton *clearButton         = nullptr;
    QLabel      *rxLedLabel          = nullptr;
    QLabel      *txLedLabel          = nullptr;
    QCheckBox   *clearWhenWriteCheck = nullptr;
    QCheckBox   *autoScrollCheck     = nullptr;

    QSerialPort *serial        = nullptr;
    QTimer      *portScanTimer = nullptr;
    QByteArray   rxBuffer;      // 패킷 모드용 누적 수신 버퍼
};

#endif // MAINWINDOW_H