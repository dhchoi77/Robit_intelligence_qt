#ifndef ARMWIDGET_H
#define ARMWIDGET_H

#include <QWidget>

class ArmWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ArmWidget(QWidget *parent = nullptr);

    void setLengths(double l1, double l2, double l3);
    void setAngle(int joint, double deg);  // joint: 0,1,2
    void reset();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    double length1_ = 50, length2_ = 50, length3_ = 50;
    double angle1_ = 0, angle2_ = 0, angle3_ = 0;
};

#endif // ARMWIDGET_H