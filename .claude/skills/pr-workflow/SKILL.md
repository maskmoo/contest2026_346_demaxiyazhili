# PR Workflow: Fork to Upstream

## Description
Submit code changes from the development fork (maskmoo) to the upstream repository (open-vela) via Pull Request, including conflict resolution.

## Repository Setup

- **Fork (origin)**: `git@github.com:maskmoo/contest2026_346_demaxiyazhili.git` — development repo
- **Upstream (openvela)**: `git@github.com:open-vela/contest2026_346_demaxiyazhili.git` — original repo (PR target)
- **Branch**: `dev-ai-contest-2026`

## Workflow Steps

### 1. Ensure Remotes Are Configured

```bash
git remote -v
# Should show:
# origin    https://github.com/maskmoo/contest2026_346_demaxiyazhili.git
# openvela  https://github.com/open-vela/contest2026_346_demaxiyazhili.git
```

If remotes are missing:
```bash
git remote add origin https://github.com/maskmoo/contest2026_346_demaxiyazhili.git
git remote add openvela https://github.com/open-vela/contest2026_346_demaxiyazhili.git
```

### 2. Fetch Latest from Both Remotes

```bash
git fetch origin
git fetch openvela
```

### 3. Rebase Local Branch onto Upstream

**Important**: Use rebase (not merge) to maintain clean linear history for GitHub's rebase merge strategy.

```bash
git rebase openvela/dev-ai-contest-2026
```

If conflicts occur during rebase:
- Resolve each conflict in the affected files
- `git add <resolved-file>`
- `git rebase --continue`
- Repeat until rebase completes

### 4. Squash Commits into One

GitHub rebase applies commits one by one, which can cause intermediate conflicts even if the final state is clean. Squash all changes into a single commit:

```bash
git reset --soft openvela/dev-ai-contest-2026
git commit -m "feat: <descriptive title>

<Detailed description of changes>"
```

### 5. Force Push to Origin

```bash
git push origin dev-ai-contest-2026 --force
```

### 6. Create or Update PR

If PR doesn't exist yet:
```bash
gh pr create --repo open-vela/contest2026_346_demaxiyazhili \
  --head dev-ai-contest-2026 \
  --base dev-ai-contest-2026 \
  --title "feat: <title>" \
  --body "<description>"
```

If PR already exists, the push automatically updates it.

## Conflict Resolution Guide

### Common Conflict Files

| File | Typical Resolution |
|------|-------------------|
| `CMakeLists.txt` | Keep HEAD build flags (`-mno-relax`, `--build-id=none`, etc.) |
| `README.md` | Keep HEAD's more complete documentation |
| `configs/*/defconfig` | Keep HEAD's config options (e.g., `BOARD_LATE_INITIALIZE`) |
| `src/gd32_bringup.c` | Keep HEAD's device registration logic |
| `src/gd32_lcd.c` | Keep HEAD's SPI mode and MADCTL parameters |
| `logs/maskmoo/manifest.json` | Keep HEAD's updated timestamps and sessions |

### Resolution Strategy

1. **Always prefer HEAD (local)** for code changes — it contains bug fixes and improvements
2. **Keep both sides** when changes are complementary (e.g., adding new sections to docs)
3. **Remove conflict markers** (`<<<<<<<`, `=======`, `>>>>>>>`) after resolution
4. **Verify no markers remain**: `grep -rn "<<<<<<" . --include="*.c" --include="*.md"`

### Handling Submodule Issues

If `chips/gd32vw55x/sdk` causes errors:
```bash
rm -rf chips/gd32vw55x/sdk
git reset --hard HEAD~1
```

## Troubleshooting

### "This branch cannot be rebased due to conflicts"

**Cause**: Multiple commits create intermediate conflict states during rebase.
**Fix**: Squash all commits into one (Step 4) and force push.

### SSH Permission Denied

**Cause**: No SSH key configured for GitHub.
**Fix**: Use HTTPS URL instead:
```bash
git remote set-url origin https://github.com/maskmoo/contest2026_346_demaxiyazhili.git
```

### Network Timeout

GitHub connections may be unstable. Retry the push command if it fails.

## Quick Reference

```bash
# Full workflow in one shot:
git fetch openvela
git rebase openvela/dev-ai-contest-2026
git reset --soft openvela/dev-ai-contest-2026
git commit -m "feat: <title>"
git push origin dev-ai-contest-2026 --force
```