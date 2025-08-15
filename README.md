# Inertia Project

## Overview
Inertia is a modular compiler framework inspired by LLVM.
It currently has:
- **Inertia**: The backend itself, which is a library, it handles the IR, Targets, Codegen, etc...
- **inrcc**: This is the planned frontend for C and C++ that uses Inertia as its backend.

The project is designed to be **modular**, so frontends can be added or customized independently of the backend.

---

## Directories
- `Inertia/` - The backend library, which includes the public headers and the implementation
- `inrcc/` - The planned C/C++ frontend that uses the Inertia backend

---

## Status
This is still very early in development. Right now the focus is on getting x86-64 target codegen working.