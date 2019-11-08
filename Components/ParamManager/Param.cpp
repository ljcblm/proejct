/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

#include "Param.h"
#include "ColorManager.h"

/**************************************************************************************************
 * 功能： 获取名称。
 * 返回： Param的名称。
 *************************************************************************************************/
ParamID Param::getParamID(void)
{
    return _paramID;
}

/**************************************************************************************************
 * 功能： 获取名称。
 * 返回： Param的名称。
 *************************************************************************************************/
const QString &Param::getName(void)
{
    return _paramName;
}

/**************************************************************************************************
 * 功能： 使能。
 *************************************************************************************************/
void Param::enable(void)
{
    _isEnable = true;
}

/**************************************************************************************************
 * 功能： 禁用。
 *************************************************************************************************/
void Param::disable(void)
{
    _isEnable = false;
}

/**************************************************************************************************
 * 功能： 是否使能。
 *************************************************************************************************/
bool Param::isEnabled(void)
{
    return _isEnable;
}

bool Param::isConnected()
{
    return true;
}

/**************************************************************************************************
 * 功能： 初始化参数。
 *************************************************************************************************/
void Param::initParam(void)
{
}

/**************************************************************************************************
 * 功能： 处理DEMO数据。
 * 参数：
 *      id：波形的标识ID。
 *      data：数据。
 *************************************************************************************************/
void Param::handDemoWaveform(WaveformID /*id*/, short /*data*/)
{
}

/**************************************************************************************************
 * 功能： 处理DEMO数据。
 *************************************************************************************************/
void Param::handDemoTrendData(void)
{
}

void Param::exitDemo()
{
}

/**************************************************************************************************
 * 功能： 获取当前的波形控件名称。
 *************************************************************************************************/
void Param::getAvailableWaveforms(QStringList *waveforms, QStringList *waveformShowName, int flag)
{
    Q_UNUSED(flag)
    waveforms->clear();
    waveformShowName->clear();
}

/**************************************************************************************************
 * 功能： 获取趋势控件名称。
 *************************************************************************************************/
QString Param::getTrendWindowName()
{
    return QString();
}

/**************************************************************************************************
 * 功能： 获取当前的波形控件名称。
 *************************************************************************************************/
QString Param::getWaveWindowName()
{
    return QString();
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short Param::getSubParamValue(SubParamID /*id*/)
{
    return 0;
}

/**************************************************************************************************
 * check whether a sub param is avaliable, always avaliable in default implement
 *************************************************************************************************/
bool Param::isSubParamAvaliable(SubParamID /*id*/)
{
    return true;
}

/**************************************************************************************************
 * 功能： 显示子参数值。
 *************************************************************************************************/
void Param::showSubParamValue()
{
}

/**************************************************************************************************
 * 功能： 获取当前的单位。
 *************************************************************************************************/
UnitType Param::getCurrentUnit(SubParamID /*id*/)
{
    return UNIT_NONE;
}

/**************************************************************************************************
 * 功能： 构造。
 *************************************************************************************************/
Param::Param(ParamID id)
    : paramUpdateTimer(NULL),
      _paramID(id),
      _paramName(paramInfo.getParamName(id)),
      _isEnable(true)
{
    paramUpdateTimer = new QTimer(this);
    paramUpdateTimer->setSingleShot(true);
    connect(paramUpdateTimer, SIGNAL(timeout()), this, SLOT(paramUpdateTimeout()));
    connect(&colorManager, SIGNAL(paletteChanged(ParamID)),
            this, SLOT(onPaletteChanged(ParamID)));
}

/**************************************************************************************************
 * 功能： 析构。
 *************************************************************************************************/
Param::~Param()
{
}

void Param::stopParamUpdateTimer()
{
    if (paramUpdateTimer != NULL)
    {
        paramUpdateTimer->stop();
    }
}
