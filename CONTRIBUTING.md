# Contributing to Inertia

Thank you for being interested in contributing to Inertia.

To start contributing to Inertia you follow the usual procedures being fork, new branch, commit then PR.

## Steps to PR

This section describes what you should be aware of when working on your contribution.

### After you fork
Make sure you have created a new branch, preferrably that describes the feature or fix you are making.

### Before you commit
Before you commit make sure your commit message matches the commit message style which is shown below.

**Commit Message Style:**
`[Project][Component] Title describing the commit.`
 - Projects: `inr` (Inertia), `inrcc`.
 - Example commit: `[inrcc][driver] Added new args to be more compatible with gcc.`

### Before you PR
After you've done the commits you wanted you should format your code if your editor doesn't already do it.
The root folder contains a `.clang-format` that you should use to format your code.

---

## Making changes

In this section you'll read about some rules in actually making code for Inertia.

### Creating new files
If you create a new C++ source file you should make sure it is added to the sources in `CMakeLists.txt` files.

The created files should fit into the directories they are created, so for example C++ source files go to `lib/` and the headers go into the `include/` folder.

Regardless of header or source, every C++ file should have a comment on top with the project's license, the license is shown below.

**License Comment:**
```cpp
// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
```

### Documenting code
Adding documentation to your new code is very important so that other people understand how to use your new feature, but you shouldn't have to document every line you write.

You can also contribute by adding documentation to existing code if you understand it, or ask the person who implemented something so that you or them can document that.

Not all parts need documentation, for example a function like:
```cpp
static inline void inrccDriverPrintHelp();
```
Is obvious in what it does, it prints out help (the `--help` flag) for inrcc.

**Note:** This is not a real function.

Although even if it's obvious adding comments to the function is still helpful and can be a useful contribution.

**Comment Style:**
This is important, as Inertia's comments use either `//` or `///` depending on location.

Starting off with `///` those comments appear to explain something and so that doxygen generates it.
This can also vary, as for example for inline documentation it would be `///<`.
Here's an example for both of these comments:
```cpp
/// @brief Does something VERY useful.
/// @note Make sure it's useful.
enum class SuperUsefulEnum {
    Useful, ///< This makes it super useful.
};
```

Then another comment is `//` which is used for implementation documentation as shown below.
```cpp
void useSuperUseful(const SuperUsefulClass& useful) {
    // Recurse because it's useful.
    useSuperUseful(useful);
}
```

### Major changes
If your change is major make sure it has an issue that you or others have made, and it's approved by other people.
Really make sure your new feature is tested and is ready to be merged, do not submit half done features unless you explicitly mention you are gonna finish it, and even then it's better to finish it in your fork.

---

## Notes

This section contains some useful info you should probably know.

### Standard
Currently Inertia (both `inr` and `inrcc`) use the C++20 standard, and there is no reason to change it.
Although if you feel like the standard should be something that is not C++20 feel free to discuss it.

### CMake
Inertia heavily utilizes CMake to manage its sources, so you should be comfortable with using, reading, and writing CMake files.
The `CMakeLists.txt` files should be modular, using one root one is not recommended (Although probably won't be rejected).

### Changing the root folder
If you want to change any files in the root folder, such as `CONTRIBUTING.md`, `README.md`, `.gitignore`, etc..
Feel free to discuss it and do it.

You can also request some `.clang-format` changes, but most likely they won't be accepted.
This shouldn't stop you from trying tho.

### AI
Uh oh hot topic, use of AI isn't forbidden but discouraged if its obvious.
You could use AI to discuss stuff such as changes, or concepts, but coding itself is really preferred to be from a human.

### New projects
Inertia currently has 2 projects, being the infrastructure which is technically Inertia itself in the folder named `inr`, and the second one is `inrcc`.

Technically adding new projects isn't forbidden, but since Inertia is still in its early stage, its too eager to add anything new.
And many new stuff can be added under `tools/` folders instead if they are not major.

### License
Inertia uses the Boost Software License 1.0 and there's no intention of changing it.