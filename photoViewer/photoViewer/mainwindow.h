#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "editor_plugin_interface.h"

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initConnect();

    void showImage(const QString &path);

    void loadPlugins();

    void testErode();

private slots:
    void openImage();

    void on_actionzoomIn_triggered(bool checked);

    void on_actionzoomOut_triggered(bool checked);

    void on_actionsaveAs_triggered(bool checked);

    void on_actionPrev_triggered(bool checked);

    void on_actionNext_triggered(bool checked);

    void on_actionblur_triggered(bool checked);

    void pluginPerform(bool checked);

private:
    Ui::MainWindow *ui;

private:
    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

    QLabel *statusLabel;

    QGraphicsPixmapItem *currentImage;
    QString currentImagePath;

    QMap<QString, EditorPluginInterface*> editPlugins;
};
#endif // MAINWINDOW_H
