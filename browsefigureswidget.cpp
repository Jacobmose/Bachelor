#include "browsefigureswidget.h"

#include "QDir"

BrowseFiguresWidget::BrowseFiguresWidget(QWidget *parent) :
    QListWidget(parent)
{
    //setAttribute(Qt::WA_AcceptTouchEvents);

    grabGesture(Qt::SwipeGesture);

    getFigureFileDirectory();
    this->setFlow(QListWidget::TopToBottom);
    this->setViewMode(QListWidget::IconMode);
    this->setIconSize(QSize(150,150));
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setDragEnabled(false);


    //connect(this, SIGNAL(itemClicked(QListWidgetItem*)), this, SIGNAL(onItemClicked(QListWidgetItem*)));
}


void BrowseFiguresWidget::getFigureFileDirectory()
{
    QDir figureDir("C:/Users/jacobmosehansen/Pictures/testpic");
    figureDir.setNameFilters(QStringList("*.png"));
    figureDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QFileInfoList files = figureDir.entryInfoList();

    QListWidgetItem *fileItem = 0;

    foreach(QFileInfo file, files)
    {
        fileItem = new QListWidgetItem(file.fileName());
        fileItem->setIcon(QIcon("C:/Users/jacobmosehansen/Pictures/testpic/" + file.fileName()));
        this->addItem(fileItem);
    }
}

bool BrowseFiguresWidget::event(QEvent *e)
{
    if(e->type() == QEvent::Gesture)
    {
        qDebug() << "QEvent::Gesture...";
        return gestureEvent(static_cast<QGestureEvent*>(e));
    }

    //return QWidget::event(e);
    return QListWidget::event(e);
}

bool BrowseFiguresWidget::gestureEvent(QGestureEvent *event)
{
    if(QGesture *swipe = event->gesture(Qt::SwipeGesture))
    {
        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
    }
    return true;
}

void BrowseFiguresWidget::swipeTriggered(QSwipeGesture *swipeGesture)
{
    qDebug() << "swipeTriggered()";

    if(swipeGesture->state() == Qt::GestureFinished)
    {
        if(swipeGesture->horizontalDirection() == QSwipeGesture::Left)
        {
            qDebug() << "swipeTriggered() next";
            //setCurrentIndex( std::max(0, currentIndex()-1));
            //setCurrentItem();


        }
        else if(swipeGesture->horizontalDirection() == QSwipeGesture::Right)
        {
            //setCurrentIndex( std::min( count()-1, currentIndex()+1));
        }
        update();
    }
}


