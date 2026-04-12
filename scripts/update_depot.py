import json
import sys
import os

if len(sys.argv) < 2:
    print("Usage: python3 update_depot.py <version>")
    sys.exit(1)

version = sys.argv[1].replace("v", "") # Remove 'v' if present
repo = os.environ.get("GITHUB_REPOSITORY", "jonahchang207/Vortex")

# Load existing
try:
    with open("stable.json", "r") as f:
        depot = json.load(f)
except FileNotFoundError:
    depot = []

# New entry
new_entry = {
    "metadata": {
        "location": f"https://github.com/{repo}/releases/download/v{version}/vortex@{version}.zip"
    },
    "name": "vortex",
    "py/object": "pros.conductor.templates.base_template.BaseTemplate",
    "supported_kernels": "^4.0.0",
    "target": "v5",
    "version": version
}

depot.insert(0, new_entry)

with open("stable.json", "w") as f:
    json.dump(depot, f, indent=2)
