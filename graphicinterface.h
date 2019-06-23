#ifndef GRAPHICINTERFACE_H
#define GRAPHICINTERFACE_H

#include <QMainWindow>
#include <searchengine.h>

namespace Ui {
class GraphicInterface;
}

class GraphicInterface : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphicInterface(QWidget *parent = nullptr);
    ~GraphicInterface();

private:
    Ui::GraphicInterface* ui;
    search::SearchEngine  mSearchEngine;

    bool mIsStopped;
    int  mUrlsCount;

private:
    void clear();

private slots:
    void start();
    void stop();

    void addLineToResultTable(const QString& url, const QString& status, const QString& result);
    void modifyLineInResultTable(const QString& url, const QString& status, const QString& result);

    void searchfinished();
};

#endif // GRAPHICINTERFACE_H
