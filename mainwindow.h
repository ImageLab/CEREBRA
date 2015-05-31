#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "packetfilereader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QString fileName;
    QString directory;
    QString comboText;
    QString comboText2;
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    PacketFileReader reader;
    QGLAbstractScene *m_scene;
    QGLSceneNode *m_rootNode;

public slots:
    void loadButtonClicked();
    void startButtonClicked();
    void loadMatFileButtonClicked();
};

#endif // MAINWINDOW_H
