#pragma once
#include "SPO2Define.h"

// 定义ECG数据提供对象需要实现的接口方法。
class SPO2ProviderIFace
{
public:

    // 设置灵敏度。
    virtual void setSensitive(SPO2Sensitive sens) = 0;

    // 查询状态。
    virtual void sendStatus(void) { }

    // 获取波形的采样速度。
    virtual int getSPO2WaveformSample(void) = 0;

    // 获取波形基线
    virtual int getSPO2BaseLine(void) = 0;

    // 取值范围
    virtual int getSPO2MaxValue(void) = 0;

    // SPO2值与PR值。
    virtual bool isResult_SPO2PR(unsigned char */*packet*/) {return false;}

    //描记波
    virtual bool isResult_BAR(unsigned char */*packet*/) {return false;}

    //状态
    virtual bool isStatus(unsigned char */*packet*/) {return false;}


    //发送协议命令
    virtual void sendCmdData(unsigned char /*cmdId*/, const unsigned char */*data*/,
                             unsigned int /*len*/) { }

    // 析构。
    virtual ~SPO2ProviderIFace() { }


public:
    //Masimo

    // 设置灵敏度和FastSat。
    virtual void setSensitivityFastSat(SensitivityMode /*mode*/, bool /*fastSat*/) { }

    // 设置平均时间。
    virtual void setAverageTime(AverageTime /*mode*/) { }

    // 设置SMart Tone使能选项。
    virtual void setSmartTone(bool /*enable*/) { }

    //Nellcor
    /**
     * @brief setWarnLimitValue  设置报警限值
     * @param spo2Low  血氧低限
     * @param spo2High  血氧高限
     * @param prLow  脉率低限
     * @param prHigh  脉率高限
     */
    virtual void setWarnLimitValue(char /*spo2Low*/, char /*spo2High*/, short /*prLow*/, short /*prHigh*/){}
    /**
     * @brief getWarnLimitValue 查询报警限值
     */
    virtual void getWarnLimitValue(){}
    /**
     * @brief setSatSeconds 设置过高低限值的容忍时间
     * @param type 时间类型
     */
    virtual void setSatSeconds(Spo2SatSecondsType /*type*/){}
};
