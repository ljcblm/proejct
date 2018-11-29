/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/9
 **/

#include "ErrorLogWindow.h"
#include "LanguageManager.h"
#include "TableView.h"
#include "Button.h"
#include <QLabel>
#include "ErrorLogTableModel.h"
#include "TableHeaderView.h"
#include "TableViewItemDelegate.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "ThemeManager.h"
#include "USBManager.h"
#include <QTimer>
#include "ErrorLogItem.h"
#include "ErrorLog.h"
#include "ErrorLogViewerWindow.h"
#include "MessageBox.h"
#include "ExportDataWidget.h"
#include "IConfig.h"

#define TABLE_ROW_NR        7

#define HEIGHT_HINT (themeManger.getAcceptableControlHeight())

class ErrorLogWindowPrivate
{
public:
    ErrorLogWindowPrivate()
        : table(NULL), model(NULL), summaryBtn(NULL),
          exportBtn(NULL), eraseBtn(NULL), upPageBtn(NULL),
          downPageBtn(NULL), infoLab(NULL), usbCheckTimer(NULL)
    {}

public:
    TableView *table;
    ErrorLogTableModel *model;
    Button *summaryBtn;
    Button *exportBtn;
    Button *eraseBtn;
    Button *upPageBtn;
    Button *downPageBtn;
    QLabel *infoLab;
    QTimer *usbCheckTimer;
};
ErrorLogWindow::ErrorLogWindow()
    : Window(), d_ptr(new ErrorLogWindowPrivate())
{
    setFixedSize(800, 580);

    d_ptr->table = new TableView();
    TableHeaderView *horizontalHeader = new TableHeaderView(Qt::Horizontal);
    TableHeaderView *verticalHeader = new TableHeaderView(Qt::Vertical);
    d_ptr->table->setHorizontalHeader(horizontalHeader);
    d_ptr->table->setVerticalHeader(verticalHeader);
    horizontalHeader->setResizeMode(QHeaderView::ResizeToContents);
    verticalHeader->setResizeMode(QHeaderView::ResizeToContents);
    verticalHeader->setVisible(false);
    d_ptr->table->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    d_ptr->table->setShowGrid(false);
    d_ptr->model = new ErrorLogTableModel();
    d_ptr->table->setModel(d_ptr->model);
    d_ptr->table->setFixedHeight(d_ptr->model->getHeaderHeightHint()
                                 + d_ptr->model->getRowHeightHint() * TABLE_ROW_NR);
    d_ptr->table->setFixedWidth(800);
    d_ptr->table->setItemDelegate(new TableViewItemDelegate(this));
    connect(d_ptr->table, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClickSlot(QModelIndex)));

    d_ptr->summaryBtn = new Button(trs("Summary"));
    d_ptr->summaryBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->summaryBtn->setFixedHeight(HEIGHT_HINT);
    connect(d_ptr->summaryBtn, SIGNAL(released()), this, SLOT(summaryReleased()));

    d_ptr->exportBtn = new Button(trs("ErrorLogExport"));
    d_ptr->exportBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->exportBtn->setFixedHeight(HEIGHT_HINT);
    connect(d_ptr->exportBtn, SIGNAL(released()), this, SLOT(exportReleased()));

    d_ptr->eraseBtn = new Button(trs("ErrorLogErase"));
    d_ptr->eraseBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->eraseBtn->setFixedHeight(HEIGHT_HINT);
    connect(d_ptr->eraseBtn, SIGNAL(released()), this, SLOT(eraseReleased()));

    d_ptr->upPageBtn = new Button("", QIcon("/usr/local/nPM/icons/up.png"));
    d_ptr->upPageBtn->setButtonStyle(Button::ButtonIconOnly);
    d_ptr->upPageBtn->setFixedHeight(HEIGHT_HINT);
    connect(d_ptr->upPageBtn, SIGNAL(released()), d_ptr->model, SLOT(upBtnReleased()));

    d_ptr->downPageBtn = new Button("", QIcon("/usr/local/nPM/icons/down.png"));
    d_ptr->downPageBtn->setButtonStyle(Button::ButtonIconOnly);
    d_ptr->downPageBtn->setFixedHeight(HEIGHT_HINT);
    connect(d_ptr->downPageBtn, SIGNAL(released()), d_ptr->model, SLOT(downBtnReleased()));

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(d_ptr->summaryBtn, 2);
    hLayout->addWidget(d_ptr->exportBtn, 2);
    hLayout->addWidget(d_ptr->eraseBtn, 2);
    hLayout->addWidget(d_ptr->upPageBtn, 1);
    hLayout->addWidget(d_ptr->downPageBtn, 1);

    d_ptr->infoLab = new QLabel(trs("WarningNoUSB"));
    d_ptr->infoLab->setFixedHeight(HEIGHT_HINT);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(d_ptr->table);
    layout->addLayout(hLayout);
    layout->addWidget(d_ptr->infoLab, 1, Qt::AlignRight);

    setWindowLayout(layout);

    d_ptr->usbCheckTimer = new QTimer();
    d_ptr->usbCheckTimer->setInterval(500);
    connect(d_ptr->usbCheckTimer, SIGNAL(timeout()), this, SLOT(USBCheckTimeout()));
}

ErrorLogWindow *ErrorLogWindow::getInstance()
{
    static ErrorLogWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new ErrorLogWindow();
    }
    return instance;
}

ErrorLogWindow::~ErrorLogWindow()
{
}

void ErrorLogWindow::init()
{
    d_ptr->model->loadData();

    if (usbManager.isUSBExist())
    {
        d_ptr->infoLab->hide();
    }

    d_ptr->usbCheckTimer->start();
}

void ErrorLogWindow::showEvent(QShowEvent *ev)
{
    init();
    Window::showEvent(ev);
}

void ErrorLogWindow::itemClickSlot(QModelIndex index)
{
    int row = index.row();
    int realIndex = d_ptr->model->getErrorLogIndex(row);
    ErrorLogItemBase *item = errorLog.getLog(realIndex);
    if (item->isLogEmpty())
    {
        delete item;
        return;
    }

    ErrorLogViewerWindow(item).exec();
    delete item;
}

void ErrorLogWindow::summaryReleased()
{
    QString str;
    QTextStream stream(&str);
    ErrorLog::Summary summary = errorLog.getSummary();
    stream << "Number of Errors: " << summary.NumOfErrors << endl;
    stream << "Number of Critical Faults: " << summary.numOfCriticalErrors << endl;
    stream << "Most Recent Error: " << summary.mostRecentErrorDate << endl;
    stream << "Most Recent Critical Fault: " << summary.mostRecentCriticalErrorDate << endl;
    stream << "Oldest Error: " << summary.oldestErrorDate << endl;
    stream << "Last Erase Time: " << summary.lastEraseTimeDate << endl;
    stream << "Number of shocks > 120J: " << summary.totalShockCount << endl;

    ErrorLogViewerWindow viewer;
    viewer.setWindowTitle(trs("Summary"));
    viewer.setText(str);
    viewer.exec();
}

void ErrorLogWindow::exportReleased()
{
    if (usbManager.isUSBExist())
    {
        ExportDataWidget exportDataWidget(EXPORT_ERROR_LOG_BY_USB);
        connect(&usbManager, SIGNAL(exportProcessChanged(unsigned char)), &exportDataWidget, SLOT(setBarValue(unsigned char)));
        connect(&usbManager, SIGNAL(exportError()), &exportDataWidget, SLOT(reject()));
        connect(&exportDataWidget, SIGNAL(cancel()), &usbManager, SLOT(cancelExport()));

        // start export
        if (usbManager.exportErrorLog())
        {
            QDialog::DialogCode statue = static_cast<QDialog::DialogCode>(exportDataWidget.exec());
            if (QDialog::Rejected == statue)
            {
                QString msg;
                DataExporterBase::ExportStatus status = usbManager.getLastExportStatus();
                if (status == DataExporterBase::Cancel || exportDataWidget.isTransferCancel())
                {
                    msg = trs("TransferCancel");
                }
                else if (status == DataExporterBase::Disconnect)
                {
                    msg = trs("TransferDisconnect");
                }
                else if (status == DataExporterBase::NoSpace)
                {
                    msg = trs("WarnLessUSBFreeSpace");
                }
                else
                {
                    msg = trs("TransferFailed");
                }
                MessageBox messageBox(trs("Warn"), msg, QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
            else if (QDialog::Accepted == statue)  // 导出成功
            {
                eraseReleased();  // 询问是否擦除errorlog
            }
        }
    }
    else
    {
        MessageBox messageBox(trs("Warn"), trs("WarningNoUSB"), QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
}

void ErrorLogWindow::eraseReleased()
{
    MessageBox messageBox(trs("ErrorLogErase"), trs("ComfirmClearErrorLog"));
    if (1 == messageBox.exec())
    {
        errorLog.clear();
        init();
        unsigned timestamp = QDateTime::currentDateTime().toTime_t();
        systemConfig.setNumValue("ErrorLogEraseTime", timestamp);
        systemConfig.save();
        systemConfig.saveToDisk();
    }
}

void ErrorLogWindow::USBCheckTimeout()
{
    // 检查U盘
    if (!usbManager.isUSBExist())
    {
        d_ptr->infoLab->show();
    }
    else
    {
        d_ptr->infoLab->hide();
    }
}
