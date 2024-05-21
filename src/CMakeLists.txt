kcoreaddons_add_plugin(slidingnotifications INSTALL_NAMESPACE "kwin/effects/plugins")

target_sources(slidingnotifications PRIVATE
    main.cpp
    slidingnotificationseffect.cpp
)

kconfig_add_kcfg_files(slidingnotifications
    slidingnotificationsconfig.kcfgc
)

target_link_libraries(slidingnotifications
    Qt6::Core
    Qt6::Gui

    KF6::ConfigCore
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::WindowSystem

    KWin::kwin
)
