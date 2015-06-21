#ifndef TIMELINEWIDGET_H
#define TIMELINEWIDGET_H

#include <QWidget>

class TimelineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimelineWidget(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent* ev) /* override */;
    void mousePressEvent(QMouseEvent* ev) /* override */;
    // void mouseReleaseEvent(QMouseEvent* ev) /* override */;
    void wheelEvent(QWheelEvent* ev) /* override */;

signals:
    void cursorMoved(double time);

public slots:
    void setCursor(double time);

private:
    double mCursor; // in seconds

    double mLength; // in seconds
    double mViewOffset; // in seconds
    double mScale;
};

#endif // TIMELINEWIDGET_H