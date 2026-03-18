# Contributing to Inertia

Thank you for your interest in contributing! Inertia is open source, and contributions are always welcome.

At this stage, Inertia is early in development, so the main ways to contribute are:

### 1 - Reporting issues or bugs
 - If something doesn’t work as expected, open an issue describing what happened, the steps to reproduce it, and any relevant context.

### 2 - Suggesting improvements or features
 - Any ideas to improve or add new stuff to Inertia are always welcome, open an issue to discuss those changes.

### 3 - Contributing code
 - Fork the repository, make your changes, and submit a pull request.
 - Keep changes modular and minimal, focus on one feature or fix per PR.
 - Include tests or examples if possible.

**Commit Message Style:**

`[Commit type]: Title describing the commit.`
 - Types: `feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `chore`, `build`, `ci`, `revert`.
 - Example from a previous commit: `[docs]: Improved docs on the lexer.`

PRs may be returned if the commit messages do not follow this style.

**License comments:**

Include this license comment at the top of any new C++ header or source file:
```cpp
// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
```

### 4 - Documentation
 - Improvements to the README, DESIGN, CONTRIBUTING, etc. are very welcome.
 - Documenting code with doxygen comments is a must, and please follow the project's style.
 - Clear explanations of new instructions or targets are especially useful for future contributors.

---

## Code Style

 - Inertia has a `.clang-format` file at the root of the repository.
 - Please use it to format all code before submitting a pull request.
 - Follow existing naming conventions and code structure to keep the codebase consistent.

The order of formatting (e.g. Before commit, before PR, etc..) does not matter as long as the PR has the code formatted correctly by `.clang-format`.

---

## Notes

 - Inertia uses C++20. Contributors should ideally be comfortable with modern C++.
 - Larger contributions may be discussed before implementation to ensure they fit the project’s architecture.