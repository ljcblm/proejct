/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/30
 **/

#include "ScreenLayoutModel.h"
#include "WindowManager.h"
#include "DemoProvider.h"
#include "ParamManager.h"
#include "ScreenLayoutDefine.h"
#include <QLinkedList>
#include <QVector>
#include <QByteArray>
#include "IConfig.h"
#include "ParamInfo.h"

#define COLUMN_COUNT 6
#define ROW_COUNT 8
#define MAX_WAVE_ROW 6
#define WAVE_START_COLUMN 4

struct LayoutNode
{
    LayoutNode()
        : pos(0), span(1), waveId(WAVE_NONE), editable(true)
    {}

    QString name;
    int pos;
    int span;
    WaveformID waveId;
    bool editable;
};

typedef QLinkedList<LayoutNode *> LayoutRow;

class ScreenLayoutModelPrivate
{
public:
    ScreenLayoutModelPrivate()
        : demoProvider(NULL)
    {
        demoProvider = qobject_cast<DemoProvider *>(paramManager.getProvider("DemoProvider"));
    }

    ~ScreenLayoutModelPrivate()
    {
        clearAllLayoutNodes();
    }

    /**
     * @brief clearAllLayoutNodes clear all the layout nodes
     */
    void clearAllLayoutNodes()
    {
        QVector<LayoutRow *>::iterator iter;
        for (iter = layoutNodes.begin(); iter != layoutNodes.end(); ++iter)
        {
            qDeleteAll(**iter);
            (*iter)->clear();
        }
        qDeleteAll(layoutNodes);
        layoutNodes.clear();
    }

    static bool layoutNodeLessThan(const LayoutNode *n1, const LayoutNode *n2)
    {
        return n1->pos < n2->pos;
    }

    /**
     * @brief loadLayoutFromConfig load layout config
     * @param config
     */
    void loadLayoutFromConfig(const QVariantMap &config)
    {
        // clear all the node first
        clearAllLayoutNodes();


        QVariantList layoutRows = config["LayoutRow"].toList();
        if (layoutRows.isEmpty())
        {
            // might has only one element
            QVariant tmp = config["LayoutRow"];
            if (tmp.isValid())
            {
                layoutRows.append(tmp.toMap());
            }
            else
            {
                return;
            }
        }
        QVariantList::ConstIterator iter;
        for (iter = layoutRows.constBegin(); iter != layoutRows.constEnd(); ++iter)
        {
            QVariantList nodes = iter->toMap().value("LayoutNode").toList();

            if (nodes.isEmpty())
            {
                // might has only one element
                QVariantMap nm = iter->toMap().value("LayoutNode").toMap();
                if (nm.isEmpty())
                {
                    continue;
                }
                nodes.append(nm);
            }

            LayoutRow *row = new LayoutRow();

            QVariantList::ConstIterator nodeIter;
            for (nodeIter = nodes.constBegin(); nodeIter != nodes.constEnd(); ++nodeIter)
            {
                QVariantMap nodeMap = nodeIter->toMap();
                int span = nodeMap["@span"].toInt();
                if (span == 0)
                {
                    span = 1;
                }
                int pos = nodeMap["@pos"].toInt();
                bool editable = nodeMap["@editable"].toBool();
                LayoutNode *node = new LayoutNode();
                node->name = nodeMap["@text"].toString();
                node->editable = editable;
                node->pos = pos;
                node->span = span;
                node->waveId = waveIDMaps.value(node->name, WAVE_NONE);

                if (row->isEmpty())
                {
                    row->append(node);
                }
                else
                {
                    // the layout should be sorted by the pos
                    LayoutRow::Iterator iter = row->begin();
                    for (; iter != row->end(); ++iter)
                    {
                        if ((*iter)->pos > node->pos)
                        {
                            break;
                        }
                    }

                    row->insert(iter, node);
                }
            }
            layoutNodes.append(row);
        }
    }

    /**
     * @brief getLayoutMap get the layout map from the current layout info
     * @return
     */
    QVariantMap getLayoutMap() const
    {
        QVariantList layoutRows;
        QVector<LayoutRow *>::ConstIterator iter = layoutNodes.constBegin();
        for (; iter != layoutNodes.constEnd(); ++iter)
        {
            QVariantList nodes;

            LayoutRow::ConstIterator nodeIter = (*iter)->constBegin();
            for (; nodeIter != (*iter)->constEnd(); ++nodeIter)
            {
                LayoutNode *n = *nodeIter;
                QVariantMap m;
                m["@span"] = n->span;
                m["@pos"] = n->pos;
                m["@editable"] = n->editable ? 1 : 0;
                m["@text"] = n->name;
                nodes.append(m);
            }

            QVariantMap nm;
            nm["LayoutNode"] = nodes;
            layoutRows.append(nm);
        }

        QVariantMap rm;
        rm["LayoutRow"] = layoutRows;

        return rm;
    }

    /**
     * @brief getWaveData get the wave data from the wave cache, if the wave is not exist in the cache,
     *                    we will try to fetch the data from the demo provider
     * @param waveid
     * @return
     */
    QByteArray getWaveData(WaveformID waveid)
    {
        if (!waveCaches.contains(waveid))
        {
            QByteArray d;
            if (demoProvider)
            {
                d = demoProvider->getDemoWaveData(waveid);
            }
            waveCaches.insert(waveid, d);
        }
        return waveCaches[waveid];
    }

    /**
     * @brief fillWaveData fill the wave info if the @info has a valid waveid
     * @param info the info structure that need to fill wave infos
     */
    void fillWaveData(ScreenLayoutItemInfo &info)
    {
        if (info.waveid == WAVE_NONE || !demoProvider)
        {
            return;
        }

        info.waveContent = getWaveData(info.waveid);
        if (info.waveid >= WAVE_ECG_I && info.waveid <= WAVE_ECG_V6)
        {
            info.baseLine = demoProvider->getBaseLine();
            info.waveMaxValue = 255;
            info.waveMinValue = 0;
            info.sampleRate = demoProvider->getWaveformSample();
        }
        else if (info.waveid == WAVE_RESP)
        {
            info.waveMaxValue = demoProvider->maxRESPWaveValue();
            info.waveMinValue = demoProvider->minRESPWaveValue();
            info.baseLine = demoProvider->getRESPBaseLine();
            info.sampleRate = demoProvider->getRESPWaveformSample();
        }
        else if (info.waveid == WAVE_SPO2)
        {
            info.waveMaxValue = demoProvider->getSPO2MaxValue();
            info.waveMinValue = 0;
            info.baseLine = demoProvider->getSPO2BaseLine();
            info.sampleRate = demoProvider->getSPO2WaveformSample();
        }
        else if (info.waveid == WAVE_CO2)
        {
            info.waveMaxValue = demoProvider->getCO2MaxWaveform();
            info.drawSpeed = 6.25;
            info.waveMinValue = 0;
            info.baseLine = demoProvider->getCO2BaseLine();
            info.sampleRate = demoProvider->getSPO2WaveformSample();
        }
        else if (info.waveid >= WAVE_N2O && info.waveid <= WAVE_O2)
        {
            info.waveMaxValue = demoProvider->getN2OMaxWaveform();
            info.waveMinValue = 0;
            info.baseLine = demoProvider->getN2OBaseLine();
            info.sampleRate = demoProvider->getN2OWaveformSample();
        }
        else if (info.waveid >= WAVE_ART && info.waveid <= WAVE_AUXP2)
        {
            info.waveMaxValue = demoProvider->getIBPMaxWaveform();
            info.waveMinValue = 0;
            info.baseLine = demoProvider->getIBPBaseLine();
            info.sampleRate = demoProvider->getIBPWaveformSample();
        }
    }


    void loadLayoutNodes()
    {
        for (int i = WAVE_ECG_I; i < WAVE_NR; i++)
        {
            WaveformID id = static_cast<WaveformID>(i);
            waveIDMaps.insert(paramInfo.getParamWaveformName(id), id);
        }
    }

    /**
     * @brief findNode find a layout node at the modelindex
     * @param index the index of the model
     * @return the layout node if exists, otherwise return NULL
     */
    LayoutNode *findNode(const QModelIndex &index)
    {
        if (!index.isValid() || index.row() >= layoutNodes.count() || index.column() >= COLUMN_COUNT)
        {
            return NULL;
        }

        LayoutRow *r = layoutNodes[index.row()];
        LayoutRow::ConstIterator iter = r->constBegin();
        for (; iter != r->end(); ++iter)
        {
            if ((*iter)->pos == index.column())
            {
                return *iter;
            }
        }

        return NULL;
    }

    DemoProvider *demoProvider;
    QMap<WaveformID, QByteArray> waveCaches;
    QVector<LayoutRow *> layoutNodes;
    QList<QString> supportWaveforms;
    QList<QString> supportParams;
    QMap<QString, WaveformID> waveIDMaps;
};

ScreenLayoutModel::ScreenLayoutModel(QObject *parent)
    : QAbstractTableModel(parent), d_ptr(new ScreenLayoutModelPrivate)
{
    d_ptr->loadLayoutNodes();
}

int ScreenLayoutModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return COLUMN_COUNT;
}

int ScreenLayoutModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ROW_COUNT;
}

bool ScreenLayoutModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        if (value.type() == QVariant::String)
        {
            LayoutNode *node = d_ptr->findNode(index);
            if (node)
            {
                QString text = value.toString();
                // check the exists node
                if (text != trs("Off"))
                {
                    for (int i = 0; i < d_ptr->layoutNodes.count(); i++)
                    {
                        LayoutRow *r = d_ptr->layoutNodes.at(i);
                        LayoutRow::Iterator iter;
                        bool found = false;
                        int column = 0;
                        for (iter = r->begin(); iter != r->end(); ++iter)
                        {
                            if ((*iter)->name == text)
                            {
                                 (*iter)->name = trs("Off");
                                 found = true;
                                 QModelIndex changeIndex = this->index(i, column);
                                 emit dataChanged(changeIndex, changeIndex);
                                 break;
                            }
                            column += (*iter)->span;
                        }

                        if (found)
                        {
                            break;
                        }
                    }
                }
                node->name = text;
                emit dataChanged(index, index);
            }
        }

        return true;
    }

    return false;
}

QVariant ScreenLayoutModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
    case Qt::DisplayRole:
    {
        LayoutNode *node = d_ptr->findNode(index);
        if (node)
        {
            ScreenLayoutItemInfo info;
            info.waveid = node->waveId;
            info.name = node->name;
            d_ptr->fillWaveData(info);
            return qVariantFromValue(info);
        }
    }
    break;
    case Qt::EditRole:
    {
        if (index.column() >= WAVE_START_COLUMN && index.row() < MAX_WAVE_ROW)
        {
            return QVariant(d_ptr->supportWaveforms);
        }

        return QVariant(d_ptr->supportParams);
    }
    break;
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags ScreenLayoutModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QAbstractTableModel::flags(index);
    }

    if (index.row() < MAX_WAVE_ROW && index.column() < WAVE_START_COLUMN)
    {
        // the waveform area is not selectable or editable
        return Qt::ItemIsEnabled;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QSize ScreenLayoutModel::span(const QModelIndex &index) const
{
    if (index.isValid())
    {
        LayoutNode *node = d_ptr->findNode(index);
        // if the node exist, use the node's span value, otherwise, use the default value
        if (node)
        {
            return QSize(node->span, 1);
        }
    }

    return QSize();
}

void ScreenLayoutModel::saveLayoutInfo()
{
    systemConfig.setConfig("PrimaryCfg|UILayout|ScreenLayout|Normal", d_ptr->getLayoutMap());
}

void ScreenLayoutModel::loadLayoutInfo()
{
    const QVariantMap config = systemConfig.getConfig("PrimaryCfg|UILayout|ScreenLayout|Normal");
    beginResetModel();
    d_ptr->loadLayoutFromConfig(config);
    endResetModel();
}

void ScreenLayoutModel::updateWaveAndParamInfo()
{
    QStringList waveList;
    waveList << "ECG" << "RESP" << "SPO2" << "CO2" << "ART" << "PA" << trs("Off");
    d_ptr->supportWaveforms = waveList;

    QStringList paramList;
    paramList << "SPO2" << "C.O." << "CO2" << "RESP" << "NIBP" << "NIBPList" << trs("Off");
    d_ptr->supportParams = paramList;
}

ScreenLayoutModel::~ScreenLayoutModel()
{
    delete d_ptr;
}
