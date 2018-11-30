/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/22
 **/

#include "RescueDataDeleteWindow.h"
#include "RescueDataListNewWidget.h"
#include "TableView.h"
#include "Button.h"
#include <QPointer>
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "MessageBox.h"
#include <QEventLoop>
#include "Utility.h"
#include "DataStorageDirManager.h"
#include "ThemeManager.h"

#define PATH_ICON_UP "/usr/local/nPM/icons/ArrowUp.png"
#define PATH_ICON_DOWN "/usr/local/nPM/icons/ArrowDown.png"

RescueDataDeleteWindow *RescueDataDeleteWindow::_selfObj = NULL;

static long deleteSelectIncidnets(const QVariant &para)
{
    QVariantList list = para.toList();
    foreach(QVariant var, list)
    {
        int index = var.toInt();
        dataStorageDirManager.deleteData(index);
    }
    return 0;
}

static long deleteAllData(const QVariant & para)
{
    static QMutex mutex;
    Q_UNUSED(para);
    if (mutex.tryLock())
    {
        dataStorageDirManager.deleteAllData();
        mutex.unlock();
        return 1;
    }
    return 0;
}

class RescueDataDeleteWindowPrivate
{
public:
    RescueDataDeleteWindowPrivate()
        : dataListWidget(NULL),
          deleteCase(NULL),
          deleteAll(NULL),
          up(NULL),
          down(NULL),
          widgetHeight(themeManger.getAcceptableControlHeight())
    {}
    ~RescueDataDeleteWindowPrivate(){}

    RescueDataListNewWidget *dataListWidget;
    Button *deleteCase;
    Button *deleteAll;
    Button *up;
    Button *down;
    QPointer<QThread> deleteThreadPtr;
    int widgetHeight;
};

RescueDataDeleteWindow::~RescueDataDeleteWindow()
{
    delete d_ptr;
}

void RescueDataDeleteWindow::_updateWindowTitle()
{
    QString str;
    if (languageManager.getCurLanguage() == LanguageManager::English)
    {
        str = QString("%1 (page %2 of %3)")
                .arg(trs("EraseData"))
                .arg(d_ptr->dataListWidget->getCurPage() + 1)
                .arg(d_ptr->dataListWidget->getTotalPage() == 0 ?
                         1 : d_ptr->dataListWidget->getTotalPage());
    }
    else
    {
        str = QString("%1 (%2/%3)")
                .arg(trs("EraseData"))
                .arg(d_ptr->dataListWidget->getCurPage() + 1)
                .arg(d_ptr->dataListWidget->getTotalPage() == 0 ?
                         1 : d_ptr->dataListWidget->getTotalPage());
    }
    setWindowTitle(str);
}

void RescueDataDeleteWindow::_deleteSelectReleased()
{
    QStringList list;
    d_ptr->dataListWidget->getStrList(list);
    if (list.empty())
    {
        MessageBox msgbox(trs("Prompt"), trs("NoIncidents"), QStringList(trs("EnglishYESChineseSURE")));
        msgbox.exec();
        return;
    }

    QStringList checkList;
    d_ptr->dataListWidget->getCheckList(checkList);
    if (checkList.isEmpty())
    {
        MessageBox msgbox(trs("Prompt"), trs("SelectIncidents"), QStringList(trs("EnglishYESChineseSURE")));
        msgbox.exec();
        return;
    }

    MessageBox messageBox(trs("Prompt"), trs("EraseSelectedRescueData"));
    if (messageBox.exec() == 0)
    {
        return;
    }

    if (d_ptr->deleteThreadPtr)
    {
        // exist delete thread, do nothing
        MessageBox msgbox(trs("Warn"), trs("WarningSystemBusyTryLater"));
        msgbox.exec();
        return;
    }

    // find delete index
    int count = checkList.count();
    int totalCount = list.count();
    QVariantList indexList;
    for (int i = 0; i < count; ++i)
    {
        int index = list.indexOf(checkList.at(i));
        if (-1 != index)
        {
            indexList.append(totalCount - index - 1);
        }
    }

    d_ptr->deleteThreadPtr = new Util::WorkerThread(deleteSelectIncidnets, indexList);
    QEventLoop eventLoop;
    connect(d_ptr->deleteThreadPtr.data(), SIGNAL(finished()), &eventLoop, SLOT(quit()));
    d_ptr->deleteThreadPtr->start();
    eventLoop.exec();

    d_ptr->dataListWidget->reload();
    checkList.clear();
    list.clear();
}

void RescueDataDeleteWindow::_deleteAllReleased()
{
    QStringList list;
    d_ptr->dataListWidget->getStrList(list);
    if (list.empty())
    {
        MessageBox msgbox(trs("Prompt"), trs("NoIncidents"), QStringList(trs("EnglishYESChineseSURE")));
        msgbox.exec();
        return;
    }

    MessageBox messageBox(trs("Prompt"), trs("ClearAllRescueData"));
    if (0 == messageBox.exec())
    {
        return;
    }

    if (d_ptr->deleteThreadPtr)
    {
        // exist delete thread, do nothing
        MessageBox msgbox(trs("Warn"), trs("WarningSystemBusyTryLater"));
        msgbox.exec();
        return;
    }

    d_ptr->deleteThreadPtr = new Util::WorkerThread(deleteAllData, QVariant());
    QEventLoop eventLoop;
    connect(d_ptr->deleteThreadPtr.data(), SIGNAL(finished()), &eventLoop, SLOT(quit()));
    d_ptr->deleteThreadPtr->start();
    eventLoop.exec();

    d_ptr->dataListWidget->reload();
}

void RescueDataDeleteWindow::_upReleased()
{
    if (NULL != d_ptr->dataListWidget)
    {
        d_ptr->dataListWidget->pageChange(true);
    }
}

void RescueDataDeleteWindow::_downReleased()
{
    if (NULL != d_ptr->dataListWidget)
    {
        d_ptr->dataListWidget->pageChange(false);
    }
}

void RescueDataDeleteWindow::_updatePageBtnStatus()
{
    int dataListCurPage = d_ptr->dataListWidget->getCurPage();
    int dataListTotalPage = d_ptr->dataListWidget->getTotalPage();

    // previous page button
    if (dataListCurPage <= 0)
    {
        d_ptr->up->setEnabled(false);
        d_ptr->up->setIcon(QIcon(""));
    }
    else
    {
        d_ptr->up->setEnabled(true);
        d_ptr->up->setIcon(QIcon(PATH_ICON_UP));
    }
    // next page button
    if (dataListCurPage >= dataListTotalPage - 1)
    {
        d_ptr->down->setEnabled(false);
        d_ptr->down->setIcon(QIcon(""));
    }
    else
    {
        d_ptr->down->setEnabled(true);
        d_ptr->down->setIcon(QIcon(PATH_ICON_DOWN));
    }
}

void RescueDataDeleteWindow::_updateEraseBtnStatus()
{
    QStringList checkList , strList;
    d_ptr->dataListWidget->getCheckList(checkList);
    d_ptr->dataListWidget->getStrList(strList);
    int checkListCount = checkList.count();
    int strListCount = strList.count();
    // erase select button
    if (checkListCount > 0)
    {
        d_ptr->deleteCase->setEnabled(true);
    }
    else
    {
        d_ptr->deleteCase->setEnabled(false);
    }

    // erase all button
    if (strListCount > 0)
    {
        d_ptr->deleteAll->setEnabled(true);
    }
    else
    {
        d_ptr->deleteAll->setEnabled(false);
    }
}

RescueDataDeleteWindow::RescueDataDeleteWindow()
    :Window(),
      d_ptr(new RescueDataDeleteWindowPrivate())
{
    QVBoxLayout *contentLayout = new QVBoxLayout();
    int maxw = windowManager.getPopMenuWidth();
    int maxh = windowManager.getPopMenuHeight();

    d_ptr->dataListWidget = new RescueDataListNewWidget(maxw - 20,
                                                        maxh - d_ptr->widgetHeight * 2 - 36);
    d_ptr->dataListWidget->setShowCurRescue(false);
    connect(d_ptr->dataListWidget , SIGNAL(pageInfoChange()),
            this, SLOT(_updateWindowTitle()));
    connect(d_ptr->dataListWidget , SIGNAL(pageInfoChange()),
            this, SLOT(_updatePageBtnStatus()));
    connect(d_ptr->dataListWidget, SIGNAL(btnRelease()),
            this, SLOT(_updateEraseBtnStatus()));

    d_ptr->deleteCase = new Button(trs("EraseSelect"));
    d_ptr->deleteCase->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->deleteCase, SIGNAL(released()), this, SLOT(_deleteSelectReleased()));

    d_ptr->deleteAll = new Button(trs("EraseAll"));
    d_ptr->deleteAll->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->deleteAll, SIGNAL(released()), this, SLOT(_deleteAllReleased()));

    d_ptr->up = new Button();
    d_ptr->up->setButtonStyle(Button::ButtonIconOnly);
    d_ptr->up->setIcon(QIcon(PATH_ICON_UP));
    connect(d_ptr->up, SIGNAL(released()), this, SLOT(_upReleased()));

    d_ptr->down = new Button();

    d_ptr->down->setButtonStyle(Button::ButtonIconOnly);
    d_ptr->down->setIcon(QIcon(PATH_ICON_DOWN));
    connect(d_ptr->down, SIGNAL(released()), this, SLOT(_downReleased()));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(2);
    layout->addStretch(4);
    layout->addWidget(d_ptr->deleteCase, 2);
    layout->addWidget(d_ptr->deleteAll, 2);
    layout->addStretch(2);
    layout->addWidget(d_ptr->up, 1);
    layout->addWidget(d_ptr->down, 1);

    contentLayout->setSpacing(1);
    contentLayout->addWidget(d_ptr->dataListWidget);
    contentLayout->addLayout(layout);

    setFixedSize(maxw, maxh);
    setWindowLayout(contentLayout);
}
