#include <QDateTime>
#include <QFileInfo>
#include <QDesktopWidget>

#include <osso-intl.h>
#include <libalarm.h>
#include <gconfitem.h>

#include "qnewalarmdialog.h"
#include "ui_newalarmdialog.h"
#include "alarmsndpick.h"
#include "utils.h"

/* osso-clock text needed for snooze/stop button translation */
#define TEXTDOMAIN "osso-clock"
#define STOP_BUTTON_KEY "cloc_bd_stop"
#define SNOOZE_BUTTON_KEY "cloc_bd_alarm_notification_snooze"

QString sndFile;
bool dateChoosen = false;
bool fromEdit = false;

QNewAlarmDialog::QNewAlarmDialog(QWidget *parent, bool edit, QString Aname,
                   const QTime &time, uint32_t wday, bool Acheck, long Acook, bool show):

    QDialog(parent),
    ui(new Ui::QNewAlarmDialog)
{
    this->setAttribute(Qt::WA_Maemo5AutoOrientation, true);
    ui->setupUi(this);

    // get the current default alarm tone
    GConfItem *item = new GConfItem("/apps/clock/alarm-tone");

    sndFile = item->value().toString();
    ui->soundButton->setEnabled(false);  // <- remove once custom sounds work!
    ui->soundButton->setSoundFile(sndFile);

    isEditing = edit;
    deleted = 1;

    realcookie = Acook;
    this->time = time;
    name = Aname;
    this->wday = wday;
    enabled = Acheck;
    showed = show;

    if (edit)
    {
        this->setWindowTitle(_("cloc_ti_edit_alarm_title"));
        // we need to find it's current alarm tone
        alarm_event_t *eve = 0;
        eve = alarmd_event_get(realcookie);
        QString aSnd;

        aSnd = QString::fromUtf8(eve->sound);

        if (!aSnd.isEmpty()) // a tone is set
        {
            sndFile = aSnd;

            /* why do we need that ? */
            aSnd.remove(QRegExp("\\.ogg$|\\.mp3$|\\.aac$"));
            aSnd = sndFile;
            ui->soundButton->setSoundFile(sndFile);
        }

        // cleanup memory
        alarm_event_delete(eve);
        sndFile = aSnd;
    }
    else
        this->setWindowTitle(_("clock_ti_new_alarm"));

    ui->titleEdit->setPlaceholderText(_("cloc_va_placeholder_title"));
    ui->daysButton->setDays(wday);
    ui->checkBox->setText(_("cloc_fi_active"));

    intl("osso-connectivity-ui");
    ui->landscapeButtonBox->button(QDialogButtonBox::Retry)->setText(_("conn_bd_devices_delete"));
    ui->landscapeButtonBox->button(QDialogButtonBox::Apply)->setText(_("conn_bd_receive_ok"));
    ui->portraitButtonBox->button(QDialogButtonBox::Retry)->setText(_("conn_bd_devices_delete"));
    ui->portraitButtonBox->button(QDialogButtonBox::Apply)->setText(_("conn_bd_receive_ok"));
    intl("osso-clock");
    ui->landscapeButtonBox->button(QDialogButtonBox::Retry)->setVisible(edit);
    ui->portraitButtonBox->button(QDialogButtonBox::Retry)->setVisible(edit);

    // set alarmname
    if (!name.isEmpty())
        ui->titleEdit->setText(name);

    // set time
    ui->timeButton->setCurrentTime(time);

    time_t nextDate;
    if (realcookie && !wday)
    {
        // get date&time alarm
        alarm_event_t *ae = alarmd_event_get(realcookie);

        if (ae->snooze_total)
            nextDate = ae->snooze_total;
        else if (ae->trigger != -1)
            nextDate = ae->trigger;
        else if (ae->alarm_time != -1)
            nextDate = ae->alarm_time;
        else
            nextDate = QDateTime::currentDateTime().toTime_t();

        if (nextDate < (time_t)QDateTime::currentDateTime().toTime_t())
        {
            /* we can't enable an alarm that is in the past */
            ui->checkBox->setEnabled(false);
            ui->landscapeButtonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
            ui->portraitButtonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
        }

        alarm_event_delete(ae);
    }
    else
        nextDate = QDateTime::currentDateTime().addDays(1).toTime_t();

    ui->dateButton->setCurrentDate(nextDate);
    ui->daysButton->setDays(wday);

    if (wday)
        ui->dateButton->hide();

    if ( !isEditing )
        ui->checkBox->hide();

    connect(QApplication::desktop(), SIGNAL(resized(int)),
            this, SLOT(orientationChanged()));
    orientationChanged();
}

QNewAlarmDialog::~QNewAlarmDialog()
{
    delete ui;
}

void QNewAlarmDialog::orientationChanged()
{
    int space = 0;
    if ( isEditing )
        space = 76;

    int startspace_portrait = 460;
    int startspace_landscape = 380;
    if ( ui->dateButton->isHidden() )
    {
        startspace_portrait = 385;
        startspace_landscape = 305;
    }

    if (QApplication::desktop()->screenGeometry().width() < QApplication::desktop()->screenGeometry().height()) {
	// portrait
        ui->landscapeButtonBox->hide();
        ui->portraitButtonBox->show();
       	this->setMinimumHeight(startspace_portrait+space+space);
	this->setMaximumHeight(startspace_portrait+space+space);
    } else {
	// landscape
        ui->portraitButtonBox->hide();
        ui->landscapeButtonBox->show();
        this->setMinimumHeight(startspace_landscape+space);
        this->setMaximumHeight(startspace_landscape+space);
    }

    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);

}

void QNewAlarmDialog::on_timeButton_selected(const QTime &time)
{
    // The long date string
    if (!dateChoosen && wday)
    {
        // show if the alarm is still today or else tomorrow
        time_t date;

        if (time >= QTime::currentTime())
            date = QDateTime::currentDateTime().toTime_t();
        else
            date = QDateTime::currentDateTime().addDays(1).toTime_t();

        ui->dateButton->setCurrentDate(date);
    }
    else
    {
        QDateTime pickedDate;

        pickedDate.setDate(ui->dateButton->currentDate());
        pickedDate.setTime(time);

        if (pickedDate < QDateTime::currentDateTime())
        {
            // in the past
            ui->checkBox->setEnabled(false);
            ui->landscapeButtonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
            ui->portraitButtonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
        }
        else
        {
            ui->checkBox->setEnabled(true);
            ui->landscapeButtonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
            ui->portraitButtonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
        }
    }
}

void QNewAlarmDialog::on_dateButton_selected(const QDate &date)
{
    dateChoosen = true;
    ui->checkBox->setEnabled(true);
    enabled = true;
    ui->checkBox->setChecked(enabled);

    QDateTime pickedDateTime;
    pickedDateTime.setTime(ui->timeButton->currentTime());
    pickedDateTime.setDate(date);
    if (pickedDateTime < QDateTime::currentDateTime())
    {
        // in the past
        ui->checkBox->setEnabled(false);
        ui->landscapeButtonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
        ui->portraitButtonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    }
    else
    {
        ui->checkBox->setEnabled(true);
        ui->landscapeButtonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
        ui->portraitButtonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
    }
}

void QNewAlarmDialog::on_daysButton_selected(uint32_t days)
{
    wday = days;

    if (days)
    {
        // a repeating alarm; "active checkbox" and "done" can always be enabled
        ui->checkBox->setEnabled(true);
        ui->landscapeButtonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
        ui->portraitButtonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
        if ( !ui->dateButton->isHidden() )
        {
            ui->dateButton->hide();
            this->setMinimumHeight(this->height() - 75);
            this->setMaximumHeight(this->height() - 75);
        }
    }
    else
    {
        if ( ui->dateButton->isHidden() )
        {
            this->setMinimumHeight(this->height() + 75);
            this->setMaximumHeight(this->height() + 75);
            ui->dateButton->show();
        }
    }

    // we have a change, so make active by default
    ui->checkBox->setChecked(true);
}

void QNewAlarmDialog::on_soundButton_pressed()
{
    AlarmSndPick *hw = new AlarmSndPick(this);

    hw->exec();

    if (hw->selected != "nothing")
    {
           sndFile = hw->selected;
           ui->soundButton->setSoundFile(sndFile);
    }

    delete hw;
}

void QNewAlarmDialog::on_landscapeButtonBox_clicked(QAbstractButton* button)
{
    intl("osso-connectivity-ui");

    if ( button->text() == _("conn_bd_devices_delete") )
    {
	// really remove alarm
        deleted = 1;
        removeAlarm(realcookie);
    }
    else
    {
        deleted = 0;

        if ( realcookie != 0  )
            alarmd_event_del(realcookie); // remove cookie if exists

        addAlarm();
    }

    intl("osso-clock");
    this->accept();

}

void QNewAlarmDialog::on_portraitButtonBox_clicked(QAbstractButton* button)
{
    on_landscapeButtonBox_clicked(button);
}

void QNewAlarmDialog::removeAlarm(long cookie)
{
    alarmd_event_del(cookie);
}

void QNewAlarmDialog::addAlarm()
{

    intl("osso-clock");

    name = ui->titleEdit->text();

    time = ui->timeButton->currentTime();
    enabled = ui->checkBox->isChecked();

    alarm_event_t * event = 0;
    alarm_action_t * act = 0;
    // Setup
    event = alarm_event_create(); // a new alarm
    // The application id field is needed for every alarm you set
    alarm_event_set_alarm_appid(event, "worldclock_alarmd_id");
    // Alarm description displayed when the alarm is triggered
    QByteArray ba = name.toUtf8();
    const char *str1 = ba.data();
    alarm_event_set_message(event, str1);
    /* Set the textdomain to be used while making
     * translation lookups */
    alarm_event_set_attr_string(event, "textdomain", TEXTDOMAIN);
    /* Set the title of the alarm. */
    //alarm_event_set_title(event, "Alarm"); "cloc_ti_alarm_notification_title"

    alarm_event_set_alarm_appid(event,"worldclock_alarmd_id");

    // Snooze action
    act = alarm_event_add_actions(event, 1);
    alarm_action_set_label(act, SNOOZE_BUTTON_KEY);
    act->flags = ALARM_ACTION_WHEN_RESPONDED;
    act->flags |= ALARM_ACTION_TYPE_SNOOZE;

    if ( !enabled )
        event->flags |= ALARM_EVENT_DISABLED;

    event->flags |= ALARM_EVENT_ACTDEAD;
    event->flags |= ALARM_EVENT_SHOW_ICON;
    event->flags |= ALARM_EVENT_POSTPONE_DELAYED;

    intl("osso-clock");

    if (wday)
        event->flags |= ALARM_EVENT_BACK_RESCHEDULE;

    // Acknowledge action
    act = alarm_event_add_actions(event, 1);
    alarm_action_set_label(act, STOP_BUTTON_KEY);

    if(!wday)
    {
       act->flags |= ALARM_ACTION_WHEN_RESPONDED;
       act->flags |= ALARM_ACTION_TYPE_DISABLE;
    } else {
       act->flags = ALARM_ACTION_WHEN_RESPONDED;
       act->flags |= ALARM_ACTION_TYPE_NOP;
    }

    QDateTime alarmDate;
    alarmDate.setTime(ui->timeButton->currentTime());

    if ( ! ui->dateButton->isHidden() && (dateChoosen || !wday) )
    {

        alarmDate.setDate(ui->dateButton->currentDate());
        event->alarm_time = alarmDate.toTime_t();
        event->alarm_tm.tm_hour = -1;
        event->alarm_tm.tm_min = -1;
    }

    event->alarm_time = alarmDate.toTime_t(); //needed? looks like it...

    if( !wday ) {
        if (! dateChoosen) {
            event->alarm_time = -1;
            event->alarm_tm.tm_hour = alarmDate.time().hour();
            event->alarm_tm.tm_min = alarmDate.time().minute();
        }
    }
    else
    {
        //qDebug("Using the new recurrence API");
        event->recur_count = -1;
        event->recur_secs = 0; // We re not using this way for recurrence
        alarm_recur_t* recur = alarm_event_add_recurrences(event, 1);
        // Set event time
        recur->special = ALARM_RECUR_SPECIAL_NONE;
        recur->mask_mon = ALARM_RECUR_MON_ALL;
        recur->mask_mday = ALARM_RECUR_MDAY_ALL;
        recur->mask_hour = (1ul << alarmDate.time().hour());
        recur->mask_min = (1ull << alarmDate.time().minute());

        recur->mask_wday = wday;
        Q_ASSERT(event->recurrence_cnt == 1);
    }

    // set the alarm signal
    if (sndFile.isEmpty())
    {
	// noting changed and not a default tone: so get the custom one
    	GConfItem *item = new GConfItem("/apps/clock/alarm-custom");
    	sndFile = item->value().toString();
    }

    alarm_event_set_sound(event, sndFile.toUtf8().constData());
    realcookie = alarmd_event_add(event); // add alarm
    // cleanup
    alarm_event_delete(event);

    event = alarmd_event_get(realcookie);

    if (enabled)
        showAlarmTimeBanner(event->trigger);

    alarm_event_delete(event);

    dateChoosen = false;
    act = 0;
    event = 0;
}