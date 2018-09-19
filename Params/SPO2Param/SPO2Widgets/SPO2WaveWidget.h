/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/27
 **/


#pragma once
#include "WaveWidget.h"
#include "SPO2Define.h"

class WaveWidgetLabel;

class PopupList;
class SPO2WaveWidget : public WaveWidget
{
    Q_OBJECT

public:
    void setNotify(bool enable, QString str = " ");

    SPO2WaveWidget(const QString &waveName, const QString &title);
    ~SPO2WaveWidget();

    // 设置波形增益。
    void setGain(SPO2Gain gain);

    // 获取不同增益下的极值
    static void getGainToValue(SPO2Gain gain, int &min, int &max);

    bool waveEnable();

protected:
    // 窗体大小调整事件
    virtual void resizeEvent(QResizeEvent *e);
    virtual void focusInEvent(QFocusEvent */*e*/);
    virtual void paintEvent(QPaintEvent *e);

private slots:
    void _releaseHandle(IWidget *);

private:
    void _initValueRange(SPO2Gain gain);

    void _loadConfig(void);
//    WaveWidgetLabel *_name;
    WaveWidgetLabel *_gain;         // 增益标签
    WaveWidgetLabel *_notify;
};
