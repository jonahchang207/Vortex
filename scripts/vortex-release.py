#!/usr/bin/env python3
import os
import re
import sys
import subprocess
import click
from rich.console import Console
from rich.panel import Panel
from rich.progress import Progress, SpinnerColumn, TextColumn
from rich.prompt import Confirm
from rich.theme import Theme
from rich.table import Table

# --- Configuration & Branding ---
VORTEX_THEME = Theme({
    "info": "cyan",
    "warning": "yellow",
    "error": "bold red",
    "success": "bold green",
    "brand": "bold #39FF14",  # Neon Green
    "version": "bold magenta",
})

console = Console(theme=VORTEX_THEME)

VORTEX_ASCII = r"""
[brand] __      __  ____   _____   _______  ______  __   __ [/brand]
[brand] \ \    / / / __ \ |  __ \ |__   __||  ____| \ \ / / [/brand]
[brand]  \ \  / / | |  | || |__) |   | |   | |__     \ V /  [/brand]
[brand]   \ \/ /  | |  | ||  _  /    | |   |  __|     > <   [/brand]
[brand]    \  /   | |__| || | \ \    | |   | |____   / ^ \  [/brand]
[brand]     \/     \____/ |_|  \_\   |_|   |______| /_/ \_\ [/brand]
[brand]                                                     [/brand]
"""

# --- Configuration & Branches ---
STABLE_BRANCH = "stable"
DEV_BRANCH = "dev"
PROD_PATHS = ["src/", "include/", "example/", "firmware/", "Makefile", "common.mk", "project.pros", "LICENSE", "README.md", ".gitignore", "scripts/"]

class VersionManager:
    def __init__(self, file_path="Makefile"):
        self.file_path = file_path
        self.version_pattern = re.compile(r'^VERSION\s*:=\s*(\d+\.\d+\.\d+)', re.MULTILINE)

    def get_current_version(self):
        if not os.path.exists(self.file_path):
            return "0.0.0"
        with open(self.file_path, "r") as f:
            content = f.read()
            match = self.version_pattern.search(content)
            return match.group(1) if match else "0.0.0"

    def increment(self, current, part):
        major, minor, patch = map(int, current.split("."))
        if part == "major":
            major += 1
            minor = 0
            patch = 0
        elif part == "minor":
            minor += 1
            patch = 0
        else: # patch
            patch += 1
        return f"{major}.{minor}.{patch}"

    def update_file(self, new_version):
        with open(self.file_path, "r") as f:
            content = f.read()
        
        new_content = self.version_pattern.sub(f"VERSION:={new_version}", content)
        
        with open(self.file_path, "w") as f:
            f.write(new_content)

class ProcessManager:
    @staticmethod
    def get_pros_cmd():
        # Check if 'pros' is in PATH
        try:
            if subprocess.run(["which", "pros"], capture_output=True).returncode == 0:
                return "pros"
        except:
            pass
        
        # Check common locations
        common_paths = [
            "/usr/local/bin/pros",
            "/opt/homebrew/bin/pros",
        ]
        
        # Add user-specific paths
        user_home = os.path.expanduser(f"~{os.environ.get('SUDO_USER', '')}")
        common_paths.extend([
            os.path.join(user_home, "Library/Python/3.9/bin/pros"),
            os.path.join(user_home, "Library/Python/3.8/bin/pros"),
            os.path.join(user_home, ".local/bin/pros"),
        ])

        for path in common_paths:
            if os.path.exists(path):
                return path
        
        return "pros" # Fallback to name and let it fail if not found

    @staticmethod
    def run(cmd, description, dry_run=False, capture=True):
        if dry_run:
            console.print(rf"[warning]\[DRY RUN][/warning] Would run: [info]{' '.join(cmd)}[/info]")
            return True
        
        # Inject full path for 'pros' if it's the first element
        env = os.environ.copy()
        if cmd and cmd[0] == "pros":
            pros_path = ProcessManager.get_pros_cmd()
            cmd[0] = pros_path
            if os.path.isabs(pros_path):
                pros_dir = os.path.dirname(pros_path)
                env["PATH"] = f"{pros_dir}:{env.get('PATH', '')}"
        
        try:
            subprocess.run(cmd, check=True, capture_output=capture, text=True, env=env)
            return True
        except subprocess.CalledProcessError as e:
            console.print(f"[error]Error during {description}:[/error]")
            if capture:
                console.print(e.stderr)
            return False

    @staticmethod
    def get_output(cmd):
        try:
            result = subprocess.run(cmd, check=True, capture_output=True, text=True)
            return result.stdout.strip()
        except:
            return ""

def show_header():
    console.print(VORTEX_ASCII)
    console.print(Panel("[brand]VORTEX CROSS-BRANCH RELEASE AUTOMATION[/brand]", expand=False, border_style="brand"))

@click.command()
@click.option("--major", is_flag=True, help="Increment major version")
@click.option("--minor", is_flag=True, help="Increment minor version")
@click.option("--patch", is_flag=True, default=True, help="Increment patch version (default)")
@click.option("--version", help="Specify a custom version string")
@click.option("--dry-run", is_flag=True, help="Preview actions without executing")
@click.option("--no-push", is_flag=True, help="Commit and tag locally, but do not push to remote")
def main(major, minor, patch, version, dry_run, no_push):
    show_header()
    
    # Check current branch
    current_branch = ProcessManager.get_output(["git", "rev-parse", "--abbrev-ref", "HEAD"])
    if current_branch != DEV_BRANCH and not dry_run:
        console.print(f"[error]Error:[/error] You must be on the [brand]{DEV_BRANCH}[/brand] branch to release.")
        console.print(f"Current branch: [warning]{current_branch}[/warning]")
        sys.exit(1)

    vm = VersionManager()
    current = vm.get_current_version()
    
    # Determine target version
    if version:
        target = version
    else:
        part = "patch"
        if major: part = "major"
        elif minor: part = "minor"
        target = vm.increment(current, part)
    
    console.print(f"Current Version: [version]{current}[/version]")
    console.print(f"Target Version:  [version]{target}[/version]")
    console.print(f"Release Branch:  [brand]{STABLE_BRANCH}[/brand]")
    console.print("")

    if not dry_run:
        if not Confirm.ask("Proceed with cross-branch release?"):
            console.print("[warning]Release cancelled by user.[/warning]")
            sys.exit(0)

    # 1. Update Version File on DEV
    with Progress(SpinnerColumn(), TextColumn("[progress.description]{task.description}"), console=console) as progress:
        task = progress.add_task(description=f"Updating {DEV_BRANCH} version...", total=None)
        if not dry_run:
            vm.update_file(target)
            ProcessManager.run(["git", "add", "Makefile"], "git add Makefile", dry_run)
            ProcessManager.run(["git", "commit", "-m", f"chore: bump version to {target} on {DEV_BRANCH}"], "git commit version", dry_run)
        progress.update(task, completed=True, description=f"{DEV_BRANCH} version updated!")

    # 2. Sync to STABLE
    with Progress(SpinnerColumn(), TextColumn("[progress.description]{task.description}"), console=console) as progress:
        task = progress.add_task(description=f"Synchronizing to {STABLE_BRANCH}...", total=None)
        
        # Checkout stable
        if not ProcessManager.run(["git", "checkout", STABLE_BRANCH], f"checkout {STABLE_BRANCH}", dry_run):
            sys.exit(1)
        
        # Pull production files from dev
        sync_cmd = ["git", "checkout", DEV_BRANCH, "--"] + PROD_PATHS
        if not ProcessManager.run(sync_cmd, "sync production files", dry_run):
            sys.exit(1)
            
        # Commit sync
        if not ProcessManager.run(["git", "add", "."], "git add sync", dry_run):
            sys.exit(1)
        if not ProcessManager.run(["git", "commit", "-m", f"chore: sync v{target} from {DEV_BRANCH}"], "git commit sync", dry_run):
            sys.exit(1)
            
        progress.update(task, completed=True, description=f"{STABLE_BRANCH} synchronization complete!")

    # 3. Build Template (on stable)
    with Progress(SpinnerColumn(), TextColumn("[progress.description]{task.description}"), console=console) as progress:
        task = progress.add_task(description="Building PROS template...", total=None)
        
        # We pass PROS path to make via environment variable to handle Makefile calls
        # AND we update PATH so sub-shells can find 'pros'
        env = os.environ.copy()
        pros_path = ProcessManager.get_pros_cmd()
        env["pros"] = pros_path
        
        if os.path.isabs(pros_path):
            pros_dir = os.path.dirname(pros_path)
            env["PATH"] = f"{pros_dir}:{env.get('PATH', '')}"
        
        try:
            subprocess.run([pros_path, "make", "template"], check=True, capture_output=True, text=True, env=env)
        except subprocess.CalledProcessError as e:
            console.print("[error]Error during PROS build:[/error]")
            console.print(e.stderr)
            # Try to return to dev before exit
            ProcessManager.run(["git", "checkout", DEV_BRANCH], "cleanup checkout dev", False)
            sys.exit(1)
            
        progress.update(task, completed=True, description="PROS template built!")

    # 4. Tag and Push
    with Progress(SpinnerColumn(), TextColumn("[progress.description]{task.description}"), console=console) as progress:
        task = progress.add_task(description=f"Finalizing v{target}...", total=None)
        
        # Tag on stable
        tag_name = f"v{target}"
        if not ProcessManager.run(["git", "tag", "-a", tag_name, "-m", f"Release {tag_name}"], "git tag", dry_run):
            sys.exit(1)
        
        # Push
        if not no_push:
            # Push stable
            if not ProcessManager.run(["git", "push", "origin", STABLE_BRANCH], "git push stable", dry_run):
                sys.exit(1)
            # Push tag
            if not ProcessManager.run(["git", "push", "origin", tag_name], "git push tag", dry_run):
                sys.exit(1)
            # Switch back to dev and push
            if not ProcessManager.run(["git", "checkout", DEV_BRANCH], "checkout dev", dry_run):
                sys.exit(1)
            if not ProcessManager.run(["git", "push", "origin", DEV_BRANCH], "git push dev", dry_run):
                sys.exit(1)
        else:
            # Always switch back to dev
            ProcessManager.run(["git", "checkout", DEV_BRANCH], "checkout dev", dry_run)
        
        progress.update(task, completed=True, description="Release finalized!")

    # Final Summary
    table = Table(title="Cross-Branch Release Summary", show_header=False, border_style="brand")
    table.add_row("Status", "[success]SUCCESS[/success]" if not dry_run else "[warning]DRY RUN COMPLETE[/warning]")
    table.add_row("Version", f"[version]{target}[/version]")
    table.add_row("Target Branch", f"[brand]{STABLE_BRANCH}[/brand]")
    table.add_row("Source Branch", f"{DEV_BRANCH}")
    
    console.print("")
    console.print(table)
    console.print(f"\n[brand]Vortex v{target} has been synced to {STABLE_BRANCH} and tagged![/brand] 🚀")

if __name__ == "__main__":
    main()
