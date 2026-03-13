# Vortex CLI

`vortex-cli` is a shell script that handles version bumping, building, and GitHub releases.

## Interactive Mode

Run with no arguments to open the menu:

```bash
./vortex-cli
```

The menu shows the current version, branch, and staged change count.
Select a number to act:

| Key | Action |
|-----|--------|
| `1` | Patch bump (`x.x.N+1`), commit, optional release |
| `2` | Minor bump (`x.N+1.0`), commit, optional release |
| `3` | Major bump (`N+1.0.0`), commit, optional release |
| `4` | Manual version, commit, optional release |
| `5` | Dev push — commit + push, no version change |
| `6` | Release current version — tag, merge to stable, upload |
| `7` | `make library` locally |
| `8` | `mkdocs build` |
| `9` | `pros c purge Vortex` — clear install cache |
| `q` | Quit |

## Flag Mode (CI / scripting)

```bash
./vortex-cli [flags] ["commit message"]
```

| Flag | Effect |
|------|--------|
| `-p` | Patch bump (default) |
| `-m` | Minor bump |
| `-M` | Major bump |
| `-v 1.2.3` | Set exact version |
| `-R` | Full release after bump (merge → stable, tag, upload) |
| `-B` | Build library only |
| `-D` | Build docs only |
| `-P` | Purge PROS cache |

### Examples

```bash
./vortex-cli                            # patch bump, dev push
./vortex-cli -R "feat: pure pursuit"   # patch bump + full release
./vortex-cli -m -R                     # minor bump + full release
./vortex-cli -v 3.0.0 -R "v3 rewrite" # exact version + full release
```

## Release Flow (on `main`)

1. `apply_version()` writes the new version to `Makefile`, `template.pros`, `README.md`
2. Commits all staged changes
3. Checks out `stable`, merges `main`, creates and pushes git tag
4. If `gh` CLI is available: runs `make library`, zips the template, creates the GitHub release
5. Checks out `main` and merges `stable` back in to keep them in sync

On any other branch, the release is tagged as a pre-release on that branch.
