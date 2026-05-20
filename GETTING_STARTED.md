# Getting Started with Eter

This guide covers two paths:

- **[Contributing to the compiler](#contributing-to-the-compiler)** — set up the
  build environment, build Eter from source, and run the test suite.
- **[Using the language](#using-the-language)** — run the `eterc` driver and
  explore the language reference.

---

## Contributing to the Compiler

This section gives you the fastest path from a fresh checkout to a working
build. For the full contributor workflow — coding style, formatting, hooks,
patch submission — see [CONTRIBUTING.md](./CONTRIBUTING.md).

### Prerequisites

- CMake 3.20+, Ninja, a C++17-capable compiler
- LLVM/MLIR 22.x and `lit`

Platform-specific installation instructions are in
[CONTRIBUTING.md § Requirements](./CONTRIBUTING.md#requirements).

### Clone

```bash
git clone https://github.com/eter-lang/eter
cd eter
```

### Build and Test

The `x` script at the root handles LLVM path discovery and wraps all common
CMake commands:

```bash
./x config      # configure (Debug by default)
./x build       # compile
./x check-all   # run unit tests + lit tests
```

That's the full cycle. If everything passes you are ready to work.

> **Non-default LLVM path?** Pass `LLVM_PREFIX_PATH` before the command:
> ```bash
> LLVM_PREFIX_PATH=/usr/lib/llvm-22 ./x config
> ```
> Run `./x help` for all available commands.

### IDE Setup

Generate a `.clangd` file so your editor finds the right compilation database:

```bash
./x create-clangd
```

### Git Hooks (optional but recommended)

Enable pre-commit formatting/linting and pre-push build+test checks:

```bash
git config core.hooksPath .githooks
chmod +x .githooks/pre-commit .githooks/pre-push
```

### What's Next

See [CONTRIBUTING.md](./CONTRIBUTING.md) for:

- manual CMake invocations and advanced build flags
- sanitizers and debug output
- clang-format and clang-tidy usage
- how to write and run tests
- coding style and patch submission guidelines

---

## Using the Language

> **Eter is in early development.** The language design and implementation are
> subject to significant changes. The compiler is not yet feature-complete.

### Language Reference

The primary source of truth for Eter syntax and semantics is the
[Eter Reference](https://eter-lang.github.io/eter/).

The [API Reference](https://eter-lang.github.io/eter/api/) provides the
Doxygen-generated documentation for the compiler's C++ internals, intended for
contributors and advanced users.

### Running the Driver

After building, the `eterc` driver is available at:

```bash
./build/tools/eter/eterc --version
```

To enable debug output for a specific subsystem:

```bash
./build/tools/eter/eterc --debug-only=driver program.eter
```

Debug output is only available in `Debug` and `RelWithDebInfo` builds (the
default when using `./x config`).
