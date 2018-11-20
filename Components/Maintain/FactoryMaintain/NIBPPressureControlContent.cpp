/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/24
 **/
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "ComboBox.h"
#include "NIBPPressureControlContent.h"
#include "NIBPParam.h"
#include "NIBPSymbol.h"
#include "PatientManager.h"
#include "SpinBox.h"
#include "NIBPRepairMenuWindow.h"
#include "Button.h"
#include "MenuWindow.h"
#include <QLayout>
#include "NIBPProviderIFace.h"
#include "NIBPServiceStateDefine.h"
#include "MessageBox.h"

#define CALIBRATION_INTERVAL_TIME              (100)
#define TIMEOUT_WAIT_NUMBER                    (5000 / CALIBRATION_INTERVAL_TIME)
#define INFLATE_WAIT_NUMBER                    (100000 / CALIBRATION_INTERVAL_TIME)

class NIBPPressureControlContentPrivate
{
public:
    NIBPPressureControlContentPrivate();
    SpinBox *chargePressure;          // 设定充气压力值
    ComboBox *patientType;           // 病人类型
    Button *inflateBtn;             // 充气、放气控制按钮
    QLabel *value;
    int inflatePressure;               //  充气压力值
    int patientVaulue;                  //  病人类型
    bool inflateSwitch;                 //  充气、放气标志
    bool pressureControlFlag;          //  进入模式标志

    Button *modeBtn;                // 进入/退出模式
    bool isPressureControlMode;     // 是否处于压力操控模式
    int inModeTimerID;              // 进入压力操控模式定时器ID
    int timeoutNum;
    int isInflate;                  // 是否充气;
    int inflateTimerID;             // 充气和放气回复定时器ID
    int pressureTimerID;            // 获取压力定时器ID
    int pressure;
};

NIBPPressureControlContentPrivate::NIBPPressureControlContentPrivate()
    : chargePressure(NULL),
      patientType(NULL),
      inflateBtn(NULL),
      value(NULL),
      inflatePressure(0),
      patientVaulue(0),
      inflateSwitch(0),
      pressureControlFlag(false),
      modeBtn(NULL), isPressureControlMode(false), inModeTimerID(-1),
      timeoutNum(0), isInflate(true), inflateTimerID(-1), pressureTimerID(-1),
      pressure(InvData())
{
}

// 压力控制模式
/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void NIBPPressureControlContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    Button *button;
    QLabel *label;

    button = new Button(trs("EnterPressureContrlMode"));
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, 0, 2);
    connect(button, SIGNAL(released()), this, SLOT(enterPressureContrlReleased()));
    d_ptr->modeBtn = button;

    label = new QLabel(trs("InflationPressure"));
    layout->addWidget(label, 1, 0, Qt::AlignCenter);

    d_ptr->chargePressure = new SpinBox();
    d_ptr->chargePressure->setRange(50, 300);
    d_ptr->chargePressure->setValue(250);
    d_ptr->chargePressure->setStep(5);
    layout->addWidget(d_ptr->chargePressure, 1, 1);

    button  = new Button(trs("ServiceInflate"));
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setEnabled(false);
    layout->addWidget(button, 1, 2);
    connect(button, SIGNAL(released()), this, SLOT(inflateBtnReleased()));
    d_ptr->inflateBtn = button;

    label = new QLabel(trs("ServicePressure"));
    layout->addWidget(label, 2, 0, Qt::AlignCenter);

    label = new QLabel(InvStr());
    layout->addWidget(label, 2, 1, Qt::AlignCenter);
    d_ptr->value = label;

    label = new QLabel();
    label->setText(Unit::getSymbol(nibpParam.getUnit()));
    layout->addWidget(label, 2, 2, Qt::AlignCenter);

    layout->setRowStretch(3, 1);
}

void NIBPPressureControlContent::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->inModeTimerID == ev->timerId())
    {
        bool reply = nibpParam.geReply();
        if (reply || d_ptr->timeoutNum == INFLATE_WAIT_NUMBER)
        {
            if (reply && nibpParam.getResult())
            {
                if (d_ptr->isPressureControlMode)
                {
                    d_ptr->isPressureControlMode = false;
                    d_ptr->modeBtn->setText(trs("EnterPressureContrlMode"));
                    d_ptr->inflateBtn->setEnabled(false);
                    killTimer(d_ptr->pressureTimerID);
                    d_ptr->pressureTimerID = -1;
                }
                else
                {
                    d_ptr->isPressureControlMode = true;
                    d_ptr->modeBtn->setText(trs("QuitPressureContrlMode"));
                    d_ptr->inflateBtn->setEnabled(true);
                    d_ptr->pressureTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
                }
            }
            else
            {
                MessageBox messbox(trs("Warn"), trs("OperationFailedPleaseAgain"), false);
                messbox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
                messbox.exec();
            }
            killTimer(d_ptr->inModeTimerID);
            d_ptr->inModeTimerID = -1;
            d_ptr->timeoutNum = 0;
        }
        else
        {
            d_ptr->timeoutNum++;
        }
    }
    else if (d_ptr->inflateTimerID == ev->timerId())
    {
        bool reply = nibpParam.geReply();
        if (reply || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER * 10)
        {
            if (reply && nibpParam.getResult())
            {
                if (d_ptr->isInflate)
                {
                    d_ptr->isInflate = false;
                    d_ptr->inflateBtn->setText(trs("ServiceDeflate"));
                }
                else
                {
                    d_ptr->isInflate = true;
                    d_ptr->inflateBtn->setText(trs("ServiceInflate"));
                }
            }
            else
            {
                MessageBox messbox(trs("Warn"), trs("OperationFailedPleaseAgain"), false);
                messbox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
                messbox.exec();
            }
            killTimer(d_ptr->inflateTimerID);
            d_ptr->inflateTimerID = -1;
            d_ptr->timeoutNum = 0;
        }
        else
        {
            d_ptr->timeoutNum++;
        }
    }
    else if (d_ptr->pressureTimerID == ev->timerId())
    {
        if (d_ptr->pressure != nibpParam.getManometerPressure())
        {
            d_ptr->pressure = nibpParam.getManometerPressure();
            d_ptr->value->setNum(nibpParam.getManometerPressure());
        }
    }
}

/**************************************************************************************************
 * 充气、放气控制按钮。
 *************************************************************************************************/
void NIBPPressureControlContent::inflateBtnReleased()
{
    d_ptr->inflateTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
    if (d_ptr->isInflate)
    {
        int value = d_ptr->chargePressure->getValue();
        nibpParam.provider().servicePressureinflate(value);
    }
    else
    {
        nibpParam.provider().servicePressuredeflate();
    }
}

void NIBPPressureControlContent::enterPressureContrlReleased()
{
    d_ptr->inModeTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
    if (d_ptr->isPressureControlMode)
    {
        nibpParam.provider().servicePressurecontrol(false);
        nibpParam.switchState(NIBP_SERVICE_STANDBY_STATE);
    }
    else
    {
        nibpParam.switchState(NIBP_SERVICE_PRESSURECONTROL_STATE);
    }
}

NIBPPressureControlContent *NIBPPressureControlContent::getInstance()
{
    static NIBPPressureControlContent *instance = NULL;
    if (!instance)
    {
        instance = new NIBPPressureControlContent();
    }
    return instance;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPPressureControlContent::NIBPPressureControlContent()
    : MenuContent(trs("ServicePressureControl"),
                  trs("ServicePressureControlDesc")),
      d_ptr(new NIBPPressureControlContentPrivate)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPPressureControlContent::~NIBPPressureControlContent()
{
    delete d_ptr;
}

