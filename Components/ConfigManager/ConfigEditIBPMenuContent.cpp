/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/18
 **/
#include "ConfigEditIBPMenuContent.h"
#include <QGridLayout>
#include <QLabel>
#include "LanguageManager.h"
#include <QMap>
#include "ComboBox.h"
#include "ComboBox.h"
#include "ConfigManager.h"
#include "IBPSymbol.h"
#include "Button.h"
#include "ConfigEditMenuWindow.h"

class ConfigEditIBPMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_PRESSURE_NAME1 = 0,
        ITEM_CBO_PRESSURE_NAME2,
        ITEM_CBO_SWEEP_SPEED,
        ITEM_CBO_FILTER_MODE,
        ITEM_CBO_IBP_SENS,
        ITEM_CBO_MAX,
    };

    ConfigEditIBPMenuContentPrivate();
    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, ComboBox *> combos;
};

ConfigEditIBPMenuContentPrivate::ConfigEditIBPMenuContentPrivate()
{
    combos.clear();
}

ConfigEditIBPMenuContent::ConfigEditIBPMenuContent():
    MenuContent(trs("ConfigEditIBPMenu"),
                trs("ConfigEditIBPMenuDesc")),
    d_ptr(new ConfigEditIBPMenuContentPrivate)
{
}

ConfigEditIBPMenuContent::~ConfigEditIBPMenuContent()
{
    delete d_ptr;
}

void ConfigEditIBPMenuContentPrivate::loadOptions()
{
    Config *config = ConfigEditMenuWindow
                     ::getInstance()->getCurrentEditConfig();
    QStringList strList = QStringList()
                          << "ChannelPressureEntitle1"
                          << "ChannelPressureEntitle2"
                          << "IBPSweepSpeed"
                          << "FilterMode"
                          << "Sensitivity";
    for (int i = 0; i < strList.count(); i++)
    {
        int index = 0;
        MenuItem item = static_cast<MenuItem>(i);
        combos[item]->blockSignals(true);
        config->getNumValue(QString("IBP|%1").arg(strList.at(i)), index);
        combos[item]->setCurrentIndex(index);
        combos[item]->blockSignals(false);
    }
}

void ConfigEditIBPMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void ConfigEditIBPMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    ComboBox *comboBox;
    int itemID;

    // channel pressure entitle
    label = new QLabel(trs("ChannelPressureEntitle1"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(IBPSymbol::convert(IBP_PRESSURE_ART))
                       << trs(IBPSymbol::convert(IBP_PRESSURE_PA))
                       << trs(IBPSymbol::convert(IBP_PRESSURE_CVP))
                       << trs(IBPSymbol::convert(IBP_PRESSURE_LAP))
                       << trs(IBPSymbol::convert(IBP_PRESSURE_RAP))
                       << trs(IBPSymbol::convert(IBP_PRESSURE_ICP))
                       << trs(IBPSymbol::convert(IBP_PRESSURE_AUXP1))
                       << trs(IBPSymbol::convert(IBP_PRESSURE_AUXP2))
                      );
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate
                         ::ITEM_CBO_PRESSURE_NAME1, comboBox);
    itemID = ConfigEditIBPMenuContentPrivate
             ::ITEM_CBO_PRESSURE_NAME1;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // channel pressure entitle2
    label = new QLabel(trs("ChannelPressureEntitle2"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(IBPSymbol::convert(IBP_PRESSURE_ART))
                       << trs(IBPSymbol::convert(IBP_PRESSURE_PA))
                       << trs(IBPSymbol::convert(IBP_PRESSURE_CVP))
                       << trs(IBPSymbol::convert(IBP_PRESSURE_LAP))
                       << trs(IBPSymbol::convert(IBP_PRESSURE_RAP))
                       << trs(IBPSymbol::convert(IBP_PRESSURE_ICP))
                       << trs(IBPSymbol::convert(IBP_PRESSURE_AUXP1))
                       << trs(IBPSymbol::convert(IBP_PRESSURE_AUXP2))
                      );
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate
                         ::ITEM_CBO_PRESSURE_NAME2, comboBox);
    itemID = ConfigEditIBPMenuContentPrivate
             ::ITEM_CBO_PRESSURE_NAME2;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // Sweep Speed
    label = new QLabel(trs("IBPSweepSpeed"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(IBPSymbol::convert(IBP_SWEEP_SPEED_62_5))
                       << trs(IBPSymbol::convert(IBP_SWEEP_SPEED_125))
                       << trs(IBPSymbol::convert(IBP_SWEEP_SPEED_250))
                       << trs(IBPSymbol::convert(IBP_SWEEP_SPEED_500))
                      );
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate
                         ::ITEM_CBO_SWEEP_SPEED, comboBox);
    itemID = ConfigEditIBPMenuContentPrivate
             ::ITEM_CBO_SWEEP_SPEED;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // Filter Mode
    label = new QLabel(trs("FilterMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(IBPSymbol::convert(IBP_FILTER_MODE_0))
                       << trs(IBPSymbol::convert(IBP_FILTER_MODE_1))
                      );
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate
                         ::ITEM_CBO_FILTER_MODE, comboBox);
    itemID = ConfigEditIBPMenuContentPrivate
             ::ITEM_CBO_FILTER_MODE;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // Sensitivity
    label = new QLabel(trs("Sensitivity"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(IBPSymbol::convert(IBP_SENSITIVITY_HIGH))
                       << trs(IBPSymbol::convert(IBP_SENSITIVITY_MID))
                       << trs(IBPSymbol::convert(IBP_SENSITIVITY_LOW))
                      );
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate
                         ::ITEM_CBO_IBP_SENS, comboBox);
    itemID = ConfigEditIBPMenuContentPrivate
             ::ITEM_CBO_IBP_SENS;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    layout->setRowStretch(d_ptr->combos.count(), 1);
}

void ConfigEditIBPMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    int indexType = combo->property("Item").toInt();
    Config *config = ConfigEditMenuWindow
                     ::getInstance()->getCurrentEditConfig();
    QString str;
    switch (indexType)
    {
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_PRESSURE_NAME1:
        str = "ChannelPressureEntitle1";
        break;
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_PRESSURE_NAME2:
        str = "ChannelPressureEntitle2";
        break;
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED:
        str = "IBPSweepSpeed";
        break;
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_FILTER_MODE:
        str = "FilterMode";
        break;
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_IBP_SENS:
        str = "Sensitivity";
        break;
    default :
        qdebug("Invalid combo id.");
        break;
    }
    config->setNumValue(QString("IBP|%1").arg(str), index);
}
















