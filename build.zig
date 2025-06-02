const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});


    //---------------------------VZV---------------------------//
    const vzv = b.addExecutable(.{
        .name = "vzv",
        .target = target,
        .optimize = optimize,
    });

    // Add source files
    vzv.addCSourceFiles(.{
        .files = &.{
            "src/main.c",
            "colorschemes/gruvbox/gruvbox.c",
        },
        .flags = &.{},
    });

    // Add include paths
    vzv.addIncludePath(b.path("src"));
    vzv.addIncludePath(b.path("colorschemes/gruvbox"));

    vzv.linkSystemLibrary("raylib");
    vzv.linkSystemLibrary("m");
    vzv.linkLibC();

    b.installArtifact(vzv);

    const run_vzv = b.addRunArtifact(vzv);
    run_vzv.step.dependOn(b.getInstallStep());

    const vzv_step = b.step("run-vzv", "Run VZV");
    vzv_step.dependOn(&run_vzv.step);


    //---------------------------Palette---------------------------//
    const palette = b.addExecutable(.{
        .name = "palette",
        .target = target,
        .optimize = optimize,
    });

    // Add source files
    palette.addCSourceFiles(.{
        .files = &.{
            "src/palette.c",
            "colorschemes/gruvbox/gruvbox.c",
        },
        .flags = &.{},
    });

    // Add include paths
    palette.addIncludePath(b.path("src"));
    palette.addIncludePath(b.path("colorschemes/gruvbox"));

    palette.linkSystemLibrary("raylib");
    palette.linkSystemLibrary("m");
    palette.linkLibC();

    b.installArtifact(palette);

    const run_palette = b.addRunArtifact(palette);
    run_palette.step.dependOn(b.getInstallStep());

    const palette_step = b.step("run-palette", "Run Palette");
    palette_step.dependOn(&run_palette.step);
}
