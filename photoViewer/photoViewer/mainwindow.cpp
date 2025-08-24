#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "opencv2/core/core.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/utils/logger.hpp>

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QPluginLoader>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    currentImage = nullptr;

    imageScene = new QGraphicsScene(this);
    imageView = new QGraphicsView(imageScene);
    setCentralWidget(imageView);

    statusLabel = new QLabel(this);
    ui->statusbar->addWidget(statusLabel);

    initConnect();

    loadPlugins();

    cv::setUseOptimized(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadPlugins()
{
    QString path = QApplication::instance()->applicationDirPath() + QDir::separator() + "plugins";
    QDir pluginDir(path);
    QStringList nameFilters;
    nameFilters << "*.dll";
    QFileInfoList plugins = pluginDir.entryInfoList(
                nameFilters, QDir::NoDotAndDotDot | QDir::Files, QDir::Name);

    foreach(QFileInfo plugin, plugins)
    {
        QPluginLoader pluginLoader(plugin.absoluteFilePath(), this);
        EditorPluginInterface *plugin_ptr = dynamic_cast<EditorPluginInterface*>(pluginLoader.instance());
        if (plugin_ptr)
        {
            QAction *action = new QAction(plugin_ptr->name());
            ui->menuEdit->addAction(action);
            ui->toolBar->addAction(action);
            editPlugins[plugin_ptr->name()] = plugin_ptr;

            connect(action, SIGNAL(triggered(bool)),
                    this, SLOT(pluginPerform(bool)));
        }
        else
        {
            qDebug() << "bad plugin: " << plugin.absoluteFilePath();
        }
    }
}

void MainWindow::pluginPerform(bool checked)
{
    Q_UNUSED(checked)

    if (currentImage == nullptr)
    {
        QMessageBox::information(this, "info", "No Image");
        return;
    }

    QAction *active_action = qobject_cast<QAction*>(sender());
    EditorPluginInterface *plugin_ptr = editPlugins[active_action->text()];
    if (!plugin_ptr)
    {
        QMessageBox::information(this, "info", "No plugin");
        return;
    }

    QPixmap pixImage = currentImage->pixmap();
    QImage image = pixImage.toImage();
    image = image.convertToFormat(QImage::Format_RGB888);

    cv::Mat mat = cv::Mat(
                image.height(),
                image.width(),
                CV_8UC3,
                image.bits(),
                image.bytesPerLine());

    plugin_ptr->edit(mat, mat);

    QImage image_edit(
                mat.data,
                mat.cols,
                mat.rows,
                mat.step,
                QImage::Format_RGB888);
    pixImage = QPixmap::fromImage(image_edit);

    imageScene->clear();
    imageView->resetMatrix();

    currentImage = imageScene->addPixmap(pixImage);
    imageScene->update();
    imageView->setSceneRect(pixImage.rect());
}

void MainWindow::initConnect()
{
    connect(ui->actionOpenFile, SIGNAL(triggered(bool)), this, SLOT(openImage()));
}

void MainWindow::openImage()
{
    QString str = QFileDialog::getOpenFileName(this, "Open Image", "", "*.png *.jpg");
    if (str.isEmpty())
        return;

    showImage(str);
}

void MainWindow::showImage(const QString &path)
{
    imageScene->clear();
    imageView->resetMatrix();

    QPixmap image(path);
    currentImagePath = path;
    currentImage = imageScene->addPixmap(image);
    imageScene->update();
    imageView->setSceneRect(image.rect());

    QString status = QString("%1, %2x%3, %4 Bytes").arg(path).arg(image.width())
                 .arg(image.height()).arg(QFile(path).size());
    statusLabel->setText(status);
}

void MainWindow::on_actionzoomIn_triggered(bool checked)
{
    Q_UNUSED(checked);
    imageView->scale(1.2, 1.2);
}

void MainWindow::on_actionzoomOut_triggered(bool checked)
{
    Q_UNUSED(checked);
    imageView->scale(0.8, 0.8);
}

void MainWindow::on_actionsaveAs_triggered(bool checked)
{
    Q_UNUSED(checked);

    if (currentImage == nullptr)
    {
        QMessageBox::information(this, "Info", "Nothing to save");
        return;
    }

    QFileDialog dialog(this);
    dialog.setWindowTitle("save image");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Images (*.png *.jpg)"));
    QStringList fileNames;
    if (dialog.exec() == QFileDialog::Accepted)
    {
        fileNames = dialog.selectedFiles();
        if(QRegExp(".+\\.(png|jpg)").exactMatch(fileNames.at(0))) {
            currentImage->pixmap().save(fileNames.at(0));
        } else {
            QMessageBox::information(this, "Information", "Save error: bad format or filename.");
        }
    }
}

void MainWindow::on_actionPrev_triggered(bool checked)
{
    Q_UNUSED(checked);

    QFileInfo current(currentImagePath);
    QDir current_dir = current.absoluteDir();

    QStringList nameFilters;
    nameFilters << "*.png" << "*.jpg";
    QStringList fileNames = current_dir.entryList(nameFilters, QDir::Files, QDir::Name);
    int idx = fileNames.indexOf(QRegExp(QRegExp::escape(current.fileName())));
    if (idx > 0)
    {
        idx -= 1;
        if (idx <= 0)
            ui->actionPrev->setEnabled(false);
        ui->actionNext->setEnabled(true);
        showImage(current_dir.absoluteFilePath(fileNames.at(idx)));
    }
    else
        QMessageBox::information(this, "Information", "Current image is the first one.");
}

void MainWindow::on_actionNext_triggered(bool checked)
{
    Q_UNUSED(checked);

    QFileInfo current(currentImagePath);
    QDir dir = current.absoluteDir();
    QStringList filter;
    filter << "*.png" << "*.jpg";
    QStringList fileNames = dir.entryList(filter, QDir::Files, QDir::Name);
    int idx = fileNames.indexOf(QRegExp(QRegExp::escape(current.fileName())));
    if (idx >= fileNames.size() - 1)
    {
        QMessageBox::information(this, "Information",
                                 "Current image is the last one.");
    }
    else
    {
        idx += 1;
        if (idx >= fileNames.size() - 1)
            ui->actionNext->setEnabled(false);
        ui->actionPrev->setEnabled(true);
        showImage(dir.absoluteFilePath(fileNames.at(idx)));
    }
}

void MainWindow::on_actionblur_triggered(bool checked)
{
    Q_UNUSED(checked);

    if (currentImage == nullptr)
    {
        QMessageBox::information(this, "info", "NO image");
        return;
    }

    QPixmap pixmap = currentImage->pixmap();
    QImage image = pixmap.toImage();
    image = image.convertToFormat(QImage::Format_RGB888);
    cv::Mat mat = cv::Mat(
                image.height(),
                image.width(),
                CV_8UC3,
                image.bits(),
                image.bytesPerLine());

    cv::Mat tmp;
    cv::blur(mat, tmp, cv::Size(8, 8));
    mat = tmp;

    QImage imaged_blurred(
                mat.data,
                mat.cols,
                mat.rows,
                mat.step,
                QImage::Format_RGB888);
    pixmap = QPixmap::fromImage(imaged_blurred);

    imageScene->clear();
    imageView->resetMatrix();
    currentImage = imageScene->addPixmap(pixmap);
    imageScene->update();
    imageView->setSceneRect(pixmap.rect());
}
