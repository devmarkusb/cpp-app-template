# cpp-app-template

<!-- markdownlint-disable-next-line line-length -->
![Continuous Integration Tests](https://github.com/devmarkusb/cpp-app-template/actions/workflows/ci.yml/badge.svg) ![Lint Check (pre-commit)](https://github.com/devmarkusb/cpp-app-template/actions/workflows/pre-commit-check.yml/badge.svg) [![Coverage](https://coveralls.io/repos/github/devmarkusb/cpp-app-template/badge.svg?branch=main)](https://coveralls.io/github/devmarkusb/cpp-app-template?branch=main)

Minimal C++ **application** template: a small static library in `libs/core/`, an executable in
`apps/main.cpp`, plus CMake presets, tests (GoogleTest), CI, and the
[`devenv`](https://github.com/devmarkusb/devenv) submodule for shared tooling.

## Quick start

Initialize submodules, then configure and build (CMake 3.30+, C++26 on GCC/Clang/AppleClang
and C++23 on MSVC presets; Ninja):

```bash
git submodule update --init --recursive

cmake --preset gcc-debug
cmake --build build/gcc-debug
ctest --preset gcc-debug
```

Sync submodules later: `devenv/git-sub.sh`.

Pinned dependencies use the repo-root `fetchcontent-lockfile.json`; details are in `devenv/README.md`.

## New project from this template

Prefer generating a tree with the script (names, paths, optional README badges, fresh git + `devenv` submodule):

```bash
python3 scripts/new-cpp-app.py \
  --dest ~/dev/apps/my-new-app \
  --vendor acme \
  --app my-widget \
  --github OWNER/my-new-app \
  --fresh-git
```

Use **`--template /path/to/cpp-app-template`** for a local checkout instead of cloning GitHub. You can
also use GitHub “Use this template”, init submodules, then rename using the top-of-file note in
`CMakeLists.txt`.

## CMake

| Option                            | Default             | Purpose                                                 |
|-----------------------------------|---------------------|---------------------------------------------------------|
| `MB_CPP_APP_TEMPLATE_BUILD_TESTS` | `ON` when top-level | Build `mb.cpp-app-template.test` and GoogleTest wiring. |

`CMakePresets.json` lists compiler presets (e.g. `gcc-debug`, `msvc-release`).
