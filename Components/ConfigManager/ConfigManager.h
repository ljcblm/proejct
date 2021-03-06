/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/30
 **/

#pragma once

#include <QObject>
#include <QScopedPointer>
#include <QList>
#include "ConfigManagerInterface.h"

class ConfigManagerPrivate;
class ConfigManager : public QObject, public ConfigManagerInterface
{
    Q_OBJECT
public:
    struct UserDefineConfigInfo {
      QString name;         // name of the this config
      QString fileName;     // xml filename
      QString patType;         // patient type
    };

    ~ConfigManager();

    /**
     * @brief getInstance signleton
     * @return
     */
    static ConfigManager &getInstance();

    /**
     * @brief getConfig get a config object of the specific patient type
     * @param patType the paient type, adult, ped or neo
     * @return pointer to the specific patient type config object
     */
    Config *getConfig(PatientType patType);

    /**
     * @brief getCurConfig get the reference of the current running config
     * @return  reference to the current running config
     */
    Config &getCurConfig();

    /* reimplement */
    Config *getSysConfig();

    /* reimplement */
    Config *getMachineConfig();

    /**
     * @brief getUserConfigInfos get user define config info
     * @return a list of the user define config info
     */
    QList<UserDefineConfigInfo> getUserDefineConfigInfos();

    /**
     * @brief saveUserDefineConfig the user define config
     * @param configName config name
     * @param configObj pointer to the config object
     * @param type patient type
     * @return save successfully or not
     */
    bool saveUserDefineConfig(const QString & configName, Config * configObj, const PatientType &type);

    /**
     * @brief saveUserConfigInfo save the user config info to the system config
     * @param infos
     * @note the old user config info will be remove
     */
    void saveUserConfigInfo(const QList<ConfigManager::UserDefineConfigInfo> infos);

    /**
     * @brief factoryConfigFilename get the factory config file name for specific patient type
     * @param patType patient type
     * @return config file name
     */
    QString factoryConfigFilename(PatientType patType);

    /**
     * @brief runningConfigFilename get the running config file name for specific patient type
     * @param patType patient type
     * @return  config file name
     */
    QString runningConfigFilename(PatientType patType);

    /**
     * @brief isReadOnly ???????????????????????????
     * @return 1:????????????
     */
    bool isReadOnly()const;

    /**
     * @brief setWidgetIfOnlyShown ?????????????????? ???????????????
     * @param status 1, 0
     */
    void setWidgetIfOnlyShown(bool status);

    /**
     * @brief getWidgetStatus
     * @return
     */
    bool getWidgetsPreStatus() const;

    /**
     * @brief setWidgetStatus
     * @param status
     */
    void setWidgetStatus(bool status);

    /**
     * @brief getUserDefineConfigMaxLen ??????????????????????????????????????????
     * @return
     */
    int getUserDefineConfigMaxLen();

    /**
     * @brief hasExistConfig ??????????????????????????????????????????
     * @param name
     * @param type
     * @return
     */
    bool hasExistConfig(const QString &name, PatientType type);

    /**
     * @brief loadConfig ??????????????????????????????????????????????????????
     * @param type???????????????
     */
    void loadConfig(PatientType type);

    /**
     * @brief getOriginalConfig ??????????????????????????????????????????.original.xml???userDefine.xml???
     * @param type
     * @return
     */
    QString getOriginalConfig(PatientType type);

    /**
     * @brief setOriginalConfig ??????????????????????????????????????????.original.xml???userDefine.xml???
     * @param type
     * @param path
     */
    void setOriginalConfig(PatientType type, QString path);
signals:
    /**
     * @brief configChange
     * @note emit when the current running confg object switch to other config
     *       object. Usually happens after select patient type
     *
     */
    void configChanged();

    /**
     * @brief userDefineConfigChanged
     * @note 
     */
    void userDefineConfigChanged();

    /**
     * @brief configUpdated  ????????????????????????
     */
    void configUpdated(void);

private slots:
    void _onPatientTypeChange(PatientType type);

private:
    ConfigManager();
    // ????????????
    QScopedPointer<ConfigManagerPrivate> d_ptr;
};

#define configManager (ConfigManager::getInstance())
#define currentConfig (ConfigManager::getInstance().getCurConfig())
