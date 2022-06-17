/*
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <kwineffects.h>

#include <optional>

using namespace KWin; // sue me...

class SlideAnimation
{
public:
    TimeLine timeline;
    KWin::EffectWindowVisibleRef visibleRef;

    QRect clip;
    QPointF startOffset;
    QPointF endOffset;
};

class SlidingNotificationsEffect : public Effect
{
    Q_OBJECT

public:
    explicit SlidingNotificationsEffect();
    ~SlidingNotificationsEffect() override;

    void reconfigure(ReconfigureFlags flags) override;
    bool isActive() const override;

    void prePaintWindow(EffectWindow *window, WindowPrePaintData &data, std::chrono::milliseconds presentTime) override;
    void paintWindow(EffectWindow *window, int mask, QRegion region, WindowPaintData &data) override;
    void postPaintScreen() override;

private slots:
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
