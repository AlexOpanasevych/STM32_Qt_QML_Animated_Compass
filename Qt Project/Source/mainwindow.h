#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void update_matrix();
    QObject *rect;
    ~MainWindow();
private slots:
    void handleBytesWritten(qint64 bytes);
    void copy_array_an_to_bn(unsigned char *source, int *destination, int count, int aposition, int bposition);
    void serialReceived();
    void timerOverflowed();
    void on_actionConnectButtonAction_triggered();
    int compare_array(unsigned char *arr1, unsigned char *arr2, int count, int position);

private:
    void on_actionSetButtonAction_triggered();
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
