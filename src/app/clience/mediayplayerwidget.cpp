#include "mediayplayerwidget.h"
#include "ui_mediayplayerwidget.h"

#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

MediayPlayerWidget::MediayPlayerWidget(QWidget * parent)
    : QWidget(parent)
    , ui(new Ui::MediayPlayerWidget)
{
    ui->setupUi(this);

    // 初始化 HttpClient
    m_pHttpClient = new HttpClient(this);

    // 连接 TreeWidget 的项点击事件
    connect(ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, &MediayPlayerWidget::onItemDoubleClicked);

    on_pushButton_refresh_clicked();

    connect(ui->webView, SIGNAL(loadProgress(int)), SLOT(onLoadProgress(int)));
    connect(ui->webView, SIGNAL(titleChanged(QString)), SLOT(onTitleChanged(QString)));
    connect(ui->webView, SIGNAL(urlChanged(QUrl)), SLOT(onUrlChanged(QUrl)));

    onLoadProgress(ui->webView->LoadProgress());
}

MediayPlayerWidget::~MediayPlayerWidget()
{
    delete ui;
}

void MediayPlayerWidget::updateTreeWidget()
{
    m_pHttpClient->updateTreeWidget(ui->treeWidget);
}

void MediayPlayerWidget::on_pushButton_refresh_clicked()
{
    // 请求一次 然后刷新
    m_pHttpClient->getFileList();
    updateTreeWidget();
}

void MediayPlayerWidget::onItemDoubleClicked(QTreeWidgetItem * item, int column)
{
    FileInfo fileInfo = item->data(0, Qt::UserRole).value<FileInfo>();

    if (fileInfo.isDir == false)
    {
        ui->webView->setUrl(m_pHttpClient->getVedioUrl(fileInfo.fullPath));
    }
}

void MediayPlayerWidget::onLoadProgress(int progress)
{
    // 显示加载进度
    progress = progress < 100 ? progress : 0;
    ui->progressBar->setValue(progress);
    ui->progressBar->setVisible(progress > 0);
}

void MediayPlayerWidget::onTitleChanged(const QString & title)
{
    // setWindowTitle(title);
}

void MediayPlayerWidget::onUrlChanged(const QUrl & url)
{
}