#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void fromRADtoGradMinSek(double fi, double &grad, double &min, double &sek);
    double fromGradMinSektoRAD(double grad, double min, double sek);
    void fromXYZtoBLH(double x, double y, double z, double &L, double &B, double &H);
    void fromBLHtoXYZ(double L, double B, double H, double &x, double &y, double &z);

private slots:
    void on_pushButton_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_comboBox2_currentIndexChanged(int index);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
