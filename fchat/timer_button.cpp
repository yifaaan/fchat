#include "timer_button.h"

#include <QMouseEvent>

TimerButton::TimerButton(QWidget* parent)
    : QPushButton{parent}, timer_{new QTimer{this}}, counter_{10}
{
    connect(timer_, &QTimer::timeout, [this]{
        counter_--;
        if (counter_ <= 0)
        {
            timer_->stop();
            counter_ = 10;
            this->setText(tr("获取"));
            this->setEnabled(true);
            return;
        }
        this->setText(QString::number(counter_));
    });
}

TimerButton::~TimerButton()
{
    timer_->stop();
}

void TimerButton::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        this->setEnabled(false);
        this->setText(QString::number(counter_));
        timer_->start(1000);
    }
    emit clicked();
    QPushButton::mouseReleaseEvent(e);
}
