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
#include <condition_variable>


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
    void downloadSuccessfull(QNetworkReply *reply);
    void processUrl(const QString &url);

signals:
    void foundUrl(const QString& url, const QString& status, const QString& result);
    void updateUrlStatus(const QString& url, const QString& status, const QString& result);
    void processNextUrl(const QString&);
    void searchFinished();

private:
    bool findText(const QString& filename);


private:
    QString                 mStartUrl;
    QString                 mTextToFind;
    int                     mThreadsNumber;
    size_t                  mMaxUrls;
    size_t                  mFoundUrlsCount;
    size_t                  mCurrentUrlIndex;
    bool                    mIsStopped;

    QNetworkAccessManager   mNetworkManager;
    QList<QNetworkReply*>   mCurrentDownloads;
    std::vector<QString>    mFoundUrlsInFile;

    std::mutex              mUrlsListMtx;
};

}//end of namespace
#endif // SEARCHENGINE_H
