Example setup for building a C/C++ program in VSCode using the MSVC build tools.
I've provided a basic tasks.json and launch.json to build a single source file program.
Example was taken from a VSCode Github issues thread: https://github.com/Microsoft/vscode-cpptools/issues/1839

1. Make sure you have Microsoft Visual Studio or the MS Build tools installed
2. Create the paths ${workspaceFolder}/Debug/x86 and x64 before compiling with MSVC
