<h1>Ciopillis</h1>

Build status: ![WinBuild](![C/C++ CI](https://github.com/firststef/Ciopillis/workflows/BuildWindows/badge.svg))

Ciopillis is a game written in c/c++ with the help of the raylib engine. In this wiki I will present mostly how the CiopillisEngine works. Checkout the <a href="https://github.com/firststef/Ciopillis/wiki">wiki</a> for more information.

<h3>Building Ciopillis</h3>
You can build the project with CMake both on Linux and Windows(with MSVC). Just run:

```shell
git clone git@github.com:firststef/Ciopillis.git && cd ciopillis
git submodule update --init --recursive
mkdir build && cd build
cmake ..
```

Note: On Windows it will generate a *.sln* file. You can then then build from inside Microsoft Visual Studio by the menu option `Build Solution` and then `Build Project X`.

You can consult the project documentation here: https://github.com/firststef/Ciopillis/blob/master/Documentation.md
