#include "ContinuousPageGenerator.h"
#include "LanguageManager.h"
#include "PatientManager.h"
#include "TrendCache.h"
#include "TimeDate.h"
#include "SystemManager.h"
#include "ParamInfo.h"
#include "WaveformCache.h"
#include "RecorderManager.h"
#include "WindowManager.h"
#include "ECGParam.h"
#include "RESPParam.h"
#include "SPO2Param.h"
#include "CO2Param.h"
#include "AGParam.h"
#include "IBPParam.h"
#include "ParamManager.h"
#include "Utility.h"

class ContinuousPageGeneratorPrivate
{
public:
    ContinuousPageGeneratorPrivate(ContinuousPageGenerator * const q_ptr)
        : q_ptr(q_ptr),
          curPageType(RecordPageGenerator::TitlePage),
          curDrawWaveSegment(0),
          totalDrawWaveSegment(-1)
    {
        TrendCacheData data;
        TrendAlarmStatus almStatus;
        unsigned t = timeDate.time();
        trendCache.getTendData(t, data);
        trendCache.getTrendAlarmStatus(t, almStatus);
        bool alarm = false;
        foreach (bool st, almStatus.alarms) {
            if(st)
            {
                alarm = true;
                break;
            }
        }
        trendData.subparamValue = data.values;
        trendData.subparamAlarm = almStatus.alarms;
        trendData.co2Baro = data.co2baro;
        trendData.time = t;
        trendData.alarmFlag = alarm;
    }

    QList<RecordWaveSegmentInfo> getPrintWaveInfos();

    void fetchWaveData();

    bool isParamStop(WaveformID id);

    ContinuousPageGenerator *q_ptr;
    RecordPageGenerator::PageType curPageType;
    TrendDataPackage trendData;
    QList<RecordWaveSegmentInfo> waveInfos;
    int curDrawWaveSegment;
    int totalDrawWaveSegment;   //total wave segments that need to draw, -1 means unlimit
};

/**
 * @brief ContinuousPageGeneratorPrivate::getPrintWaveInfo get the continuous print wave info
 * @return a list contain the wave segment info of the waves that need to print
 */
QList<RecordWaveSegmentInfo> ContinuousPageGeneratorPrivate::getPrintWaveInfos()
{
    QList<int> waveID;

    int printWaveNum = recorderManager.getPrintWaveNum();

    windowManager.getDisplayedWaveform(waveID);

    QList<WaveformID> waves;

    //FIXME: Currently, we print the the first @printWaveNum waves in the window manager,
    //       but we should dispay the waveform form the configuration
    for(int i = 0; i< waveID.length() && i < printWaveNum; i++)
    {
        waves.append((WaveformID)waveID.at(i));
    }

    return RecordPageGenerator::getWaveInfos(waves);
}

/**
 * @brief ContinuousPageGeneratorPrivate::fetchWaveData
 *          get the wave data from the realtime channel in the wavecache.
 *          we should fetch 1 second wave data form the realtimeChannel,
 *          if wavecache don't have enough data, we fill invalid data.
 */
void ContinuousPageGeneratorPrivate::fetchWaveData()
{
    QList<RecordWaveSegmentInfo>::iterator iter;
    for(iter = waveInfos.begin(); iter < waveInfos.end(); iter++)
    {
        if(iter->secondWaveBuff.size() != iter->sampleRate)
        {
            iter->secondWaveBuff.resize(iter->sampleRate);
        }

        int curSize = 0;
        int retryCount = 0;
        int lastReadSize = 0;
        while(curSize < iter->sampleRate)
        {
            if(recorderManager.isAbort())
            {
                //param stop
                break;
            }

            if(isParamStop(iter->id))
            {
                //param stop
                break;
            }

            curSize += waveformCache.readRealtimeChannel(iter->id,
                                                         iter->sampleRate - curSize,
                                                         iter->secondWaveBuff.data() + curSize);

            if(++retryCount >= 200) //1000 ms has passed and haven't finished reading
            {
                if(curSize == lastReadSize)
                {
                    break;
                }
            }

            lastReadSize = curSize;
            Util::waitInEventLoop(5);
        }

        if(curSize < iter->sampleRate)
        {
            //no enough data, fill with invalid
            qFill(iter->secondWaveBuff.data() + curSize, iter->secondWaveBuff.data()+iter->sampleRate, (WaveDataType) INVALID_WAVE_FALG_BIT);
        }
    }
}

/**
 * @brief ContinuousPageGeneratorPrivate::isParamStop check whether the param is stopped
 * @param id waveform id
 * @return true if stopped, otherwise, return false;
 */
bool ContinuousPageGeneratorPrivate::isParamStop(WaveformID id)
{
    return paramManager.isParamStopped(paramInfo.getParamID(id));
}

ContinuousPageGenerator::ContinuousPageGenerator(QObject *parent)
    :RecordPageGenerator(parent), d_ptr(new ContinuousPageGeneratorPrivate(this))
{
    d_ptr->waveInfos = d_ptr->getPrintWaveInfos();
}

ContinuousPageGenerator::~ContinuousPageGenerator()
{
}

int ContinuousPageGenerator::type() const
{
    return Type;
}

RecordPageGenerator::PrintPriority ContinuousPageGenerator::getPriority() const
{
    return PriorityContinuous;
}

RecordPage *ContinuousPageGenerator::createPage()
{
    switch(d_ptr->curPageType)
    {
    case TitlePage:
        d_ptr->curPageType = TrendPage;
        return createTitlePage(QString(trs("RealtimeSegmentWaveRecording")), patientManager.getPatientInfo(), 0);
    case TrendPage:
        d_ptr->curPageType = WaveScalePage;
        return createTrendPage(d_ptr->trendData, false);
    case WaveScalePage:
        d_ptr->curPageType = WaveSegmentPage;
        return createWaveScalePage(d_ptr->waveInfos, recorderManager.getPrintSpeed());
    case WaveSegmentPage:
    if(!recorderManager.isAbort())
    {
        RecordPage *page;
        d_ptr->fetchWaveData();

        page = createWaveSegments(d_ptr->waveInfos, d_ptr->curDrawWaveSegment++, recorderManager.getPrintSpeed());

        if(d_ptr->totalDrawWaveSegment > 0 && d_ptr->curDrawWaveSegment >= d_ptr->totalDrawWaveSegment)
        {
            d_ptr->curPageType = EndPage;
        }
        return page;
    }
    //fall through
    case EndPage:
        d_ptr->curPageType = NullPage;
        return createEndPage();
    default:
        break;
    }
    return NULL;
}

void ContinuousPageGenerator::onStartGenerate()
{
    waveformCache.startRealtimeChannel();
}

void ContinuousPageGenerator::onStopGenerate()
{
    waveformCache.stopRealtimeChannel();
}

