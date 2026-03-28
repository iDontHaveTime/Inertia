# Inertia

## What is Inertia?

Inertia is an open source compiler backend made for simplicity and flexibility rather than pure performance.

While the backend (IR, MIR, ISel, DAG, etc..) is under development, the DSL (`inr-gen`) is fully functional and is ready for codegen backends (generating C++ from the records).

## Inertia DSL (`inr-gen`)

The DSL was designed from the ground up to be a flexible way to represent repetitive data that would be annoying to write manually in C++. `inr-gen` was inspired by LLVM's Tablegen, thus uses more or less the same syntax and similar types, but the implementation is fully Inertia's.

Here is some example usage of `inr-gen`:
```c++
class A<string name>{
    string NameA = name;
}

class B<string name>{
    string NameB = name;
}

class C<string nameA, string nameB> : A<nameA>, B<nameB>;

def D : C<"A", "B">;
```

## Contributing

Since Inertia is open source, contributions are welcome. For guidelines and details, check out the [CONTRIBUTING](CONTRIBUTING.md) file.

## Status

Inertia is still early in development, but it’s already close to producing runnable x86-64 code.

## Directory Structure

Inertia's directory structure mirrors LLVM's directory structure.
For those who are unfamiliar with LLVM's directory structure, it goes as follows:
```sh
project-name/ # In this case it would be Inertia (inr)
 lib/ # Implementations for headers (C++)
 include/ # Headers
  project-name/ # C++ headers
  project-name-c/ # C bindings
```

## License

This project is licensed under the [**Boost Software License (BSL-1.0)**](LICENSE).