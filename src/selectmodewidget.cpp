#include "selectmodewidget.h"

#include <QPainter>

SelectModeWidget::SelectModeWidget(QList<QPair<QPixmap, QVariant> > dataList, int initState, QWidget *parent)
    :QWidget(parent), dataList(dataList)
{
    setCurrentState(initState);
}

QSize SelectModeWidget::sizeHint() const
{
    return QSize(50,50);
//    int candidate = qMin(width(), height());
//    return QSize(candidate, candidate);
}

void SelectModeWidget::setCurrentState(int newState)
{
    currentMode = newState;
    setFixedSize(dataList[currentMode].first.size());
    if (!isHidden())
        repaint();
    emit methodChanged(dataList[currentMode].second);
}

int SelectModeWidget::currentState() const
{
    return currentMode;
}

void SelectModeWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    //int pixmapIndex = (currentMode + 1)%dataList.count();
    int pixmapIndex = currentMode;
    painter.drawPixmap(0,0,width(), height(), dataList[pixmapIndex].first);
}

void SelectModeWidget::mousePressEvent(QMouseEvent *)
{
    setCurrentState((currentMode + 1)%dataList.count());
}

//void SelectModeWidget::resizeEvent(QResizeEvent *event)
//{
//    int candidate = qMin(width(), height());
//    QWidget::resizeEvent(event);
//    resize(candidate, candidate);
//}
