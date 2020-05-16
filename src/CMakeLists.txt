set(slidingnotificationseffect_SOURCES
    slidingnotificationseffect.cpp
    slidingnotificationsplugin.cpp
)

kconfig_add_kcfg_files(slidingnotificationseffect_SOURCES
    slidingnotificationsconfig.kcfgc
)

add_library(kwin4_effect_slidingnotifications SHARED ${slidingnotificationseffect_SOURCES})

target_link_libraries(kwin4_effect_slidingnotifications
    Qt5::Core
    Qt5::Gui

    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::WindowSystem

    kwineffects::kwineffects
)

install(TARGETS kwin4_effect_slidingnotifications DESTINATION ${PLUGIN_INSTALL_DIR}/kwin/effects/plugins/)
