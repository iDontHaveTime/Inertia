# Inertia Project

## What is Inertia?

The Inertia Project is a compiler-related project that as of now contains two subprojects:
 - `Inertia`
 - `inrcc`

`Inertia` is the "main" purpose of this project, thus the name. It's a collection of libraries that form a backend compiler, as of now `Inertia` supports AOT compilation.

`inrcc` on the other hand is a C frontend targeting `Inertia`.

## Contributing

Since Inertia is open source, contributions are welcome. For guidelines and details, check out the [CONTRIBUTING](CONTRIBUTING.md) file.

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