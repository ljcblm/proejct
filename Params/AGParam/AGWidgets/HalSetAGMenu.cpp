/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/27
 **/


#include "HalSetAGMenu.h"
#include "CO2Symbol.h"
#include "ConfigManager.h"
#include "FontManager.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include <QGridLayout>

enum HalSetType
{
    OPERATION_MODE = 0,
    ANESTHETIC,
    Hal_SET_MAX
};

class HalSetAGMenuPrivate
{
public:
    ComboBox *combos[Hal_SET_MAX];
    QLabel *labels[Hal_SET_MAX];
};

HalSetAGMenu::~HalSetAGMenu()
{
}

HalSetAGMenu::HalSetAGMenu(): Dialog(), d_ptr(new HalSetAGMenuPrivate())
{
    setWindowTitle(trs("HalOfAGSetUp"));

    QStringList typeName;
    typeName.append("OperationMode");
    typeName.append("Anesthetic");

    QStringList comboList[Hal_SET_MAX];
    for (int i = 0; i < OPERATION_MODE_NR; i++)
    {
        comboList[OPERATION_MODE].append(CO2Symbol::convert(CO2OperationMode(i)));
    }
    comboList[ANESTHETIC].append("Null");

    int typeIndex[Hal_SET_MAX] = {0};
    for (int i = 0; i < Hal_SET_MAX; i++)
    {
        int index = 0;
        currentConfig.getNumValue(QString("AG|%1").arg(typeName.at(i)), index);
        typeIndex[i] = index;
    }
    QGridLayout *gl = new QGridLayout();
    gl->setContentsMargins(10, 10, 10, 10);
    for (int i = 0; i < Hal_SET_MAX; i++)
    {
        d_ptr->labels[i] = new QLabel(trs(typeName.at(i)));
        d_ptr->labels[i]->setFont(fontManager.textFont(fontManager.getFontSize(2)));
        d_ptr->combos[i] = new ComboBox();
        d_ptr->combos[i]->addItems(comboList[i]);
        d_ptr->combos[i]->setCurrentIndex(typeIndex[i]);
        d_ptr->combos[i]->setProperty("comboId", qVariantFromValue(i));
        d_ptr->combos[i]->setFont(fontManager.textFont(fontManager.getFontSize(2)));
        gl->addWidget(d_ptr->labels[i], i + 1, 0);
        gl->addWidget(d_ptr->combos[i], i + 1, 1);
        connect(d_ptr->combos[i], SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged(int)));
    }
    setWindowLayout(gl);
}

void HalSetAGMenu::onComboIndexChanged(int index)
{
    ComboBox *combos = qobject_cast<ComboBox *>(sender());
    int typeIndex = combos->property("comboId").toInt();
    QString typeName("");
    switch ((HalSetType)typeIndex)
    {
    case OPERATION_MODE:
        typeName = "OperationMode";
        break;
    case ANESTHETIC:
        typeName = "Anesthetic";
        break;
    case Hal_SET_MAX:
        typeName = "";
        break;
    }
    if (!typeName.isEmpty())
    {
        currentConfig.setNumValue(QString("AG|%1").arg(typeName), index);
    }
}


