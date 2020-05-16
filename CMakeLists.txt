cmake_minimum_required(VERSION 3.0)
project(kwin-effects-sliding-notifications)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
)

find_package(KF5 REQUIRED COMPONENTS
    Config
    ConfigWidgets
    CoreAddons
    WindowSystem
)

find_package(kwineffects REQUIRED COMPONENTS
    kwineffects
    kwinglutils
)

add_subdirectory(src)

feature_summary(WHAT ALL)
