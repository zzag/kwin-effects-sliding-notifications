/*
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "slidingnotificationseffect.h"
#include "slidingnotificationsconfig.h"

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

SlidingNotificationsEffect::~SlidingNotificationsEffect()
{
}

void SlidingNotificationsEffect::reconfigure(ReconfigureFlags)
{
    SlidingNotificationsConfig::self()->read();
    m_slideDuration = animationTime<SlidingNotificationsConfig>(500);
}

static Qt::Edge slideEdgeForWindow(EffectWindow *window)
{
    const QRect screenRect = effects->clientArea(ScreenArea, window);
    const QRect windowRect = window->frameGeometry();

    const int screenCenterX = screenRect.x() + screenRect.width() / 2;
    const int screenCenterY = screenRect.y() + screenRect.height() / 2;

    if (windowRect.x() + windowRect.width() < screenCenterX)
        return Qt::LeftEdge;
    if (windowRect.x() > screenCenterX)
        return Qt::RightEdge;

    if (windowRect.y() + windowRect.height() < screenCenterY)
        return Qt::TopEdge;
    else
        return Qt::BottomEdge;
}

void SlidingNotificationsEffect::slotWindowAdded(EffectWindow *window)
{
    if (effects->activeFullScreenEffect())
        return;
    if (!window->isNotification() && !window->isCriticalNotification())
        return;

    window->setData(WindowAddedGrabRole, QVariant::fromValue<void *>(this));

    forceBlurEffect(window);
    forceContrastEffect(window);

    const QRect rect = window->expandedGeometry();

    FPx2 translationFrom;

    switch (slideEdgeForWindow(window)) {
    case Qt::RightEdge:
        translationFrom.set(rect.width(), 0);
        break;
    case Qt::LeftEdge:
        translationFrom.set(-rect.width(), 0);
        break;
    case Qt::TopEdge:
        translationFrom.set(0, -rect.height());
        break;
    case Qt::BottomEdge:
        translationFrom.set(0, rect.height());
        break;
    }

    animate(window, Translation, 0, m_slideDuration, FPx2(), m_slideInCurve, 0, translationFrom);
    animate(window, Clip, 0, m_slideDuration, FPx2(1, 1), m_slideInCurve, 0, FPx2(1, 1));
    animate(window, Opacity, 0, m_slideDuration, FPx2(1, 1), m_slideInCurve, 0, FPx2(0, 0));
}

void SlidingNotificationsEffect::slotWindowClosed(EffectWindow *window)
{
    if (effects->activeFullScreenEffect())
        return;
    if (!window->isNotification() && !window->isCriticalNotification())
        return;

    window->setData(WindowClosedGrabRole, QVariant::fromValue<void *>(this));

    forceBlurEffect(window);
    forceContrastEffect(window);

    const QRect rect = window->expandedGeometry();

    FPx2 translationTo;

    switch (slideEdgeForWindow(window)) {
    case Qt::RightEdge:
        translationTo.set(rect.width(), 0);
        break;
    case Qt::LeftEdge:
        translationTo.set(-rect.width(), 0);
        break;
    case Qt::TopEdge:
        translationTo.set(0, -rect.height());
        break;
    case Qt::BottomEdge:
        translationTo.set(0, rect.height());
        break;
    }

    animate(window, Translation, 0, m_slideDuration, translationTo, m_slideOutCurve);
    animate(window, Clip, 0, m_slideDuration, FPx2(1, 1), m_slideOutCurve, 0, FPx2(1, 1));
}

void SlidingNotificationsEffect::animationEnded(EffectWindow *window, Attribute, uint)
{
    unforceBlurEffect(window);
    unforceContrastEffect(window);
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
