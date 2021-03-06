/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/20
 **/

#include "WindowManagerInterface.h"
#include <stddef.h>

static WindowManagerInterface *windowManager = NULL;

WindowManagerInterface *WindowManagerInterface::registerWindowManager(WindowManagerInterface *instance)
{
    WindowManagerInterface *old = windowManager;
    windowManager = instance;
    return old;
}

WindowManagerInterface *WindowManagerInterface::getWindowManager()
{
    return windowManager;
}
