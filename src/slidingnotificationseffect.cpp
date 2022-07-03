/*
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "slidingnotificationseffect.h"
#include "slidingnotificationsconfig.h"

namespace KWin
{

SlidingNotificationsEffect::SlidingNotificationsEffect()
    : m_slideInCurve(QEasingCurve::BezierSpline)
    , m_slideOutCurve(QEasingCurve::BezierSpline)
{
    reconfigure(ReconfigureAll);

    m_slideInCurve.addCubicBezierSegment(QPointF(0, 0), QPointF(0, 1), QPointF(1, 1));
    m_slideOutCurve.addCubicBezierSegment(QPointF(1, 0), QPointF(1, 1), QPointF(1, 1));

    connect(effects, &EffectsHandler::windowAdded,
            this, &SlidingNotificationsEffect::slotWindowAdded);
    connect(effects, &EffectsHandler::windowClosed,
            this, &SlidingNotificationsEffect::slotWindowClosed);
}

void SlidingNotificationsEffect::reconfigure(ReconfigureFlags)
{
    SlidingNotificationsConfig::self()->read();
    m_slideDuration = std::chrono::milliseconds(animationTime<SlidingNotificationsConfig>(250));
}

bool SlidingNotificationsEffect::isActive() const
{
    return !m_animations.isEmpty();
}

void SlidingNotificationsEffect::prePaintWindow(EffectWindow *window, WindowPrePaintData &data, std::chrono::milliseconds presentTime)
{
    auto it = m_animations.find(window);
    if (it != m_animations.end()) {
        data.setTransformed();
        it->timeline.advance(presentTime);
    }

    effects->prePaintWindow(window, data, presentTime);
}

template <typename T>
T interpolated(T a, T b, qreal progress)
{
    return a * (1 - progress) + b * progress;
}

void SlidingNotificationsEffect::paintWindow(EffectWindow *window, int mask, QRegion region, WindowPaintData &data)
{
    auto it = m_animations.constFind(window);
    if (it != m_animations.constEnd()) {
        region = it->clip.translated(window->pos());

        const QPointF translation = interpolated(it->startOffset, it->endOffset, it->timeline.value());
        data.translate(translation.x(), translation.y());
    }

    effects->paintWindow(window, mask, region, data);
}

void SlidingNotificationsEffect::postPaintScreen()
{
    for (auto it = m_animations.begin(); it != m_animations.end();) {
        EffectWindow *window = it.key();
        window->addRepaint(it->clip);

        if (it->timeline.done()) {
            unforceBlurEffect(window);
            unforceContrastEffect(window);
            if (window->isDeleted()) {
                window->unrefWindow();
            }
            it = m_animations.erase(it);
        } else {
            ++it;
        }
    }

    effects->postPaintScreen();
}

static Qt::Edge slideEdgeForWindow(EffectWindow *window)
{
    const QRect screenRect = effects->clientArea(ScreenArea, window);
    const QRect windowRect = window->frameGeometry();

    const int screenCenterX = screenRect.x() + screenRect.width() / 2;
    const int screenCenterY = screenRect.y() + screenRect.height() / 2;

    if (windowRect.x() + windowRect.width() < screenCenterX) {
        return Qt::LeftEdge;
    }
    if (windowRect.x() > screenCenterX) {
        return Qt::RightEdge;
    }

    if (windowRect.y() + windowRect.height() < screenCenterY) {
        return Qt::TopEdge;
    } else {
        return Qt::BottomEdge;
    }
}

void SlidingNotificationsEffect::slotWindowAdded(EffectWindow *window)
{
    if (effects->activeFullScreenEffect() || effects->isScreenLocked()) {
        return;
    }
    if (!window->isNotification() && !window->isCriticalNotification()) {
        return;
    }

    window->setData(WindowAddedGrabRole, QVariant::fromValue<void *>(this));

    forceBlurEffect(window);
    forceContrastEffect(window);

    SlideAnimation animation;
    animation.timeline.setEasingCurve(m_slideInCurve);
    animation.timeline.setDuration(m_slideDuration);

    const QRect rect = window->expandedGeometry();
    const QRect workArea = effects->clientArea(MaximizeArea, window);

    animation.clip = rect;
    animation.clip.translate(-window->pos());

    switch (slideEdgeForWindow(window)) {
    case Qt::RightEdge:
        animation.clip.setWidth(workArea.x() + workArea.width() - rect.x());
        animation.startOffset = QPointF(animation.clip.width(), 0);
        break;
    case Qt::LeftEdge:
        animation.clip.setWidth(rect.x() + rect.width() - workArea.x());
        animation.startOffset = QPointF(-animation.clip.width(), 0);
        break;
    case Qt::TopEdge:
        animation.clip.setHeight(rect.y() + rect.height() - workArea.y());
        animation.startOffset = QPointF(0, -animation.clip.height());
        break;
    case Qt::BottomEdge:
        animation.clip.setHeight(workArea.y() + workArea.height() - rect.y());
        animation.startOffset = QPointF(0, animation.clip.height());
        break;
    }

    m_animations[window] = animation;
    window->addRepaintFull();
}

void SlidingNotificationsEffect::slotWindowClosed(EffectWindow *window)
{
    if (effects->activeFullScreenEffect() || effects->isScreenLocked()) {
        return;
    }
    if (!window->isNotification() && !window->isCriticalNotification()) {
        return;
    }

    window->setData(WindowClosedGrabRole, QVariant::fromValue<void *>(this));
    window->refWindow();

    forceBlurEffect(window);
    forceContrastEffect(window);

    SlideAnimation animation;
    animation.visibleRef = KWin::EffectWindowVisibleRef(window, EffectWindow::PAINT_DISABLED_BY_DELETE);
    animation.timeline.setEasingCurve(m_slideOutCurve);
    animation.timeline.setDuration(m_slideDuration);

    const QRect rect = window->expandedGeometry();
    const QRect workArea = effects->clientArea(MaximizeArea, window);

    animation.clip = rect;
    animation.clip.translate(-window->pos());

    switch (slideEdgeForWindow(window)) {
    case Qt::RightEdge:
        animation.clip.setWidth(workArea.x() + workArea.width() - rect.x());
        animation.endOffset = QPointF(animation.clip.width(), 0);
        break;
    case Qt::LeftEdge:
        animation.clip.setWidth(rect.x() + rect.width() - workArea.x());
        animation.endOffset = QPointF(-animation.clip.width(), 0);
        break;
    case Qt::TopEdge:
        animation.clip.setHeight(rect.y() + rect.height() - workArea.y());
        animation.endOffset = QPointF(0, -animation.clip.height());
        break;
    case Qt::BottomEdge:
        animation.clip.setHeight(workArea.y() + workArea.height() - rect.y());
        animation.endOffset = QPointF(0, animation.clip.height());
        break;
    }

    m_animations[window] = animation;
    window->addRepaintFull();
}

void SlidingNotificationsEffect::forceBlurEffect(EffectWindow *window)
{
    window->setData(WindowForceBlurRole, QVariant(true));
}

void SlidingNotificationsEffect::unforceBlurEffect(EffectWindow *window)
{
    window->setData(WindowForceBlurRole, QVariant());
}

void SlidingNotificationsEffect::forceContrastEffect(EffectWindow *window)
{
    window->setData(WindowForceBackgroundContrastRole, QVariant(true));
}

void SlidingNotificationsEffect::unforceContrastEffect(EffectWindow *window)
{
    window->setData(WindowForceBackgroundContrastRole, QVariant());
}

} // namespace KWin
