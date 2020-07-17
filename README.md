# Sliding Notifications Effect

[![AUR version](https://img.shields.io/aur/version/kwin-effects-sliding-notifications)](https://aur.archlinux.org/packages/kwin-effects-sliding-notifications/)

This is a simple effect that makes notification windows slide in and out when they are shown or hidden. [Demo](https://youtu.be/6uzv8r8Oqf4)


#### Building from Git

You will need the following dependencies to build this effect:

* CMake
* any C++14 enabled compiler
* Qt
* libkwineffects
* KDE Frameworks 5:
    - Config
    - CoreAddons
    - Extra CMake Modules
    - WindowSystem

On Arch Linux

```sh
sudo pacman -S cmake extra-cmake-modules kwin
```

On Fedora

```sh
sudo dnf install cmake extra-cmake-modules kf5-kconfig-devel \
    kf5-kcoreaddons-devel kf5-kwindowsystem-devel kwin-devel \
    qt5-qtbase-devel
```

On Ubuntu

```sh
sudo apt install cmake extra-cmake-modules kwin-dev \
    libkf5config-dev libkf5configwidgets-dev libkf5coreaddons-dev \
    libkf5windowsystem-dev qtbase5-dev
```

After you installed all the required dependencies, you can build
the effect:

```sh
git clone https://github.com/zzag/kwin-effects-sliding-notifications.git
cd kwin-effects-sliding-notifications
mkdir build && cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr
make
sudo make install
```

