#include "OxyCRGPageGenerator.h"
#include "PatientManager.h"
#include "TrendCache.h"
#include "TimeDate.h"

class OxyCRGPageGeneratorPrivate
{
public:
    OxyCRGPageGeneratorPrivate()
        :curPageType(RecordPageGenerator::TitlePage)
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

    TrendDataPackage trendData;
    RecordPageGenerator::PageType curPageType;
    QList<TrendGraphInfo> trendInfos;
    OxyCRGWaveInfo oxyCRGWaveInfo;
};

OxyCRGPageGenerator::OxyCRGPageGenerator(const QList<TrendGraphInfo> &trendInfos,
                                         const OxyCRGWaveInfo &waveInfo,
                                         QObject *parent)
    :RecordPageGenerator(parent), d_ptr(new OxyCRGPageGeneratorPrivate)
{
    d_ptr->trendInfos = trendInfos;
    d_ptr->oxyCRGWaveInfo = waveInfo;
}

OxyCRGPageGenerator::~OxyCRGPageGenerator()
{

}

int OxyCRGPageGenerator::type() const
{
    return Type;
}

RecordPage *OxyCRGPageGenerator::createPage()
{
    switch(d_ptr->curPageType)
    {
    case TitlePage:
        // BUG: patient info of the event might not be the current session patient
        d_ptr->curPageType = TrendPage;
        return createTitlePage(QString(trs("OxyCRGRecording")), patientManager.getPatientInfo());
    case TrendPage:
        d_ptr->curPageType = TrendOxyCRGPage;
        return createTrendPage(d_ptr->trendData, true);
    case TrendOxyCRGPage:
        d_ptr->curPageType = EndPage;
        return createOxyCRGGraph(d_ptr->trendInfos, d_ptr->oxyCRGWaveInfo);
    case EndPage:
        d_ptr->curPageType = NullPage;
        return createEndPage();
    default:
        break;
    }
    return NULL;
}