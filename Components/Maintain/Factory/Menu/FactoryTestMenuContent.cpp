/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/25
 **/
#include <QGridLayout>
#include <QLabel>
#include "LanguageManager.h"
#include "ComboBox.h"
#include "FactoryTestMenuContent.h"
#include "Debug.h"
#include "Button.h"
#include <QProcess>
#include "MessageBox.h"
#include "USBManager.h"
#include "FactoryMaintainManager.h"
#include "SpinBox.h"

class FactoryTestMenuContentPrivate
{
public:
    FactoryTestMenuContentPrivate();

#ifdef TEST_REFRESH_RATE_CONTENT
    SpinBox *freshRateSpinBox;
#endif

    static QString btnStr[FACTORY_CONTENT_TEST_NR];

    Button *lbtn[FACTORY_CONTENT_TEST_NR];
};

FactoryTestMenuContentPrivate::FactoryTestMenuContentPrivate()
{
#ifdef TEST_REFRESH_RATE_CONTENT
    freshRateSpinBox = NULL;
#endif
    for (int i = 0; i < FACTORY_CONTENT_TEST_NR; i++)
    {
        lbtn[i] = NULL;
    }
}

#ifdef OUTPUT_TESTPACKET_THROUGH_USB_CONTENT
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <IMessageBox.h>
#include "IThread.h"


static void usbOutPutTestPacket(bool isFullSpeed)
{
#define USB_PORT_CONTROL_ADDRESS 0x2184384
#define USB_CONTROL_TEST_PACKET_DATA 0x18441205
#define USB_CONTROL_FORCE_FS_MODE 0x18461205
#define USB_CONTROL_STOP_TEST 0x18401205

    if (!usbManager.isUSBExist())
    {
        IMessageBox("Error", "Connect U disk first.", false).exec();
        return;
    }
    int64_t *pageAddr = NULL;
    int64_t *v_addr = NULL;
    off_t pageOffset = (off_t) USB_PORT_CONTROL_ADDRESS % getpagesize();
    off_t pageAddress = (off_t)(USB_PORT_CONTROL_ADDRESS - pageOffset);
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0)
    {
        IMessageBox("Error", "Open /dev/mem failed!", false).exec();
        return;
    }

    pageAddr = static_cast<int64_t *> mmap(NULL, getpagesize(),
                                           PROT_WRITE, MAP_SHARED, mem_fd, pageAddress);
    if (pageAddr == static_cast<int64_t *>(-1))
    {
        IMessageBox("Error", "memory remap failed!", false).exec();
        return;
    }

    v_addr = static_cast<int64_t *>((int64_t)pageAddr + pageOffset);

    if (isFullSpeed)
    {
        *v_addr = USB_CONTROL_FORCE_FS_MODE;
        IThread::msleep(100);
    }
    *v_addr = USB_CONTROL_TEST_PACKET_DATA;

    IMessageBox("Success", "The usb port should output continual test packets.\nClose this window to stop test.",
                false).exec();

    *v_addr = USB_CONTROL_STOP_TEST;
    munmap(pageAddr, getpagesize());
    close(mem_fd);
}
#endif


QString FactoryTestMenuContentPrivate::btnStr[FACTORY_CONTENT_TEST_NR] =
{
    "FactoryLight",
    "FactorySound",
    "FactoryKey",
    "FactoryPrinter",
#ifdef OUTPUT_TESTPACKET_THROUGH_USB_CONTENT
    "USB High Speed Test packet",
    "USB Full Speed Test Packet",
#endif
    "FactoryWifi",
    "FactoryEnthernet",
    "FactoryBattery"
};

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
FactoryTestMenuContent::FactoryTestMenuContent()
    : MenuContent(trs("FactoryTest"),
                  trs("FactoryTestDesc")),
      d_ptr(new FactoryTestMenuContentPrivate)
{
}

void FactoryTestMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    Button *button;
    int row = 0;

    for (int i = 0; i < FACTORY_CONTENT_TEST_NR; i++)
    {
        label = new QLabel(trs(d_ptr->btnStr[i]));
        layout->addWidget(label, i, 0);
        button = new Button(trs("FactoryTest"));
        button->blockSignals(true);
        button->setButtonStyle(Button::ButtonTextOnly);
        layout->addWidget(button, i, 1);
        button->setProperty("Item", qVariantFromValue(i));
        connect(button, SIGNAL(released()), this, SLOT(onBtnReleased(int)));
        button->blockSignals(false);
        d_ptr->lbtn[i] = button;
    }

#ifdef TEST_REFRESH_RATE_CONTENT
    label = new QLabel(trs("RefreshRate"));
    layout->addWidget(label, FACTORY_CONTENT_TEST_NR, 0);
    SpinBox *spin = new SpinBox();
    spin->blockSignals(true);
    spin->setRange(50, 70);
    spin->setStep(1);
    layout->addWidget(spin, FACTORY_CONTENT_TEST_NR, 1);
    row++;
    connect(spin, SIGNAL(valueChange(int, int)), this, SLOT(onFreshRateChanged(int)));

    int curRate = 60;
    QProcess process;
    process.start("fw_printenv");

    if (process.waitForFinished(2000))
    {
        QString output = process.readAll();
        int index = output.indexOf("800x480M@");
        if (index >= 0)
        {
            index += 9;
            QString rateStr = output.mid(index, 2);
            bool ok = false;
            int rate;
            rate = rateStr.toInt(&ok);
            if (ok)
            {
                curRate = rate;
            }
        }
    }
    spin->setValue(curRate);
    spin->blockSignals(false);
    d_ptr->freshRateSpinBox = spin;

#endif

    layout->setRowStretch(row + FACTORY_CONTENT_TEST_NR, 1);
}
/**************************************************************************************************
 * 按钮槽函数。
 *************************************************************************************************/
void FactoryTestMenuContent::onBtnReleased(int id)
{
    switch (id)
    {
#ifdef OUTPUT_TESTPACKET_THROUGH_USB_CONTENT
    case FACTORY_CONTENT_TEST_USB_HS:
        usbOutPutTestPacket(false);
        break;
    case FACTORY_CONTENT_TEST_USB_FS:
        usbOutPutTestPacket(true);
        break;
#endif
    default:
        break;
    }
}

#ifdef TEST_REFRESH_RATE_CONTENT
void FactoryTestMenuContent::onFreshRateChanged(int valStr)
{
    QString cmdStr = QString("fw_setenv vidargs video=mxcfb0:dev=ldb,800x480M@%1,if=RGB666 video=mxcfb1:off fbmem=8M").arg(
                         valStr);
    QProcess::execute(cmdStr);
    QProcess::execute("sync");
    MessageBox msgBox(trs("Note"), trs("RefreshRateUpdatedRebootNow"), true);
    if (msgBox.exec() == 1)
    {
        QProcess::execute("reboot");
    }
}
#endif

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
FactoryTestMenuContent::~FactoryTestMenuContent()
{
    delete d_ptr;
}

void FactoryTestMenuContent::readyShow()
{
    d_ptr->lbtn[0]->setFocus();
}



