# Inertia

## What is Inertia?
Inertia is an experimental and modular backend compiler inspired by LLVM. It takes the IR and compiles it to various target architectures. Currently, no architectures are implemented yet, but I am working on it.

## How are architectures defined?
Architectures are defined using the Inertia Target format. This is a custom-made format that gets translated into C++ and then gets used for code generation. It defines things like registers, target name, endianness, and instructions. You can find it under `include/Inertia/Target`.

## Status
The project is currently focused on developing the Inertia Target DSL.