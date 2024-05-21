/*
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "slidingnotificationseffect.h"
#include "slidingnotificationsconfig.h"

#include <kwin/effect/effecthandler.h>

namespace KWin
{

SlidingNotificationsEffect::SlidingNotificationsEffect()
    : m_slideInCurve(QEasingCurve::OutCubic)
    , m_slideOutCurve(QEasingCurve::InCubic)
{
    reconfigure(ReconfigureAll);

    connect(effects, &EffectsHandler::windowAdded,
            this, &SlidingNotificationsEffect::slotWindowAdded);
    connect(effects, &EffectsHandler::windowClosed,
            this, &SlidingNotificationsEffect::slotWindowClosed);
}

void SlidingNotificationsEffect::reconfigure(ReconfigureFlags)
{
    SlidingNotificationsConfig::self()->read();
    m_slideDuration = std::chrono::milliseconds(animationTime<SlidingNotificationsConfig>(std::chrono::milliseconds(200)));
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

void SlidingNotificationsEffect::paintWindow(const RenderTarget &renderTarget, const RenderViewport &viewport, EffectWindow *window, int mask, QRegion region, WindowPaintData &data)
{
    auto it = m_animations.constFind(window);
    if (it != m_animations.constEnd()) {
        region = it->clip.translated(window->pos()).toAlignedRect();

        const QPointF translation = interpolated(it->startOffset, it->endOffset, it->timeline.value());
        data.translate(translation.x(), translation.y());
    }

    effects->paintWindow(renderTarget, viewport, window, mask, region, data);
}

void SlidingNotificationsEffect::postPaintScreen()
{
    for (auto it = m_animations.begin(); it != m_animations.end();) {
        EffectWindow *window = it.key();
        effects->addRepaint(it->clip.translated(window->pos()));

        if (it->timeline.done()) {
            unforceBlurEffect(window);
            unforceContrastEffect(window);
            it = m_animations.erase(it);
        } else {
            ++it;
        }
    }

    effects->postPaintScreen();
}

static std::optional<Qt::Edge> slideEdgeForWindow(EffectWindow *window)
{
    const QRectF screenRect = effects->clientArea(ScreenArea, window);
    const QRectF windowRect = window->frameGeometry();

    if (std::abs(screenRect.left() - windowRect.left()) < windowRect.width() / 2) {
        return Qt::LeftEdge;
    }

    if (std::abs(screenRect.right() - windowRect.right()) < windowRect.width() / 2) {
        return Qt::RightEdge;
    }

    return std::nullopt;
}

void SlidingNotificationsEffect::slotWindowAdded(EffectWindow *window)
{
    if (effects->activeFullScreenEffect() || effects->isScreenLocked()) {
        return;
    }
    if (!window->isNotification() && !window->isCriticalNotification()) {
        return;
    }

    const std::optional<Qt::Edge> slideEdge = slideEdgeForWindow(window);
    if (!slideEdge.has_value()) {
        return;
    }

    window->setData(WindowAddedGrabRole, QVariant::fromValue<void *>(this));

    forceBlurEffect(window);
    forceContrastEffect(window);

    SlideAnimation animation;
    animation.timeline.setEasingCurve(m_slideInCurve);
    animation.timeline.setDuration(m_slideDuration);
    animation.deletedRef = EffectWindowDeletedRef(window);

    const QRectF rect = window->expandedGeometry();
    const QRectF workArea = effects->clientArea(MaximizeArea, window);

    animation.clip = rect;
    animation.clip.translate(-window->pos());

    switch (slideEdge.value()) {
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

    const std::optional<Qt::Edge> slideEdge = slideEdgeForWindow(window);
    if (!slideEdge.has_value()) {
        return;
    }

    window->setData(WindowClosedGrabRole, QVariant::fromValue<void *>(this));

    forceBlurEffect(window);
    forceContrastEffect(window);

    SlideAnimation animation;
    animation.timeline.setEasingCurve(m_slideOutCurve);
    animation.timeline.setDuration(m_slideDuration);
    animation.deletedRef = EffectWindowDeletedRef(window);

    const QRectF rect = window->expandedGeometry();
    const QRectF workArea = effects->clientArea(MaximizeArea, window);

    animation.clip = rect;
    animation.clip.translate(-window->pos());

    switch (slideEdge.value()) {
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
