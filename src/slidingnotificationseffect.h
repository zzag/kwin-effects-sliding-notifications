/*
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <kwin/effect/effect.h>
#include <kwin/effect/effectwindow.h>
#include <kwin/effect/timeline.h>

namespace KWin
{

class SlideAnimation
{
public:
    TimeLine timeline;
    EffectWindowDeletedRef deletedRef;

    QRectF clip;
    QPointF startOffset;
    QPointF endOffset;
};

class SlidingNotificationsEffect : public Effect
{
    Q_OBJECT

public:
    explicit SlidingNotificationsEffect();

    void reconfigure(ReconfigureFlags flags) override;
    bool isActive() const override;

    void prePaintWindow(EffectWindow *window, WindowPrePaintData &data, std::chrono::milliseconds presentTime) override;
    void paintWindow(const RenderTarget &renderTarget, const RenderViewport &viewport, EffectWindow *window, int mask, QRegion region, WindowPaintData &data) override;
    void postPaintScreen() override;

private Q_SLOTS:
    void slotWindowAdded(EffectWindow *window);
    void slotWindowClosed(EffectWindow *window);

private:
    void forceBlurEffect(EffectWindow *window);
    void forceContrastEffect(EffectWindow *window);
    void unforceBlurEffect(EffectWindow *window);
    void unforceContrastEffect(EffectWindow *window);

    QEasingCurve m_slideInCurve;
    QEasingCurve m_slideOutCurve;
    QHash<EffectWindow *, SlideAnimation> m_animations;
    std::chrono::milliseconds m_slideDuration;
};

} // namespace KWin
