Zig 0.14.0 or later must be installed on your system. See [Zig Getting Started Guide](https://ziglang.org/learn/getting-started/).

Build/Run Steps:
1. unzip the project .zip file into a folder
2. cd into the folder
3. `zig build run` for the main app or `zig build run-palette` for the color palette previewer

NOTE: Linux users using the Wayland display protocol may need to change `.linux_display_backend = .X11` to `.linux_display_backend = .Wayland` in the build.zig file.
