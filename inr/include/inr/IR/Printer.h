#ifndef INERTIA_IR_PRINTER_H
#define INERTIA_IR_PRINTER_H

/// @file IR/Printer.h
/// @brief Contains the IR printer class.

#include <inr/ADT/HMap.h>
#include <inr/IR/InstDef.h>
#include <inr/IR/TUnit.h>
#include <inr/Support/Stream.h>

#include <cstdint>

namespace inr {

/// @brief Prints out the IR.
class IRPrinter {
    uint32_t counter_;
    HMap<const Def*, uint32_t> defNames_;
    const TUnit& unit_;

public:
    IRPrinter(const TUnit& unit) : unit_(unit) {}

    stream& printDef(stream& os, const Def* def, bool prefix = true);

    void printRetInst(stream&, const RetInst&);
    void printJmpInst(stream&, const JmpInst&);
    void printCmpInst(stream&, const CmpInst&);
    void printBinaryInst(stream&, const BinaryInst&);
    void printPhi(stream&, const PhiInst&);
    void printLoad(stream&, const LoadInst&);
    void printStore(stream&, const StoreInst&);
    void printAlloca(stream&, const AllocaInst&);

    void printInstruction(stream&, const InstDef&);
    void printBlock(stream&, const BlockDef&);
    void printFunction(stream&, const FuncDef&);
    void printSignature(stream&, const FuncDef&);
    void print(stream& os);
};

} // namespace inr

#endif // INERTIA_IR_PRINTER_H
