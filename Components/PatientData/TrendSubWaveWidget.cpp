/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/25
 **/

#include "TrendSubWaveWidget.h"
#include "IWidget.h"
#include <QPainter>
#include <QLabel>
#include "TimeDate.h"
#include "ParamInfo.h"
#include "ParamManager.h"
#include "AlarmConfig.h"

#define GRAPH_POINT_NUMBER          120
#define DATA_INTERVAL_PIXEL         5
#define TREND_DISPLAY_OFFSET        60
#define isEqual(a, b) (qAbs((a)-(b)) < 0.000001)

TrendSubWaveWidget::TrendSubWaveWidget(SubParamID id, TrendGraphType type) : _id(id), _type(type),
    _trendInfo(TrendGraphInfo()), _xSize(0), _ySize(0), _trendDataHead(0), _cursorPosIndex(0)
{
    SubParamID subID = id;
    ParamID paramId = paramInfo.getParamID(subID);
    UnitType unitType = paramManager.getSubParamUnit(paramId, subID);

    if (_type == TREND_GRAPH_TYPE_NORMAL || _type == TREND_GRAPH_TYPE_AG_TEMP)
    {
        ParamRulerConfig config = alarmConfig.getParamRulerConfig(subID, unitType);
        if (config.scale == 1)
        {
            _valueY.min = config.downRuler;
            _valueY.max = config.upRuler;
        }
        else
        {
            _valueY.min = static_cast<double>(config.downRuler) / config.scale;
            _valueY.max = static_cast<double>(config.upRuler) / config.scale;
        }
        _paramName = paramInfo.getSubParamName(id);
        if (_type == TREND_GRAPH_TYPE_AG_TEMP)
        {
            _paramName = _paramName.right(_paramName.length() - 2) + "(Et/Fi)";
        }
        if (_id == SUB_PARAM_T1)
        {
            _paramName = "T1/T2";
        }
    }
    else if (_type == TREND_GRAPH_TYPE_ART_IBP || _type == TREND_GRAPH_TYPE_NIBP)
    {
        ParamRulerConfig configUp = alarmConfig.getParamRulerConfig(SubParamID(subID), unitType);
        ParamRulerConfig configDown = alarmConfig.getParamRulerConfig(SubParamID(subID + 1), unitType);
        if (configUp.scale == 1)
        {
            _valueY.min = configDown.downRuler;
            _valueY.max = configUp.upRuler;
        }
        else
        {
            _valueY.min = static_cast<double>(configDown.downRuler) / configDown.scale;
            _valueY.max = static_cast<double>(configUp.upRuler) / configUp.scale;
        }
        QString str = paramInfo.getSubParamName(id);
        _paramName = str.left(str.length() - 4);
    }
    _paramUnit = Unit::localeSymbol(paramInfo.getUnitOfSubParam(id));

    setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

TrendSubWaveWidget::~TrendSubWaveWidget()
{
    _trendInfo.reset();
}

void TrendSubWaveWidget::trendDataInfo(TrendGraphInfo &info)
{
    _trendInfo = info;
    _cursorPosIndex = 0;
}

void TrendSubWaveWidget::loadTrendSubWidgetInfo(TrendSubWidgetInfo &info)
{
    _info = info;
    _valueY.start = info.yTop;
    _valueY.end = info.yBottom;
    _timeX.start = info.xHead;
    _timeX.end = info.xTail;


    _xSize = info.xHead - info.xTail;
    _ySize = info.yBottom - info.yTop;
    _trendDataHead = info.xHead + info.xTail;
}

void TrendSubWaveWidget::getValueLimit(int &max, int &min)
{
    max = _valueY.max;
    min = _valueY.min;
}

void TrendSubWaveWidget::setThemeColor(QColor color)
{
    if (color != QColor(0, 0, 0))
    {
        _color = color;
    }
}

void TrendSubWaveWidget::setRulerRange(int down, int up, int scale)
{
    _valueY.max = static_cast<double>(up) / scale;
    _valueY.min = static_cast<double>(down) / scale;
    update();
}

void TrendSubWaveWidget::setTimeRange(unsigned leftTime, unsigned rightTime)
{
    _timeX.max = leftTime;
    _timeX.min = rightTime;
}

void TrendSubWaveWidget::cursorMove(int position)
{
    _cursorPosIndex = position;
    update();
}

SubParamID TrendSubWaveWidget::getSubParamID()
{
    return _id;
}

QList<QPainterPath> TrendSubWaveWidget::generatorPainterPath(const TrendGraphInfo &graphInfo)
{
    QList<QPainterPath> paths;
    switch (_type)
    {
    case TREND_GRAPH_TYPE_NIBP:
    {
        QPainterPath path;

        QVector<TrendGraphDataV3>::ConstIterator iter = graphInfo.trendDataV3.constBegin();
        for (; iter != graphInfo.trendDataV3.constEnd(); iter ++)
        {
            if (iter->data[0] == InvData())
            {
                continue;
            }

            qreal x = _mapValue(_timeX, iter->timestamp);
            qreal sys = _mapValue(_valueY, iter->data[0]);
            qreal dia = _mapValue(_valueY, iter->data[1]);
            qreal map = _mapValue(_valueY, iter->data[2]);

            path.moveTo(x - 3, sys - 3);
            path.lineTo(x, sys);
            path.lineTo(x + 3, sys - 3);

            path.moveTo(x - 3, dia + 3);
            path.lineTo(x, dia);
            path.lineTo(x + 3, dia + 3);

            path.moveTo(x, sys);
            path.lineTo(x, dia);

            path.moveTo(x - 3, map);
            path.lineTo(x + 3, map);
        }
        paths.append(path);
    }
    break;
    case TREND_GRAPH_TYPE_AG_TEMP:
    {
        QPainterPath fristPath;
        QPainterPath secondPath;

        bool lastPointInvalid = true;
        QPointF fristPoint;
        QPointF secondPoint;

        QVector<TrendGraphDataV2>::ConstIterator iter = graphInfo.trendDataV2.constBegin();
        for (; iter != graphInfo.trendDataV2.constEnd(); iter++)
        {
            if (iter->data[0] == InvData())
            {
                if (!lastPointInvalid)
                {
                    fristPath.lineTo(fristPoint);
                    secondPath.lineTo(secondPoint);
                    lastPointInvalid = true;
                }
                continue;
            }

            qreal x = _mapValue(_timeX, iter->timestamp);
            qreal value1 = _mapValue(_valueY, iter->data[0] / 10);
            qreal value2 = _mapValue(_valueY, iter->data[1] / 10);

            if (lastPointInvalid)
            {
                fristPath.moveTo(x, value1);
                secondPath.moveTo(x, value2);
                lastPointInvalid = false;
            }
            else
            {
                if (!isEqual(fristPoint.y(), value1))
                {
                    fristPath.lineTo(fristPoint);
                    fristPath.lineTo(x, value1);
                }

                if (!isEqual(secondPoint.y(), value2))
                {
                    secondPath.lineTo(secondPoint);
                    secondPath.lineTo(x, value2);
                }
            }

            fristPoint.rx() = x;
            fristPoint.ry() = value1;
            secondPoint.rx() = x;
            secondPoint.ry() = value2;
        }

        if (!lastPointInvalid)
        {
            fristPath.lineTo(fristPoint);
            secondPath.lineTo(secondPoint);
        }

        paths.append(fristPath);
        paths.append(secondPath);
    }
    break;
    case TREND_GRAPH_TYPE_ART_IBP:
    {
        QPainterPath sysPath;
        QPainterPath diaPath;
        QPainterPath mapPath;

        bool lastPointInvalid = true;
        QPointF sysLastPoint;
        QPointF diaLastPoint;
        QPointF mapLastPoint;

        QVector<TrendGraphDataV3>::ConstIterator iter = graphInfo.trendDataV3.constBegin();
        for (; iter != graphInfo.trendDataV3.constEnd(); iter++)
        {
            if (iter->data[0] == InvData())
            {
                if (!lastPointInvalid)
                {
                    sysPath.lineTo(sysLastPoint);
                    diaPath.lineTo(diaLastPoint);
                    mapPath.lineTo(mapLastPoint);
                    lastPointInvalid = true;
                }
                continue;
            }

            qreal x = _mapValue(_timeX, iter->timestamp);
            qreal sys = _mapValue(_valueY, iter->data[0]);
            qreal dia = _mapValue(_valueY, iter->data[1]);
            qreal map = _mapValue(_valueY, iter->data[2]);

            if (lastPointInvalid)
            {
                sysPath.moveTo(x, sys);
                diaPath.moveTo(x, dia);
                mapPath.moveTo(x, map);
                lastPointInvalid = false;
            }
            else
            {
                if (!isEqual(sysLastPoint.y(), sys))
                {
                    sysPath.lineTo(sysLastPoint);
                    sysPath.lineTo(x, sys);
                }

                if (!isEqual(diaLastPoint.y(), dia))
                {
                    diaPath.lineTo(diaLastPoint);
                    diaPath.lineTo(x, dia);
                }

                if (!isEqual(mapLastPoint.y(), map))
                {
                    mapPath.lineTo(mapLastPoint);
                    mapPath.lineTo(x, map);
                }
            }

            sysLastPoint.rx() = x;
            sysLastPoint.ry() = sys;
            diaLastPoint.rx() = x;
            diaLastPoint.ry() = dia;
            mapLastPoint.rx() = x;
            mapLastPoint.ry() = map;
        }

        if (!lastPointInvalid)
        {
            sysPath.lineTo(sysLastPoint);
            diaPath.lineTo(diaLastPoint);
            mapPath.lineTo(mapLastPoint);
        }

        paths.append(sysPath);
        paths.append(diaPath);
        paths.append(mapPath);
    }
    break;
    case TREND_GRAPH_TYPE_NORMAL:
    {
        QPainterPath path;

        QPointF lastPoint;
        bool lastPointInvalid = true;
        QVector<TrendGraphData>::ConstIterator iter = graphInfo.trendData.constBegin();
        for (; iter != graphInfo.trendData.constEnd(); iter ++)
        {
            if (iter->data == InvData())
            {
                if (!lastPointInvalid)
                {
                    path.lineTo(lastPoint);
                    lastPointInvalid = true;
                }
                continue;
            }

            qreal x = _mapValue(_timeX, iter->timestamp);
            qreal y = _mapValue(_valueY, iter->data);

            if (lastPointInvalid)
            {
                path.moveTo(x, y);
                lastPointInvalid = false;
            }
            else
            {
                if (!isEqual(lastPoint.y(), y))
                {
                    path.lineTo(lastPoint);
                    path.lineTo(x, y);
                }
            }

            lastPoint.rx() = x;
            lastPoint.ry() = y;
        }

        if (!lastPointInvalid)
        {
            path.lineTo(lastPoint);
        }

        paths.append(path);
    }
    break;
    }
    return paths;
}

void TrendSubWaveWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);

    QPainter barPainter(this);
    barPainter.setPen(QPen(_color, 1, Qt::SolidLine));

    barPainter.drawLine(_info.xHead / 3, _info.yTop, _info.xHead / 3 + 5, _info.yTop);
    barPainter.drawLine(_info.xHead / 3, _info.yTop, _info.xHead / 3, _info.yBottom);
    barPainter.drawLine(_info.xHead / 3, _info.yBottom, _info.xHead / 3 + 5, _info.yBottom);

    // 趋势标尺上下限
    QRect upRulerRect(_info.xHead / 3 + 7, _info.yTop - 10, _info.xHead, 30);
    QRect downRulerRect(_info.xHead / 3 + 7, _info.yBottom - 10, _info.xHead, 30);
    if (_type == TREND_GRAPH_TYPE_AG_TEMP)
    {
        barPainter.drawText(upRulerRect, Qt::AlignLeft | Qt::AlignTop, QString::number(_valueY.max, 'f', 1));
        barPainter.drawText(downRulerRect, Qt::AlignLeft | Qt::AlignTop, QString::number(_valueY.min, 'f', 1));
    }
    else
    {
        barPainter.drawText(upRulerRect, Qt::AlignLeft | Qt::AlignTop, QString::number(_valueY.max));
        barPainter.drawText(downRulerRect, Qt::AlignLeft | Qt::AlignTop, QString::number(_valueY.min));
    }


    // 边框线
    barPainter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
    barPainter.drawLine(rect().bottomLeft(), rect().bottomRight());

    // 趋势波形图
    barPainter.setPen(QPen(_color, 2, Qt::SolidLine));
    if (!_trendInfo.alarmInfo.count())
    {
        return;
    }
    barPainter.save();
    QList<QPainterPath> paths = generatorPainterPath(_trendInfo);
    QList<QPainterPath>::ConstIterator iter;
    for (iter = paths.constBegin(); iter != paths.constEnd(); iter++)
    {
        barPainter.save();
        barPainter.drawPath(*iter);
        barPainter.restore();
    }
    barPainter.restore();
    barPainter.setPen(QPen(_color, 1, Qt::SolidLine));

    QFont font;
    font.setPixelSize(15);
    barPainter.setFont(font);
    // 趋势参数名称
    QRect nameRect(_trendDataHead + 5, 5, width() - _trendDataHead - 10, 30);
    barPainter.drawText(nameRect, Qt::AlignLeft | Qt::AlignTop, _paramName);

    // 趋势参数单位
    QRect unitRect(_trendDataHead + 5, 5, width() - _trendDataHead - 10, 30);
    barPainter.drawText(unitRect, Qt::AlignRight | Qt::AlignTop, _paramUnit);
    font.setPixelSize(60);
    barPainter.setFont(font);

    // 趋势参数数据
    QRect dataRect(_trendDataHead, height() / 5, width() - _trendDataHead, height() / 5 * 4);
    if (_type == TREND_GRAPH_TYPE_NORMAL)
    {
        if (_trendInfo.trendData.isEmpty())
        {
            return;
        }
        if (_trendInfo.trendData.at(_cursorPosIndex).isAlarm)
        {
            barPainter.fillRect(dataRect, Qt::white);
        }
        else
        {
            barPainter.fillRect(dataRect, Qt::black);
        }
        TrendDataType value = _trendInfo.trendData.at(_cursorPosIndex).data;
        if (value != InvData())
        {
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height() / 3 * 2, QString::number(value));
        }
        else
        {
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height() / 3 * 2, "---");
        }
    }
    else if (_type == TREND_GRAPH_TYPE_NIBP || _type == TREND_GRAPH_TYPE_ART_IBP)
    {
        font.setPixelSize(30);
        barPainter.setFont(font);
        if (_trendInfo.trendDataV3.isEmpty())
        {
            return;
        }

        if (_trendInfo.trendDataV3.at(_cursorPosIndex).isAlarm)
        {
            barPainter.fillRect(dataRect, Qt::white);
        }
        else
        {
            barPainter.fillRect(dataRect, Qt::black);
        }
        TrendDataType sys =  _trendInfo.trendDataV3.at(_cursorPosIndex).data[0];
        TrendDataType dia = _trendInfo.trendDataV3.at(_cursorPosIndex).data[1];
        TrendDataType map = _trendInfo.trendDataV3.at(_cursorPosIndex).data[2];

        if (map != InvData() && dia != InvData() && sys != InvData())
        {
            QString trendStr = QString::number(sys) + "/" + QString::number(dia);
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height() / 2, trendStr);
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET + 10, height() / 3 * 2, QString::number(map));
        }
        else
        {
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height() / 2, "---/---");
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET + 10, height() / 3 * 2, "(---)");
        }
    }
    else if (_type == TREND_GRAPH_TYPE_AG_TEMP)
    {
        font.setPixelSize(30);
        barPainter.setFont(font);

        if (_trendInfo.trendDataV2.isEmpty())
        {
            return;
        }

        if (_trendInfo.trendDataV2.at(_cursorPosIndex).isAlarm)
        {
            barPainter.fillRect(dataRect, Qt::white);
        }
        else
        {
            barPainter.fillRect(dataRect, Qt::black);
        }
        double value1 = _trendInfo.trendDataV2.at(_cursorPosIndex).data[0];
        double value2 = _trendInfo.trendDataV2.at(_cursorPosIndex).data[1];

        if (value1 != InvData() && value2 != InvData())
        {
            value1 /= 10;
            value2 /= 10;
            QString trendStr = QString::number(value1, 'f', 1) + "/" + QString::number(value2, 'f', 1);
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height() / 2, trendStr);
        }
        else
        {
            barPainter.drawText(_trendDataHead + TREND_DISPLAY_OFFSET, height() / 2, "---/---");
        }
    }
}

double TrendSubWaveWidget::_mapValue(TrendConvertDesc desc, int data)
{
    if (data == InvData())
    {
        return InvData();
    }

    double dpos = 0;
    dpos = (desc.max - data) * (desc.end - desc.start) / (desc.max - desc.min) + desc.start;

    if (dpos < desc.start)
    {
        dpos = desc.start;
    }
    else if (dpos > desc.end)
    {
        dpos = desc.end;
    }

    return dpos;
}
