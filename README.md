# Inertia Project

## What is Inertia?

In this section you'll read about Inertia and what it is.

### Compiler or Compiler backend?
Inertia can refer to two things, one being the infrastructure/backend which is in the `inr` folder.
The other one can refer to this project as a whole.

As for the question itself, depends, like `inrcc` is a full compiler from source to machine.
But Inertia as in the infrastructure, is a collection of libraries which in fact do lower IR to machine code but it also contains libraries independent of lowering, so Inertia can be used outside of compiler context.

### inr
As mentioned above this is the infrastructure/backend, the reason for the slash is that it is both.
So whether you want to use this as a backend for your compiler or just a general support library, feel free to do it.

### inrcc
Now this is a full compiler, this is meant to be a C/C++ compiler that uses Inertia as its backend.
It is completely optional, Inertia has zero dependency on `inrcc`.

---

## Working with Inertia

### Using Inertia
Feel at home using Inertia whether it's `inr` or `inrcc` or anything else, you can use it in your projects, experiments, or anything else you want.

All Inertia components are modular and are made with libraries, so you can fork Inertia and modify it however you want.

### Contributing
If you are willing to spend your time helping Inertia to go forward, you can read this [CONTRIBUTING](CONTRIBUTING.md) file.

### First time using it?
There is a file meant specifically explaning how to use Inertia almost step by step in [TRY_IT](TRY_IT.md).

---

## Extra

Some extra info for you to know.

### License
Inertia uses the Boost Software License 1.0.