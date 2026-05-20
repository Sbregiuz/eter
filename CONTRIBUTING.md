# Contributing to Eter

Thank you for your interest in contributing to Eter. Contributions of code,
documentation, tests, and bug reports are all welcome.

## Table of Contents

- [Ways to Contribute](#ways-to-contribute)
- [Requirements](#requirements)
  - [Software](#software)
  - [Installing LLVM, MLIR, lit, and FileCheck](#installing-llvm-mlir-lit-and-filecheck)
- [Source Checkout](#source-checkout)
- [The `x` Script](#the-x-script)
- [Building Eter (Manual CMake)](#building-eter-manual-cmake)
  - [Pointing CMake at LLVM and MLIR](#pointing-cmake-at-llvm-and-mlir)
  - [Sanitizers](#sanitizers)
  - [Debug Output](#debug-output)
  - [Clang-Tidy Build Integration (optional)](#clang-tidy-build-integration-optional)
- [IDE and LSP Integration](#ide-and-lsp-integration)
- [Running the Test Suite](#running-the-test-suite)
  - [Running All Tests (Recommended)](#running-all-tests-recommended)
  - [Running Unit Tests](#running-unit-tests)
  - [Running Lit (Integration) Tests](#running-lit-integration-tests)
  - [Adding New Tests](#adding-new-tests)
  - [Running the Driver](#running-the-driver)
- [API Documentation](#api-documentation)
- [Formatting and Linting](#formatting-and-linting)
  - [clang-format](#clang-format)
  - [clang-tidy](#clang-tidy)
- [Git Hooks](#git-hooks)
  - [pre-commit](#pre-commit)
  - [pre-push](#pre-push)
- [Before You Send a Change](#before-you-send-a-change)
- [Coding Style](#coding-style)
- [Submitting a Patch](#submitting-a-patch)
- [Common Problems](#common-problems)
- [Bug Reports and Review Context](#bug-reports-and-review-context)
- [Licensing and Headers](#licensing-and-headers)
- [Questions](#questions)

## Ways to Contribute

You can help the project in several ways:

- report bugs or confusing behavior
- improve documentation and examples
- add tests or developer tooling
- implement fixes, refactorings, dialects, and passes

## Requirements

### Software

Before building Eter, make sure the following tools are available:

- **CMake 3.20+**
- **Ninja** as the recommended generator
- **A modern C++-17-capable toolchain** such as AppleClang, Clang, or GCC
- **LLVM/MLIR 22.x**
- **`lit`** and **`FileCheck`** for the regression test suite

Eter is configured to require **C++-17** by default.

### Installing LLVM, MLIR, `lit`, and `FileCheck`

Eter currently expects **LLVM/MLIR 22.x**. The setup differs slightly by
platform.

#### macOS

On **macOS** with Homebrew, a practical setup is:

```bash
brew install llvm@22
python3 -m pip install --user lit
```

Notes:

- the Homebrew `llvm@22` package provides **LLVM**, **MLIR**, and tools such as
  **`FileCheck`**
- `lit` is often easiest to install with `pip`
- on Apple Silicon, the LLVM tools are typically under
  `/opt/homebrew/opt/llvm@22/bin`

If needed, make the LLVM tools visible in your shell:

```bash
export PATH="/opt/homebrew/opt/llvm@22/bin:$PATH"
```

#### Linux

On **Ubuntu/Debian**, a practical setup for **LLVM/MLIR 22.x** is to use the
`apt.llvm.org` repository and then install the required packages:

```bash
sudo apt update
sudo apt install -y wget lsb-release software-properties-common gnupg python3-pip
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 22
sudo apt install -y clang-22 lld-22 llvm-22-dev libmlir-22-dev mlir-22-tools
python3 -m pip install --user lit
```

Notes:

- `mlir-22-tools` provides utilities such as **`FileCheck`**
- on many systems, the LLVM tools are under `/usr/lib/llvm-22/bin`
- on non-Debian distributions, install the equivalent LLVM, MLIR, and
  **`FileCheck`** packages from your package manager or use an official LLVM
  binary release

If needed, make the LLVM tools visible in your shell:

```bash
export PATH="/usr/lib/llvm-22/bin:$PATH"
```

#### ArchLinux

On **ArchLinux** all packages can be easily installed using the **pacman**
package manager except for **mlir** which must be installed via an AUR
compatible package manager like **yay** or **paru**.

```bash
sudo pacman -S ninja cmake llvm clang python-pip
yay -S --needed mlir
python3 -m pip install --user lit
```

#### Verifying the Installation

You can verify the install with:

```bash
clang --version
FileCheck --version
lit --version
```

If your Linux distribution installs versioned binaries only, use the versioned
commands directly, such as `clang-22 --version`.

## Source Checkout

If you do not already have a local checkout, clone the repository and enter the
source tree:

```bash
git clone https://github.com/eter-lang/eter
cd eter
```

## The `x` Script

The repository includes an `x` script at the root that wraps the most common
CMake and tooling commands. It derives everything — `PATH`, compiler paths,
and CMake config directories — from a single `LLVM_PREFIX_PATH` variable,
which it auto-discovers if not set (via `llvm-config`, then `brew`, then a
hardcoded fallback).

| Task | `x` command |
|---|---|
| Configure (Debug) | `./x config` |
| Configure (Release) | `./x config Release` |
| Build | `./x build` |
| Re-configure + build from scratch | `./x rebuild` |
| Run all tests | `./x check-all` |
| Run lit tests | `./x check-eter` |
| Run unit tests | `./x test` |
| Run specific lit tests | `./x run-lit-test <path>` |
| Generate API docs | `./x doc-doxygen` |
| Check clang-format | `./x check-clang-format HEAD` |
| Apply clang-format | `./x clang-format HEAD` |
| Run clang-tidy | `./x clang-tidy HEAD` |
| Create `.clangd` file | `./x create-clangd` |
| Show configuration | `./x show-config` |
| Clean build directory | `./x clean` |

Run `./x help` for the full reference.

The `x` script respects the `LLVM_PREFIX_PATH` environment variable if you
need to point it at a non-default LLVM installation:

```bash
LLVM_PREFIX_PATH=/usr/lib/llvm-22 ./x config
```

## Building Eter (Manual CMake)

Eter uses an out-of-tree CMake build. A typical development build looks like
this:

```bash
cmake -G Ninja -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DLLVM_ENABLE_ASSERTIONS=ON
cmake --build build
```

This configures the project in `build/` and then builds the default targets.

### Pointing CMake at LLVM and MLIR

If CMake does not discover your LLVM/MLIR installation automatically, specify
it via `CMAKE_PREFIX_PATH` or explicitly with `LLVM_DIR` and `MLIR_DIR`.

For **macOS** with Homebrew:

```bash
cmake -G Ninja -S . -B build \
  -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/llvm@22 \
  -DCMAKE_C_COMPILER=/opt/homebrew/opt/llvm@22/bin/clang \
  -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm@22/bin/clang++
```

Or with explicit CMake config paths:

```bash
cmake -G Ninja -S . -B build \
  -DLLVM_DIR=/opt/homebrew/opt/llvm@22/lib/cmake/llvm \
  -DMLIR_DIR=/opt/homebrew/opt/llvm@22/lib/cmake/mlir
```

For **Linux** with `apt.llvm.org` packages:

```bash
cmake -G Ninja -S . -B build \
  -DLLVM_DIR=/usr/lib/llvm-22/lib/cmake/llvm \
  -DMLIR_DIR=/usr/lib/llvm-22/lib/cmake/mlir
```

### Sanitizers

To enable runtime sanitizer checks for supported Clang/GCC builds, add the
sanitizer options at configure time:

```bash
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Debug -DETER_ENABLE_SANITIZERS=ON
```

By default, Eter enables `address`, `undefined`, and `leak`. To customize the
sanitizers, set `-DETER_SANITIZERS="address;undefined;leak;thread"` or use
`memory` for MemorySanitizer.

### Debug Output

Eter provides a custom debug output mechanism (`ETER_DEBUG`) that is
independent of LLVM's debug system. It can be enabled or disabled at CMake
configure time via the `ETER_ENABLE_DEBUG` option.

**Default behavior:**

- `Debug` / `RelWithDebInfo` builds: `ETER_ENABLE_DEBUG=ON` (debug macros are active)
- `Release` / `MinSizeRel` builds: `ETER_ENABLE_DEBUG=OFF` (debug macros are stripped)

**Override from the CLI:**

```bash
# Force debug ON even in a Release build
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Release -DETER_ENABLE_DEBUG=ON

# Force debug OFF even in a Debug build
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Debug -DETER_ENABLE_DEBUG=OFF
```

When debug is enabled, the driver accepts `--debug` and `--debug-only=<type>`:

```bash
./build/tools/eter/eterc --debug-only=driver program.eter
```

When `ETER_ENABLE_DEBUG=OFF`, the `ETER_DEBUG(...)` macro expands to nothing
and the debug code is completely removed from the compiled binary.
The macro is defined in `include/eter/Base/Debug.h`.

### Clang-Tidy Build Integration (optional)

To reproduce warnings shown by `clangd`/`clang-tidy` in your editor or to run
static checks as part of the build, Eter provides an optional CMake flag that
integrates `clang-tidy` into the compilation pipeline.

- Requirements: have `clang-tidy` installed and available in your `PATH`.
- Enable: pass `-DETER_ENABLE_CLANG_TIDY=ON` when configuring with CMake.
- Caveats: enabling `clang-tidy` will slow down builds and may cause the build
  to fail if `clang-tidy` reports errors or warnings treated as errors.

```bash
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Debug -DETER_ENABLE_CLANG_TIDY=ON
cmake --build build
```

Recommendation: keep `ETER_ENABLE_CLANG_TIDY` off by default and enable it in
CI or locally when you want stricter checks.

## IDE and LSP Integration

For `clangd`-based IDE or LSP services, keep a `.clangd` file in the
repository root with the following content:

```yaml
CompileFlags:
  CompilationDatabase: build/ # Search build/ directory for compile_commands.json
```

This tells `clangd` to use the compilation database generated in `build/`.

The repository `.gitignore` already ignores `build*`, so in normal use there
is no need to add `build/` to a global Git ignore file.

## Running the Test Suite

Eter uses a **two-tier testing approach**:

1. **Unit tests** (in `unittests/`): Built with Google Test (gtest), covering
   individual functions and classes in the `Base`, `Driver`, and `Lexer`
   libraries.

2. **Integration/smoke tests** (in `test/`): Built with LLVM's `lit` framework
   and `FileCheck`, testing the compiler end-to-end via command-line
   invocations.

### Running All Tests (Recommended)

```bash
cmake --build build --target check-all
```

### Running Unit Tests

```bash
cmake --build build --target test
```

Or directly via CTest from the build directory:

```bash
cd build && ctest --output-on-failure
```

### Running Lit (Integration) Tests

```bash
cmake --build build --target check-eter
```

Or manually with `lit`:

```bash
lit -va ./build/test/
```

To run tests for a specific folder or file:

```bash
lit -a -s -v <folder-or-file>
```

### Adding New Tests

**Unit tests**: Add test code to the appropriate `.cpp` file in
`unittests/<Library>/`. Tests are consolidated into one executable per library
(e.g., `EterBaseTests`, `EterDriverTests`). Add new `TEST()` macros to the
existing files or include new files from the main test runner.

**Lit tests**: Add a new `.smoke` (or `.mlir`) file in the appropriate `test/`
subdirectory. Use the following template:

```text
// RUN: %eter <test-input> | %FileCheck %s
// CHECK: expected-output
```

Local test configuration can be customized per directory using `lit.local.cfg`
files.

### Running the Driver

To run the current driver executable directly:

```bash
./build/tools/eter/eterc --version
```

This is also a simple smoke check that the build completed successfully.

## API Documentation

The project supports Doxygen-based API documentation generation via the
`doc-doxygen` CMake target.

```bash
cmake --build build --target doc-doxygen
```

The generated HTML output is written to `build/doxygen/html/index.html`.

If Doxygen is not installed, CMake will configure normally and print a status
message indicating that `doc-doxygen` is disabled.

## Formatting and Linting

### clang-format

Eter uses LLVM-style `clang-format`. To check and apply formatting on the
staged diff:

```bash
# Check only (dry-run) — exits non-zero if changes are needed
git clang-format --diff HEAD

# Apply formatting to staged changes
git clang-format HEAD
```

To check or apply formatting on a specific commit range:

```bash
git clang-format --diff HEAD~1 HEAD
git clang-format HEAD~1
```

To run `clang-format` directly on individual files:

```bash
# Check without modifying
clang-format --dry-run --Werror <files>

# Apply in place
clang-format -i <files>
```

If `git clang-format` is not available, install LLVM/Clang and make sure its
tools are on your `PATH`. On Linux, install the Clang tooling package for your
distribution so that `clang-format` and `git-clang-format` are available.

### clang-tidy

Before running `clang-tidy`, configure the build so that
`build/compile_commands.json` exists:

```bash
cmake -G Ninja -S . -B build
```

To run `clang-tidy` on staged changes (useful for manual pre-commit checks):

```bash
clang-tidy -p build --warnings-as-errors='*' --header-filter='.*' \
  $(git diff --cached --name-only --diff-filter=ACMR | grep -E '\.(c|cc|cpp|cxx)$')
```

On macOS, where `clang-tidy` needs the SDK sysroot:

```bash
clang-tidy -p build --warnings-as-errors='*' --header-filter='.*' \
  $(git diff --cached --name-only --diff-filter=ACMR | grep -E '\.(c|cc|cpp|cxx)$') \
  -- -isysroot $(xcrun --show-sdk-path)
```

To run `clang-tidy` manually on a single file:

```bash
clang-tidy -p build path/to/file.cpp -- -Iinclude [other compiler args]
```

## Git Hooks

Eter includes native Git hooks under `.githooks/` so contributors can run the
basic formatting, linting, build, and test checks automatically before commit
and push operations.

Enable them with:

```bash
git config core.hooksPath .githooks
chmod +x .githooks/pre-commit .githooks/pre-push
```

You can also run the same checks manually at any time:

```bash
.githooks/pre-commit
.githooks/pre-push
```

Before enabling the hooks, make sure that:

- `clang-format` and `clang-tidy` are available on your `PATH`
- you have already configured the project so that
  `build/compile_commands.json` exists (`./x config` or `cmake -G Ninja -S . -B build`)

### `pre-commit`

The `pre-commit` hook checks:

- staged diff hygiene with `git diff --cached --check`
- `clang-format` for C and C++ headers/sources (via `./x check-clang-format HEAD`)
- `clang-tidy` for staged C and C++ source files (via `./x clang-tidy HEAD`)

### `pre-push`

The `pre-push` hook checks:

- full project build (via `./x build`)
- full test suite — unit tests and lit tests (via `./x check-all`)
- documentation build (via `./x doc-doxygen`)

## Before You Send a Change

Before opening a patch or pull request, please:

1. build the project locally
2. run the current regression suite
3. keep the change focused and reviewable
4. add tests when behavior changes
5. format code consistently with the project style

A typical local validation flow:

```bash
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
cmake --build build --target check-all
```

Or with the `x` script:

```bash
./x config
./x build
./x check-all
```

## Coding Style

Eter follows LLVM-style conventions where practical:

- `clang-format` is based on LLVM style
- `clang-tidy` is configured for LLVM/MLIR-oriented development
- naming and structure should stay consistent with the surrounding code

Please avoid unrelated formatting or cleanup changes in the same patch unless
those changes are the main purpose of the review.

## Submitting a Patch

When your change is ready:

1. create a branch for the work
2. commit the change with a clear message
3. open a pull request
4. respond to review feedback promptly and incrementally

Small, isolated patches are much easier to review than large mixed changes.

## Common Problems

### LLVM/MLIR version mismatch

If you have multiple LLVM installations, ensure that `LLVM_DIR` and `MLIR_DIR`
point to the same 22.x install, or set `LLVM_PREFIX_PATH` to the correct
prefix before running `./x config`.

### Missing `lit` or `FileCheck`

Make sure those tools are installed and reachable through your `PATH`, or
provided by your LLVM installation.

### Linker warnings on macOS

Depending on the local Xcode or Homebrew setup, you may see warnings about LLVM
search paths. These should be investigated if they turn into hard failures.

### CMake cannot find LLVM or MLIR

Pass the config paths explicitly (see [Pointing CMake at LLVM and
MLIR](#pointing-cmake-at-llvm-and-mlir)) or use `./x config` which sets
`CMAKE_PREFIX_PATH` automatically from `LLVM_PREFIX_PATH`.

## Bug Reports and Review Context

When reporting an issue or asking for review, include as much concrete context
as possible:

- operating system and compiler
- LLVM/MLIR version
- exact CMake configure command
- the failing command or test
- a minimal reproducer if available

## Licensing and Headers

New source or configuration files should include the project's Apache 2.0 with
LLVM-exception header where appropriate.

## Questions

If something is unclear, prefer opening a discussion early rather than
guessing. Early feedback is usually cheaper than a late rewrite.
