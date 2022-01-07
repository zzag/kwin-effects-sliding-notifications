/*
    SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "slidingnotificationseffect.h"

KWIN_EFFECT_FACTORY_ENABLED(SlidingNotificationsEffect,
                            "metadata.json",
                            return false;)

#include "slidingnotificationsplugin.moc"
