# Contributing to Inertia

Thank you for being interested in contributing to Inertia! Your contributions help make this modular compiler infrastructure even better.

## Communication and Issues
- For any big change or a new feature, please open an issue first to discuss it.
- This makes sure that work is aligned with the goals of this project.
- Small fixes, such as typos or minor docs updates, can be submitted directly as PRs.

## Licensing and Copyright
- By submitting a contribution, you agree that your changes are licensed under the **Boost Software License 1.0 (BSL 1.0)**, the same as the rest of the project.

## Testing
- Before submitting a pull request, please test your code with ASan (Address Sanitizer) to catch memory errors.
- Testing on other architectures is optional but appreciated.

## Documentation
- Contributions to documentation are appreciated.
- Whenever you add or modify a feature, consider updating the docs to make sure they align with the feature.

## Coding style
- Try to keep coding style as close as possible to the existing style.
- Contributors won't be denied for their coding style, but they may be modified to fit the project's style.
- The style would be explained later in the file.

## Feature Contributions
- New features are welcomed at this stage.
- Please open an issue first for significant contributions.
- The C/C++ frontend (inrcc) is not open for contributions yet, as development of it has not started yet.

---

# Coding Style Description

## Indentation and spacing
- **Indentation**: Use 4 spaces per indentation level.
- **Braces**: No spaces between control keywords and opening brace.
    - Correct: `if(cond){ /* ... */ }`
    - Incorrect: `if (cond) { /* ... */ }`
- **Loops**: Same rules apply to `for` and `while` loops.
    - Correct: `for(int i = 0; i < n; i++)` 
    - Prefer `i++` over `++i` for incrementing unless there's a solid reason to do otherwise.
- **Functions**: No space between parentheses and opening brace.
    - Correct: `int foo(){ /* ... */ }`

## Alignment
- **Comments**: Just allow the comments to flow naturally, alignment isn't required.
- **Equals**: The equals sign should not be aligned with other variables. Like shown below:
```cpp
int a = 1;
int abcdef = 2;
```

## Constructors
- **Semicolon**: This is a weird rule, but for one line constructor please instert a semicolon after the brace.
    - Example: `Class() : field(0){};`
- **Multi-line**: They are as usual, just follow previous rules.

## Flow
- Just let the code flow naturally, no need to overthink the style, it can always be changed after.
- Readability is more important than some style.
- The style might change as the project advances.

## .clang-format
- The reason this project doesn't use `.clang-format` is simple: it cannot fit the project's coding style.

## Changing the style
- I'm always open to style changes. If you feel something could be improved, open an issue and we can discuss it.
- The style isn't set in stone, this style is just what I've been using for all of my projects.
- Don't be afraid to suggest tweaks, but know that some parts might stay the way they are if they fit the flow.