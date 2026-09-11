#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
private slots:
    void on_button1_clicked();
    void on_button_1_clicked();
    void on_button_2_clicked();
    void on_button_3_clicked();
    void on_button_4_clicked();
    void on_button_5_clicked();
    void on_button_6_clicked();
    void on_button_7_clicked();
    void on_button_8_clicked();
    void on_button_9_clicked();
    void on_button_0_clicked();
    void on_button2_clicked();
    void on_button3_clicked();
    void on_button_back_clicked();

    void on_button_Tab_clicked();
    void on_button_Q_clicked();
    void on_button_W_clicked();
    void on_button_E_clicked();
    void on_button_R_clicked();
    void on_button_T_clicked();
    void on_button_Y_clicked();
    void on_button_U_clicked();
    void on_button_I_clicked();
    void on_button_O_clicked();
    void on_button_P_clicked();
    void on_button4_clicked();
    void on_button5_clicked();
    void on_button6_clicked();

    void on_button_CL_clicked();
    void on_button_A_clicked();
    void on_button_S_clicked();
    void on_button_D_clicked();
    void on_button_F_clicked();
    void on_button_G_clicked();
    void on_button_H_clicked();
    void on_button_J_clicked();
    void on_button_K_clicked();
    void on_button_L_clicked();
    void on_button7_clicked();
    void on_button8_clicked();
    void on_button_Enter_clicked();

    void on_button_LShift_clicked();
    void on_button_Z_clicked();
    void on_button_X_clicked();
    void on_button_C_clicked();
    void on_button_V_clicked();
    void on_button_B_clicked();
    void on_button_N_clicked();
    void on_button_M_clicked();
    void on_button9_clicked();
    void on_button10_clicked();
    void on_button11_clicked();
    void on_button_RShift_clicked();

    void on_button_LCtrl_clicked();
    void on_button_Fn_clicked();
    void on_button_LWin_clicked();
    void on_button_Alt_clicked();
    void on_button_Space_clicked();
    void on_button_Kor_clicked();
    void on_button_RWin_clicked();
    void on_button_Menu_clicked();
    void on_button_RCtrl_clicked();

    void on_button_Copy_clicked();
    void on_button_Clear_clicked();
private:
    Ui::MainWindow *ui;
    int  on_Shift = 0;
    bool capsLock = false;
    bool korMode  = false;

    // 조합 중인 글자 상태 (-1/0 = 없음)
    int  cCho = -1, cJung = -1, cJong = 0;
    bool composing = false;

    QPushButton* buttonForKey(int key);
    bool isUpper();
    void inputKorean(int baseJamo, int shiftJamo);
    void commitSyllable();
    void renderSyllable();
    QString composeString();
    void korBackspace();
};
#endif // MAINWINDOW_H
