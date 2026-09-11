#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->angle1, &QSlider::valueChanged, this, [=](int value1){
        //qDebug() << "angle1:" << value1;
        value1*=3.64;
        ui->armWidget->setAngle(0, value1);
        ui->angle1_print->setPlainText(QString::number(value1));
    });
    connect(ui->angle2, &QSlider::valueChanged, this, [=](int value2){
        //qDebug() << "angle2:" << value2;
        value2*=3.64;
        ui->armWidget->setAngle(1, value2);
        ui->angle2_print->setPlainText(QString::number(value2));
    });
    connect(ui->angle3, &QSlider::valueChanged, this, [=](int value3){
        //qDebug() << "angle3:" << value3;
        value3*=3.64;
        ui->armWidget->setAngle(2, value3);
        ui->angle3_print->setPlainText(QString::number(value3));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_generate_button_clicked()
{
    std::cout<<"Hello world!"<<std::endl;
    int value1 = ui->length1->value();  // QSpinBox
    std::cout <<"length1: "<<value1<<std::endl;
    int value2 = ui->length2->value();  // QSpinBox
    std::cout <<"length2: "<<value2<<std::endl;
    int value3 = ui->length3->value();  // QSpinBox
    std::cout <<"length3: "<<value3<<std::endl;

    double l1 = ui->length1->value();
    double l2 = ui->length2->value();
    double l3 = ui->length3->value();
    ui->armWidget->setLengths(l1, l2, l3);
}

void MainWindow::on_reset_button_clicked()
{
    // std::cout<<"www!"<<std::endl;
    // int value1 = 3.64*(ui->angle1->value());  // QSpinBox
    // std::cout <<"angle1: "<<value1<<std::endl;
    // int value2 = 3.64*(ui->angle2->value());  // QSpinBox
    // std::cout <<"angle2: "<<value2<<std::endl;
    // int value3 = 3.64*(ui->angle3->value());  // QSpinBox
    // std::cout <<"angle3: "<<value3<<std::endl;

    ui->angle1->setValue(0);
    ui->angle2->setValue(0);
    ui->angle3->setValue(0);

    ui->armWidget->reset();
}

