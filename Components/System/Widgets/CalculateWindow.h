/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/6
 **/

#pragma once
#include "Window.h"

class CalculateWindowPrivate;
class CalculateWindow : public Window
{
    Q_OBJECT
public:
    CalculateWindow();
    ~CalculateWindow();

    /**
     * @brief layoutExec
     */
    void layoutExec();

private slots:
    /**
     * @brief onBtnReleased
     */
    void onBtnReleased(void);

private:
    CalculateWindowPrivate *d_ptr;
};