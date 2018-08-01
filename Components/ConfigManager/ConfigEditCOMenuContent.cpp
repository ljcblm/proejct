/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/16
 **/

#include "ConfigEditCOMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include "Button.h"
#include <QGridLayout>
#include <QList>
#include "COSymbol.h"
#include "SoundManager.h"
#include "SystemManager.h"
#include "COParam.h"
#include "IConfig.h"
#include "NumberInput.h"
#include "IMessageBox.h"
#include "ConfigEditMenuWindow.h"

class ConfigEditCOMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_CO_RATIO = 0,
        ITEM_CBO_INJECT_TEMP_SOURCE,
        ITEM_CBO_INJECTION_TEMP,
        ITEM_CBO_INJECTION_VOLUMN,
        ITEM_CBO_MEASURE_CONTROL,
    };

    ConfigEditCOMenuContentPrivate() : measureSta(CO_INST_START) {}

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, Button *> buttons;
    COInstCtl measureSta;              // 测量状态
};

void ConfigEditCOMenuContentPrivate::loadOptions()
{
    buttons[ITEM_CBO_CO_RATIO]->blockSignals(true);
    buttons[ITEM_CBO_INJECTION_TEMP]->blockSignals(true);
    buttons[ITEM_CBO_INJECTION_VOLUMN]->blockSignals(true);
    buttons[ITEM_CBO_MEASURE_CONTROL]->blockSignals(true);
    combos[ITEM_CBO_INJECT_TEMP_SOURCE]->blockSignals(true);

    Config *config = ConfigEditMenuWindow::getInstance()
                     ->getCurrentEditConfig();
    int number = 0;
    config->getNumValue("CO|Ratio", number);
    buttons[ITEM_CBO_CO_RATIO]->setText(QString::number(static_cast<double>(number) / 1000));
    number = 0;
    config->getNumValue("CO|InjectionTempSource", number);
    combos[ITEM_CBO_INJECT_TEMP_SOURCE]->setCurrentIndex(number);
    number = 0;
    if (combos[ITEM_CBO_INJECT_TEMP_SOURCE]->currentIndex() == CO_TI_MODE_AUTO)
    {
        buttons[ITEM_CBO_INJECTION_TEMP]->setEnabled(false);
    }
    else
    {
        buttons[ITEM_CBO_INJECTION_TEMP]->setEnabled(true);
    }
    config->getNumValue("CO|InjectionTemp", number);
    buttons[ITEM_CBO_INJECTION_TEMP]->setText(QString::number(static_cast<double>(number) / 10));
    number = 0;
    config->getNumValue("CO|InjectionVolumn", number);
    buttons[ITEM_CBO_INJECTION_VOLUMN]->setText(QString::number(number));
    number = 0;
    config->getNumValue("CO|MeasureMode", number);
    buttons[ITEM_CBO_MEASURE_CONTROL]->setText(trs(COSymbol::convert((COInstCtl)number)));

    buttons[ITEM_CBO_CO_RATIO]->blockSignals(false);
    buttons[ITEM_CBO_INJECTION_TEMP]->blockSignals(false);
    buttons[ITEM_CBO_INJECTION_VOLUMN]->blockSignals(false);
    buttons[ITEM_CBO_MEASURE_CONTROL]->blockSignals(false);
    combos[ITEM_CBO_INJECT_TEMP_SOURCE]->blockSignals(false);
}

ConfigEditCOMenuContent::ConfigEditCOMenuContent()
    : MenuContent(trs("COMenu"), trs("COMenuDesc")),
      d_ptr(new ConfigEditCOMenuContentPrivate)
{
}

ConfigEditCOMenuContent::~ConfigEditCOMenuContent()
{
    delete d_ptr;
}

void ConfigEditCOMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void ConfigEditCOMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    Button *button;
    QLabel *label;
    int itemID;

    // Ratio
    label = new QLabel(trs("CORatio"));
    layout->addWidget(label, d_ptr->buttons.count() + d_ptr->combos.count(), 0);
    button = new Button("0.542");
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ConfigEditCOMenuContentPrivate::ITEM_CBO_CO_RATIO);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count() + d_ptr->combos.count(), 1);
    d_ptr->buttons.insert(ConfigEditCOMenuContentPrivate::ITEM_CBO_CO_RATIO, button);

    // temp source
    label = new QLabel(trs("InjectionTempSource"));
    layout->addWidget(label, d_ptr->buttons.count() + d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(COSymbol::convert(CO_TI_MODE_AUTO))
                       << trs(COSymbol::convert(CO_TI_MODE_MANUAL))
                      );
    itemID = static_cast<int>(ConfigEditCOMenuContentPrivate::ITEM_CBO_INJECT_TEMP_SOURCE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->buttons.count() + d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditCOMenuContentPrivate::ITEM_CBO_INJECT_TEMP_SOURCE, comboBox);

    // injection temp
    label = new QLabel(trs("InjectionTemp"));
    layout->addWidget(label, d_ptr->buttons.count() + d_ptr->combos.count(), 0);
    button = new Button("20");
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ConfigEditCOMenuContentPrivate::ITEM_CBO_INJECTION_TEMP);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count() + d_ptr->combos.count(), 1);
    d_ptr->buttons.insert(ConfigEditCOMenuContentPrivate::ITEM_CBO_INJECTION_TEMP, button);

    // injection volumn
    label = new QLabel(trs("InjectionVolumn"));
    layout->addWidget(label, d_ptr->buttons.count() + d_ptr->combos.count(), 0);
    button = new Button("10");
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ConfigEditCOMenuContentPrivate::ITEM_CBO_INJECTION_VOLUMN);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count() + d_ptr->combos.count(), 1);
    d_ptr->buttons.insert(ConfigEditCOMenuContentPrivate::ITEM_CBO_INJECTION_VOLUMN, button);

    // measure contrl
    label = new QLabel(trs("MeasureControl"));
    layout->addWidget(label, d_ptr->buttons.count() + d_ptr->combos.count(), 0);
    button = new Button(trs("COStart"));
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ConfigEditCOMenuContentPrivate::ITEM_CBO_MEASURE_CONTROL);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count() + d_ptr->combos.count(), 1);
    d_ptr->buttons.insert(ConfigEditCOMenuContentPrivate::ITEM_CBO_MEASURE_CONTROL, button);

    layout->setRowStretch(d_ptr->combos.count() + d_ptr->buttons.count(), 1);
}

void ConfigEditCOMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        Config *config = ConfigEditMenuWindow::getInstance()->getCurrentEditConfig();

        ConfigEditCOMenuContentPrivate::MenuItem item
            = (ConfigEditCOMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case ConfigEditCOMenuContentPrivate::ITEM_CBO_INJECT_TEMP_SOURCE:
        {
            if (index == static_cast<int>(CO_TI_MODE_MANUAL))
            {
                d_ptr->buttons.value(ConfigEditCOMenuContentPrivate::ITEM_CBO_INJECTION_TEMP)->setEnabled(true);
            }
            else if (index == static_cast<int>(CO_TI_MODE_AUTO))
            {
                d_ptr->buttons.value(ConfigEditCOMenuContentPrivate::ITEM_CBO_INJECTION_TEMP)->setEnabled(false);
            }
            int temp = d_ptr->buttons[ConfigEditCOMenuContentPrivate::ITEM_CBO_INJECTION_TEMP]->text().toFloat() * 10;
            config->setNumValue("CO|InjectionTemp", (unsigned)temp);
            config->setNumValue("CO|InjectionTempSource", index);
            break;
        }
        default:
            break;
        }
    }
}

void ConfigEditCOMenuContent::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (button)
    {
        Config *config = ConfigEditMenuWindow::getInstance()->getCurrentEditConfig();

        ConfigEditCOMenuContentPrivate::MenuItem item
            = (ConfigEditCOMenuContentPrivate::MenuItem) button->property("Item").toInt();
        switch (item)
        {
        case ConfigEditCOMenuContentPrivate::ITEM_CBO_CO_RATIO:
        {

            NumberInput numberPad;
            numberPad.setTitleBarText(trs("CORatio"));
            numberPad.setMaxInputLength(5);
            numberPad.setInitString(button->text());
            if (numberPad.exec())
            {
                QString text = numberPad.getStrValue();
                bool ok = false;
                float value = text.toFloat(&ok);
                int16_t actualValue = value * 1000;
                if (ok)
                {
                    if (actualValue >= 1 && actualValue <= 999)
                    {
                        button->setText(text);
                        config->setNumValue("CO|Ratio", static_cast<int>(actualValue));
                    }
                    else
                    {
                        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0.001-0.999", QStringList(trs("EnglishYESChineseSURE")));
                        messageBox.exec();
                    }
                }
            }
            break;
        }
        case ConfigEditCOMenuContentPrivate::ITEM_CBO_INJECTION_TEMP:
        {
            NumberInput numberPad;
            numberPad.setTitleBarText(trs("InjectionTemp"));
            numberPad.setMaxInputLength(4);
            numberPad.setInitString(button->text());
            if (numberPad.exec())
            {
                QString text = numberPad.getStrValue();
                bool ok = false;
                float value = text.toFloat(&ok);
                int16_t actualValue = value * 10;
                if (ok)
                {
                    if (actualValue <= 270)
                    {
                        button->setText(text);
                        config->setNumValue("CO|InjectionTempSource", static_cast<int>(CO_TI_MODE_MANUAL));
                        config->setNumValue("CO|InjectionTemp", static_cast<int>(actualValue));
                    }
                    else
                    {
                        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0.0-27.0", QStringList(trs("EnglishYESChineseSURE")));
                        messageBox.exec();
                    }
                }
            }
            break;
        }
        case ConfigEditCOMenuContentPrivate::ITEM_CBO_INJECTION_VOLUMN:
        {
            NumberInput numberPad;
            numberPad.setTitleBarText(trs("InjectionVolumn"));
            numberPad.setMaxInputLength(3);
            numberPad.setInitString(button->text());
            if (numberPad.exec())
            {
                QString text = numberPad.getStrValue();
                bool ok = false;
                unsigned int value = text.toInt(&ok);
                if (ok)
                {
                    if (value >= 1 && value <= 200)
                    {
                        button->setText(text);
                        config->setNumValue("CO|InjectionVolumn", static_cast<int>(value));
                    }
                    else
                    {
                        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "1-200", QStringList(trs("EnglishYESChineseSURE")));
                        messageBox.exec();
                    }
                }
            }
            break;
        }
        case ConfigEditCOMenuContentPrivate::ITEM_CBO_MEASURE_CONTROL:
        {
            config->setNumValue("CO|MeasureMode", static_cast<int>(d_ptr->measureSta));
            if (d_ptr->measureSta == CO_INST_START)
            {
                button->setText(trs("COEnd"));
                d_ptr->measureSta = CO_INST_END;
            }
            else if (d_ptr->measureSta == CO_INST_END)
            {
                button->setText(trs("COStart"));
                d_ptr->measureSta = CO_INST_START;
            }
            break;
        }
        default:
            break;
        }
    }
}