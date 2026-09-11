#include "armwidget.h"
#include <QPainter>
#include <cmath>

ArmWidget::ArmWidget(QWidget *parent) : QWidget(parent)
{
}

void ArmWidget::setLengths(double l1, double l2, double l3)
{
    length1_ = l1;
    length2_ = l2;
    length3_ = l3;
    update();  // paintEvent 다시 호출되게 함 (repaint 요청)
}

void ArmWidget::setAngle(int joint, double deg)
{
    if (joint == 0) angle1_ = deg;
    else if (joint == 1) angle2_ = deg;
    else if (joint == 2) angle3_ = deg;
    update();
}

void ArmWidget::reset()
{
    angle1_ = angle2_ = angle3_ = 0;
    update();
}

void ArmWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 베이스 위치: 위젯 왼쪽 중앙
    QPointF base(width() / 2.0, height() / 2.0);

    // 각도를 라디안으로 변환 (도 -> rad)
    double rad1 = qDegreesToRadians(angle1_);
    double rad2 = qDegreesToRadians(angle1_ + angle2_);   // 상대각 누적
    double rad3 = qDegreesToRadians(angle1_ + angle2_ + angle3_);

    // 정기구학(FK): 각 관절 끝점 좌표 계산
    // 화면 좌표는 y가 아래로 증가하므로 sin에 -를 붙여 위로 향하게 함
    QPointF p1 = base + QPointF(length1_ * cos(rad1), -length1_ * sin(rad1));
    QPointF p2 = p1   + QPointF(length2_ * cos(rad2), -length2_ * sin(rad2));
    QPointF p3 = p2   + QPointF(length3_ * cos(rad3), -length3_ * sin(rad3));

    // 링크 그리기
    painter.setPen(QPen(Qt::black, 4));
    painter.drawLine(base, p1);
    painter.drawLine(p1, p2);
    painter.drawLine(p2, p3);

    // 관절 점 그리기
    painter.setBrush(Qt::red);
    painter.drawEllipse(base, 5, 5);
    painter.drawEllipse(p1, 5, 5);
    painter.drawEllipse(p2, 5, 5);
    painter.drawEllipse(p3, 5, 5);
}