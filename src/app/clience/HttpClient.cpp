#include "HttpClient.h"
#include <QSettings>
#include <QDebug>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QUrl>
#include <QTreeWidgetItem>

HttpClient::HttpClient(QObject * parent)
    : QObject(parent)
{
    // 从配置文件读取服务端的 IP 和端口
    loadConfig();

    // 创建 QNetworkAccessManager
    manager = new QNetworkAccessManager(this);
}

HttpClient::~HttpClient()
{
    reply->deleteLater();
}

void HttpClient::onRequestFinished()
{
    // 获取响应数据
    QByteArray responseData = reply->readAll();
    // qDebug() << "Response Data:" << responseData;

    // 解析 JSON 数据
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if (!jsonDoc.isNull())
    {
        QJsonObject jsonResponse = jsonDoc.object();
        if (jsonResponse.contains("files"))
        {
            QJsonArray fileArray = jsonResponse["files"].toArray();
            // 清空 fileList 以便更新 TreeWidget
            parseFileList(fileArray, m_fileList);
        }
        else
        {
            qDebug() << "Error: No 'files' key in response.";
        }
    }
    else
    {
        qDebug() << "Failed to parse JSON response.";
    }
}

void HttpClient::loadConfig()
{
    QSettings settings("config/config.ini", QSettings::IniFormat);
    ip   = settings.value("Server/ip", "127.0.0.1").toString();
    port = settings.value("Server/port", 8080).toInt();
}

void HttpClient::parseFileList(const QJsonArray & fileArray, QList<FileInfo> & fileList, const QString & parentDir)
{
    fileList.clear();
    for (const QJsonValue & value : fileArray)
    {
        QJsonObject fileObject = value.toObject();
        FileInfo    fileInfo;
        fileInfo.name     = fileObject["name"].toString();
        fileInfo.isDir    = fileObject["isDir"].toBool();
        fileInfo.fullPath = parentDir + "/" + fileInfo.name; // 设置完整路径

        // 如果是目录，递归解析子目录
        if (fileInfo.isDir && fileObject.contains("children"))
        {
            QJsonArray childrenArray = fileObject["children"].toArray();
            parseFileListRecursively(childrenArray, fileInfo.children, fileInfo.fullPath);
        }

        fileList.append(fileInfo);
    }
}

void HttpClient::parseFileListRecursively(const QJsonArray & childrenArray, QList<FileInfo> & childrenList, const QString & parentDir)
{
    for (const QJsonValue & value : childrenArray)
    {
        QJsonObject childObject = value.toObject();
        FileInfo    childInfo;
        childInfo.name     = childObject["name"].toString();
        childInfo.isDir    = childObject["isDir"].toBool();
        childInfo.fullPath = parentDir + "/" + childInfo.name; // 设置完整路径

        // 如果是目录，递归解析子目录
        if (childInfo.isDir && childObject.contains("children"))
        {
            QJsonArray childChildrenArray = childObject["children"].toArray();
            parseFileListRecursively(childChildrenArray, childInfo.children, childInfo.fullPath);
        }

        childrenList.append(childInfo);
    }
}

void HttpClient::updateTreeWidget(QTreeWidget * treeWidget)
{
    treeWidget->clear();

    // 调用递归方法更新树形控件
    for (const FileInfo & fileInfo : m_fileList)
    {
        QTreeWidgetItem * item = new QTreeWidgetItem(treeWidget);
        item->setText(0, fileInfo.name);
        QVariant var = QVariant::fromValue<FileInfo>(fileInfo);
        item->setData(0, Qt::UserRole, var); // 设置文件的完整路径为数据
        if (fileInfo.isDir)
        {
            item->setIcon(0, QIcon(":/icons/folder.png")); // 设置目录图标
            addSubdirectories(item, fileInfo);             // 递归添加子目录
        }
        else
        {
            item->setIcon(0, QIcon(":/icons/file.png")); // 设置文件图标
        }
    }
}

// 递归添加子目录
void HttpClient::addSubdirectories(QTreeWidgetItem * parentItem, const FileInfo & parentInfo)
{
    for (const FileInfo & childInfo : parentInfo.children)
    {
        QTreeWidgetItem * childItem = new QTreeWidgetItem(parentItem);
        childItem->setText(0, childInfo.name);
        QVariant var = QVariant::fromValue<FileInfo>(childInfo);

        childItem->setData(0, Qt::UserRole, var); // 设置子文件的完整路径为数据
        if (childInfo.isDir)
        {
            childItem->setIcon(0, QIcon(":/icons/folder.png")); // 设置目录图标
            addSubdirectories(childItem, childInfo);            // 递归添加子目录
        }
        else
        {
            childItem->setIcon(0, QIcon(":/icons/file.png")); // 设置文件图标
        }
    }
}

void HttpClient::getFileList()
{
    // 如果有旧的回复对象，先断开连接并删除
    if (reply)
    {
        disconnect(reply, &QNetworkReply::finished, this, &HttpClient::onRequestFinished); // 断开槽函数
        reply->deleteLater();                                                              // 删除之前的 reply 对象
    }

    // 设置请求的 URL
    QUrl            url(QString("http://%1:%2/filelist").arg(ip).arg(port));
    QNetworkRequest request(url);

    // 发送 GET 请求
    reply = manager->get(request);

    // 连接新的 reply 对象的 finished 信号与槽函数
    connect(reply, &QNetworkReply::finished, this, &HttpClient::onRequestFinished);
}

QUrl HttpClient::getVedioUrl(QString path)
{
    QUrl url(QString("http://%1:%2%3").arg(ip).arg(port).arg(path));
	qDebug()<<url;
    return url;
}
