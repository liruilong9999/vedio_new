#ifndef HTTPFILESERVER_H
#define HTTPFILESERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QSettings>   // 导入 QSettings
#include <QJsonArray>  // 导入 JSON 支持
#include <QJsonObject> // 导入 JSON 支持

class HttpFileServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit HttpFileServer(QObject * parent = nullptr);
    ~HttpFileServer();

    // 启动服务器，通过配置文件获取路径和端口
    void startServer();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    QString m_directory; // 服务器提供的文件目录
    int     m_port;      // 服务器监听的端口

    // 处理文件响应，发送HTTP响应和文件内容
    void sendHttpResponse(QTcpSocket * socket, const QString & filePath);

    // 从配置文件中读取配置
    void loadSettings();

    // 处理文件列表请求并返回 JSON 格式的文件结构
    void sendFileList(QTcpSocket * socket);
	QJsonArray getFileListFromDir(const QString &dirPath);
};


#endif // HTTPFILESERVER_H
