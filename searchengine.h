#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <QObject>
#include <QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QFile>
#include <QUrl>
#include <mutex>

namespace search
{
class SearchEngine : public QObject
{
    Q_OBJECT
public:
    explicit SearchEngine(QObject *parent = nullptr);
    ~SearchEngine();

    void start(const QString &url, const QString &text, const int threadsNumber, const int urlsNumber);
    void stop();

public slots:
    void downloadSuccessfull(QNetworkReply*);

signals:
    void foundUrl(const QString& url, const QString& status, const QString& result);
    void updateUrlStatus(const QString& url, const QString& status, const QString& result);
    void processNextUrl(const QString&);
    void searchFinished();

private:
    void processUrl(const QString &url);
    bool findText(const QString& filename);


private:
    QString                 mStartUrl;
    QString                 mTextToFind;
    int                     mThreadsNumber;
    size_t                  mMaxUrlsCount;

    size_t                  mFoundUrlsCount;
    size_t                  mUrlsProcessedCount;

    bool                    mIsStopped;

    QNetworkAccessManager   mNetworkManager;
    QList<QNetworkReply*>   mCurrentDownloadsList;

    std::list<QString>      mUrlsQueue;
    std::vector<QString>    mFoundUrlsVector;
    std::mutex              mUrlsListMtx;
};
}//end of namespace
#endif // SEARCHENGINE_H
