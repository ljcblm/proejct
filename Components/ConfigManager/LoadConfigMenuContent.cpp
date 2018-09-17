/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018.07.13
 **/

#include "LoadConfigMenuContent.h"
#include "Button.h"
#include <QLabel>
#include <QMap>
#include "LanguageManager.h"
#include "ConfigManager.h"
#include <QList>
#include "IConfig.h"
#include <QFile>
#include "PatientManager.h"
#include "MessageBox.h"
#include "MenuManager.h"
#include <QHBoxLayout>
#include "ListView.h"
#include "ListDataModel.h"
#include "ListViewItemDelegate.h"
#include "ConfigEditMenuWindow.h"
#include "WindowManager.h"

#define CONFIG_DIR "/usr/local/nPM/etc"
#define USER_DEFINE_CONFIG_NAME "UserConfig"

#define CONFIG_MAX_NUM 3
#define LISTVIEW_MAX_VISIABLE_TIME 6
class LoadConfigMenuContentPrivate
{
public:
    LoadConfigMenuContentPrivate();

    void loadConfigs();
    void updateConfigList();

    QListWidgetItem *lastSelectItem;
    QList<ConfigManager::UserDefineConfigInfo> configs;
    Config *curConfig;
    int curEditIndex;
    Button *loadBtn;
    Button *ViewBtn;
    ListView *configListView;
    ListDataModel *configDataModel;
};

LoadConfigMenuContentPrivate::LoadConfigMenuContentPrivate():
    lastSelectItem(NULL),
    curConfig(NULL),
    curEditIndex(0),
    loadBtn(NULL),
    ViewBtn(NULL),
    configListView(NULL),
    configDataModel(NULL)
{
    configs.clear();
}

void LoadConfigMenuContentPrivate::loadConfigs()
{
    configs.clear();
    ConfigManager::UserDefineConfigInfo userConfig[] =
    {
        {"AdultConfig.Original", "AdultConfig.Original.xml"},
        {"PedConfig.Original", "PedConfig.Original.xml"},
        {"NeoConfig.Original", "NeoConfig.Original.xml"}
    };
    for (int i = 0; i < 3; i++)
    {
        configs.append(userConfig[i]);
    }
    configs = configs + configManager.getUserDefineConfigInfos();
}
/**
 * @brief LoadConfigMenuContentPrivate::updateConfigList
 *        update the config list widget
 */
void LoadConfigMenuContentPrivate::updateConfigList()
{
    QStringList configNameList;
    for (int i = 0; i < configs.count(); i++)
    {
        configNameList.append(configs.at(i).name);
    }
    configDataModel->setStringList(configNameList);

    int curSelectedRow = configListView->curCheckedRow();
    bool isEnable;
    if (curSelectedRow == -1)
    {
        isEnable = false;
    }
    else
    {
        isEnable = true;
    }
    loadBtn->setEnabled(isEnable);
    ViewBtn->setEnabled(isEnable);
}

LoadConfigMenuContent::LoadConfigMenuContent():
    MenuContent(trs("LoadConfigMenuContent"),
                trs("LoadConfigMenuContentDesc")),
    d_ptr(new LoadConfigMenuContentPrivate)
{
    connect(this, SIGNAL(configUpdated()), &configManager, SIGNAL(configUpdated()));
}

LoadConfigMenuContent::~LoadConfigMenuContent()
{
    delete d_ptr;
}

void LoadConfigMenuContent::readyShow()
{
    d_ptr->loadConfigs();
    d_ptr->updateConfigList();
}

void LoadConfigMenuContent::layoutExec()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(10);
    layout->setAlignment(Qt::AlignTop);

    // load config
    QLabel *label = new QLabel(trs("LoadConfig"));
    layout->addWidget(label);

    ListView *listView = new ListView();
    listView->setItemDelegate(new ListViewItemDelegate(listView));
    layout->addWidget(listView);
    ListDataModel *model = new ListDataModel(this);
    d_ptr->configDataModel = model;
    listView->setFixedHeight(LISTVIEW_MAX_VISIABLE_TIME * model->getRowHeightHint()
                             + listView->spacing() * (LISTVIEW_MAX_VISIABLE_TIME * 2));
    listView->setModel(model);
    listView->setRowsSelectMode();
    connect(listView, SIGNAL(enterSignal()), this, SLOT(updateBtnStatus()));
    d_ptr->configListView = listView;

    Button *button;
    QHBoxLayout *hl = new QHBoxLayout;

    // load buton
    button = new Button();
    button->setText(trs("Load"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onBtnClick()));
    hl->addWidget(button);
    d_ptr->loadBtn = button;

    // view buton
    button = new Button();
    button->setText(trs("View"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onBtnClick()));
    hl->addWidget(button);
    d_ptr->ViewBtn = button;

    layout->addLayout(hl);
    layout->addStretch();
}


void LoadConfigMenuContent::onBtnClick()
{
    Button *btn = qobject_cast<Button *>(sender());

    if (!btn)
    {
        return;
    }

    // load the config
    if (btn == d_ptr->loadBtn)
    {
        // add new config setting
        int index = d_ptr->configListView->curCheckedRow();
        d_ptr->curEditIndex = index;
        QFile::remove(systemConfig.getCurConfigName());
        QFile::copy(QString("%1/%2").arg(CONFIG_DIR).arg(d_ptr->configs.at(index).fileName),
                    systemConfig.getCurConfigName());

        // update patient type
        int patitentTypeInt = 255;
        QString fileNameStr(d_ptr->configs.at(index).fileName);
        if (fileNameStr.indexOf("Adult") >= 0)
        {
            patitentTypeInt = 0;
        }
        else if (fileNameStr.indexOf("Ped") >= 0)
        {
            patitentTypeInt = 1;
        }
        else if (fileNameStr.indexOf("Neo") >= 0)
        {
            patitentTypeInt = 2;
        }
        else
        {
            patitentTypeInt = 0;
        }
        systemConfig.setNumValue("General|PatientType", patitentTypeInt);
        systemConfig.updateCurConfigName();
        patientManager.setType((PatientType)patitentTypeInt);
        currentConfig.reload();
        // send update signal
        emit configUpdated();

        QString title(trs("LoadConfig"));
        QString text(trs("SuccessToLoad"));
        MessageBox message(title, text, false);
        message.exec();
    }
    // view config
    else if (btn == d_ptr->ViewBtn)
    {
        int index = d_ptr->configListView->curCheckedRow();
        if (index != -1)
        {
            QString fileName = d_ptr->configs.at(index).fileName;
            QString name = d_ptr->configs.at(index).name;
            d_ptr->curEditIndex = index;
            d_ptr->curConfig = new Config(QString("%1/%2")
                                          .arg(CONFIG_DIR)
                                          .arg(fileName));
            ConfigEditMenuWindow *cmWnd = new ConfigEditMenuWindow;
            cmWnd->setCurrentEditConfigName(name);
            cmWnd->setCurrentEditConfig(d_ptr->curConfig);
            cmWnd->initializeSubMenu();

            QString pathName;
            if (fileName.indexOf("User") >= 0)
            {
                pathName = "View-";
                pathName += name;
            }
            else
            {
                pathName = "View-DefaultSetting";
            }
            cmWnd->setMenuPath(pathName);

            configManager.setWidgetStatus(true);
            windowManager.showWindow(cmWnd, WindowManager::ShowBehaviorModal);
            configManager.setWidgetStatus(false);
            if (cmWnd)
            {
                delete cmWnd;
            }
        }
    }
    else
    {
        qdebug("Unknown singal sender!");
    }
}

void LoadConfigMenuContent::updateBtnStatus()
{
    int row = d_ptr->configListView->curCheckedRow();
    bool isEnabled;
    if (row == -1)
    {
        isEnabled = false;
    }
    else
    {
        isEnabled = true;
    }
    d_ptr->loadBtn->setEnabled(isEnabled);
    d_ptr->ViewBtn->setEnabled(isEnabled);
}
