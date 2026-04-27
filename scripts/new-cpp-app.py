#!/usr/bin/env python3
"""Instantiate a new C++ application from cpp-app-template (this repo).

Replaces vendor/app names, renames paths and files, optionally rewrites README
badges for your GitHub repo, and can re-init git with a fresh devenv submodule.

Example:
    python3 scripts/new-cpp-app.py \\
        --dest ~/src/acme-my-widget \\
        --vendor acme \\
        --app my-widget \\
        --github acme-corp/acme-my-widget
"""

from __future__ import annotations

import argparse
import re
import shutil
import subprocess
import sys
from pathlib import Path

# Template identity (must match this repository)
OLD_VENDOR = "mb"
OLD_APP_KEBAB = "cpp-app-template"
OLD_CMAKE_PROJECT = f"{OLD_VENDOR}.{OLD_APP_KEBAB}"
OLD_CMAKE_ALIAS = f"{OLD_VENDOR}::{OLD_APP_KEBAB}"
OLD_CXX_NS = "mb::cpp_app_template"
OLD_OPT = "MB_CPP_APP_TEMPLATE"
OLD_INTERNAL = "_mb_cpp_app_template"
OLD_INCLUDE = f"{OLD_VENDOR}/{OLD_APP_KEBAB}"
OLD_HEADER = f"{OLD_APP_KEBAB}.hpp"
OLD_CPP = f"{OLD_APP_KEBAB}.cpp"
OLD_TEST_CPP = f"{OLD_APP_KEBAB}.test.cpp"

OLD_CMAKELISTS_HEADER = """# CMake project `mb.cpp-app-template` — options `MB_CPP_APP_TEMPLATE_*`, C++ `mb::cpp_app_template`, helper lib under `libs/core/`.
"""

OLD_README_USAGE_SECTION = """## Syncing with the upstream template

This repo was generated with `scripts/new-cpp-app.py`. To pull structural improvements from the original template, run:

    git remote add template https://github.com/devmarkusb/cpp-app-template.git
    git fetch template

Upstream repository: [devmarkusb/cpp-app-template](https://github.com/devmarkusb/cpp-app-template).

"""


def die(msg: str, code: int = 1) -> None:
    print(f"new-cpp-app.py: {msg}", file=sys.stderr)
    raise SystemExit(code)


def validate_segment(name: str, kind: str) -> None:
    if not re.fullmatch(r"[a-z][a-z0-9-]*", name):
        die(
            f"{kind} {name!r} is invalid: use lowercase letters, digits, hyphens; "
            "must start with a letter (one segment, no dots — the CMake project is vendor.app)."
        )


def kebab_to_cxx_namespace(vendor: str, app_kebab: str) -> str:
    v = vendor.replace("-", "_")
    a = app_kebab.replace("-", "_")
    return f"{v}::{a}"


def cmake_option_prefix(cmake_project: str) -> str:
    return cmake_project.upper().replace(".", "_").replace("-", "_")


def cmake_internal_prefix(cmake_project: str) -> str:
    return "_" + cmake_project.replace(".", "_").replace("-", "_")


def copy_template(src: Path, dest: Path) -> None:
    if dest.exists():
        die(f"destination {dest} already exists; remove it or pick another path.")
    ignore = shutil.ignore_patterns(
        ".git",
        "build",
        ".idea",
        ".venv",
        "__pycache__",
        "*.pyc",
        ".DS_Store",
    )
    shutil.copytree(src, dest, ignore=ignore, symlinks=False)


def git_clone_template(url: str, dest: Path) -> None:
    if dest.exists():
        die(f"destination {dest} already exists.")
    dest.parent.mkdir(parents=True, exist_ok=True)
    r = subprocess.run(
        [
            "git",
            "clone",
            "--depth",
            "1",
            "--recurse-submodules",
            url,
            str(dest),
        ],
        check=False,
    )
    if r.returncode != 0:
        die("git clone failed; check the URL and network access.")


def should_skip_dir(path: Path) -> bool:
    parts = path.parts
    if ".git" in parts:
        return True
    if "build" in parts and parts[0] == "build":
        return True
    return False


def is_probably_text(data: bytes) -> bool:
    if b"\x00" in data[:4096]:
        return False
    return True


def patch_readme_badge_line(readme: Path, owner_repo: str) -> None:
    """Rewrite only the CI/coveralls badge line (still points at devmarkusb/... in the template)."""
    if not readme.is_file():
        return
    lines = readme.read_text(encoding="utf-8").splitlines(keepends=True)
    old = "devmarkusb/cpp-app-template"
    new = owner_repo
    for i, line in enumerate(lines):
        if "![Continuous Integration Tests]" in line and old in line:
            lines[i] = line.replace(old, new)
            break
    else:
        return
    readme.write_text("".join(lines), encoding="utf-8")


def replace_in_files(root: Path, replacements: list[tuple[str, str]]) -> None:
    # Longest keys first to avoid partial overlaps
    reps = sorted(replacements, key=lambda x: len(x[0]), reverse=True)
    for path in root.rglob("*"):
        if path.is_dir() or should_skip_dir(path.relative_to(root)):
            continue
        try:
            data = path.read_bytes()
        except OSError:
            continue
        if not is_probably_text(data):
            continue
        text = data.decode("utf-8")
        original = text
        for old, new in reps:
            if old in text:
                text = text.replace(old, new)
        if text != original:
            path.write_text(text, encoding="utf-8")


def rename_paths(root: Path, moves: list[tuple[Path, Path]]) -> None:
    # Sort by depth descending so we move children before parents when needed
    moves_sorted = sorted(moves, key=lambda m: len(m[0].parts), reverse=True)
    for src, dst in moves_sorted:
        full_src = root / src
        full_dst = root / dst
        if not full_src.exists():
            continue
        full_dst.parent.mkdir(parents=True, exist_ok=True)
        full_src.rename(full_dst)


def relocate_include_headers(
    root: Path,
    old_vendor: str,
    old_app: str,
    new_vendor: str,
    new_app: str,
) -> None:
    """Move include/<vendor>/<app>/ and rename the primary public header inside."""
    src_dir = root / "include" / old_vendor / old_app
    dst_dir = root / "include" / new_vendor / new_app
    if not src_dir.exists():
        return
    dst_dir.parent.mkdir(parents=True, exist_ok=True)
    src_dir.rename(dst_dir)
    old_h = dst_dir / f"{old_app}.hpp"
    new_h = dst_dir / f"{new_app}.hpp"
    if old_h.exists() and old_h != new_h:
        old_h.rename(new_h)
    # Drop empty include/<old_vendor> if possible
    old_vdir = root / "include" / old_vendor
    if old_vdir.is_dir() and old_vendor != new_vendor:
        try:
            old_vdir.rmdir()
        except OSError:
            pass


def fresh_git_with_devenv(dest: Path, devenv_url: str) -> None:
    git_dir = dest / ".git"
    if git_dir.exists():
        shutil.rmtree(git_dir)
    subprocess.run(
        ["git", "-C", str(dest), "init", "-b", "main"],
        check=True,
    )
    devenv = dest / "devenv"
    if devenv.exists():
        shutil.rmtree(devenv)
    subprocess.run(
        ["git", "-C", str(dest), "submodule", "add", devenv_url, "devenv"],
        check=True,
    )
    subprocess.run(
        ["git", "-C", str(dest), "submodule", "update", "--init", "--recursive"],
        check=True,
    )


def git_initial_commit(dest: Path) -> None:
    """Create one commit so `gh repo create --source=. --push` works."""
    subprocess.run(["git", "-C", str(dest), "add", "-A"], check=True)
    st = subprocess.run(
        ["git", "-C", str(dest), "diff", "--cached", "--quiet"],
        check=False,
    )
    if st.returncode == 0:
        return
    r = subprocess.run(
        [
            "git",
            "-C",
            str(dest),
            "commit",
            "-m",
            "Initial import from cpp-app-template",
        ],
        capture_output=True,
        text=True,
        check=False,
    )
    if r.returncode != 0:
        die(
            "Could not create initial commit (set git user.name and user.email). "
            'Then run: git add -A && git commit -m "Initial import from cpp-app-template"'
        )


def main() -> None:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument(
        "--template",
        type=Path,
        help="Path to cpp-app-template checkout (default: clone --clone-url).",
    )
    p.add_argument(
        "--clone-url",
        default="https://github.com/devmarkusb/cpp-app-template.git",
        help="Git URL to clone if --template is omitted (default: upstream template).",
    )
    p.add_argument(
        "--dest",
        type=Path,
        required=True,
        help="New project directory (must not exist).",
    )
    p.add_argument(
        "--vendor",
        required=True,
        help="First CMake segment / include segment, e.g. mb, acme (lowercase, no dots).",
    )
    p.add_argument(
        "--app",
        dest="app_kebab",
        required=True,
        help="Kebab-case application name, e.g. cpp-app-template, my-widget (becomes vendor.app).",
    )
    p.add_argument(
        "--github",
        metavar="OWNER/REPO",
        help="If set, README CI badge URLs use github.com/OWNER/REPO/....",
    )
    p.add_argument(
        "--fresh-git",
        action="store_true",
        help="Remove .git and run git init + submodule add devenv (clean history), "
        "then an initial commit so gh repo create --push works.",
    )
    p.add_argument(
        "--devenv-url",
        default="https://github.com/devmarkusb/devenv.git",
        help="Submodule URL for devenv (with --fresh-git).",
    )
    args = p.parse_args()

    validate_segment(args.vendor, "vendor")
    validate_segment(args.app_kebab, "app")

    new_cmake = f"{args.vendor}.{args.app_kebab}"
    new_alias = f"{args.vendor}::{args.app_kebab}"
    new_cxx = kebab_to_cxx_namespace(args.vendor, args.app_kebab)
    new_opt = cmake_option_prefix(new_cmake)
    new_internal = cmake_internal_prefix(new_cmake)
    new_include = f"{args.vendor}/{args.app_kebab}"
    new_header = f"{args.app_kebab}.hpp"
    new_cpp = f"{args.app_kebab}.cpp"
    new_test = f"{args.app_kebab}.test.cpp"

    template_root = args.template
    if template_root is None:
        git_clone_template(args.clone_url, args.dest)
        root = args.dest
    else:
        template_root = template_root.resolve()
        if not (template_root / "scripts" / "new-cpp-app.py").is_file():
            die(f"--template {template_root} does not look like cpp-app-template.")
        copy_template(template_root, args.dest)
        root = args.dest.resolve()

    relocate_include_headers(
        root, OLD_VENDOR, OLD_APP_KEBAB, args.vendor, args.app_kebab
    )

    moves: list[tuple[Path, Path]] = [
        (Path("apps/main.cpp"), Path(f"apps/{new_cpp}")),
    ]
    rename_paths(root, moves)

    cmakelists_header_new = (
        f"# CMake project `{new_cmake}` — options `{new_opt}_*`, "
        f"C++ `{new_cxx}`, headers under `include/{new_include}/`.\n"
    )

    readme_usage_new = """## Syncing with the upstream template

This repo was generated with `scripts/new-cpp-app.py`. To pull structural improvements from the original template, run:

    git remote add template https://github.com/devmarkusb/cpp-app-template.git
    git fetch template

Upstream repository: [devmarkusb/cpp-app-template](https://github.com/devmarkusb/cpp-app-template).

"""

    replacements: list[tuple[str, str]] = [
        (OLD_CMAKELISTS_HEADER, cmakelists_header_new),
        (OLD_README_USAGE_SECTION, readme_usage_new),
        (OLD_CMAKE_ALIAS, new_alias),
        (OLD_CMAKE_PROJECT, new_cmake),
        (OLD_CXX_NS, new_cxx),
        (OLD_INCLUDE, new_include),
        (OLD_OPT, new_opt),
        (OLD_INTERNAL, new_internal),
        (OLD_HEADER, new_header),
        (OLD_CPP, new_cpp),
        (OLD_TEST_CPP, new_test),
        ("apps/main.cpp", f"apps/{new_cpp}"),
        ("# cpp-app-template", f"# {args.app_kebab}"),
        (
            'DESCRIPTION "cpp-app-template example application"',
            f'DESCRIPTION "{args.app_kebab} example application"',
        ),
    ]

    if args.github:
        if not re.fullmatch(r"[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+", args.github):
            die("--github must look like OWNER/REPO")

    replace_in_files(root, replacements)

    if args.github:
        patch_readme_badge_line(root / "README.md", args.github)

    if args.fresh_git:
        fresh_git_with_devenv(root, args.devenv_url)
        git_initial_commit(root)

    print(f"Created project at {root}")
    if not args.github:
        print(
            "Tip: search for devmarkusb/cpp-app-template in README.md and set your CI badge URLs,"
            " or re-run with --github OWNER/REPO."
        )
    print(
        "Next: cd there, cmake --preset gcc-debug && cmake --build build/gcc-debug && ctest --preset gcc-debug"
    )


if __name__ == "__main__":
    main()
