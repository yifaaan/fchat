#ifndef TIMER_BUTTON_H
#define TIMER_BUTTON_H

#include <QPushButton>
#include <QTimer>

class TimerButton : public QPushButton
{
public:
    TimerButton(QWidget* parent = nullptr);
    ~TimerButton();
    void mouseReleaseEvent(QMouseEvent *e) override;


private:
    QTimer* timer_;
    int counter_;
};

#endif // TIMER_BUTTON_H
