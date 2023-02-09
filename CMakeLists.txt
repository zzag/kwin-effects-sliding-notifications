cmake_minimum_required(VERSION 3.0)
project(kwin-effects-sliding-notifications)

set(KF5_MIN_VERSION "5.58.0")
set(QT_MIN_VERSION "5.12.0")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Gui
)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Config
    ConfigWidgets
    CoreAddons
    WindowSystem
)

find_package(epoxy REQUIRED)
find_package(XCB REQUIRED COMPONENTS
    XCB
)

find_package(KWinEffects 5.27.0 REQUIRED COMPONENTS
    kwineffects
)

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
