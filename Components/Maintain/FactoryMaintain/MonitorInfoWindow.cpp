/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/20
 **/

#include "MonitorInfoWindow.h"
#include <QMap>
#include <QGridLayout>
#include <QLabel>
#include "LanguageManager.h"
#include "IConfig.h"
#include "KeyInputPanel.h"
#include "Button.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include "TimeManager.h"
#include <QTimer>
#include "PowerManager.h"
#include "UnitManager.h"

class MonitorInfoWindowPrivate
{
public:
    enum MenuItem
    {
        ITEM_LAB_CMLV_WORKTIME = 0,
        ITEM_LAB_TEM_INSIDECASE,
        ITEM_LAB_BAT_CAPACITY,
        ITEM_LAB_MACHINE_TYPE,
        ITEM_LAB_MAC_ADDR,
        ITEM_LAB_SCR_RESOLASIZE,
        ITEM_BTN_ELEC_SERIALNUM,
    };
    MonitorInfoWindowPrivate()
            : button(NULL),
              timer(NULL)
    {
    }
    /**
     * @brief loadOptions  加载显示参数
     */
    void loadOptions();
    QMap <MenuItem, QLabel *> labs;
    Button *button;
    QTimer *timer;
};

MonitorInfoWindow::~MonitorInfoWindow()
{
    delete d_ptr;
}

MonitorInfoWindow::MonitorInfoWindow()
    : Window(),
      d_ptr(new MonitorInfoWindowPrivate)
{
    layoutExec();
    readyShow();
}

void MonitorInfoWindowPrivate::loadOptions()
{
    QDesktopWidget *w = QApplication::desktop();
    QString temStr;

    labs[ITEM_LAB_SCR_RESOLASIZE]->setText(trs(QString("%1*%2")
                                         .arg(w->width())
                                         .arg(w->height())));

    temStr.clear();
    systemConfig.getStrValue("MonitorInfo|CumulativeWorkingTime", temStr);
    labs[ITEM_LAB_CMLV_WORKTIME]->setText(trs(temStr));

    temStr.clear();
    systemConfig.getStrValue("MonitorInfo|TemperatureInsideCase", temStr);
    labs[ITEM_LAB_TEM_INSIDECASE]->setText(QString("%1%2").arg(trs(temStr))
                                           .arg(trs(Unit::getSymbol(UNIT_TC))));

    temStr.clear();
    labs[ITEM_LAB_BAT_CAPACITY]->setText(powerManger.getBatteryCapacity());

    temStr.clear();
    systemConfig.getStrValue("MonitorInfo|MachineType", temStr);
    labs[ITEM_LAB_MACHINE_TYPE]->setText(trs(temStr));

    temStr.clear();
    systemConfig.getStrValue("MonitorInfo|MACAddress", temStr);
    labs[ITEM_LAB_MAC_ADDR]->setText(trs(temStr));

    temStr.clear();
    machineConfig.getStrValue("SerialNumber", temStr);
    button->setText(temStr);
}

void MonitorInfoWindow::readyShow()
{
    d_ptr->loadOptions();
    onTimeOutExec();
}

void MonitorInfoWindow::onTimeOutExec()
{
    QString showtime = getRunTime();
    d_ptr->labs[MonitorInfoWindowPrivate
            ::ITEM_LAB_CMLV_WORKTIME]->setText(showtime);
}

void MonitorInfoWindow::layoutExec()
{
    setWindowTitle(trs("MonitorInfoMenu"));

    QGridLayout *layout = new QGridLayout;
    layout->setVerticalSpacing(20);
    setFixedSize(480, 450);

    QLabel *labelLeft;
    QLabel *labelRight;

    labelLeft = new QLabel(trs("CumulativeWorkingTime"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoWindowPrivate
                       ::ITEM_LAB_CMLV_WORKTIME, labelRight);

    labelLeft = new QLabel(trs("TemperatureInsideCase"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoWindowPrivate
                       ::ITEM_LAB_TEM_INSIDECASE, labelRight);

    labelLeft = new QLabel(trs("BatteryQuantity"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoWindowPrivate
                       ::ITEM_LAB_BAT_CAPACITY, labelRight);

    labelLeft = new QLabel(trs("MachineType"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoWindowPrivate
                       ::ITEM_LAB_MACHINE_TYPE, labelRight);

    labelLeft = new QLabel(trs("MACAddress"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoWindowPrivate
                       ::ITEM_LAB_MAC_ADDR, labelRight);

    labelLeft = new QLabel(trs("ScreenResolationSize"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoWindowPrivate
                       ::ITEM_LAB_SCR_RESOLASIZE, labelRight);

    labelLeft = new QLabel(trs("ElectronicSerialNumber"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    d_ptr->button = new Button("");
    d_ptr->button->setBorderWidth(0);
    d_ptr->button->setButtonStyle(Button::ButtonTextBesideIcon);
    d_ptr->button->setFocusPolicy(Qt::NoFocus);
    layout->addWidget(d_ptr->button, d_ptr->labs.count(), 1,
                      Qt::AlignCenter|Qt::AlignRight);

    layout->setRowStretch((layout->rowCount() + 1), 1);
    setWindowLayout(layout);
}

QString MonitorInfoWindow::getRunTime()
{
    QString t;
    unsigned diffTime = timeManager.getRunTime();
    int hour = diffTime / 3600;
    int min = (diffTime - hour * 3600) / 60;
    int sec = diffTime % 60;
    t = QString("%1:%2:%3").
            arg(QString::number(hour, 'g', 2)).
            arg(QString::number(min, 'g', 2)).
            arg(QString::number(sec, 'g', 2));
    return t;
}

void MonitorInfoWindow::showEvent(QShowEvent *e)
{
    Window::showEvent(e);
    if (d_ptr->timer)
    {
        delete d_ptr->timer;
        d_ptr->timer = NULL;
    }
    d_ptr->timer = new QTimer;
    d_ptr->timer->setSingleShot(false);
    connect(d_ptr->timer, SIGNAL(timeout()), SLOT(onTimeOutExec()));
    d_ptr->timer->start(1000);
}

void MonitorInfoWindow::hideEvent(QHideEvent *e)
{
    Window::hideEvent(e);
    if (d_ptr->timer)
    {
        delete d_ptr->timer;
        d_ptr->timer = NULL;
    }
}
