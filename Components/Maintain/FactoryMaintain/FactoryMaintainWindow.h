/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/

#pragma once
#include "Window.h"

class FactoryMaintainWindowPrivate;
class FactoryMaintainWindow : public Window
{
    Q_OBJECT
public:
    FactoryMaintainWindow();
    ~FactoryMaintainWindow();
    /**
     * @brief layoutExec
     */
    void layoutExec();

private slots:
    void userInputCorrect(void);

private:
    FactoryMaintainWindowPrivate * const d_ptr;
};