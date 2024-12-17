#ifndef MEDIAYPLAYERWIDGET_H
#define MEDIAYPLAYERWIDGET_H

#include <QWidget>
#include <LWebView/LWebView.h>

#include "HttpClient.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MediayPlayerWidget;
}
QT_END_NAMESPACE

class MediayPlayerWidget : public QWidget
{
    Q_OBJECT

public:
    MediayPlayerWidget(QWidget * parent = nullptr);
    ~MediayPlayerWidget();

    void updateTreeWidget();

	//void refreshWeb(QString url);

public slots:
    // 获取文件列表
    void on_pushButton_refresh_clicked();
    void onItemDoubleClicked(QTreeWidgetItem * item, int column);

	void onLoadProgress(int progress);
	void onTitleChanged(const QString & title);
	void onUrlChanged(const QUrl & url);
private:
    Ui::MediayPlayerWidget * ui;

    HttpClient * m_pHttpClient{nullptr};
};
#endif // MEDIAYPLAYERWIDGET_H
