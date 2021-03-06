/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/20
 **/

#include "NIBPServiceStandbyState.h"
#include "NIBPServiceStateDefine.h"
#include "NIBPParam.h"
#include "NIBPMaintainMgrInterface.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPServiceStandbyState::run(void)
{
}

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServiceStandbyState::enter(void)
{
    timeStop();
    NIBPMaintainMgrInterface *nibpMaintainMgr;
    nibpMaintainMgr = NIBPMaintainMgrInterface::getNIBPMaintainMgr();
    if (nibpMaintainMgr)
    {
        nibpMaintainMgr->setMonitorState(NIBP_MONITOR_STANDBY_STATE);
    }
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPServiceStandbyState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int /*argLen*/)
{
    NIBPMaintainMgrInterface *nibpMaintainMgr;
    nibpMaintainMgr = NIBPMaintainMgrInterface::getNIBPMaintainMgr();
    switch (event)
    {
    case NIBP_EVENT_MODULE_RESET:
        if (nibpMaintainMgr)
        {
            nibpMaintainMgr->unPacket(false);
        }
        switchState(NIBP_SERVICE_ERROR_STATE);
        break;

    case NIBP_EVENT_TIMEOUT:
    {
    }
    break;

    case NIBP_EVENT_SERVICE_REPAIR_ENTER_SUCCESS:
        if (nibpMaintainMgr)
        {
            nibpMaintainMgr->unPacket(true);
        }
        break;

    case NIBP_EVENT_SERVICE_REPAIR_ENTER_FAIL:
        if (nibpMaintainMgr)
        {
            nibpMaintainMgr->unPacket(false);
        }
        break;

    case NIBP_EVENT_SERVICE_CALIBRATE_ENTER:
    case NIBP_EVENT_SERVICE_MANOMETER_ENTER:
    case NIBP_EVENT_SERVICE_PRESSURECONTROL_ENTER:
    case NIBP_EVENT_SERVICE_CALIBRATE_ZERO_ENTER:
        nibpParam.setResult(!args[0]);
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServiceStandbyState::NIBPServiceStandbyState() : NIBPState(NIBP_SERVICE_STANDBY_STATE)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServiceStandbyState::~NIBPServiceStandbyState()
{
}
