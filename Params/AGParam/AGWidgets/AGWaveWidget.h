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
#include "AGDefine.h"
#include "ParamInfo.h"

class AGWaveRuler;
class PopupList;
class AGWaveWidget: public WaveWidget
{
    Q_OBJECT

public:
    // 设置麻醉剂类型
    void setAnestheticType(AGAnaestheticType type);

    // 添加波形数据。
    void addWaveformData(short data, int flag = 0);

    // 设置波形上下限
    void setLimit(int low, int high);

    // 设置标尺。
    void setRuler(AGDisplayZoom zoom);

    /**
     * @brief resetBackground reset background
     */
    void updatePalette(const QPalette &pal);

    // 构造与析构。
    AGWaveWidget(WaveformID id, const QString &waveName, const AGTypeGas gasType);
    ~AGWaveWidget();

    /* reimplment */
    void updateWidgetConfig();

protected:
    // 窗体大小调整事件
    virtual void resizeEvent(QResizeEvent *e);

    // 焦点进入
    virtual void focusInEvent(QFocusEvent */*e*/);

private slots:
    void _getItemIndex(int);

private:
    AGWaveRuler *_ruler;               // 标尺对象
    int _currentItemIndex;
};
