#include "timelinecontainer.h"
#include "ui_timelinecontainer.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include <QWheelEvent>

#include "timelinelayerwidget.h"
#include "timelinewidget.h"
#include "playbackwidget.h"

double snapInterval(double bpm, double step)
{
    return step / (bpm / 60.0);
}

TimelineContainer::TimelineContainer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimelineContainer)
{
    ui->setupUi(this);

    mCurrentLayerIdx = -1;
    mPlaybackWidget = findChild<PlaybackWidget*>("playback_widget");
    mTimelineWidget = findChild<TimelineWidget*>("timeline_widget");

    mLayerScrollArea = findChild<QScrollArea*>("scroll_layers");
    mLayerLayout = dynamic_cast<QVBoxLayout*>(mLayerScrollArea->widget()->layout());

    QObject::connect(mPlaybackWidget, &PlaybackWidget::positionChanged,
                     mTimelineWidget, &TimelineWidget::setCursor);
    QObject::connect(mPlaybackWidget, &PlaybackWidget::durationChanged,
                     mTimelineWidget, &TimelineWidget::setLength);
    QObject::connect(mTimelineWidget, &TimelineWidget::timeClicked,
                     mPlaybackWidget, &PlaybackWidget::setPosition);
    QObject::connect(mPlaybackWidget, &PlaybackWidget::snappingChanged,
                     [this] (double step) {
        if (!timeline())
            return;
        emit snapIntervalChanged(snapInterval(timeline()->bpm(), step));
    });
}

TimelineContainer::~TimelineContainer()
{
    delete ui;
}

bool TimelineContainer::dirty() const
{
    return true;
}

void TimelineContainer::setTimeline(const std::shared_ptr<Timeline>& timeline)
{
    if (mTimeline)
        mTimeline->disconnect(this);

    mTimeline = timeline;

    // clear existing timeline layer widgets
    while (mLayerLayout->count())
        mLayerLayout->takeAt(0)->widget()->deleteLater();

    if (mTimeline) {
        QObject::connect(mTimeline.get(), &Timeline::layerAdded,
                         this, &TimelineContainer::insertLayerWidget);
        QObject::connect(mTimeline.get(), &Timeline::layerRemoved,
                         this, &TimelineContainer::removeLayerWidget);
        QObject::connect(mTimeline.get(), SIGNAL(backingTrackChanged(QString)),
                         mPlaybackWidget, SLOT(setMedia(QString)));

        for (int i = 0; i < mTimeline->layers().size(); i++) {
            insertLayerWidget(i, mTimeline->layer(i));
        }
        mPlaybackWidget->setMedia(mTimeline->backingTrack());

        if (ui->playback_widget->snapSteps() != 0)
            emit snapIntervalChanged(snapInterval(mTimeline->bpm(), ui->playback_widget->snapSteps()));
    }
}

void TimelineContainer::createLayer()
{
    if (!mTimeline)
        return;

    mTimeline->createLayer();
}

void TimelineContainer::removeCurrentLayer()
{
    if (mTimeline && currentLayerIdx() != -1) {
        mTimeline->removeLayer(currentLayerIdx());
        mCurrentLayerIdx = -1;
    }
}

void TimelineContainer::insertLayerWidget(int idx, std::shared_ptr<TimelineLayer> layer)
{
    TimelineLayerWidget* widget = new TimelineLayerWidget(layer);
    QObject::connect(mTimelineWidget, &TimelineWidget::cursorMoved,
                     widget, &TimelineLayerWidget::setCursor);
    QObject::connect(mTimelineWidget, &TimelineWidget::viewportChanged,
                     widget, &TimelineLayerWidget::setViewport);

    QObject::connect(widget, &TimelineLayerWidget::focusGained,
                     [this, widget] {
        setCurrentLayerIdx(mLayerLayout->indexOf(widget));
    });
    QObject::connect(widget, &TimelineLayerWidget::focusLost,
                     [this, widget] {
        if (mLayerLayout->indexOf(widget) == mCurrentLayerIdx)
            setCurrentLayerIdx(-1);
    });

    QObject::connect(this, &TimelineContainer::snapIntervalChanged,
                     widget, &TimelineLayerWidget::setSnapInterval);

    QObject::connect(widget, &TimelineLayerWidget::selectionChanged,
                     this, &TimelineContainer::currentSelectionChanged);

    QObject::connect(widget, &TimelineLayerWidget::requestSetCursor,
                     mPlaybackWidget, &PlaybackWidget::setPosition);

    mLayerLayout->insertWidget(idx, widget);
}

void TimelineContainer::removeLayerWidget(int idx)
{
    QLayoutItem* item = mLayerLayout->itemAt(idx);
    mLayerLayout->removeItem(item);
    delete item->widget();
}

// forward unused scroll events to mTimelineWidget
void TimelineContainer::wheelEvent(QWheelEvent* ev)
{
    QApplication::sendEvent(mTimelineWidget, ev);
}

void TimelineContainer::keyPressEvent(QKeyEvent* ev)
{
    if (ev->key() == Qt::Key_Space) {
        ui->playback_widget->togglePlaying();
        ev->accept();
    }
}

double TimelineContainer::cursor() const
{
    return mTimelineWidget->cursor();
}

double TimelineContainer::cursorSnapped() const
{
    if (mTimeline == nullptr || ui->playback_widget->snapSteps() == 0)
        return cursor();

    const double interval = snapInterval(mTimeline->bpm(), ui->playback_widget->snapSteps());
    const double cursor = mTimelineWidget->cursor();
    return round(cursor / interval) * interval;
}

void TimelineContainer::setCurrentLayerIdx(int idx)
{
    mCurrentLayerIdx = idx;
    emit currentLayerChanged(idx);
    emit currentSelectionChanged(currentSelection());
}

const Selection& TimelineContainer::currentSelection() const
{
    if (mCurrentLayerIdx == -1) {
        static const Selection emptySelection;
        return emptySelection;
    } else {
        const TimelineLayerWidget* widget = (TimelineLayerWidget*)mLayerLayout->itemAt(mCurrentLayerIdx)->widget();
        return widget->selection();
    }
}

int TimelineContainer::currentLayerIdx() const
{
    return mCurrentLayerIdx;
}

std::shared_ptr<TimelineLayer> TimelineContainer::currentLayer()
{
    if (currentLayerIdx() == -1)
        return nullptr;
    return mTimeline->layer(currentLayerIdx());
}

void TimelineContainer::addEventToCurrentLayer(const std::shared_ptr<TimelineEvent> &event)
{
    if (currentLayer())
        currentLayer()->events().addEvent(event);
}

void TimelineContainer::removeSelectionInCurrentLayer()
{
    auto selection = currentSelection();
    if (currentLayer() && selection.state() == Selection::DONE)
        currentLayer()->events().removeEventsInRange(selection.left(), selection.right());
}
