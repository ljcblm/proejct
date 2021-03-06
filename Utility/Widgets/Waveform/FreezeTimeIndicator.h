#pragma once
#include "WaveWidgetItem.h"

class FreezeTimeIndicator : public WaveWidgetItem
{
public:
    explicit FreezeTimeIndicator(WaveWidget *w);
    virtual void paintItem(QPainter &painter);

    void setReviewTime(int t);

private:
    int _reviewTime;
};
