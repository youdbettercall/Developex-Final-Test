#include "searchengine.h"

#include <QDebug>
#include <QThread>
#include <QFileInfo>

namespace
{
const QString allowedUrlSymbols("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-.:_~/?#[]@!$&'()*+,;=");


QString saveFileName(const QUrl &url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "webpage";

    if (QFile::exists(basename))
    {
        // already exists, don't overwrite
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i)))
            ++i;

        basename += QString::number(i);
    }

    basename.append(".html");
    return basename;
}

bool saveFile(QIODevice *data, const QString& filename)
{
    qDebug() << __FUNCTION__ << " : " << __LINE__;

    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not open " << filename << " for writing";
        return false;
    }

    file.write(data->readAll());
    file.close();

    return true;
}

void deleteFile(const QString& filename)
{
    qDebug() << __FUNCTION__ << " : " << __LINE__;

    QFile file(filename);
    if (!file.exists()) return;

    file.remove();
}

QString findUrlInLine(const QString& line)
{
    /*
     * let me skip all non-html urls,
     * because downloading and reading files(*.docx, *.pdf etc) will be rather expensive operation
     * that on my opinion exceeds current test purpose
     */

    int startIndex = line.indexOf("http://", 0, Qt::CaseInsensitive);

    if (startIndex == -1 )
        return QString();

    int endIndex = -1;
    for (int i = startIndex; i < line.size(); i++)
    {
        if (!allowedUrlSymbols.contains(line[i]) || line[i] == '\\')
        {
            endIndex = i;
            break;
        }
    }

    if (endIndex ==  -1)
        return QString();

    QString url(line.mid(startIndex, endIndex - startIndex));


    if (url.isNull()) return QString();
    //if (!url.endsWith(".html")) return QString();

    return url;
}

}//end of anonymus namespace

namespace search
{
typedef std::unique_lock<std::mutex> Lock_t;

SearchEngine::SearchEngine(QObject */*parent*/) :
    mStartUrl(),
    mTextToFind(),
    mMaxUrlsCount(1),
    mFoundUrlsCount(1),
    mUrlsProcessedCount(0),
    mIsStopped(false)
{
    qDebug() << __FUNCTION__ << ":" << __LINE__;

    connect(&mNetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadSuccessfull(QNetworkReply*)));
}

SearchEngine::~SearchEngine()
{
    qDebug() << __FUNCTION__ << " : " << __LINE__;

}

void SearchEngine::start(const QString& url,
                         const QString& text,
                         const int threadsNumber,
                         const int urlsNumber)
{
    qDebug() << __FUNCTION__ << ":" << __LINE__;

    mStartUrl = url;
    mTextToFind = text;
    mThreadsNumber = threadsNumber;

    mMaxUrlsCount = urlsNumber <= 1 ? 1 :  urlsNumber;

    emit foundUrl(mStartUrl, QString("Downloading..."), QString(" "));
    mFoundUrlsVector.push_back(url);
    mIsStopped = false;
    processUrl(mStartUrl);
}

void SearchEngine::stop()
{
    qDebug() << __FUNCTION__ << ":" << __LINE__;

    Lock_t lock(mUrlsListMtx);
    mIsStopped = true;

    mUrlsQueue.clear();
    mFoundUrlsVector.clear();
    mCurrentDownloadsList.clear();

    mStartUrl= "";
    mTextToFind = "";
    mThreadsNumber = 1 ;
    mMaxUrlsCount = 1;
    mFoundUrlsCount = 1;
    mUrlsProcessedCount = 0;
}


void SearchEngine::downloadSuccessfull(QNetworkReply* reply)
{
    qDebug() << __FUNCTION__ << " : " << reply->url();

    if (mIsStopped)
        return;

    if (reply->error())
    {
        emit updateUrlStatus(reply->url().toString(), "Process URL failed", reply->errorString());
        reply->deleteLater();
        return;
    }

    QString filename = saveFileName(reply->url());
    if (saveFile(reply, filename) == false)
    {
        emit updateUrlStatus(filename, "Process URL failed", "Search failed");
        reply->deleteLater();
        return;
    }

    reply->deleteLater();

    if (findText(filename) ==  true)
        emit updateUrlStatus(reply->url().toString(), "Processed", "OK");
    else
        emit updateUrlStatus(reply->url().toString(), "Processed", "Searched text not found");

    deleteFile(filename);

    if (mUrlsProcessedCount++ == mMaxUrlsCount)
        emit searchFinished();

    for(size_t i = 0; i < mUrlsQueue.size(); i++)
    {
        QString url;
        {
            Lock_t lock(mUrlsListMtx);
            url = mUrlsQueue.front();
            mUrlsQueue.pop_front();
        }
        processUrl(url);
    }
}


void SearchEngine::processUrl(const QString& url)
{
    if (mIsStopped)
        return;

    if (url.isEmpty())
        return;

    qDebug() << __FUNCTION__ << " : " << url;

    QNetworkRequest request((QUrl(url)));

    QNetworkReply* reply = mNetworkManager.get(request);

    mCurrentDownloadsList.append(reply);
}

bool SearchEngine::findText(const QString& filename)
{
    qDebug() << __FUNCTION__ << " : " << __LINE__;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit updateUrlStatus(filename, "Process URL failed", "Text not found");
        return false;
    }
    size_t findCount = 0;
    QTextStream textStream(&file);
    QString line;
    do
    {
        if(textStream.status() != QTextStream::Ok)
            break;

        line = textStream.readLine();
        if (line.contains(mTextToFind, Qt::CaseInsensitive))
            ++findCount;

        {
            Lock_t lock(mUrlsListMtx);
            if (mMaxUrlsCount == mFoundUrlsCount)
                continue;
        }

        QString url = findUrlInLine(line);
        if (!url.isNull())
        {
            {
                Lock_t lock(mUrlsListMtx);
                auto it = std::find(mFoundUrlsVector.begin(), mFoundUrlsVector.end(), url);
                if (it != mFoundUrlsVector.end())
                    continue;

                mUrlsQueue.push_back(url);
                mFoundUrlsVector.push_back(url);
                mFoundUrlsCount++;
                emit foundUrl(url, "Processing...", "");
            }
        }
    }
    while( !textStream.atEnd() );

    return findCount > 0;
}

}//end of namespace
