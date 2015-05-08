#ifndef QALARMTIMEVALUEBUTTON_H
#define QALARMTIMEVALUEBUTTON_H

#include "qalarmvaluebutton.h"
#include <QMaemo5TimePickSelector>

class QAlarmTimeValueButton : public QAlarmValueButton
{
    Q_OBJECT
public:
    explicit QAlarmTimeValueButton(QWidget *parent = 0);
    QTime currentTime() const {
        return selector->currentTime();
    }
    void setCurrentTime(const QTime &time) {
        selector->setCurrentTime(time);
    }

signals:
    void selected(const QTime &time);
public slots:
    void timeSelected(QString);
protected:
    QMaemo5TimePickSelector *selector;
};

#endif // QALARMTIMEVALUEBUTTON_H
