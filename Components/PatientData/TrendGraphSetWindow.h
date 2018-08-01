/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/30
 **/

#pragma once
#include "Window.h"
#include <QScopedPointer>
#include <QHBoxLayout>
#include <QLabel>
#include "ComboBox.h"
#include "SpinBox.h"
#include "ParamInfo.h"
#include "TrendDataDefine.h"

class RulerItem : public QWidget
{
    Q_OBJECT
public:
    RulerItem(ParamID ppid, SubParamID ssid) : QWidget()
    {
        pid = ppid;
        sid = ssid;

        label = new QLabel();
        combo = new ComboBox();
        downRuler = new SpinBox();
        upRuler = new SpinBox();
        downRuler->setArrow(false);
        upRuler->setArrow(false);

        QHBoxLayout *mainlayout = new QHBoxLayout();
//        manlayout->setSpacing(2);
//        manlayout->setMargin(0);

        mainlayout->addWidget(label);
        mainlayout->addWidget(combo);
//        combo->setSpacing(2);
        mainlayout->addWidget(downRuler);
        mainlayout->addWidget(upRuler);

        setLayout(mainlayout);

        setFocusPolicy(Qt::StrongFocus);
        setFocusProxy(combo);
    }
    ~RulerItem()
    {
    }

public:
    ParamID pid;
    SubParamID sid;

    QLabel *label;
    ComboBox *combo;
    SpinBox *downRuler;
    SpinBox *upRuler;
};

class TrendGraphSetWindowPrivate;
class TrendGraphSetWindow : public Window
{
    Q_OBJECT
public:
    static TrendGraphSetWindow &construction(void)
    {
        if (selfObj == NULL)
        {
            selfObj = new TrendGraphSetWindow();
        }
        return *selfObj;
    }
    ~TrendGraphSetWindow();

public:
    TrendGroup getTrendGroup();

    void upDateTrendGroup(void);

protected:
    void showEvent(QShowEvent *ev);

private slots:
    /**
     * @brief _allAutoReleased all ruler auto slot funtion
     */
    void allAutoReleased();

    /**
     * @brief _trendGroupReleased set trend group slot funtion
     */
    void trendGroupReleased(int trendGroup);

    /**
     * @brief _timeIntervalReleased set time interval slot funtion
     */
    void timeIntervalReleased(int timeInterval);

    /**
     * @brief _waveNumberReleased set waveform number slot funtion
     */
    void waveNumberReleased(int waveNum);

    void onComboBoxChanged(int index);

    void upDownRulerChange(int value, int scale);

private:
    TrendGraphSetWindow();
    static TrendGraphSetWindow *selfObj;
    QScopedPointer<TrendGraphSetWindowPrivate> d_ptr;
};
#define trendGraphSetWindow         (TrendGraphSetWindow::construction())
#define deleteTrendGraphSetWindow   (delete TrendGraphSetWindow::selfObj)
