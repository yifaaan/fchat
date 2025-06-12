#ifndef CLICK_LABEL_H
#define CLICK_LABEL_H

#include <QLabel>
#include <QMouseEvent>

#include "global.h"

class ClickLabel : public QLabel
{
    Q_OBJECT
public:
    ClickLabel(QWidget* parent = nullptr);

    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;

    void SetState(const QString& normal = "", const QString& hover = "", const QString& press = "",
                  const QString& select = "", const QString& select_hover = "", const QString& select_press = "");

    ClickLabelState current_state();

signals:
    void clicked(void);

private:
    QString normal_;
    QString normal_hover_;
    QString normal_press_;

    QString selected_;
    QString selected_hover_;
    QString selected_press_;

    ClickLabelState current_state_;

};

#endif // CLICK_LABEL_H
