/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#pragma once
#include "TrendWidget.h"
#include <QLabel>

class RESPTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 窗口是否使能
    bool enable();

    // 刷新BR和RR的值。
    void setRRValue(int16_t rr, bool isRR);

    // 刷新趋势参数上下限
    void updateLimit(void);

    // 是否报警
    void isAlarm(bool flag);

    // 显示
    void showValue(void);

    RESPTrendWidget();
    ~RESPTrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;
public:
    virtual void doRestoreNormalStatus();

    /* reimplment */
    void updateWidgetConfig();

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

    /**
     * @brief _onBrSourceStatusUpdate  br来源状态更新槽函数
     */
    void _onBrSourceStatusUpdate();

private:
    /**
     * @brief _loadConfig  加载配置
     */
    void _loadConfig();

    QLabel *_rrValue;
    bool _isAlarm;
    QString _rrString;
    QLabel *_rrSource;
};
