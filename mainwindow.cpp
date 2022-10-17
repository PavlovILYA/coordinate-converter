#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "math.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->comboBox->addItem("широта в радианах, долгота в радианах, высота в метрах");
    ui->comboBox->addItem("широта в градусах-минутах-секундах, долгота в градусах-минутах-секундах, высота в метрах");
    ui->comboBox->addItem("пространственные прямоугольные координаты");

    ui->comboBox2->addItem("широта в градусах-минутах-секундах, долгота в градусах-минутах-секундах, высота в метрах");
    ui->comboBox2->addItem("широта в радианах, долгота в радианах, высота в метрах");
    ui->comboBox2->addItem("пространственные прямоугольные координаты");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::fromRADtoGradMinSek(double fi, double &grad, double &min, double &sek)
{
    fi = fi * 180 / M_PI;
    min = modf(fi, &grad);
    sek = modf((min / (1.0/60.0)), &min);
    fi = modf((sek / (1.0/60.0)), &sek);
}


double MainWindow::fromGradMinSektoRAD(double grad, double min, double sek)
{
    double fi;
    fi = (grad + min/60.0 + sek/3600.0) * M_PI / 180.0;
    return fi;
}


void MainWindow::fromBLHtoXYZ(double B, double L, double H, double &x, double &y, double &z)
{
    // параметры геоида WGS—84:
    double a, b, c, e2, e12, f;
    a = 6378137;                     // большая полусось (м)
    f = 1/298.257223563;             // сжатие
    b = a * (1. - f);                // малая полуось
    c = a / (1. - f);                // полярный радиус кривизны поверхности
    e2 = f * (2. - f);               // первый эксцентриситет в квадрате
    e12 = e2 / (1. - e2);            // второй эксцентриситет в квадрате

    double N;
    N = a / sqrt(1. - (e2 * sin(B) * sin(B))); // радиус кривизны первого вертикала
    x = (N + H) * cos(B) * cos(L);
    y = (N + H) * cos(B) * sin(L);
    z = (N + H - e2*N) * sin(B);
}


void MainWindow::fromXYZtoBLH(double x, double y, double z, double &B, double &L, double &H)
{
    // параметры геоида WGS—84:
    double a, b, c, e2, e12, f;
    a = 6378137;                     // большая полусось (м)
    f = 1/298.257223563;             // сжатие
    b = a * (1. - f);                // малая полуось
    c = a / (1. - f);                // полярный радиус кривизны поверхности
    e2 = f * (2. - f);               // первый эксцентриситет в квадрате
    e12 = e2 / (1. - e2);            // второй эксцентриситет в квадрате

    double p, r, tgB, N;
    p = sqrt(x*x + y*y);        // расстояние от оси вращения эллипсоида
    r = sqrt(x*x + y*y + z*z);  // геоцентрический радиус-вектор
    if (p == 0.)
    {
        B = copysign(M_PI/2, z);
        L = 0.;
        H = abs(z) - b;
    }
    else
    {
        tgB = (z / p) * (1. + e12 * b / r);  // предварительная оценка широты B
        for (int i = 0; i < 5; ++i)
        {
            tgB = tgB * (1. - f);
            B = atan(tgB);
            tgB = (z + e12 * b * pow(sin(B), 3)) / (p - e2 * a * pow(cos(B), 3));
        }
        B = atan(tgB);
        L = atan2(y, x);
        N = c / sqrt(1. + e12 * cos(B) * cos(B)); // радиус кривизны первого вертикала
        if (abs(tgB) <= 1.)
            H = p / cos(B) - N;
        else
            H = z / sin(B) - N * (1. - e2);
    }
}


void MainWindow::on_pushButton_clicked()
{
    if (ui->comboBox->currentText() == ui->comboBox2->currentText())
    {
        ui->label_info->setText("Вы пытаетесь сделать то, что уже сделано :)");
    }

    if (ui->comboBox->currentText() == "широта в радианах, долгота в радианах, высота в метрах"
            && ui->comboBox2->currentText() == "широта в градусах-минутах-секундах, долгота в градусах-минутах-секундах, высота в метрах")
    {
        double lat, lon, h, grad, min, sek;
        lat = ui->lineEdit->text().toDouble();
        lon = ui->lineEdit_2->text().toDouble();
        h = ui->lineEdit_3->text().toDouble();

        fromRADtoGradMinSek(lat, grad, min, sek);
        QString latitude_g = QString("%1").arg(grad);
        QString latitude_m = QString("%1").arg(min);
        QString latitude_s = QString("%1").arg(sek);
        fromRADtoGradMinSek(lon, grad, min, sek);
        QString longitude_g = QString("%1").arg(grad);
        QString longitude_m = QString("%1").arg(min);
        QString longitude_s = QString("%1").arg(sek);
        QString height = QString("%1").arg(h);
        ui->lineEdit_4->setText(latitude_g);
        ui->lineEdit_4_2->setText(latitude_m);
        ui->lineEdit_4_3->setText(latitude_s);
        ui->lineEdit_5->setText(longitude_g);
        ui->lineEdit_5_2->setText(longitude_m);
        ui->lineEdit_5_3->setText(longitude_s);
        ui->lineEdit_6->setText(height);
    }

    if (ui->comboBox->currentText() == "широта в градусах-минутах-секундах, долгота в градусах-минутах-секундах, высота в метрах"
            && ui->comboBox2->currentText() == "широта в радианах, долгота в радианах, высота в метрах")
    {
        double lat, lon;
        double latitude_g = ui->lineEdit->text().toDouble();
        double latitude_m = ui->lineEdit_1->text().toDouble();
        double latitude_s = ui->lineEdit_1_1->text().toDouble();
        double longitude_g = ui->lineEdit_2->text().toDouble();
        double longitude_m = ui->lineEdit_2_2->text().toDouble();
        double longitude_s = ui->lineEdit_2_3->text().toDouble();
        QString height = ui->lineEdit_3->text();

        lat = fromGradMinSektoRAD(latitude_g, latitude_m, latitude_s);
        QString latitude = QString("%1").arg(lat);
        ui->lineEdit_4->setText(latitude);

        lon = fromGradMinSektoRAD(longitude_g, longitude_m, longitude_s);
        QString longitude = QString("%1").arg(lon);
        ui->lineEdit_5->setText(longitude);

        ui->lineEdit_6->setText(height);
    }

    if (ui->comboBox->currentText() == "широта в радианах, долгота в радианах, высота в метрах"
            && ui->comboBox2->currentText() == "пространственные прямоугольные координаты")
    {
        double latitude, longitude, height, x, y, z;
        latitude = ui->lineEdit->text().toDouble();
        longitude = ui->lineEdit_2->text().toDouble();
        height = ui->lineEdit_3->text().toDouble();

        fromBLHtoXYZ(latitude, longitude, height, x, y, z);
        QString X, Y, Z;
        X = QString("%1").arg(x);
        Y = QString("%1").arg(y);
        Z = QString("%1").arg(z);
        ui->lineEdit_4->setText(X);
        ui->lineEdit_5->setText(Y);
        ui->lineEdit_6->setText(Z);
    }

    if (ui->comboBox->currentText() == "широта в градусах-минутах-секундах, долгота в градусах-минутах-секундах, высота в метрах"
            && ui->comboBox2->currentText() == "пространственные прямоугольные координаты")
    {
        double x, y, z, lat, lon;
        double latitude_g = ui->lineEdit->text().toDouble();
        double latitude_m = ui->lineEdit_1->text().toDouble();
        double latitude_s = ui->lineEdit_1_1->text().toDouble();
        double longitude_g = ui->lineEdit_2->text().toDouble();
        double longitude_m = ui->lineEdit_2_2->text().toDouble();
        double longitude_s = ui->lineEdit_2_3->text().toDouble();
        double height = ui->lineEdit_3->text().toDouble();

        lat = fromGradMinSektoRAD(latitude_g, latitude_m, latitude_s);
        lon = fromGradMinSektoRAD(longitude_g, longitude_m, longitude_s);
        fromBLHtoXYZ(lat, lon, height, x, y, z);
        QString X, Y, Z;
        X = QString("%1").arg(x);
        Y = QString("%1").arg(y);
        Z = QString("%1").arg(z);
        ui->lineEdit_4->setText(X);
        ui->lineEdit_5->setText(Y);
        ui->lineEdit_6->setText(Z);
    }

    if (ui->comboBox->currentText() == "пространственные прямоугольные координаты"
            && ui->comboBox2->currentText() == "широта в радианах, долгота в радианах, высота в метрах")
    {
        double x, y, z, lat, lon, h;
        x = ui->lineEdit->text().toDouble();
        y = ui->lineEdit_2->text().toDouble();
        z = ui->lineEdit_3->text().toDouble();

        fromXYZtoBLH(x, y, z, lat, lon, h);
        QString latitude, longitude, height;
        latitude = QString("%1").arg(lat);
        longitude = QString("%1").arg(lon);
        height = QString("%1").arg(h);
        ui->lineEdit_4->setText(latitude);
        ui->lineEdit_5->setText(longitude);
        ui->lineEdit_6->setText(height);
    }

    if (ui->comboBox->currentText() == "пространственные прямоугольные координаты"
            && ui->comboBox2->currentText() == "широта в градусах-минутах-секундах, долгота в градусах-минутах-секундах, высота в метрах")
    {
        double x, y, z, lat, lon, h, grad, min, sek;
        x = ui->lineEdit->text().toDouble();
        y = ui->lineEdit_2->text().toDouble();
        z = ui->lineEdit_3->text().toDouble();

        fromXYZtoBLH(x, y, z, lat, lon, h);

        fromRADtoGradMinSek(lat, grad, min, sek);
        QString latitude_g = QString("%1").arg(grad);
        QString latitude_m = QString("%1").arg(min);
        QString latitude_s = QString("%1").arg(sek);
        fromRADtoGradMinSek(lon, grad, min, sek);
        QString longitude_g = QString("%1").arg(grad);
        QString longitude_m = QString("%1").arg(min);
        QString longitude_s = QString("%1").arg(sek);
        QString height = QString("%1").arg(h);
        ui->lineEdit_4->setText(latitude_g);
        ui->lineEdit_4_2->setText(latitude_m);
        ui->lineEdit_4_3->setText(latitude_s);
        ui->lineEdit_5->setText(longitude_g);
        ui->lineEdit_5_2->setText(longitude_m);
        ui->lineEdit_5_3->setText(longitude_s);
        ui->lineEdit_6->setText(height);
    }
}


void MainWindow::on_comboBox2_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0:
            ui->label_unit4_2->setHidden(false);
            ui->label_unit4_3->setHidden(false);
            ui->label_unit5_2->setHidden(false);
            ui->label_unit5_3->setHidden(false);
            ui->lineEdit_4_2->setHidden(false);
            ui->lineEdit_4_3->setHidden(false);
            ui->lineEdit_5_2->setHidden(false);
            ui->lineEdit_5_3->setHidden(false);
            ui->label_unit4->setText("град");
            ui->label_unit4_2->setText("мин");
            ui->label_unit4_3->setText("сек");
            ui->label_unit5->setText("град");
            ui->label_unit5_2->setText("мин");
            ui->label_unit5_3->setText("сек");
            ui->label_unit6->setText("метров");
            ui->label_name4->setText("ШИРОТА");
            ui->label_name5->setText("ДОЛГОТА");
            ui->label_name6->setText("ВЫСОТА");
            break;
        case 1:
            ui->label_unit4_2->setHidden(true);
            ui->label_unit4_3->setHidden(true);
            ui->label_unit5_2->setHidden(true);
            ui->label_unit5_3->setHidden(true);
            ui->lineEdit_4_2->setHidden(true);
            ui->lineEdit_4_3->setHidden(true);
            ui->lineEdit_5_2->setHidden(true);
            ui->lineEdit_5_3->setHidden(true);
            ui->label_unit4->setText("радиан");
            ui->label_unit5->setText("радиан");
            ui->label_unit6->setText("метров");
            ui->label_name4->setText("ШИРОТА");
            ui->label_name5->setText("ДОЛГОТА");
            ui->label_name6->setText("ВЫСОТА");
            break;
        case 2:
            ui->label_unit4_2->setHidden(true);
            ui->label_unit4_3->setHidden(true);
            ui->label_unit5_2->setHidden(true);
            ui->label_unit5_3->setHidden(true);
            ui->lineEdit_4_2->setHidden(true);
            ui->lineEdit_4_3->setHidden(true);
            ui->lineEdit_5_2->setHidden(true);
            ui->lineEdit_5_3->setHidden(true);
            ui->label_unit4->setText("метров");
            ui->label_unit5->setText("метров");
            ui->label_unit6->setText("метров");
            ui->label_name4->setText("ОСЬ X");
            ui->label_name5->setText("ОСЬ Y");
            ui->label_name6->setText("ОСЬ Z");
            break;
    }
}


void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0:
            ui->label_unit1_2->setHidden(true);
            ui->label_unit1_3->setHidden(true);
            ui->label_unit2_2->setHidden(true);
            ui->label_unit2_3->setHidden(true);
            ui->lineEdit_1->setHidden(true);
            ui->lineEdit_1_1->setHidden(true);
            ui->lineEdit_2_2->setHidden(true);
            ui->lineEdit_2_3->setHidden(true);
            ui->label_unit1->setText("радиан");
            ui->label_unit2->setText("радиан");
            ui->label_unit3->setText("метров");
            ui->label_name1->setText("ШИРОТА");
            ui->label_name2->setText("ДОЛГОТА");
            ui->label_name3->setText("ВЫСОТА");
            ui->label_info->setText("");
            break;
        case 1:
            ui->label_unit1_2->setHidden(false);
            ui->label_unit1_3->setHidden(false);
            ui->label_unit2_2->setHidden(false);
            ui->label_unit2_3->setHidden(false);
            ui->lineEdit_1->setHidden(false);
            ui->lineEdit_1_1->setHidden(false);
            ui->lineEdit_2_2->setHidden(false);
            ui->lineEdit_2_3->setHidden(false);
            ui->label_unit1->setText("град");
            ui->label_unit1_2->setText("мин");
            ui->label_unit1_3->setText("сек");
            ui->label_unit2->setText("град");
            ui->label_unit2_2->setText("мин");
            ui->label_unit2_3->setText("сек");
            ui->label_unit3->setText("метров");
            ui->label_name1->setText("ШИРОТА");
            ui->label_name2->setText("ДОЛГОТА");
            ui->label_name3->setText("ВЫСОТА");
            ui->label_info->setText("ВНИМАНИЕ! Если хотите задать отрицательный угол, то нужно ставть минус ВЕЗДЕ: и в ГРАД, и в МИН, и в СЕК"); // курсивом бы
            break;
        case 2:
            ui->label_unit1_2->setHidden(true);
            ui->label_unit1_3->setHidden(true);
            ui->label_unit2_2->setHidden(true);
            ui->label_unit2_3->setHidden(true);
            ui->lineEdit_1->setHidden(true);
            ui->lineEdit_1_1->setHidden(true);
            ui->lineEdit_2_2->setHidden(true);
            ui->lineEdit_2_3->setHidden(true);
            ui->label_unit1->setText("метров");
            ui->label_unit2->setText("метров");
            ui->label_unit3->setText("метров");
            ui->label_name1->setText("ОСЬ X");
            ui->label_name2->setText("ОСЬ Y");
            ui->label_name3->setText("ОСЬ Z");
            ui->label_info->setText("");
            break;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QMessageBox::information(this, "Теория", "ГЕОДЕЗИЧЕСКАЯ ПРЯМОУГОЛЬНАЯ СИСТЕМА КООРДИНАТ\n    В геодезии используют прямоугольную систему координат, начало О которой находится в центре масс Земли, ось Z направлена по оси вращения Земли, ось Х совмещена с линией пересечения плоскостей экватора и начального (гринвичского) меридиана, ось Y дополняет систему до право. Такую систему координат называют геоцентрической или общеземной. В общеземной системе координат определяют положение пунктов на всей поверхности Земли. К таким можно отнести WGS-84, GRS80, ПЗ-90 (у меня в рассчетах геометрические данные Земли взяты согласно WGS-84).\n\nГЕОДЕЗИЧЕСКАЯ ЭЛЛИПСОИДАЛЬНАЯ СИСТЕМА КООРДИНАТ\n    Геодезическая эллипсоидальная система координат B,L,H связана с эллипсоидом. Координатными линиями в этой системе являются нормали к эллипсоиду. Геодезическая широта В — это угол между нормалью РРоОр к эллипсоиду и плоскостью экватора. Геодезическая долгота L — угол между плоскостью Y=0 начального меридиана и плоскостью ZOP меридиана точки Р. Геодезическая высота H — отрезок РоР нормали к эллипсоиду.\n\nПодробнее о системах координат и о переходе от одной к другой:    https://ru.wikipedia.org/wiki/Геодезическая_система_координат");
}

void MainWindow::on_pushButton_3_clicked()
{
    QMessageBox::information(this, "Справка по использованию", "ОБЩИЕ СВЕДЕНИЯ (какие данные вводить?)\nШироту принято отсчитывать от экватора на север. Таким образом, широта точек, лежащих в северном полушарии, положительна, а в южном — отрицательна. Широта любой точки экватора равна 0°, северного полюса — +90°, южного полюса — −90°.\n\nВ соответствии с международным соглашением, меридиану, который проходит через Гринвичскую обсерваторию (Лондон, Великобритания) было присвоено значение 0° долготы, иными словами, он был избран в качестве точки отсчёта долготы на земном шаре. Долгота других мест измеряется как угол на восток или запад от нулевого меридиана, в диапазоне от 0° до +180° на восток и от 0° до −180° на запад.\n\nВысота - кратчайшее расстояние до поверхности эллипсоида (грубо говоря, высота над/под уровнем моря).\n\nИНСТРУКЦИЯ ПО ПРИМЕНЕНИЮ\n\n1) Выберете начальный и конечный форматы координат\n\n2) Введите значения в соответствующие поля\nВАЖНО: если Вы выбрали начальный формат 'градусы-минуты-секунды', то при указании отрицательного значения широты/долготы знак '-' нужно прописывать в КАЖДОМ поле:\nГРАДУСЫ, МИНУТЫ, СЕКУНДЫ\n\n 3) Нажмите на кнупку РЕЗУЛЬТАТ\n\n 4) Краткая теоретическая справка появится при нажатии кнопки ТЕОРИЯ");
}
