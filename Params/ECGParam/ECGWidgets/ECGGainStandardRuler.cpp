/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/3
 **/

#include "ECGGainStandardRuler.h"
#include "ECGWaveWidget.h"
#include "FontManager.h"
#include <QPainter>
#include "SystemManager.h"
#include <QRect>
#include "Debug.h"
#include "Framework/UI/ThemeManager.h"
#include "FontManager.h"

#define START_X_OFFSET (160)

void ECGGainStandardRuler::paintItem(QPainter &painter)
{
    painter.setPen(QPen(palette().color(QPalette::Foreground).darker(150), 3, Qt::SolidLine));
    painter.setFont(font());
    for (int i = 0; i < gainRuler.count(); ++i)
    {
        float pixelHPitch = systemManager.getScreenPixelHPitch();
        int rulerPos = START_X_OFFSET + i * 80;
        int rulerHeight = gainRuler.at(i);
        int middlePos = _waveWidget->height() / 2;
        int fontHeight = fontManager.textHeightInPixels(painter.font());
        QRect rulerRect(rulerPos + 5, middlePos + 10 , 100, middlePos + 10 + fontHeight);

        painter.drawLine(rulerPos, middlePos - rulerHeight / pixelHPitch / 2, rulerPos,
                         middlePos + rulerHeight / pixelHPitch / 2);
        QString rulerStr = QString::number(rulerHeight) + QString("mm");
        painter.drawText(rulerRect, rulerStr);
    }
}

ECGGainStandardRuler::ECGGainStandardRuler(ECGWaveWidget *wave) :
    WaveWidgetItem(wave, true),
    _waveWidget(wave)
{
    // user define.
    gainRuler.append(10);
    gainRuler.append(12);
    gainRuler.append(13);
    gainRuler.append(20);
    gainRuler.append(21);
    gainRuler.append(24);
    gainRuler.append(25);
}

ECGGainStandardRuler::~ECGGainStandardRuler()
{
}
