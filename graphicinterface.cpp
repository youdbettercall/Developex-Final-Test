#include "graphicinterface.h"
#include "ui_graphicinterface.h"
#include "searchengine.h"

#include <QThread>

namespace
{

const QString allowedUrlSymbols("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-.:_~/?#[]@!$&'()*+,;=");

const QString URL_TAG("http://");

bool isValidUrl(const QString& url)
{
    if (url.isNull() || url.isEmpty())
        return false;
    if (!url.startsWith(URL_TAG, Qt::CaseInsensitive))
        return false;

    for (int i = 0; i < url.size(); i++)
    {
        if (!allowedUrlSymbols.contains(url[i]) || url[i] == '\\')
        {
            return false;
        }
    }

    return true;
}
}


GraphicInterface::GraphicInterface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphicInterface),
    mIsStopped(false),
    mUrlsCount(0)
{
    ui->setupUi(this);
    ui->startUrl->setText("http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines");
    ui->tableWidget->setColumnWidth(0, ui->tableWidget->width() * 0.45);
    ui->tableWidget->setColumnWidth(1, ui->tableWidget->width() * 0.1);
    ui->tableWidget->setColumnWidth(2, ui->tableWidget->width() * 0.45);

    ui->tableWidget->setHorizontalHeaderLabels(QStringList( { "URL", "Status", "Result" } ));

    connect(ui->startButton, &QPushButton::released, this, &GraphicInterface::start);
    connect(ui->stopButton,  &QPushButton::released, this, &GraphicInterface::stop);

    connect(&mSearchEngine, &search::SearchEngine::foundUrl,        this, &GraphicInterface::addLineToResultTable);
    connect(&mSearchEngine, &search::SearchEngine::updateUrlStatus, this, &GraphicInterface::modifyLineInResultTable);
    connect(&mSearchEngine, &search::SearchEngine::searchFinished,  this, &GraphicInterface::searchfinished);
}

GraphicInterface::~GraphicInterface()
{
    delete ui;
}

void GraphicInterface::clear()
{
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);

    mUrlsCount = 0;
    ui->tableWidget->setHorizontalHeaderLabels(QStringList( { "URL", "Status", "Result" } ));
}

void GraphicInterface::start()
{
    //validating input
    if (!isValidUrl(ui->startUrl->text()))
    {
        ui->Resultlabel->setText("Invalid URL to parse. Contains invalid symbols");
        return;
    }

    int maxThreadCount = QThread::idealThreadCount();
    if (ui->threadsNumber->text().toInt() < 1 || ui->threadsNumber->text().toInt() > maxThreadCount)
    {
        QString message("Invalid threads number. Max possible number of threads: ");
        message += QString::number(maxThreadCount);

        ui->Resultlabel->setText(message);
        return;
    }
    if (ui->urlsNumber->text().toInt() < 1 || ui->urlsNumber->text().toInt() > 1000)
    {
        ui->Resultlabel->setText("Invalid max urls number. Should be between 1 and 1000");
        return;
    }

    ui->tableWidget->setHorizontalHeaderLabels(QStringList( { "URL", "Status", "Result" } ));
    if (mIsStopped)
    {
        clear();
        mIsStopped = false;
    }

    mSearchEngine.start(ui->startUrl->text(),
                        ui->searchText->text(),
                        ui->threadsNumber->text().toInt(),
                        ui->urlsNumber->text().toInt());

    ui->Resultlabel->setText("SEARCHING...");
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
}

void GraphicInterface::stop()
{
    mSearchEngine.stop();
    mIsStopped = true;

    ui->Resultlabel->setText("SEARCH STOPPED");
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
}

void GraphicInterface::addLineToResultTable(const QString &url, const QString &status, const QString &result)
{
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());

    QTableWidgetItem *newItem = new QTableWidgetItem(url);
    ui->tableWidget->setItem(mUrlsCount, 0, newItem);

    newItem = new QTableWidgetItem(status);
    ui->tableWidget->setItem(mUrlsCount, 1, newItem);

    newItem = new QTableWidgetItem(result);
    ui->tableWidget->setItem(mUrlsCount, 2, newItem);

    mUrlsCount++;
}

void GraphicInterface::modifyLineInResultTable(const QString &url, const QString &status, const QString &result)
{
    qDebug() << __FUNCTION__ << " : " << url;
    for(int row = 0; row < ui->tableWidget->rowCount(); row++)
    {
        if (ui->tableWidget->item(row, 0)->text() == url)
        {
            qDebug() << __FUNCTION__ << " : " << "updating table: "
                     << url << status << result;
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(status));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(result));
            return;
        }
    }
}

void GraphicInterface::searchfinished()
{
    qDebug() << __FUNCTION__;

    stop();
    ui->Resultlabel->setText("SEARCH FINISHED");
}
