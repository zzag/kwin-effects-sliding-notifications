/*
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <kwinanimationeffect.h>

using namespace KWin; // sue me...

class SlidingNotificationsEffect : public AnimationEffect
{
    Q_OBJECT

public:
    explicit SlidingNotificationsEffect();
    ~SlidingNotificationsEffect() override;

    void reconfigure(ReconfigureFlags flags) override;

protected:
    void animationEnded(EffectWindow *window, Attribute, uint) override;

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
    int m_slideDuration;
};
