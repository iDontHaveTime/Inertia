// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Math/BigInt.h>
#include <inr/Support/Stream.h>

#include <string_view>

int bits_test_popcount(inr::bigint& b, unsigned bw) {
    if(b.popcount() != bw) {
        inr::err() << "popcount should be bitwidth after setBits()\n";
        return 1;
    }
    return 0;
}

int bits_test_ones(inr::bigint& b, unsigned bw) {
    if(b.countlo() != bw) {
        inr::err() << "leading ones should be bitwidth after setBits()\n";
        return 1;
    }

    if(b.countro() != bw) {
        inr::err() << "trailing ones should be bitwidth after setBits()\n";
        return 1;
    }
    return 0;
}

int bits_test_zeros(inr::bigint& b, unsigned bw) {
    if(b.countlz() != bw) {
        inr::err() << "leading zeros should be bitwidth\n";
        return 1;
    }

    if(b.countrz() != bw) {
        inr::err() << "trailing zeros should be bitwidth\n";
        return 1;
    }
    return 0;
}

int bits_test_lhs_cmp_more(inr::bigint::CmpRes res, std::string_view msg) {
    if(!(res & inr::bigint::ABOVE)) {
        inr::err() << msg << ": "
                   << "unsigned comparison should return above as 0b1... is "
                      "bigger than 0b0...\n";
        inr::err() << "Got: " << (void*)res << " instead\n";
        return 1;
    }

    if(!(res & inr::bigint::LESS)) {
        inr::err()
            << msg << ": "
            << "signed comparison should return less as -1 is less than 0\n"
            << "Got: " << (void*)res << " instead\n";
        return 1;
    }
    return 0;
}

int bits_test_equal(inr::bigint::CmpRes res, std::string_view msg) {
    if(res != inr::bigint::EQUAL) {
        inr::err() << msg << ": " << "comparison should be equal\n";
        return 1;
    }
    return 0;
}

/// @brief Tests the bigint with the given width.
int bigint_test(unsigned bw) {
    inr::bigint b(bw);
    inr::out() << bw << '\n';
    if(!b.isZero()) {
        inr::err() << "bigint should initialize to zero\n";
        return 1;
    }
    if(int r = bits_test_zeros(b, bw)) return r;

    b.setBits();

    if(int r = bits_test_popcount(b, bw)) return r;
    if(int r = bits_test_ones(b, bw)) return r;

    if(int r = bits_test_lhs_cmp_more(b.cmp(0), "b.cmp(0)")) return r;

    inr::bigint cmpT(b);
    cmpT.clearBits();

    if(int r = bits_test_lhs_cmp_more(b.cmp(cmpT), "b.cmp(cmpT)")) return r;

    if(int r = bits_test_equal(b.cmp(b), "b.cmp(b)")) return r;

    cmpT |= b;
    if(b != cmpT) {
        inr::err() << "bigints should be equal\n";
        return 1;
    }

    b ^= cmpT;
    if(!b.isZero()) {
        inr::err() << "b should be zero after xor\n";
        return 1;
    }

    b.add(bw);
    if(b != bw) {
        inr::err() << "b should equal bw\n";
        return 1;
    }

    b.sub(bw + 1);
    if(b.cmp(-1, true) != inr::bigint::EQUAL) {
        inr::err() << "b should be -1\n";
        inr::err() << "b is: ";
        b.print(inr::err(), 16, false, true, true);
        inr::err() << '\n';
        return 1;
    }

    b.add(b);
    if(b.cmp(-2, true) != inr::bigint::EQUAL) {
        inr::err() << "b should be -2\n";
        inr::err() << "b is: ";
        b.print(inr::err(), 16, false, true, true);
        inr::err() << '\n';
        return 1;
    }

    return 0;
}

int main() {
    for(unsigned i = 1; i <= 0x1000; i++) {
        if(unsigned res = bigint_test(i)) return res;
    }

    return 0;
}
