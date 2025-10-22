/* Inertia's includes. */
#include "inr/Support/inrint.hpp"

namespace inr{

void inrint::add(const inrint& other) noexcept{
    if(!valid() || !other.valid()) return;

    uint32_t* lhs_limbs;
    size_t lhs_count;
    if(on_stack()){
        lhs_limbs = (uint32_t*)&stack_base;
        lhs_count = (bits + 31) / 32;
    } 
    else{
        lhs_limbs = heap_base;
        lhs_count = cycles();
    }

    const uint32_t* rhs_limbs;
    size_t rhs_count;

    if(other.on_stack()){
        rhs_limbs = (const uint32_t*)&other.stack_base;
        rhs_count = 2;
    } 
    else{
        rhs_limbs = other.heap_base;
        rhs_count = other.cycles();
    }

    for(size_t i = 0; i < lhs_count; i++){
        uint64_t sum = uint64_t(lhs_limbs[i]) + (i < rhs_count ? rhs_limbs[i] : 0);
        lhs_limbs[i] = uint32_t(sum);
    }

    mask_trailing();
}

void inrint::lbit_shift(size_t n) noexcept{
    if(!valid() || n == 0) return;

    size_t limb_shift = n >> 5;
    size_t bit_shift = n & 31;

    uint32_t* limbs;
    size_t limb_count;

    if(on_stack()){
        limbs = (uint32_t*)&stack_base;
        limb_count = (bits + 31) >> 5;
    }
    else{
        limbs = heap_base;
        limb_count = cycles();
    }

    if(limb_shift){
        for(size_t i = limb_count; i-- > 0;){
            limbs[i] = (i >= limb_shift) ? limbs[i - limb_shift] : 0;
        }
    }
    if(bit_shift){
        uint32_t carry = 0;
        for(size_t i = 0; i < limb_count; i++){
            uint32_t new_carry = limbs[i] >> (32 - bit_shift);
            limbs[i] = (limbs[i] << bit_shift) | carry;
            carry = new_carry;
        }
    }

    mask_trailing();
}

void inrint::rbit_shift(size_t n) noexcept{
    if(!valid() || n == 0) return;

    size_t limb_shift = n >> 5;
    size_t bit_shift = n & 31;

    uint32_t* limbs;
    size_t limb_count;

    if(on_stack()){
        limbs = (uint32_t*)&stack_base;
        limb_count = (bits + 31) >> 5;
    }
    else{
        limbs = heap_base;
        limb_count = cycles();
    }

    if(limb_shift){
        for(size_t i = 0; i < limb_count; i++){
            limbs[i] = (i + limb_shift < limb_count) ? limbs[i + limb_shift] : 0;
        }
    }

    if(bit_shift){
        uint32_t carry = 0;
        for(size_t i = limb_count; i-- > 0;){
            uint32_t new_carry = limbs[i] << (32 - bit_shift);
            limbs[i] = (limbs[i] >> bit_shift) | carry;
            carry = new_carry;
        }
    }

    mask_trailing();
}

}