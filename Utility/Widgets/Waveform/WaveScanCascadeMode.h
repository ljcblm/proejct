////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的级联扫描模式
////////////////////////////////////////////////////////////////////////////////
#ifndef WAVE_SCAN_CASCADE_MODE_H
#define WAVE_SCAN_CASCADE_MODE_H

#include "WaveCascadeMode.h"

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的级联扫描模式
////////////////////////////////////////////////////////////////////////////////
class WaveScanCascadeMode: public WaveCascadeMode
{
public:
    explicit WaveScanCascadeMode(WaveWidget *wave);
    ~WaveScanCascadeMode();
    bool match(int mode, bool isCascade);
    void addData(int value, int flag, bool isUpdated=true);
    bool updateRegion(QRegion &region);
    int maxUpdateRate();
    void paintWave(QPainter &painter, const QRect &rect);
};

#endif
