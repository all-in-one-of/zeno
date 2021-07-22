#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <vector>

namespace zfx::x64 {

/*
 * () ~ * + << <= == & ^ | && || ?: = ,
 */

namespace opcode {
    enum {
        mov = 0x10,
        add = 0x58,
        sub = 0x5c,
        mul = 0x59,
        div = 0x5e,
        min = 0x5d,
        max = 0x5f,
        bit_and = 0x54,
        bit_andnot = 0x55,
        bit_or = 0x56,
        bit_xor = 0x57,
        sqrt = 0x51,
        loadu = 0x10,
        loada = 0x28,
        storeu = 0x11,
        storea = 0x29,
    };
};

namespace opreg {
    enum {
        mm0, mm1, mm2, mm3, mm4, mm5, mm6, mm7,
    };
    enum {
        rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi,
        r8, r9, r10, r11, r12, r13, r14, r15,
    };

    // Linux: https://stackoverflow.com/questions/18024672/what-registers-are-preserved-through-a-linux-x86-64-function-call
    // Windows: https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-160#parameter-passing
    // TL;DR: Linux use RDI, RSI, RDX, RCX, R8, R9; Windows use RCX, RDX, R8, R9
#if defined(_WIN32)
    enum {
        a1 = rcx,
        a2 = rdx,
        a3 = r8,
        a4 = r9,
    };
#else
    enum {
        a1 = rdi,
        a2 = rsi,
        a3 = rdx,
        a4 = rcx,
        a5 = r8,
        a6 = r9,
};
#endif
};

namespace memflag {
    enum {
        reg = 0x00,
        reg_reg = 0x04,
        reg_imm8 = 0x40,
        reg_imm32 = 0x80,
        reg_reg_imm8 = 0x44,
        reg_reg_imm32 = 0x84,
    };
};

namespace optype {
    enum {
        xmmps = 0xc0,
        xmmpd = 0xc1,
        xmmss = 0xc2,
        xmmsd = 0xc3,
        ymmps = 0xc4,
        ymmpd = 0xc5,
    };
};

struct SIMDBuilder {   // requires AVX2
    std::vector<uint8_t> res;

    struct MemoryAddress {
        int adr, mflag, immadr, adr2, adr2shift;

        MemoryAddress
        ( int adr
        , int mflag = memflag::reg
        , int immadr = 0
        , int adr2 = 0
        , int adr2shift = 0
        )
        : adr(adr)
        , mflag(mflag)
        , immadr(immadr)
        , adr2(adr2)
        , adr2shift(adr2shift)
        {}

        void dump(std::vector<uint8_t> &res, int val, int flag = 0) {
            if (mflag & (memflag::reg_imm8 | memflag::reg_imm32)) {
                mflag &= ~(memflag::reg_imm8 | memflag::reg_imm32);
                if (-128 <= immadr && immadr <= 127) {
                    mflag |= memflag::reg_imm8;
                } else {
                    mflag |= memflag::reg_imm32;
                }
            }
            flag |= mflag | val << 3 & 0x38 | adr;
            //if (adr == opreg::rsp)
                //flag |= 0x10;
            if (adr == opreg::rbp)
                flag |= memflag::reg_imm8;
            res.push_back(flag);
            if (adr == opreg::rsp)
                res.push_back(0x24);
            if (mflag & memflag::reg_reg) {
                res.push_back(adr2 | adr2shift << 6);
            }
            if (mflag & memflag::reg_imm8) {
                res.push_back(immadr & 0xff);
            } else if (mflag & memflag::reg_imm32) {
                res.push_back(immadr & 0xff);
                res.push_back(immadr >> 8 & 0xff);
                res.push_back(immadr >> 16 & 0xff);
                res.push_back(immadr >> 24 & 0xff);
            }
        }
    };

    static constexpr size_t scalarSizeOfType(int type) {
        switch (type) {
        case optype::xmmps: return sizeof(float);
        case optype::xmmpd: return sizeof(double);
        case optype::xmmss: return sizeof(float);
        case optype::xmmsd: return sizeof(double);
        case optype::ymmps: return sizeof(float);
        case optype::ymmpd: return sizeof(double);
        default: return 0;
        }
    }

    static constexpr size_t sizeOfType(int type) {
        switch (type) {
        case optype::xmmps: return 4 * sizeof(float);
        case optype::xmmpd: return 2 * sizeof(double);
        case optype::xmmss: return 1 * sizeof(float);
        case optype::xmmsd: return 1 * sizeof(double);
        case optype::ymmps: return 8 * sizeof(float);
        case optype::ymmpd: return 4 * sizeof(double);
        default: return 0;
        }
    }

    void addAvxBroadcastLoadOp(int type, int val, MemoryAddress adr) {
        res.push_back(0xc4);
        res.push_back(0xe2);
        res.push_back(0x79 | type << 2 & 0x04);
        res.push_back(0x18 | type >> 2 & 0x01);
        adr.dump(res, val);
    }

    void addAvxRoundOp(int type, int dst, int src, int opid) {
        res.push_back(0xc4);
        res.push_back(0xe3);
        res.push_back(0x79 | type << 2 & 0x04);
        res.push_back(0x08 | type >> 2 & 0x01);
        res.push_back(0xc0 | dst << 3 | src);
        res.push_back(opid);
    }

    void addAvxMemoryOp(int type, int op, int val, MemoryAddress adr) {
        res.push_back(0xc5);
        res.push_back(type | 0x38);
        res.push_back(op);
        adr.dump(res, val);
    }

    void addRegularLoadOp(int val, MemoryAddress adr) {
        res.push_back(0x48 | val >> 3);
        res.push_back(0x8b);
        adr.dump(res, val);
    }

    void addRegularStoreOp(int val, MemoryAddress adr) {
        res.push_back(0x48 | val >> 3);
        res.push_back(0x89);
        adr.dump(res, val);
    }

    void addRegularMoveOp(int dst, int src) {
        res.push_back(0x48 | dst >> 3 | src >> 1 & 0x04);
        res.push_back(0x89);
        res.push_back(0xc0 | dst & 0x07 | src << 3 & 0x38);
    }

    void addAdjStackTop(int imm_add) {
        res.push_back(0x48);
        res.push_back(0x83);
        res.push_back(0xc4);
        res.push_back(imm_add);
    }

    void addCallOp(MemoryAddress adr) {
        if (adr.adr & 0x08)
            res.push_back(0x41);
        res.push_back(0xff);
        adr.dump(res, 0, 0x10);
    }

    void addAvxBinaryOp(int type, int op, int dst, int lhs, int rhs) {
        res.push_back(0xc5);
        res.push_back(type | ~lhs << 3);
        res.push_back(op);
        res.push_back(0xc0 | dst << 3 | rhs);
    }

    void addAvxUnaryOp(int type, int op, int dst, int src) {
        addAvxBinaryOp(type, op, dst, 0, src);
    }

    void addAvxMoveOp(int type, int dst, int src) {
        addAvxBinaryOp(opcode::loadu, opcode::mov, dst, opreg::mm0, src);
    }

    void addPushReg(int reg) {
        res.push_back(0x50 | reg);
    }

    void addPopReg(int reg) {
        res.push_back(0x58 | reg);
    }

    void addReturn() {
        res.push_back(0xc3);
    }

    void printHexCode() const {
        for (auto const &i: res) {
            printf("%02X", i);
        }
        printf("\n");
    }

    auto const &getResult() const {
        return res;
    }
};

}
