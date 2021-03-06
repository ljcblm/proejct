/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/6/4
 **/

#pragma once
#include <QObject>
#include "SystemManagerInterface.h"

class PluginProviderPrivate;
class Provider;
class PluginProvider : public QObject
{
    Q_OBJECT
public:
    enum PluginType
    {
        PLUGIN_TYPE_INVALID = -1,    // invalid packet type
        PLUGIN_TYPE_CTRL = 0xF0,     // control packet type
        PLUGIN_TYPE_SPO2 = 0xF2,
        PLUGIN_TYPE_CO2,
        PLUGIN_TYPE_IBP
    };

    enum PacketPortBaudrate
    {
        BAUDRATE_9600,
        BAUDRATE_19200,
        BAUDRATE_28800,
        BAUDRATE_38400,
        BAUDRATE_57600,
        BAUDRATE_115200,
        BAUDRATE_230400,
        BAUDRATE_NR
    };

    struct PluginInfo
    {
        PluginInfo() : plugIn(NULL), pluginType(PLUGIN_TYPE_INVALID) {}
        PluginProvider *plugIn;
        PluginType pluginType;
    };

public:
    explicit PluginProvider(const QString &name, QObject *parent = NULL);
    virtual ~PluginProvider();

    QString getName() const;
    /**
     * @brief connectProvider connect data dispatcher to the provider
     * @param type the plugin type
     * @param provider pointer to the provider
     */
    void connectProvider(PluginType type, Provider *provider);

    /**
     * @brief sendData send data through the data dispatcher
     * @param buff ther data buffer
     * @param len the buffer length
     * @return the length of data have beed send
     */
    int sendData(PluginType type, const unsigned char *buff, int len);
    /**
     * @brief addPluginProvider add a plugin provider to the system
     * @param PluginProvider the new added PluginProvider
     */
    static void addPluginProvider(PluginProvider *pluginProvider);

    /**
     * @brief getPluginProvider get a plugin provider from the system base on the name
     * @param name the plugin provider name
     * @return the plugin provider object or null is not exist
     */
    static PluginProvider *getPluginProvider(const QString &name);

    virtual void sendVersion(void) {}

    virtual void disconnected(void) {}          // ?????????????????????????????????????????????
    virtual void reconnected(void) {}        // ?????????????????????????????????????????????

    /**
     * @brief setPacketPortBaudrate ?????????????????????
     * @param type ????????????
     * @param baud ?????????
     * @return
     */
    bool setPacketPortBaudrate(PluginType type, PluginProvider::PacketPortBaudrate baud);

    /**
     * @brief updateUartBaud ?????????????????????
     * @param baud ?????????
     */
    void updateUartBaud(unsigned int baud);

protected:
    void timerEvent(QTimerEvent *ev);

private slots:
    /**
     * @brief dataArrived ????????????
     */
    void dataArrived();

    /**
     * @brief changeBaudrate ???????????????
     */
    void changeBaudrate();

    /**
     * @brief startInitModule start the module initlization process
     */
    void startInitModule();

    /**
     * @brief onWorkModeChanged handle the work mode change event
     * @param curMode the current work mode
     */
    void onWorkModeChanged(WorkMode curMode);

private:
    PluginProviderPrivate *const d_ptr;
};
