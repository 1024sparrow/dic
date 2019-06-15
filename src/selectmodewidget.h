#ifndef SELECTMODEWIDGET_H
#define SELECTMODEWIDGET_H

#include <QWidget>
#include <QList>
#include <QPair>
#include <QPixmap>
#include <QVariant>

class SelectModeWidget : public QWidget
{
    Q_OBJECT
public:
    SelectModeWidget(QList<QPair<QPixmap, QVariant> > dataList, int initState, QWidget *parent = 0);
    QSize sizeHint() const;
    void setCurrentState(int);
    int currentState() const;
signals:
    void methodChanged(const QVariant &);
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
//    void resizeEvent(QResizeEvent * event);

private:
    QList<QPair<QPixmap, QVariant> > dataList;
    int currentMode;
};

#endif // SELECTMODEWIDGET_H
