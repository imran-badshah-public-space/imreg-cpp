#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ImageRegistration.h"

class ImageRegistration : public QMainWindow
{
    Q_OBJECT

public:
    ImageRegistration(QWidget *parent = Q_NULLPTR);

private:
    Ui::ImageRegistrationClass ui;
};
