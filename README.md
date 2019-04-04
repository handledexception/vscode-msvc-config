Example setup for building a C/C++ program in VSCode using the MSVC build tools or GN/Ninja.
I've provided a basic tasks.json and launch.json to build a single source file program.

VSCode setup was taken from a VSCode Github issues thread: https://github.com/Microsoft/vscode-cpptools/issues/1839

1. Make sure you have Microsoft Visual Studio or the MS Build tools installed
2. Create the paths ${workspaceFolder}/Debug/x86 and x64 before compiling with MSVC
3. Install gn/ninja in your PATH if you want to build with those tools.


