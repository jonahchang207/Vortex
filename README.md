# Vortex Example Projects

Welcome to the `examples` branch of Vortex!

This branch is meant to host fully functional, ready-to-deploy PROS example projects utilizing the Vortex Odometry Motion Template. Because different seasons require wildly different hardware geometries and game strategies, maintaining entirely separate project workspaces here prevents clutter on the `main` core library branch.

## Structure

This branch should be organized by Season and Robot-Type:

```text
Vortex/ (examples branch)
├── HighStakes/          # 2024-2025 Game Example
│   ├── project.pros
│   ├── src/
│   │   └── main.cpp     # Full initialization & routines utilizing Vortex
│   └── include/
├── OverUnder/           # 2023-2024 Game Example
│   ├── project.pros
│   └── ...
└── README.md            # This file
```

## How to use

When a new VEX Season starts, you don't need to bloat the main library! Instead, you can:
1. Ensure you are on the `examples` branch: `git checkout examples`.
2. Generate a new PROS project inside a new folder: `pros conduct new-project ./HighStakes`.
3. Fetch the latest `vortex` template from your releases and apply it to the `HighStakes` folder.
4. Implement your test routines and push! Users browsing the repository can simply switch to the `examples` branch and clone the entire workspace for their respective game.
