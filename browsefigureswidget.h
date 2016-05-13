#ifndef BROWSEFIGURESWIDGET_H
#define BROWSEFIGURESWIDGET_H

#include <QListWidget>

#include <QEvent>
#include <QGesture>
#include <QGestureEvent>
#include <QPainter>
#include <QEvent>
#include <QList>
#include <QDebug>

class BrowseFiguresWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit BrowseFiguresWidget(QWidget *parent = 0);

    bool event(QEvent *e);
    bool gestureEvent(QGestureEvent *event);
    void swipeTriggered(QSwipeGesture* swipeGesture);

    void getFigureFileDirectory();

signals:
    //void onItemClicked(QListWidgetItem* item);


};

#endif // BROWSEFIGURESWIDGET_H
