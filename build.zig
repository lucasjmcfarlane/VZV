const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    //---------------------------RAYLIB DEPENDENCY---------------------------//
    //link raylib dependency
    const raylib_dep = b.dependency("raylib", .{
        .target=target,
        .optimize=optimize,
        .shared = true,
        .linux_display_backend = .Both,
    });
    const raylib = raylib_dep.artifact("raylib");
    b.installArtifact(raylib);

    //---------------------------VZV---------------------------//
    //define executable
    const vzv = b.addExecutable(.{
        .name = "vzv",
        .target = target,
        .optimize = optimize,
    });

    //link libraries
    vzv.linkLibrary(raylib);
    vzv.linkSystemLibrary("m");
    vzv.linkLibC();

    //add source files
    vzv.addCSourceFiles(.{
        .files = &.{
            "src/main.c",
            "src/raygui.c",
            "colorschemes/gruvbox/gruvbox.c",
            "src/font_manager.c",
            "src/text_renderer.c",
            "src/file_dialog_manager.c",
            "src/gui_window_file_dialog.c",
            "src/scroll_manager.c",
            "src/palette.c",
        },
        .flags = &.{},
    });

    //add include paths
    vzv.addIncludePath(b.path("src"));
    vzv.addIncludePath(b.path("colorschemes/gruvbox"));
    vzv.addIncludePath(raylib_dep.path("include"));

    b.installArtifact(vzv);

    //create run step
    const run_vzv = b.addRunArtifact(vzv);
    run_vzv.step.dependOn(b.getInstallStep());

    const vzv_step = b.step("run", "Run VZV");
    vzv_step.dependOn(&run_vzv.step);
}
