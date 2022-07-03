kcoreaddons_add_plugin(kwin4_effect_slidingnotifications INSTALL_NAMESPACE "kwin/effects/plugins")

target_sources(kwin4_effect_slidingnotifications PRIVATE
    main.cpp
    slidingnotificationseffect.cpp
)

kconfig_add_kcfg_files(kwin4_effect_slidingnotifications
    slidingnotificationsconfig.kcfgc
)

target_link_libraries(kwin4_effect_slidingnotifications
    Qt5::Core
    Qt5::Gui

    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::WindowSystem

    KWinEffects::kwineffects
)
