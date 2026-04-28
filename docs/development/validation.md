# Validation

Vortex ships with automated checks to keep template quality high as features evolve.

## What CI Validates

- `pros make template` succeeds and emits a template zip
- `Vortex-Example-Project.zip` is generated as a full standalone PROS project
- the example zip contains key entry points such as `project.pros`, `src/main.cpp`, and `include/robot.hpp`
- major README feature claims are backed by both code and API docs

## Feature Claim Validation

The feature validator checks that major claims in `README.md` are still reflected in:

- public headers under `include/vortex/`
- API docs under `docs/api/`

This prevents drift where marketing/docs promise features that are no longer documented or exposed cleanly.

## Local Commands

```bash
python3 scripts/validate_feature_claims.py
./scripts/package_example_project.sh "/tmp/vortex-example-project" "Vortex-Example-Project.zip" "$PWD"
pros make template
```
