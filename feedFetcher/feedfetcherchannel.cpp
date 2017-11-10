#include "feedfetcherchannel.h"

FeedFetcherChannel::FeedFetcherChannel(QString channelName, QString channelID, QSettings *settings, QString clientId, QString clientSecret)
{
    this->channelName = channelName;
    this->channelID = channelID;
    this->settings = settings;
    this->clientId = clientId;
    this->clientSecret = clientSecret;
    this->quotaCount = 0;

    playlistList = new QList<FeedFetcherPlaylist *>();

    initPlaylistsInfos();
}

QString FeedFetcherChannel::getChannelId()
{
    return this->channelID;
}

void FeedFetcherChannel::fetch(QString currentToken)
{
    bool isAlreadyFetched = false;
    QString playlistId;

    this->currentToken = currentToken;

    //qDebug() << "Request to fetch channels.";

    //Seach in the current config if we didn't already fetch the right playlist ID for this channel
    for(int j=0; j<playlistInfos.size(); j++)
    {
        if(playlistInfos.at(j).channelID == channelID)
        {
            playlistId = playlistInfos.at(j).playlistID;
            isAlreadyFetched = true;
        }
    }

    if(isAlreadyFetched == true)
    {
        addPlayList(playlistId);
    }
    else
    {
        QList<QString> channelList;
        channelList.append(channelID);

        getPlaylistId(channelList);
    }
}


QList<Video *> *FeedFetcherChannel::getVideos()
{
    QList<Video *> *videoList = new QList<Video *>();

    for(int i=0; i<playlistList->count(); i++)
    {
        videoList->append(*(playlistList->at(i)->getVideos()));
    }

    return videoList;
}


void FeedFetcherChannel::getPlaylistId(QList<QString> channelIDs)
{
    QString url;

    url = "https://www.googleapis.com/youtube/v3";
    url += "/channels";

    url += "?part=contentDetails";
    url += "&id=";
    for(int i=0; i<channelIDs.count(); i++)
        url += channelIDs.at(i) + ",";
    url = url.left(url.size()-1);

    url += "&maxResults=50";
    url += "&access_token=" + currentToken;

    //qDebug() << url;

    QNetworkRequest request(url);
    manager.get(request);
    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(decondePlaylistId(QNetworkReply*)));

    addQuotaUsage(1+2);  // 1 for the request, 2 for the contentDetails
}



void FeedFetcherChannel::decondePlaylistId(QNetworkReply* reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (statusCode >= 200 && statusCode < 300) {
        QString data = (QString)reply->readAll();

        //qDebug() << "received data for channel : " << this->channelName << " : " << data;

        QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject jsonResponseObj = jsonResponse.object();

        QJsonArray playlistList = jsonResponseObj.value("items").toArray();

        for(int i=0; i<playlistList.count(); i++)
        {
            QJsonObject playlist = playlistList.at(i).toObject();

            QString channelID = playlist.value("id").toString();
            QString playlistID = playlist.value("contentDetails").toObject().value("relatedPlaylists").toObject().value("uploads").toString();

            //qDebug() << "Ask about the playlist : " << playlistID;
            addPlayList(playlistID);

            PlaylistInfo playlistInfo;
            playlistInfo.channelID = channelID;
            playlistInfo.playlistID = playlistID;
            playlistInfos.append(playlistInfo);
        }

        savePlaylistsInfos();
    }
}

void FeedFetcherChannel::addPlayList(QString playlistId)
{
    FeedFetcherPlaylist *playlist;
    bool alreadyExists = false;

    for(int i=0; i<playlistList->count(); i++)
    {
        if(playlistList->at(i)->getPlaylistId().compare(playlistId) == 0)
        {
            alreadyExists = true;
            playlist = playlistList->at(i);
        }
    }

    if(alreadyExists == false)
    {
        playlist = new FeedFetcherPlaylist(playlistId, settings, clientId, clientSecret);
        playlistList->append(playlist);

        connect(playlist, SIGNAL(doneFetching()), this, SLOT(playlistFetched()), Qt::UniqueConnection);
    }

    playlist->fetch(currentToken);
}


void FeedFetcherChannel::playlistFetched()
{
    emit doneFetching();
}


void FeedFetcherChannel::initPlaylistsInfos()
{
    int size = settings->beginReadArray("playlists");
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        PlaylistInfo playlist;

        playlist.channelID = settings->value("channel").toString();
        playlist.playlistID = settings->value("playlist").toString();

        playlistInfos.append(playlist);
    }
    settings->endArray();
}


void FeedFetcherChannel::savePlaylistsInfos()
{
    settings->beginWriteArray("playlists");
    for(int i=0; i<playlistInfos.count(); i++)
    {
        settings->setArrayIndex(i);
        settings->setValue("channel", playlistInfos.at(i).channelID);
        settings->setValue("playlist", playlistInfos.at(i).playlistID);
    }

    settings->endArray();
    settings->sync();
}

void FeedFetcherChannel::addQuotaUsage(int amount)
{
    quotaCount += amount;
}

int FeedFetcherChannel::getQuotaUsed()
{
    int totalQuota = 0;

    totalQuota += quotaCount;

    for(int i=0; i<playlistList->count(); i++)
        totalQuota += playlistList->at(i)->getQuotaUsed();

    return totalQuota;
}
