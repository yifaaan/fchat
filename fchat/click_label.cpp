#include "click_label.h"

ClickLabel::ClickLabel(QWidget* parent)
    : QLabel{parent}, current_state_{ClickLabelState::kNormal}
{

}

void ClickLabel::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (current_state_ == ClickLabelState::kNormal)
        {
            qDebug() << "Clicked, change to selected hover: " << selected_hover_;
            current_state_ = ClickLabelState::kSelect;
            setProperty("state", selected_hover_);
            Repolish(this);
            update();
        }
        else
        {
            qDebug() << "Clicked, change to normal hover: " << normal_hover_;
            current_state_ = ClickLabelState::kNormal;
            setProperty("state", normal_hover_);
            Repolish(this);
            update();
        }
        emit clicked();
    }
    QLabel::mousePressEvent(event);
}

void ClickLabel::SetState(const QString& normal, const QString& hover, const QString& press, const QString& select, const QString& select_hover, const QString& select_press)
{
    normal_         = normal;
    normal_hover_   = hover;
    normal_press_   = press;
    selected_       = select;
    selected_hover_ = select_hover;
    selected_press_ = select_press;

    setProperty("state", normal);  // 初始状态用 normal
    Repolish(this);                // 立即套用
}

ClickLabelState ClickLabel::current_state()
{
    return current_state_;
}




void ClickLabel::enterEvent(QEnterEvent *event)
{
    if (current_state_ == ClickLabelState::kNormal)
    {
        setProperty("state", normal_hover_);
    }
    else
    {
        setProperty("state", selected_hover_);
    }
    Repolish(this);
    update();
    QLabel::enterEvent(event);
}

void ClickLabel::leaveEvent(QEvent *event)
{
    if (current_state_ == ClickLabelState::kNormal)
    {
        setProperty("state", normal_);
    }
    else
    {
        setProperty("state", selected_);
    }
    Repolish(this);
    update();
    QLabel::leaveEvent(event);
}
