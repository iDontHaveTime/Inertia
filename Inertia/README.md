# Inertia Backend

## Overview
This folder contains everything related to the Inertia backend, which is responsible for:
- Handling the IR
- Lowering the IR
- Optimizations
- Generating the final product
- Providing an API for frontends

---

## Directories
- `include/` - Public headers for the backend
- `lib/` - Backend implementation (C++ source files)
- `src/` - Mainly used for internal testing, not part of the final library
- `tools/` - (Planned) Source files for the tools using the backend