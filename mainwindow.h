#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "alarmlist.h"
#include "world.h"
#include <time.h>
#include <alarmd/alarm_dbus.h>
#include <alarmd/libalarm.h>
#include <QListWidgetItem>
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    AlarmList *sw;
    World * ww;
    bool SecondsAdded;
    bool HH24true;

private:
    Ui::MainWindow *ui;
    QTimer *timer;

private slots:
    void on_action_cloc_alarm_settings_title_triggered();
    void on_action_cloc_me_menu_settings_regional_triggered();
    void on_action_dati_ia_adjust_date_and_time_triggered();
    void on_action_sfil_va_number_of_objects_images_triggered();
    void on_action_disp_seconds_triggered();
    void updateTime();
    void loadAlarm();
    void loadWorld();
    void on_listWidget_itemActivated(QListWidgetItem* item);
    void on_nwAlarm_pushButton_released();
    void on_nwAlarm_pushButton_pressed();
    void on_timeButton_landscape_released();
    void on_timeButton_portrait_released();
    void on_wrldClk_pushButton_pressed();
    void on_wrldClk_pushButton_released();
    void on_Alarm_pushButton_pressed();
    void on_Alarm_pushButton_released();
    void orientationChanged();
    void getAMPM();
};

#endif // MAINWINDOW_H
