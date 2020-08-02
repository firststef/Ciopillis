<h1>Ciopillis</h1>

Build status: ![WinBuild](https://github.com/firststef/Ciopillis/workflows/BuildWindows/badge.svg) ![LinuxBuild](https://github.com/firststef/Ciopillis/workflows/BuildLinux/badge.svg)

![Logo](https://github.com/firststef/Ciopillis/blob/master/Resources/logo.png)

Ciopillis is a game written in c/c++ with the help of the raylib engine. In this wiki I will present mostly how the CiopillisEngine works. 

<h3>Building Ciopillis</h3>
You can build the project with CMake both on Linux and Windows(with MSVC). Just run:

```shell
git clone git@github.com:firststef/Ciopillis.git && cd ciopillis
git submodule update --init --recursive
mkdir build && cd build
cmake ..
```

Note: On Windows it will generate a *.sln* file. You can then then build from inside Microsoft Visual Studio by the menu option `Build Solution` and then `Build Project X` or by running `msbuild Project.vcxproj`.

You can consult the project documentation here: https://github.com/firststef/Ciopillis/blob/master/Documentation.md

### The game

Ciopillis was initially built to be a card game that turns a turn sequence into an action character battle. You might have played the insipration for this, [Archon - The Light and the dark](https://en.wikipedia.org/wiki/Archon:_The_Light_and_the_Dark).

Unfortunately, the production of the game has stopped, due to lack of time. The current state of the projects permits 2 players to battle, each character has 2 attacks: a close-range basic attack (X) and a long-range delayed attack (Y). Being hit by the long range attack significantly reduces the players movement speed.

![Photo](https://github.com/firststef/Ciopillis/blob/master/Resources/Game.PNG)

Checkout the <a href="https://github.com/firststef/Ciopillis/wiki">wiki</a> for more information.
