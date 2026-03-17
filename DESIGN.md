# Design Choices of Inertia

## ISel

### Target Tree

Inertia's instruction selection works by walking a target-specific tree with a target-agnostic walker, so to paint the picture clearly we can visualize a tree:
```md
           Instruction Type
               /      \
              /        \
           add         mov
          / | \          \
         /  |  \          \
        /   |   \          \
  Regi32-Regi32-Memi32      \
      \   /    \   /         \
    addi32RR  addi32RM      /|\
                           / | \
                          /  |  \
                         /   |   \
                        /    |    \
                       /     |     \
                   Regi32-Regi32-Memi32
                       \   /    \   /
                     movi32RR  movi32RM 
```
This tree works by separating the instruction types first, after the separation we are left with only operands and their types.
The operands follow a `op src, dest` order (like in GAS syntax assembly).
Which means in the tree we can see two types of instructions `add` and `mov`, both of them have 3 operands, 32bit register and 32bit memory (32bit memory destination not size).
So this means for example a function has this signature:
```llvm
define void @foo(i32 %arg){
entry:
  ret void;
}
```
Obviously it's not gonna generate this instruction, but lets say it did decide to spill the `i32 %arg` variable.
So the expected assembly (x86-64, AT&T Syntax, SystemV ABI) would be along the lines of this:
```x86asm
subq $4, %rsp
movl %edi, (%rsp)
addq $4, %rsp
```
We are gonna ignore the stack adjustment for now, so for that we need to move from RDI to (%RSP) which is a register to memory move, it's a 32bit move so we need to find a mov instruction with the source being a 32bit register and destination being 32bit memory.
So to find that instruction we need to use the walker (`inr::Walker`), we put in our request, being mov instruction, and those 2 operands (Reg i32, Mem i32), then it should walk the tree that you gave it trying to find that instruction. 
It should output the `movi32RM` instruction from the tree shown above.

**NOTE:** This example is not the x86 tree, this is the tree that is shown above, just using x86 for an example.


### Instruction Not Found

This is a scenario where the instruction the walker tried finding was not found.
In this scenario the ideal outcome is that the walker throws and says what instruction it didn't find, where it is in the tree, and more information for us to be able to solve that.

There are 2 ways to solve that:
 - New Instruction
 - Legalizer

**New Instruction:**

This is a super straightforward solution, if the target has an instruction that matches the walker's requirements, add it to the tree.

**Legalizer:**

Every target should have a legalizer, it is a must, because for example the instruction wasn't found we can either restrict usage of that instruction if the instruction is rarely needed (e.g. `add i65 1, %vreg` since why would you need `i65` in most code), or legalize it.

Legalizing an instruction involved replacing the original instruction with another instruction or even instructions. In the example I gave above in the parentheses which is `add i65 1, %vreg` it's preferred to disallow that until later where the target is stable, but you could split that into multiple instructions, like 2 `i64`s, yes inefficient, but that's what happens when you pick those types of sizes.

### Rematerialization

If the instruction changes during a pass or regalloc, the pass should run the walker again for that instruction, so if the instruction was originally `addi32RR` but then an operand was spilled to stack, it needs to get a new node from the tree which would be `addi32RM` in this example.

This is why the tree has the `Operand` node type and not only `RR` everywhere.

### Pointers

The ISel gives an integer with the same width as the target's pointer width to the walker.

### Location

The implementation for this can be found at `inr/include/inr/ISel`.

## Note

**This can, and will change in the future.**