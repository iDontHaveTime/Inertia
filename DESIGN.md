# Design Choices of Inertia

## ISel

### Target Tree

Inertia's instruction selection works by walking a target-specific tree with a target-agnostic walker, so to paint the picture clearly we can visualize a tree:
```md
                Root
              /      \
             /        \
            /          \
           /            \
         i32            i64
          |              |
         add            add
      /      \        /      \
     rr      ir      rr      ir
     |       |       |       |
  add32rr add32ir add64rr add64ir
```
This tree is a tree that follows the `Type, Instruction Type, Operand` order.
The order does not matter for the walker, the target can implement the order as the wish it to be.
Lets take for example an instruction like `add i32 2, %vreg` which means basically to add the integer 2 to a register that is soon to be allocated, and the type says it should be a 32 bit integer addition. 

Before explaining the walker's steps, we should make it clear on tree node types, as there are 3 of them. First node type is `Type`, this is a simple node that separates other nodes by their type, for example it would separate a 32 bit integer node and a 64 bit integer node.
The second node type is the `Instruction Type` node, which is what instruction type it is, so it could be addition, subtraction, return, store, load, and whatever more the IR has.
The third node type is the `Operand.` node, which just means what operands were requested, could be RR (register-register), IR (immediate-register), etc..

To continue the example above for the `add i32 2, %vreg` instruction, lets say the tree is in the `Type, Instruction Type, Operand` order. The walker starts off at the root node, then it requests the i32 type since the instruction is `i32` as seen and the node is a `Type` node, more on the scenario of it not finding it later. Then its in the i32 node, this node is now an `Instruction Type` node, our instruction type here is `add` so the walker requests a node that has the `add` instruction type, if found we advance to it. Now we are at the `Operand` node, same story here, the instruction is an IR type, no not intermediate representation, but immediate-register, which means if an instruction that is 32bit in width, an addition type, and has operands immediate-register is found, that is our instruction.

Technically 4th node type exists, which is the leaf node, it basically holds the final opcode and has no nodes that it holds, thus "leaf node" since its the last node in that branch.

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

Legalizing an instruction involved replacing the original instruction with another instruction or even instructions. In the example I gave above in the brackets which is `add i65 1, %vreg` its preferred to disallow that until later where the target is stable, but you could split that into multiple instructions, like 2 `i64`s, yes inefficient, but that's what happens when you pick those types of sizes.

### Rematerialization

If the instruction changes during a pass or regalloc, the pass should run the walker again for that instruction, so if the instruction was originally `ADDi32RR` but then an operand was spilled to stack, it needs to get a new node from the tree which would be `ADDi32RM` in this example.

This is why the tree has the `Operand` node type and not only `RR` everywhere.

### Pointers

The ISel gives an integer with the same width as the target's pointer width to the walker.

### Location

The implementation for this can be found at `inr/include/inr/ISel`.

## Note

**This can, and will change in the future.**