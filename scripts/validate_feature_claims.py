#!/usr/bin/env python3
from __future__ import annotations

import re
import sys
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parent.parent


def read(relative_path: str) -> str:
    return (REPO_ROOT / relative_path).read_text()


CHECKS = [
    {
        "name": "Boomerang moveToPose",
        "readme": [r"Boomerang", r"moveToPose"],
        "code": [("include/vortex/drive/chassis.hpp", r"moveToPose")],
        "docs": [("docs/api/chassis.md", r"moveToPose")],
    },
    {
        "name": "Pure Pursuit follow",
        "readme": [r"Pure Pursuit", r"follow"],
        "code": [("include/vortex/drive/chassis.hpp", r"follow")],
        "docs": [("docs/api/chassis.md", r"Pure Pursuit|follow")],
    },
    {
        "name": "Point-to-Point motion",
        "readme": [r"Point-to-Point", r"moveToPoint"],
        "code": [("include/vortex/drive/chassis.hpp", r"moveToPoint")],
        "docs": [("docs/api/chassis.md", r"moveToPoint")],
    },
    {
        "name": "Swing turns",
        "readme": [r"Swing Turns"],
        "code": [("include/vortex/drive/chassis.hpp", r"swingToHeading"), ("include/vortex/drive/chassis.hpp", r"swingToPoint")],
        "docs": [("docs/api/chassis.md", r"swingToHeading|swingToPoint")],
    },
    {
        "name": "PID with slew and anti-windup",
        "readme": [r"PID", r"anti-windup", r"slew"],
        "code": [("include/vortex/control/pid.hpp", r"windup_range"), ("include/vortex/control/pid.hpp", r"slew")],
        "docs": [("docs/api/pid-profile.md", r"windup_range"), ("docs/api/pid-profile.md", r"slew")],
    },
    {
        "name": "Trapezoidal velocity profiling",
        "readme": [r"Trapezoidal Velocity Profiling"],
        "code": [("include/vortex/control/profile.hpp", r"class TrapezoidalProfile")],
        "docs": [("docs/api/pid-profile.md", r"TrapezoidalProfile")],
    },
    {
        "name": "Arcade tank curvature drive",
        "readme": [r"Arcade / Tank / Curvature"],
        "code": [("include/vortex/drive/chassis.hpp", r"tank"), ("include/vortex/drive/chassis.hpp", r"arcade"), ("include/vortex/drive/chassis.hpp", r"curvature")],
        "docs": [("docs/api/chassis.md", r"tank"), ("docs/api/chassis.md", r"arcade"), ("docs/api/chassis.md", r"curvature")],
    },
    {
        "name": "Exponential drive curves",
        "readme": [r"Exponential Drive Curves"],
        "code": [("include/vortex/util/driveCurve.hpp", r"ExpoDriveCurve")],
        "docs": [("docs/api/utils.md", r"ExpoDriveCurve|Drive Curves")],
    },
    {
        "name": "PTO support",
        "readme": [r"PTO Support"],
        "code": [("include/vortex/drive/chassis.hpp", r"setPTO"), ("include/vortex/drive/chassis.hpp", r"resetPTO")],
        "docs": [("docs/api/chassis.md", r"setPTO|resetPTO")],
    },
    {
        "name": "Auton selector and dashboard",
        "readme": [r"Auton Selector", r"Loading Screen|GUI|telemetry"],
        "code": [("include/vortex/util/auton_selector.hpp", r"class AutonSelector"), ("include/vortex/util/gui/dashboard.hpp", r"class Dashboard")],
        "docs": [("docs/api/utils.md", r"AutonSelector|Dashboard")],
    },
    {
        "name": "Telemetry logger",
        "readme": [r"Telemetry"],
        "code": [("include/vortex/util/logger.hpp", r"telemetry")],
        "docs": [("docs/api/utils.md", r"telemetry")],
    },
    {
        "name": "Timer utility",
        "readme": [r"Timers"],
        "code": [("include/vortex/util/timer.hpp", r"class Timer")],
        "docs": [("docs/api/utils.md", r"class Timer|## Timer|vortex::Timer")],
    },
    {
        "name": "Piston wrapper",
        "readme": [r"Piston Wrapper|Pneumatic Piston Wrapper"],
        "code": [("include/vortex/util/piston.hpp", r"class Piston")],
        "docs": [("docs/api/utils.md", r"Piston")],
    },
]


def contains_all(text: str, patterns: list[str]) -> bool:
    return all(re.search(pattern, text, flags=re.IGNORECASE) for pattern in patterns)


def matches(path: str, pattern: str) -> bool:
    return re.search(pattern, read(path), flags=re.IGNORECASE | re.MULTILINE) is not None


def main() -> int:
    readme = read("README.md")
    failures: list[str] = []

    for check in CHECKS:
        if not contains_all(readme, check["readme"]):
            failures.append(f"{check['name']}: README claim not found or wording changed")
            continue

        missing_code = [f"{path} / {pattern}" for path, pattern in check["code"] if not matches(path, pattern)]
        missing_docs = [f"{path} / {pattern}" for path, pattern in check["docs"] if not matches(path, pattern)]

        if missing_code or missing_docs:
            parts = []
            if missing_code:
                parts.append("missing code: " + ", ".join(missing_code))
            if missing_docs:
                parts.append("missing docs: " + ", ".join(missing_docs))
            failures.append(f"{check['name']}: " + "; ".join(parts))

    if failures:
        print("Feature validation failed:")
        for failure in failures:
            print(f"- {failure}")
        return 1

    print(f"Validated {len(CHECKS)} README feature claims against code and docs.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
