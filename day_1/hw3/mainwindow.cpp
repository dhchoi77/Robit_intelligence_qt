#include "mainwindow.h"

#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSerialPortInfo>
#include <QTextCursor>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    buildUi();

    serial = new QSerialPort(this);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::onReadyRead);

    // 필수: 시리얼 장치 자동 검색 -> 포트가 닫혀있는 동안 주기적으로 리스트 갱신
    portScanTimer = new QTimer(this);
    connect(portScanTimer, &QTimer::timeout, this, &MainWindow::refreshPorts);
    portScanTimer->start(1000);
    refreshPorts();

    connect(openButton,  &QPushButton::clicked, this, &MainWindow::onOpenCloseClicked);
    connect(writeButton, &QPushButton::clicked, this, &MainWindow::onWriteClicked);
    connect(inputLine,   &QLineEdit::returnPressed, this, &MainWindow::onWriteClicked);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::onClearClicked);
}

MainWindow::~MainWindow()
{
    if (serial && serial->isOpen())
        serial->close();
}

// ------------------------------------------------------------------
// UI 구성 (스샷의 레이아웃: Device/BaudRate/OPEN 행, 입력/WRITE 행,
//          로그창, 하단 CLEAR + LED + 옵션 체크박스)
// ------------------------------------------------------------------
void MainWindow::buildUi()
{
    setWindowTitle("Serial Terminal - HW3");
    resize(780, 620);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // --- 1행: Device / BaudRate / OPEN ---
    QHBoxLayout *row1 = new QHBoxLayout();
    deviceCombo = new QComboBox();
    deviceCombo->setMinimumWidth(160);
    baudCombo = new QComboBox();
    baudCombo->addItems({"9600", "19200", "38400", "57600", "115200"});
    baudCombo->setCurrentText("9600");
    openButton = new QPushButton("OPEN");
    openButton->setFixedWidth(90);

    row1->addWidget(new QLabel("Device :"));
    row1->addWidget(deviceCombo, 1);
    row1->addWidget(new QLabel("BaudRate :"));
    row1->addWidget(baudCombo);
    row1->addWidget(openButton);

    // --- 2행: 입력창 / WRITE ---
    QHBoxLayout *row2 = new QHBoxLayout();
    inputLine = new QLineEdit();
    writeButton = new QPushButton("WRITE");
    writeButton->setFixedWidth(90);
    row2->addWidget(inputLine, 1);
    row2->addWidget(writeButton);

    // --- 3행 (보너스): RX 표시 형식 / 패킷 모드 ---
    QHBoxLayout *row3 = new QHBoxLayout();
    rxFormatCombo = new QComboBox();
    rxFormatCombo->addItems({"ASCII", "HEX"});
    packetModeCheck = new QCheckBox("패킷 모드 (STX/LEN/DATA/CHK/ETX, hex byte 입력)");
    row3->addWidget(new QLabel("RX 표시 :"));
    row3->addWidget(rxFormatCombo);
    row3->addStretch();
    row3->addWidget(packetModeCheck);

    // --- 로그 창 ---
    logView = new QTextEdit();
    logView->setReadOnly(true);
    logView->setStyleSheet(
        "background-color: #000000; color: #dddddd;"
        "font-family: Consolas, 'D2Coding', monospace;");

    // --- 하단 행: CLEAR / RX,TX LED / Clear when write / Auto Scroll ---
    QHBoxLayout *row4 = new QHBoxLayout();
    clearButton = new QPushButton("CLEAR");

    rxLedLabel = makeLed();
    txLedLabel = makeLed();

    clearWhenWriteCheck = new QCheckBox("Clear when write");
    autoScrollCheck = new QCheckBox("Auto Scroll");
    autoScrollCheck->setChecked(true);

    row4->addWidget(clearButton);
    row4->addSpacing(12);
    row4->addWidget(new QLabel("RX"));
    row4->addWidget(rxLedLabel);
    row4->addWidget(new QLabel("TX"));
    row4->addWidget(txLedLabel);
    row4->addStretch();
    row4->addWidget(clearWhenWriteCheck);
    row4->addWidget(autoScrollCheck);

    mainLayout->addLayout(row1);
    mainLayout->addLayout(row2);
    mainLayout->addLayout(row3);
    mainLayout->addWidget(logView, 1);
    mainLayout->addLayout(row4);
}

QLabel *MainWindow::makeLed() const
{
    QLabel *led = new QLabel();
    led->setFixedSize(16, 16);
    resetLed(led);
    return led;
}

void MainWindow::resetLed(QLabel *led) const
{
    led->setStyleSheet(
        "background-color: #555555; border-radius: 8px; border: 1px solid #222222;");
}

// 필수: rx/tx 시 초록색/파란색 점멸
void MainWindow::flashLed(QLabel *led, const QString &colorHex)
{
    led->setStyleSheet(QString(
                           "background-color: %1; border-radius: 8px; border: 1px solid #222222;")
                           .arg(colorHex));
    QTimer::singleShot(150, this, [this, led]() { resetLed(led); });
}

// ------------------------------------------------------------------
// 필수: 시리얼 장치 자동 검색 및 리스트 GUI 구성
// ------------------------------------------------------------------
void MainWindow::refreshPorts()
{
    const QString current = deviceCombo->currentText();

    deviceCombo->blockSignals(true);
    deviceCombo->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports)
        deviceCombo->addItem(info.portName());

    const int idx = deviceCombo->findText(current);
    if (idx >= 0)
        deviceCombo->setCurrentIndex(idx);
    deviceCombo->blockSignals(false);
}

void MainWindow::setPortControlsEnabled(bool enabled)
{
    deviceCombo->setEnabled(enabled);
    baudCombo->setEnabled(enabled);
}

// ------------------------------------------------------------------
// 필수: OPEN/CLOSE 버튼 통일
// ------------------------------------------------------------------
void MainWindow::onOpenCloseClicked()
{
    if (!serial->isOpen()) {
        if (deviceCombo->currentText().isEmpty()) {
            appendLog("[오류] 연결할 포트가 없습니다.", QColor("#e74c3c"));
            return;
        }

        serial->setPortName(deviceCombo->currentText());
        serial->setBaudRate(baudCombo->currentText().toInt());
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);

        if (serial->open(QIODevice::ReadWrite)) {
            openButton->setText("CLOSE");
            setPortControlsEnabled(false);
            portScanTimer->stop();
            rxBuffer.clear();
            appendLog(QString("[연결됨] %1 @ %2 bps")
                          .arg(serial->portName())
                          .arg(serial->baudRate()),
                      QColor("#f1c40f"));
        } else {
            appendLog("[오류] 포트를 열 수 없습니다: " + serial->errorString(),
                      QColor("#e74c3c"));
        }
    } else {
        serial->close();
        openButton->setText("OPEN");
        setPortControlsEnabled(true);
        portScanTimer->start(1000);
        refreshPorts();
        appendLog("[연결 종료]", QColor("#f1c40f"));
    }
}

// ------------------------------------------------------------------
// WRITE 버튼 / Enter : 데이터 전송
// (패킷 모드일 땐 입력값을 공백구분 hex byte payload로 해석)
// ------------------------------------------------------------------
void MainWindow::onWriteClicked()
{
    if (!serial->isOpen()) {
        appendLog("[오류] 포트가 열려있지 않습니다.", QColor("#e74c3c"));
        return;
    }

    const QString text = inputLine->text();
    if (text.isEmpty())
        return;

    QByteArray outBytes;

    if (packetModeCheck->isChecked()) {
        QByteArray payload;
        const QStringList tokens = text.split(' ', Qt::SkipEmptyParts);
        bool ok = true;
        for (const QString &tok : tokens) {
            const int v = tok.toInt(&ok, 16);
            if (!ok || v < 0 || v > 0xFF) {
                appendLog("[오류] 패킷 모드에서는 공백으로 구분된 16진수 바이트를 입력하세요 (예: 01 0A FF)",
                          QColor("#e74c3c"));
                return;
            }
            payload.append(static_cast<char>(v));
        }
        outBytes = buildPacket(payload);
        appendLog("PKT TX: " + QString::fromLatin1(outBytes.toHex(' ').toUpper()),
                  QColor("#3498db"));
    } else {
        outBytes = text.toUtf8();
        appendLog("TX: " + text, QColor("#3498db"));
    }

    serial->write(outBytes);
    flashLed(txLedLabel, "#3498db"); // 필수: TX 파란색 점멸

    if (clearWhenWriteCheck->isChecked())
        inputLine->clear();
}

// 필수: Clear 버튼
void MainWindow::onClearClicked()
{
    logView->clear();
}

// ------------------------------------------------------------------
// 데이터 수신
// ------------------------------------------------------------------
void MainWindow::onReadyRead()
{
    const QByteArray data = serial->readAll();
    processIncomingBytes(data);
}

void MainWindow::processIncomingBytes(const QByteArray &bytes)
{
    flashLed(rxLedLabel, "#2ecc71"); // 필수: RX 초록색 점멸

    if (packetModeCheck->isChecked()) {
        rxBuffer.append(bytes);
        tryExtractPackets();
        return;
    }

    if (rxFormatCombo->currentText() == "HEX") {
        appendLog("RX: " + QString::fromLatin1(bytes.toHex(' ').toUpper()),
                  QColor("#2ecc71"));
    } else {
        appendLog("RX: " + QString::fromUtf8(bytes), QColor("#2ecc71"));
    }
}

// ------------------------------------------------------------------
// 보너스: 패킷 분석 / 보내기
// 프레임: [STX 0x02][LEN][DATA...(LEN bytes)][CHK][ETX 0x03]
// CHK = DATA 바이트들의 XOR
// (실제 과제에서 쓰는 패킷 포맷이 다르면 이 두 함수만 고치면 됨)
// ------------------------------------------------------------------
QByteArray MainWindow::buildPacket(const QByteArray &payload) const
{
    QByteArray frame;
    frame.append(char(0x02));                                  // STX
    frame.append(static_cast<char>(payload.size() & 0xFF));    // LEN
    frame.append(payload);                                     // DATA

    unsigned char chk = 0;
    for (char b : payload)
        chk ^= static_cast<unsigned char>(b);
    frame.append(static_cast<char>(chk));                      // CHK
    frame.append(char(0x03));                                  // ETX
    return frame;
}

void MainWindow::tryExtractPackets()
{
    while (true) {
        const int stx = rxBuffer.indexOf(char(0x02));
        if (stx < 0) {
            rxBuffer.clear();
            return;
        }
        if (stx > 0)
            rxBuffer.remove(0, stx); // STX 앞의 쓰레기 바이트 버림

        if (rxBuffer.size() < 2)
            return; // LEN까지 아직 안 옴, 다음 readyRead 기다림

        const unsigned char len = static_cast<unsigned char>(rxBuffer.at(1));
        const int frameSize = 1 /*STX*/ + 1 /*LEN*/ + len + 1 /*CHK*/ + 1 /*ETX*/;

        if (rxBuffer.size() < frameSize)
            return; // 프레임이 아직 다 안 옴

        const QByteArray payload = rxBuffer.mid(2, len);
        const unsigned char chk = static_cast<unsigned char>(rxBuffer.at(2 + len));
        const unsigned char etx = static_cast<unsigned char>(rxBuffer.at(2 + len + 1));

        unsigned char calcChk = 0;
        for (char b : payload)
            calcChk ^= static_cast<unsigned char>(b);

        const bool ok = (chk == calcChk) && (etx == 0x03);

        appendLog(QString("PKT RX: LEN=%1 DATA=%2 CHK=%3 (%4)")
                      .arg(len)
                      .arg(QString::fromLatin1(payload.toHex(' ').toUpper()))
                      .arg(chk, 2, 16, QChar('0'))
                      .arg(ok ? "OK" : "FAIL"),
                  QColor(ok ? "#2ecc71" : "#e74c3c"));

        rxBuffer.remove(0, frameSize);
    }
}

// ------------------------------------------------------------------
// 필수: Auto Scroll 옵션 반영해서 로그 출력
// ------------------------------------------------------------------
void MainWindow::appendLog(const QString &text, const QColor &color)
{
    logView->append(QString("<span style='color:%1;'>%2</span>")
                        .arg(color.name(), text.toHtmlEscaped()));

    if (autoScrollCheck->isChecked()) {
        QTextCursor cursor = logView->textCursor();
        cursor.movePosition(QTextCursor::End);
        logView->setTextCursor(cursor);
        logView->ensureCursorVisible();
    }
}