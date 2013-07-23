#include "dialogdate.h"
#include "ui_dialogdate.h"
#include "osso-intl.h"

static const char *getHildonTranslation(const char *string)
{
     setlocale (LC_ALL, "");
     const char *translation = ::dgettext("hildon-libs", string);
     if (qstrcmp(string, translation) == 0)
         return 0;
     return translation;
}

const char *hildonDateDayName_Shrt = getHildonTranslation("wdgt_va_date_day_name_short");
// const char *hildonDateLng = getHildonTranslation("wdgt_va_date_long");

static QString formatHildonDate(const QDateTime &dt, const char *format)
{
     if (!format)
         return QString();

     char buf[255];
     struct tm tm = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

     if (!dt.date().isNull()) {
         tm.tm_wday = dt.date().dayOfWeek() % 7;
         tm.tm_mday = dt.date().day();
         tm.tm_mon = dt.date().month() - 1;
         tm.tm_year = dt.date().year() - 1900;
     }
     if (!dt.time().isNull()) {
         tm.tm_sec = dt.time().second();
         tm.tm_min = dt.time().minute();
         tm.tm_hour = dt.time().hour();
     }

     size_t resultSize = ::strftime(buf, sizeof(buf), format, &tm);
     if (!resultSize)
         return QString();

     return QString::fromUtf8(buf, resultSize);
}

DialogDate::DialogDate(QWidget *parent, QString displayDay, QString displayMonth, QString displayYear) :
    QDialog(parent),
    ui(new Ui::DialogDate)
{
    ui->setupUi(this);
    this->setWindowTitle(_("dati_fi_pr_date"));
    this->setAttribute(Qt::WA_Maemo5AutoOrientation, true);

    int dayPos = 1;
    int monthPos = 1;
    int yearPos = 1;

    for (int i=1; i<32; ++i)
    {
        QListWidgetItem *item1 = new QListWidgetItem(ui->listWidget_day);
        QString num = QString::number(i);
        item1->setText(num);
	if ( num == displayDay )
	    dayPos = i - 1;
        if ( num.length() == 1 )
            num = "0" + num;
        item1->setText(num);
        item1->setTextAlignment(Qt::AlignCenter);
        ui->listWidget_day->addItem(item1);
    }


    for (int i=1; i<13; ++i)
    {
        QListWidgetItem *monthLong = new QListWidgetItem(ui->listWidget_month);
        QString num = QString::number(i);
	QString monthName = QDate::fromString(num,"M").toString(Qt::DefaultLocaleLongDate);
	// remove "de" in spanish and "den" and ":e" in swedish
	monthName.replace(QRegExp("\\sde[n]?\\s")," ");
	monthName.remove(QRegExp(":e"));
	//QString monthName = formatHildonDate(QDateTime::fromString(num,"MM"), hildonDateLng);
        QStringList sl = monthName.remove(QRegExp("(\\,|\\.|\\d+)")).split(' ', QString::SkipEmptyParts);
        monthName = sl.at(1);
	//QTextStream(stdout) << "monthname_after = " << monthName;
        monthLong->setText(monthName);
        monthLong->setTextAlignment(Qt::AlignCenter);
        ui->listWidget_month->addItem(monthLong);
	if ( monthName == displayMonth )
	    monthPos = i - 1;
    }

    int curYear = QDate::currentDate().year();

    for (int i=curYear; i<2038; ++i)
    {
        QListWidgetItem *yearfull = new QListWidgetItem(ui->listWidget_year);
        QString num = QString::number(i);
        yearfull->setText(num);
        yearfull->setTextAlignment(Qt::AlignCenter);
        ui->listWidget_year->addItem(yearfull);
	if ( num == displayYear )
	    yearPos = i - curYear;
    }

    // set day on pickfield
    ui->listWidget_day->item(dayPos)->setSelected(true);
    ui->listWidget_day->setCurrentRow(dayPos);
    ui->listWidget_day->scrollToItem(ui->listWidget_day->item(dayPos), QAbstractItemView::PositionAtTop);
    // set month on pickfield
    ui->listWidget_month->item(monthPos)->setSelected(true);
    ui->listWidget_month->setCurrentRow(monthPos);
    ui->listWidget_month->scrollToItem(ui->listWidget_month->item(monthPos), QAbstractItemView::PositionAtTop);
    // set year on pickfield
    ui->listWidget_year->item(yearPos)->setSelected(true);
    ui->listWidget_year->setCurrentRow(yearPos);
    ui->listWidget_year->scrollToItem(ui->listWidget_year->item(yearPos), QAbstractItemView::PositionAtTop);
    on_listWidget_month_itemClicked();

    intl("osso-connectivity-ui");
    ui->buttonBox->button(QDialogButtonBox::Save)->setText(_("conn_iaps_bd_done"));
    ui->buttonBox_2->button(QDialogButtonBox::Save)->setText(_("conn_iaps_bd_done"));
    intl("osso-clock");

    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(orientationChanged()));
    this->orientationChanged();

}

DialogDate::~DialogDate()
{
    delete ui;
}

void DialogDate::orientationChanged()
{
    if (QApplication::desktop()->screenGeometry().width() < QApplication::desktop()->screenGeometry().height()) {
	// portrait
        ui->buttonBox->hide();
        ui->buttonBox_2->show();
        this->setMinimumHeight(680);
        this->setMaximumHeight(680);
        ui->listWidget_day->scrollToItem(ui->listWidget_day->currentItem(), QAbstractItemView::PositionAtCenter);
        ui->listWidget_month->scrollToItem(ui->listWidget_month->currentItem(), QAbstractItemView::PositionAtCenter);
        ui->listWidget_year->scrollToItem(ui->listWidget_year->currentItem(), QAbstractItemView::PositionAtCenter);
    } else {
        // landscape
        ui->buttonBox_2->hide();
        ui->buttonBox->show();
        this->setMinimumHeight(350);
        this->setMaximumHeight(350);
        ui->listWidget_day->scrollToItem(ui->listWidget_day->currentItem(), QAbstractItemView::PositionAtTop);
        ui->listWidget_month->scrollToItem(ui->listWidget_month->currentItem(), QAbstractItemView::PositionAtTop);
        ui->listWidget_year->scrollToItem(ui->listWidget_year->currentItem(), QAbstractItemView::PositionAtTop);
    }
}

void DialogDate::on_listWidget_day_itemClicked()
{
    int dayNow = QDate::currentDate().day();
    int monthNow = QDate::currentDate().month();
    int yearNow = QDate::currentDate().year();
    if (ui->listWidget_month->currentRow()+1 < monthNow && yearNow == ui->listWidget_year->currentRow()+yearNow \
        || (ui->listWidget_day->currentRow()+1 < dayNow && ui->listWidget_month->currentRow()+1 == monthNow \
        && yearNow == ui->listWidget_year->currentRow()+yearNow ))
    {
	    ui->buttonBox->setEnabled(false);
	    ui->buttonBox_2->setEnabled(false);
    }
    else
    {
	    ui->buttonBox->setEnabled(true);
	    ui->buttonBox_2->setEnabled(true);
    }
}

void DialogDate::on_listWidget_month_itemClicked()
{
    monthres = ui->listWidget_month->currentRow() + 1;
    int sel_year = ui->listWidget_year->currentRow() + QDate::currentDate().year();
    switch (monthres)
    {
       // months with 30 days
       case 4: case 6: case 9: case 11: ui->listWidget_day->takeItem(30);
               for (int i=ui->listWidget_day->count();i<30; ++i)
	       {
                      QListWidgetItem *item1 = new QListWidgetItem(ui->listWidget_day);
                      QString num = QString::number(i+1);
                      item1->setText(num);
                      item1->setTextAlignment(Qt::AlignCenter);
                      ui->listWidget_day->addItem(item1);
               }
               break;
       // february
       case 2: ui->listWidget_day->takeItem(30);
               ui->listWidget_day->takeItem(29);
               for (int i=ui->listWidget_day->count();i<29; ++i)
	       {
                      QListWidgetItem *item1 = new QListWidgetItem(ui->listWidget_day);
                      QString num = QString::number(i+1);
                      item1->setText(num);
                      item1->setTextAlignment(Qt::AlignCenter);
                      ui->listWidget_day->addItem(item1);
               }
               if (!QDate::isLeapYear(sel_year))
                   ui->listWidget_day->takeItem(28);
               break;
       // other months (31 days)
       default:
               for (int i=ui->listWidget_day->count();i<31; ++i)
	       {
                      QListWidgetItem *item1 = new QListWidgetItem(ui->listWidget_day);
                      QString num = QString::number(i+1);
                      item1->setText(num);
                      item1->setTextAlignment(Qt::AlignCenter);
                      ui->listWidget_day->addItem(item1);
               }
    }
    // disable done button if date is in past
    int dayNow = QDate::currentDate().day();
    int monthNow = QDate::currentDate().month();
    int yearNow = QDate::currentDate().year();
    if (ui->listWidget_month->currentRow()+1 < monthNow && yearNow == ui->listWidget_year->currentRow()+yearNow \
        || (ui->listWidget_day->currentRow()+1 < dayNow && ui->listWidget_month->currentRow()+1 == monthNow \
        && yearNow == ui->listWidget_year->currentRow()+yearNow ))
    {
	    ui->buttonBox->setEnabled(false);
	    ui->buttonBox_2->setEnabled(false);
    }
    else
    {
	    ui->buttonBox->setEnabled(true);
	    ui->buttonBox_2->setEnabled(true);
    }
}

//void DialogDate::on_listWidget_month_currentIndexChanged();
//{

void DialogDate::on_listWidget_year_itemClicked()
{
   on_listWidget_month_itemClicked();
}

void DialogDate::on_buttonBox_accepted()
{
    int curYear = QDate::currentDate().year();
    dayres = ui->listWidget_day->currentRow() + 1;
    monthres = ui->listWidget_month->currentRow() + 1;
    yearres = ui->listWidget_year->currentRow() + curYear;
    this->accept();
}

void DialogDate::on_buttonBox_2_accepted()
{
    on_buttonBox_accepted();
}
