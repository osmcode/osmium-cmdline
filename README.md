
# Osmium Command Line Tool

A multipurpose command line tool for working with OpenStreetMap data based on
the Osmium library.

Official web site: http://osmcode.org/osmium-tool/

[![Build Status](https://secure.travis-ci.org/osmcode/osmium-tool.svg)](http://travis-ci.org/osmcode/osmium-tool)
[![Build Status](https://ci.appveyor.com/api/projects/status/github/osmcode/osmium-tool?svg=true)](https://ci.appveyor.com/project/Mapbox/osmium-tool)

## Prerequisites

You need a C++11 compliant compiler. GCC 4.8 and later as well as clang 3.6 and
later are known to work. It also works on modern Visual Studio C++ compilers.

You also need the following libraries:

    Libosmium (>= 2.14.0)
        http://osmcode.org/libosmium
        Debian/Ubuntu: libosmium2-dev
        Fedora/CentOS: libosmium-devel

    Protozero (>= 1.5.1)
        https://github.com/mapbox/protozero
        Debian/Ubuntu: libprotozero-dev
        Fedora/CentOS: protozero-devel

    Utfcpp
        This is included in the libosmium repository and might or might not
        have been installed with it. See the libosmium README.
        http://utfcpp.sourceforge.net/
        Debian/Ubuntu: libutfcpp-dev
        Fedora/CentOS: utf8cpp-devel
        openSUSE: utfcpp-devel

    RapidJSON (>= 1.1)
        This is included in the osmium-tool repository, so you usually do
        not need to install this.
        http://rapidjson.org/
        Debian/Ubuntu: rapidjson-dev

    boost-program-options (>= 1.55)
        http://www.boost.org/doc/libs/1_55_0/doc/html/program_options.html
        Debian/Ubuntu: libboost-program-options-dev
        Fedora/CentOS: boost-devel
        openSUSE: boost-devel

    boost-crc
        http://www.boost.org/doc/libs/1_55_0/libs/crc/
        Debian/Ubuntu: libboost-dev
        Fedora/CentOS: boost-devel
        openSUSE: boost-devel

    bz2lib
        http://www.bzip.org/
        Debian/Ubuntu: libbz2-dev
        Fedora/CentOS: bzip2-devel
        openSUSE: libbz2-devel

    zlib
        https://www.zlib.net/
        Debian/Ubuntu: zlib1g-dev
        Fedora/CentOS: zlib-devel
        openSUSE: zlib-devel

    Expat
        https://libexpat.github.io/
        Debian/Ubuntu: libexpat1-dev
        Fedora/CentOS: expat-devel
        openSUSE: libexpat-devel

    cmake
        https://cmake.org/
        Debian/Ubuntu: cmake
        Fedora/CentOS: cmake
        openSUSE: cmake

    Pandoc
        (Needed to build documentation, optional)
        http://pandoc.org/
        Debian/Ubuntu: pandoc
        Fedora/CentOS: pandoc
        openSUSE: pandoc

On Linux systems most of these libraries are available through your package
manager, see the list above for the names of the packages. But make sure to
check the versions. If the packaged version available is not new enough, you'll
have to install from source. Most likely this is the case for Protozero and
Libosmium.

On macOS many of the libraries above will be available through Homebrew.

When building the tool, CMake will automatically look for these libraries in
the usual places on your system. In addition it will look for the Libosmium and
Protozero libraries in the same directory where this Osmium repository is. So
if you are building from the Git repository and want to use the newest
Libosmium, Protozero, and Osmium, clone all of them into the same directory:

    mkdir work
    cd work
    git clone https://github.com/mapbox/protozero
    git clone https://github.com/osmcode/libosmium
    git clone https://github.com/osmcode/osmium-tool


## Building

Osmium uses CMake for its builds. On Linux and macOS you can build as follows:

    cd osmium-tool
    mkdir build
    cd build
    cmake ..
    ccmake .  ## optional: change CMake settings if needed
    make

To set the build type call cmake with `-DCMAKE_BUILD_TYPE=type`. Possible
values are empty, Debug, Release, RelWithDebInfo, MinSizeRel, and Dev. The
default is RelWithDebInfo.

Please read the CMake documentation and get familiar with the `cmake` and
`ccmake` tools which have many more options.

On Windows you can compile with the Visual Studio C++ compiler using the
batch script `build-local.bat`. This script calls `build-appveyor.bat`
which downloads some precompiled libraries. You can also download and
compile all the prerequisites yourself.


## Documentation

See the [Osmium Tool website](http://osmcode.org/osmium-tool/)
and [Osmium Tool Manual](http://osmcode.org/osmium-tool/manual.html).

There are man pages in the 'man' directory. To build them you need 'pandoc'.
If the `pandoc` command was found during the CMake config step, the manpages
will be built automatically.


## Tests

Call `ctest` in the build directory to run the tests after build.

More extensive tests of the libosmium I/O system can also be run. See
`test/io/Makefile.in` for instructions.


## License

Copyright (C) 2013-2018  Jochen Topf (jochen@topf.org)

This program is available under the GNU GENERAL PUBLIC LICENSE Version 3.
See the file LICENSE.txt for the complete text of the license.


## Authors

This program was written and is maintained by Jochen Topf (jochen@topf.org).

