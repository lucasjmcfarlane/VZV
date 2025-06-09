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
        .linux_display_backend = .X11,
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
            "colorschemes/gruvbox/gruvbox.c",
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


    //---------------------------Palette---------------------------//
    
    //define executable
    const palette = b.addExecutable(.{
        .name = "palette",
        .target = target,
        .optimize = optimize,
    });

    //link libraries
    palette.linkLibrary(raylib);
    palette.linkSystemLibrary("m");
    palette.linkLibC();

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
    palette.addIncludePath(raylib_dep.path("include"));

    b.installArtifact(palette);

    //create run step
    const run_palette = b.addRunArtifact(palette);
    run_palette.step.dependOn(b.getInstallStep());

    const palette_step = b.step("run-palette", "Run Palette");
    palette_step.dependOn(&run_palette.step);
}
