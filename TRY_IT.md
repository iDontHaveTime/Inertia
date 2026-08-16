# Using Inertia

## Quick overview

This section will just explain some stuff to know before starting.
Inertia here is referring to the `inr` folder.

### Libraries
Inertia is currently divided into these libraries:
 - InrCore
 - InrCLI
 - InrIR
 - InrTarget
 - InrTIR

And then the target libraries which currently is:
 - InrX86

The first and the main library being `InrCore`, every library requires this one, but it can also be used by itself without any other library as just a regular support library.

Second library is `InrCLI` which is technically irrelevant here, but it provides a convenient way to handle CLI args and allows the arg map to be compile-time so the only real runtime spent on it is the parsing itself (then actually using them).

Third would be `InrIR` which provides IR implementations such as the TypeMap, instruction methods, IR verifier, etc..

And fourth library is `InrTarget` which provides target related base classes, such as target descriptions, lowering methods, etc..

Then the last one is `InrTIR` which is the main way of lowering Inertia's IR.

Finally the target libraries, they provide the actual methods for TIR to use to lower, such as registers, features, calling conventions, etc..

---

## Where to start

In this section we will create a file to create a simple return 0 program.
In here we would need these libraries:
 - InrCore
 - InrIR

### Goal
Our goal here would be to make a program resembling this C code:
```c
int main(void) {
    return 0;
}
```

### Starting
The first step would be actually creating the C++ file, once you made one in it we could start by adding some basic classes that we will need, which would be something like this:
```cpp
#include <inr/IR/TUnit.h>
#include <inr/IR/TypeMap.h>

int main(void) {
    inr::TUnit unit("TRY_IT.md");
    inr::TypeMap map;

    return 0;
}
```
First lets start from the headers, we need those headers to actually get the `TypeMap` and `TUnit` classes, then in our main function we declare them in there.

The `TUnit` class represents a translation unit, kinda like a source file in C, this will hold all of our functions.
Then the `TypeMap` class is what provides us with types, such as integers, floats, void, etc..

### Creating main
Next step would be to actually add a function, in our case we would add main with the signature of:
```c
int main(void);
```
Which would mean we would need these headers:
```cpp
#include <inr/IR/ArgDef.h>
#include <inr/IR/FuncDef.h>
#include <inr/IR/Linkage.h>
```
So then we actually create a function, we do that by using the `createFunction(...)` method in our unit.
We are not gonna use args for now so all we need here is just the return type, and linkage type.
For this we would assume int is a signed 32bit integer, and the linkage type is global.
```cpp
FuncDef* main_f = unit.createFunction(
        map.getFunc(/* Return type */ map.getI32(), /* Args */ {}, /* Is vararg */ false), /* Name for the function */ "main",
        /* Linkage */ inr::Linkage::Global, /* Extending return type */ inr::TypeExt::SignExt);
```
Now to break this down a little, creating a function returns `FuncDef*` and its signature consists of the function type, name, linkage, and return extension.

The return type in our case is i32 as mentioned previously, we have no args, and our function is not vararg. The name is main, with global linkage, and the return is signed extended (since int is a signed integer).

So our C++ file should look like this now:
```cpp
#include <inr/IR/ArgDef.h>
#include <inr/IR/FuncDef.h>
#include <inr/IR/Linkage.h>
#include <inr/IR/TUnit.h>
#include <inr/IR/TypeMap.h>

int main(void) {
    inr::TUnit unit("TRY_IT.md");
    inr::TypeMap map;

    FuncDef* main_f = unit.createFunction(
        map.getFunc(map.getI32(), {}, false), "main", 
        inr::Linkage::Global, inr::TypeExt::SignExt);

    return 0;
}
```
Everything will clean up, you should not worry about freeing pointers here.

### Returning 0
This is the final step for getting the program to match our C one.

For this we would need these headers:
```cpp
#include <inr/IR/BlockDef.h>
#include <inr/IR/InstDef.h>
#include <inr/Math/BigInt.h>
```

So before we create our instruction we need an entry block to put it in, to create one we would use the `createBlock(...)` method once again in the unit.
```cpp
BlockDef* entry_b = unit.createBlock(map, main_f, "entry");
```
In here we create a block with the name entry to our main function.

After that we can finally put our return instruction which would be via a static method under it, like this:
```cpp
inr::RetInst::createRet(map, entry_b, unit.createConst(map.getI32(), inr::bigint(32, 0)));
```
In here we create a return instruction to our block, and then we also create an integer constant of type i32, and then we pass in an 0 integer.
Bigint is Inertia's arbitrary precision integer class, in this case the constructor is just `inr::bigint(bitwidth, uint64_t value)`.

So this C++ file should represent the C program that is mentioned above:
```cpp
#include <inr/IR/ArgDef.h>
#include <inr/IR/BlockDef.h>
#include <inr/IR/FuncDef.h>
#include <inr/IR/InstDef.h>
#include <inr/IR/Linkage.h>
#include <inr/IR/TUnit.h>
#include <inr/IR/TypeMap.h>
#include <inr/Math/BigInt.h>

int main(void) {
    inr::TUnit unit("TRY_IT.md");
    inr::TypeMap map;

    FuncDef* main_f = unit.createFunction(
        map.getFunc(map.getI32(), {}, false), "main", 
        inr::Linkage::Global, inr::TypeExt::SignExt);

    BlockDef* entry_b = unit.createBlock(map, main_f, "entry");

    inr::RetInst::createRet(map, entry_b, unit.createConst(map.getI32(), inr::bigint(32, 0)));

    return 0;
}
```

### Verifying
Now this step in this case is optional, but normally shouldn't be skipped.

Verifying is really simple, you only need this header:
```cpp
#include <inr/IR/Verifier.h>
```
Then after you have built your whole unit, you can verify it as a whole using this function:
```cpp
inr::Verifier::verify(unit, &inr::out());
```
Which takes your unit, then the stream to output the errors to.

So if you added verifying your C++ file should have something like this:
```cpp
if(!inr::Verifier::verify(unit, &inr::out())) {
    return 1;
}
```
**After** creating your instructions.

### Printing
If you want to see your IR in textual form, you could print it out to a file or a standard stream.
We will use stdout for this example.

For this we would need this header:
```cpp
#include <inr/IR/Printer.h>
```

Then printing it out is as simple as declaring the class then passing in the unit and stream like this:
```cpp
inr::IRPrinter ir_printer(unit);
ir_printer.print(inr::out());
```

---

## Lowering

This section will cover lowering IR, in here we would need these libraries:
 - InrCore
 - InrIR
 - InrTarget
 - InrTIR
 - Your host target library (for example InrX86 on x86 targets)

### Getting the target
In this example we use the host target.

The headers you will require:
```cpp
#include <inr/Target/Host.h>
#include <inr/Target/TargetDesc.h>
```

So this is super straightforward, all it takes is querying the registry for the host target which is done with this line:
```cpp
std::unique_ptr<TargetInfo> info = inr::TargetRegistry::getDesc(inr::host::getTarget());
```
Be sure to check that it's not null before proceeding.

### Translating to TIR
First and foremost we should lower our IR much closer to target and to a different IR entirely.

To do that we would need these headers:
```cpp
#include <inr/TIR/Translator.h>
#include <inr/TIR/TModule.h>
```

This is really simple as well, you pass in the target, IR unit, and it returns a module.
```cpp
inr::Translator ir_to_tir(info.get());
TModule tir_mod = ir_to_tir.translate(unit);
```

### Printing TIR
This step is super similar to printing IR, and all we need is this header:
```cpp
#include <inr/TIR/Printer.h>
```

Then to print it its incredibly similar to IR just in here we also pass in our target.
```cpp
inr::TIRPrinter tir_printer(tir_mod, info.get());
tir_printer.print(inr::out());
```

---

## Lowering Further

This is not yet implemented.