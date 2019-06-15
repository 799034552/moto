#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void showMessage(QString a);
    void getAngle(QString a);
    ~MainWindow();

private slots:
    void on_action_triggered();


    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();
    void hideSuccess();
    void hideSuccess1();
    void tcpGet();


    void on_pushButton_4_clicked(bool checked);
    
    void on_action_triggered(bool checked);

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
protected:
    void paintEvent(QPaintEvent *Event);
};
extern MainWindow* that;

#endif // MAINWINDOW_H
