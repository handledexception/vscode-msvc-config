Example setup for building a C/C++ program in VSCode using the MSVC build tools or GN/Ninja.
I've provided a basic tasks.json and launch.json to build a single source file program.

This project is also my playground for learning Win32 GUI programming.

VSCode setup was taken from a VSCode Github issues thread: https://github.com/Microsoft/vscode-cpptools/issues/1839

1. Make sure you have Microsoft Visual Studio or the MS Build tools installed
2. Create the paths ${workspaceFolder}/Debug/x86 and x64 before compiling with MSVC
3. Install gn/ninja in your PATH if you want to build with those tools.

## Notes

- Microsoft - [Recommended 8-Bit YUV Formats for Video Rendering](https://docs.microsoft.com/en-us/windows/desktop/medfound/recommended-8-bit-yuv-formats-for-video-rendering#converting-8-bit-yuv-to-rgb888)

- Microsoft - [Holographic face tracking NV12 example](https://github.com/microsoft/Windows-universal-samples/tree/fe8567faf2efdea3672c2ba642ba7b925ff6467e/Samples/HolographicFaceTracking)


