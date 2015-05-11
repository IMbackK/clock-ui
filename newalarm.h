#ifndef NEWALARM_H
#define NEWALARM_H

#include <QDialog>
#include <QDateTime>
#include <QAbstractButton>

namespace Ui {
    class NewAlarm;
}

class NewAlarm : public QDialog
{
    Q_OBJECT

public:
    explicit NewAlarm(QWidget *parent = 0, bool edit = false,
                      QString Aname = "", const QTime &time = QTime(),
                      uint32_t wday = 0,
                      bool Acheck = false, long Acook = 0, bool show = true);
    ~NewAlarm();
    bool isEditing;
    long realcookie;
    bool enabled;
    QString name;
    QTime time;
    uint32_t wday;
    int deleted;
    bool showed;

private:
    Ui::NewAlarm *ui;

public slots:
    void removeAlarm(long cookie);
    void addAlarm();

private slots:
    void on_buttonBox_2_clicked(QAbstractButton* button);
    void orientationChanged();
    void on_buttonBox_clicked(QAbstractButton* button);
    void on_alarmTimeButton_selected(const QTime &time);
    void on_date_pushButton_selected(const QDate &date);
    void on_repeat_pushButton_selected(uint32_t days);
    void on_alsound_pushButton_pressed();
};

#endif // NEWALARM_H
