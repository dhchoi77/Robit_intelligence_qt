#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <QDebug>
#include <QKeyEvent>
#include <QTextCursor>

// ===== 자모 코드표 =====
static const int CHO_COMPAT[19] = {
    0x3131,0x3132,0x3134,0x3137,0x3138,0x3139,0x3141,0x3142,0x3143,0x3145,
    0x3146,0x3147,0x3148,0x3149,0x314A,0x314B,0x314C,0x314D,0x314E };
static const int JONG_COMPAT[28] = {
    0,0x3131,0x3132,0x3133,0x3134,0x3135,0x3136,0x3137,0x3139,0x313A,0x313B,
    0x313C,0x313D,0x313E,0x313F,0x3140,0x3141,0x3142,0x3144,0x3145,0x3146,
    0x3147,0x3148,0x314A,0x314B,0x314C,0x314D,0x314E };

static int choIndexOf(int c){
    for(int i=0;i<19;i++) if(CHO_COMPAT[i]==c) return i;
    return -1;
}
static int jungIndexOf(int c){
    if(c>=0x314F && c<=0x3163) return c-0x314F;   // 모음은 연속 배치
    return -1;
}
static int jongIndexOf(int c){                    // 홑받침 → 종성 index (없으면 -1)
    for(int i=1;i<28;i++) if(JONG_COMPAT[i]==c) return i;
    return -1;
}
// 복합 모음: 현재 중성 a + 새 중성 b
static int combineJung(int a,int b){
    if(a==8 &&b==0) return 9;   if(a==8 &&b==1) return 10;  if(a==8 &&b==20) return 11;
    if(a==13&&b==4) return 14;  if(a==13&&b==5) return 15;  if(a==13&&b==20) return 16;
    if(a==18&&b==20) return 19;
    return -1;
}
// 복합 받침: 현재 종성 a + 새 홑받침 b
static int combineJong(int a,int b){
    if(a==1 &&b==19) return 3;  if(a==4 &&b==22) return 5;  if(a==4 &&b==27) return 6;
    if(a==8 &&b==1)  return 9;  if(a==8 &&b==16) return 10; if(a==8 &&b==17) return 11;
    if(a==8 &&b==19) return 12; if(a==8 &&b==25) return 13; if(a==8 &&b==26) return 14;
    if(a==8 &&b==27) return 15; if(a==17&&b==19) return 18;
    return 0;
}
static bool isComplexJong(int j){
    return j==3||j==5||j==6||j==9||j==10||j==11||j==12||j==13||j==14||j==15||j==18;
}
static int jongFirst(int j){   // 복합받침의 앞부분(남는 종성)
    switch(j){ case 3:return 1; case 5:case 6:return 4;
    case 9:case 10:case 11:case 12:case 13:case 14:case 15:return 8;
    case 18:return 17; } return 0;
}
static int jongSecondCho(int j){ // 복합받침 뒷부분 → 새 초성
    switch(j){ case 3:return 9; case 5:return 12; case 6:return 18; case 9:return 0;
    case 10:return 6; case 11:return 7; case 12:return 9; case 13:return 16;
    case 14:return 17; case 15:return 18; case 18:return 9; } return -1;
}
static int jongToCho(int j){    // 홑받침 → 초성 (받침 이동용)
    switch(j){ case 1:return 0; case 2:return 1; case 4:return 2; case 7:return 3;
    case 8:return 5; case 16:return 6; case 17:return 7; case 19:return 9;
    case 20:return 10; case 21:return 11; case 22:return 12; case 23:return 14;
    case 24:return 15; case 25:return 16; case 26:return 17; case 27:return 18; }
    return -1;
}
static bool isComplexJung(int v){ return v==9||v==10||v==11||v==14||v==15||v==16||v==19; }
static int jungFirst(int v){
    if(v==9||v==10||v==11) return 8; if(v==14||v==15||v==16) return 13;
    if(v==19) return 18; return -1;
}

// ===== 현재 상태 → 글자 =====
QString MainWindow::composeString(){
    if(cCho>=0 && cJung>=0){
        int code = 0xAC00 + (cCho*21 + cJung)*28 + (cJong<0?0:cJong);
        return QString(QChar(code));
    }
    if(cCho>=0)  return QString(QChar(CHO_COMPAT[cCho]));
    if(cJung>=0) return QString(QChar(0x314F + cJung));
    return QString();
}
// 마지막 글자를 다시 그림 (조합중이면 교체, 아니면 삽입)
void MainWindow::renderSyllable(){
    QString s = composeString();
    QTextCursor cur = ui->print->textCursor();
    if(composing) cur.deletePreviousChar();
    if(!s.isEmpty()) cur.insertText(s);
    ui->print->setTextCursor(cur);
    composing = !s.isEmpty();
}
// 현재 글자 확정 (문서엔 그대로 두고 상태만 초기화)
void MainWindow::commitSyllable(){
    composing = false;
    cCho = -1; cJung = -1; cJong = 0;
}

// ===== 핵심: 자모 하나 입력 =====
void MainWindow::inputKorean(int baseJamo, int shiftJamo){
    int jamo = (on_Shift ? shiftJamo : baseJamo);
    int ji = jungIndexOf(jamo);

    if(ji >= 0){ // ----- 모음 -----
        if(cJong > 0){                       // 받침 이동
            int movingCho, remain;
            if(isComplexJong(cJong)){ remain = jongFirst(cJong); movingCho = jongSecondCho(cJong); }
            else { remain = 0; movingCho = jongToCho(cJong); }
            cJong = remain; renderSyllable();  // 앞 글자 완성
            commitSyllable();
            cCho = movingCho; cJung = ji; cJong = 0; renderSyllable();
        }
        else if(cCho>=0 && cJung<0){ cJung = ji; renderSyllable(); }        // 초성+모음
        else if(cCho<0 && cJung<0){  cJung = ji; renderSyllable(); }        // 모음 단독
        else {                                                             // 이미 모음 있음
            int comb = combineJung(cJung, ji);
            if(comb>=0){ cJung = comb; renderSyllable(); }                  // 복합모음
            else { commitSyllable(); cJung = ji; renderSyllable(); }        // 못 합침 → 새 글자
        }
    }
    else { // ----- 자음 -----
        int ci = choIndexOf(jamo);
        int jo = jongIndexOf(jamo);
        if(cCho<0 && cJung<0){ commitSyllable(); cCho = ci; renderSyllable(); }     // 처음
        else if(cJung<0){ commitSyllable(); cCho = ci; renderSyllable(); }          // 초성만 있었음
        else if(cJong==0){
            if(cCho>=0 && jo>0){ cJong = jo; renderSyllable(); }                    // 받침으로
            else { commitSyllable(); cCho = ci; renderSyllable(); }                 // ㄸㅃㅉ 등
        }
        else {
            int comb = combineJong(cJong, jo);
            if(comb>0){ cJong = comb; renderSyllable(); }                           // 복합받침
            else { commitSyllable(); cCho = ci; renderSyllable(); }                 // 새 글자
        }
    }
}

// ===== 조합 중 백스페이스 (한 단계 분해) =====
void MainWindow::korBackspace(){
    if(!composing){ ui->print->textCursor().deletePreviousChar(); return; }
    if(cJong>0)       cJong = isComplexJong(cJong) ? jongFirst(cJong) : 0;
    else if(cJung>=0) cJung = isComplexJung(cJung) ? jungFirst(cJung) : -1;
    else if(cCho>=0)  cCho = -1;
    renderSyllable();
}

bool MainWindow::isUpper(){ return capsLock ^ (on_Shift != 0); }
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->print->setReadOnly(true);
    // 모든 버튼이 키보드 포커스를 안 받도록 → Space/Enter로 눌리는 것 방지
    const auto buttons = this->findChildren<QPushButton*>();
    for (QPushButton *b : buttons)
        b->setFocusPolicy(Qt::NoFocus);

    this->setFocusPolicy(Qt::StrongFocus);   // 대신 MainWindow가 키를 받음

    this->setStyleSheet(R"(
    QPushButton {
        background-color: #f0f0f0;
        border: 1px solid #aaa;
        border-radius: 4px;
        padding: 6px;
    }
    QPushButton:hover  { background-color: #e0e0e0; }
    QPushButton:pressed {
        background-color: #7aa7ff;
    }
)");

}

MainWindow::~MainWindow()
{
    delete ui;
}
QPushButton* MainWindow::buttonForKey(int key)
{
    switch (key) {
    case Qt::Key_Q: return ui->button_Q;
    case Qt::Key_W: return ui->button_W;
    case Qt::Key_E: return ui->button_E;
    case Qt::Key_R: return ui->button_R;
    case Qt::Key_T: return ui->button_T;
    case Qt::Key_Y: return ui->button_Y;
    case Qt::Key_U: return ui->button_U;
    case Qt::Key_I: return ui->button_I;
    case Qt::Key_O: return ui->button_O;
    case Qt::Key_P: return ui->button_P;
    case Qt::Key_A: return ui->button_A;
    case Qt::Key_S: return ui->button_S;
    case Qt::Key_D: return ui->button_D;
    case Qt::Key_F: return ui->button_F;
    case Qt::Key_G: return ui->button_G;
    case Qt::Key_H: return ui->button_H;
    case Qt::Key_J: return ui->button_J;
    case Qt::Key_K: return ui->button_K;
    case Qt::Key_L: return ui->button_L;
    case Qt::Key_Z: return ui->button_Z;
    case Qt::Key_X: return ui->button_X;
    case Qt::Key_C: return ui->button_C;
    case Qt::Key_V: return ui->button_V;
    case Qt::Key_B: return ui->button_B;
    case Qt::Key_N: return ui->button_N;
    case Qt::Key_M: return ui->button_M;

    case Qt::Key_1: return ui->button_1;
    case Qt::Key_2: return ui->button_2;
    case Qt::Key_3: return ui->button_3;
    case Qt::Key_4: return ui->button_4;
    case Qt::Key_5: return ui->button_5;
    case Qt::Key_6: return ui->button_6;
    case Qt::Key_7: return ui->button_7;
    case Qt::Key_8: return ui->button_8;
    case Qt::Key_9: return ui->button_9;
    case Qt::Key_0: return ui->button_0;

    case Qt::Key_QuoteLeft:    return ui->button1;
    case Qt::Key_Minus:        return ui->button2;
    case Qt::Key_Equal:        return ui->button3;
    case Qt::Key_BracketLeft:  return ui->button4;
    case Qt::Key_BracketRight: return ui->button5;
    case Qt::Key_Backslash:    return ui->button6;
    case Qt::Key_Semicolon:    return ui->button7;
    case Qt::Key_Apostrophe:   return ui->button8;
    case Qt::Key_Comma:        return ui->button9;
    case Qt::Key_Period:       return ui->button10;
    case Qt::Key_Slash:        return ui->button11;

    case Qt::Key_Backspace: return ui->button_back;
    case Qt::Key_Tab:       return ui->button_Tab;
    case Qt::Key_Return:
    case Qt::Key_Enter:     return ui->button_Enter;
    case Qt::Key_Space:     return ui->button_Space;

    case Qt::Key_Hangul:   return ui->button_Kor;    // 한/영 키
    case Qt::Key_CapsLock: return ui->button_CL;
    case Qt::Key_Meta:                               // Windows 키 (눌림 표시만)
    case Qt::Key_Super_L:  return ui->button_LWin;
    default: return nullptr;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{   ui->print->setFocusPolicy(Qt::NoFocus);
    qDebug() << "key:" << event->key() << Qt::hex << event->key();
    if (event->isAutoRepeat()) return;

    // 물리 Shift: 누르는 동안 켬
    if (event->key() == Qt::Key_Shift) {
        on_Shift = true;
        ui->button_LShift->setDown(true);   // 화면 Shift도 눌린 모양
        return;
    }

    QPushButton *btn = buttonForKey(event->key());
    if (btn) {
        btn->click();
        btn->setDown(true);
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    // 물리 Shift: 떼면 끔
    if (event->key() == Qt::Key_Shift) {
        on_Shift = false;
        ui->button_LShift->setDown(false);
        return;
    }

    QPushButton *btn = buttonForKey(event->key());
    if (btn) btn->setDown(false);
    else QMainWindow::keyReleaseEvent(event);
}
// ===== 1행 =====
void MainWindow::on_button1_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "~" : "`"); }
void MainWindow::on_button_1_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "!" : "1"); }
void MainWindow::on_button_2_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "@" : "2"); }
void MainWindow::on_button_3_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "#" : "3"); }
void MainWindow::on_button_4_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "$" : "4"); }
void MainWindow::on_button_5_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "%" : "5"); }
void MainWindow::on_button_6_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "^" : "6"); }
void MainWindow::on_button_7_clicked(){ if(korMode) commitSyllable(); ui->print->insertPlainText(on_Shift ? "&" : "7"); }
void MainWindow::on_button_8_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "*" : "8"); }
void MainWindow::on_button_9_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "(" : "9"); }
void MainWindow::on_button_0_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? ")" : "0"); }
void MainWindow::on_button2_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "_" : "-"); }
void MainWindow::on_button3_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "+" : "="); }
void MainWindow::on_button_back_clicked(){
    if(korMode) korBackspace();
    else ui->print->textCursor().deletePreviousChar();
}

// ===== 2행 =====
void MainWindow::on_button_Tab_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText("\t"); }

void MainWindow::on_button4_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "{" : "["); }
void MainWindow::on_button5_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "}" : "]"); }
void MainWindow::on_button6_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "|" : "\\"); }

// ===== 3행 =====
void MainWindow::on_button_CL_clicked(){ capsLock = !capsLock;
    ui->button_CL->setDown(capsLock); }

void MainWindow::on_button7_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? ":" : ";"); }
void MainWindow::on_button8_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "\"" : "'"); }
void MainWindow::on_button_Enter_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText("\n"); }

// ===== 4행 =====
void MainWindow::on_button_LShift_clicked(){ on_Shift = !on_Shift; }

void MainWindow::on_button9_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "<" : ","); }
void MainWindow::on_button10_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? ">" : "."); }
void MainWindow::on_button11_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(on_Shift ? "?" : "/"); }
void MainWindow::on_button_RShift_clicked(){ on_Shift = !on_Shift; }

// ===== 5행 =====
void MainWindow::on_button_LCtrl_clicked(){ }       // Ctrl(좌)
void MainWindow::on_button_Fn_clicked(){ }          // Fn
void MainWindow::on_button_LWin_clicked(){ }        // Win(좌)
void MainWindow::on_button_Alt_clicked(){ }         // Alt
void MainWindow::on_button_Space_clicked(){ if(korMode) commitSyllable();ui->print->insertPlainText(" "); }
void MainWindow::on_button_Kor_clicked(){ commitSyllable();
    korMode = !korMode;
    ui->button_Kor->setText(korMode ? "한" : "영");}  // (선택) 라벨 표시}         // 한/영
void MainWindow::on_button_RWin_clicked(){ }        // Win(우)
void MainWindow::on_button_Menu_clicked(){ }        // Menu
void MainWindow::on_button_RCtrl_clicked(){ }       // Ctrl(우)

void MainWindow:: on_button_Copy_clicked(){
    ui->print->selectAll(); ui->print->copy();
}
void MainWindow::on_button_Clear_clicked(){
    ui->print->clear();
}

void MainWindow::on_button_Q_clicked(){ if(korMode) inputKorean(0x3142,0x3143); else ui->print->insertPlainText(isUpper()?"Q":"q"); } // ㅂㅃ
void MainWindow::on_button_W_clicked(){ if(korMode) inputKorean(0x3148,0x3149); else ui->print->insertPlainText(isUpper()?"W":"w"); } // ㅈㅉ
void MainWindow::on_button_E_clicked(){ if(korMode) inputKorean(0x3137,0x3138); else ui->print->insertPlainText(isUpper()?"E":"e"); } // ㄷㄸ
void MainWindow::on_button_R_clicked(){ if(korMode) inputKorean(0x3131,0x3132); else ui->print->insertPlainText(isUpper()?"R":"r"); } // ㄱㄲ
void MainWindow::on_button_T_clicked(){ if(korMode) inputKorean(0x3145,0x3146); else ui->print->insertPlainText(isUpper()?"T":"t"); } // ㅅㅆ
void MainWindow::on_button_Y_clicked(){ if(korMode) inputKorean(0x315B,0x315B); else ui->print->insertPlainText(isUpper()?"Y":"y"); } // ㅛ
void MainWindow::on_button_U_clicked(){ if(korMode) inputKorean(0x3155,0x3155); else ui->print->insertPlainText(isUpper()?"U":"u"); } // ㅕ
void MainWindow::on_button_I_clicked(){ if(korMode) inputKorean(0x3151,0x3151); else ui->print->insertPlainText(isUpper()?"I":"i"); } // ㅑ
void MainWindow::on_button_O_clicked(){ if(korMode) inputKorean(0x3150,0x3152); else ui->print->insertPlainText(isUpper()?"O":"o"); } // ㅐㅒ
void MainWindow::on_button_P_clicked(){ if(korMode) inputKorean(0x3154,0x3156); else ui->print->insertPlainText(isUpper()?"P":"p"); } // ㅔㅖ
void MainWindow::on_button_A_clicked(){ if(korMode) inputKorean(0x3141,0x3141); else ui->print->insertPlainText(isUpper()?"A":"a"); } // ㅁ
void MainWindow::on_button_S_clicked(){ if(korMode) inputKorean(0x3134,0x3134); else ui->print->insertPlainText(isUpper()?"S":"s"); } // ㄴ
void MainWindow::on_button_D_clicked(){ if(korMode) inputKorean(0x3147,0x3147); else ui->print->insertPlainText(isUpper()?"D":"d"); } // ㅇ
void MainWindow::on_button_F_clicked(){ if(korMode) inputKorean(0x3139,0x3139); else ui->print->insertPlainText(isUpper()?"F":"f"); } // ㄹ
void MainWindow::on_button_G_clicked(){ if(korMode) inputKorean(0x314E,0x314E); else ui->print->insertPlainText(isUpper()?"G":"g"); } // ㅎ
void MainWindow::on_button_H_clicked(){ if(korMode) inputKorean(0x3157,0x3157); else ui->print->insertPlainText(isUpper()?"H":"h"); } // ㅗ
void MainWindow::on_button_J_clicked(){ if(korMode) inputKorean(0x3153,0x3153); else ui->print->insertPlainText(isUpper()?"J":"j"); } // ㅓ
void MainWindow::on_button_K_clicked(){ if(korMode) inputKorean(0x314F,0x314F); else ui->print->insertPlainText(isUpper()?"K":"k"); } // ㅏ
void MainWindow::on_button_L_clicked(){ if(korMode) inputKorean(0x3163,0x3163); else ui->print->insertPlainText(isUpper()?"L":"l"); } // ㅣ
void MainWindow::on_button_Z_clicked(){ if(korMode) inputKorean(0x314B,0x314B); else ui->print->insertPlainText(isUpper()?"Z":"z"); } // ㅋ
void MainWindow::on_button_X_clicked(){ if(korMode) inputKorean(0x314C,0x314C); else ui->print->insertPlainText(isUpper()?"X":"x"); } // ㅌ
void MainWindow::on_button_C_clicked(){ if(korMode) inputKorean(0x314A,0x314A); else ui->print->insertPlainText(isUpper()?"C":"c"); } // ㅊ
void MainWindow::on_button_V_clicked(){ if(korMode) inputKorean(0x314D,0x314D); else ui->print->insertPlainText(isUpper()?"V":"v"); } // ㅍ
void MainWindow::on_button_B_clicked(){ if(korMode) inputKorean(0x3160,0x3160); else ui->print->insertPlainText(isUpper()?"B":"b"); } // ㅠ
void MainWindow::on_button_N_clicked(){ if(korMode) inputKorean(0x315C,0x315C); else ui->print->insertPlainText(isUpper()?"N":"n"); } // ㅜ
void MainWindow::on_button_M_clicked(){ if(korMode) inputKorean(0x3161,0x3161); else ui->print->insertPlainText(isUpper()?"M":"m"); } // ㅡ


