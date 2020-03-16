#include "instruction.h"
#include "instructions/alu16.h"
#include "instructions/alu8.h"
#include "instructions/bits.h"
#include "instructions/call.h"
#include "instructions/jump.h"
#include "instructions/misc.h"
#include "instructions/transfer16.h"
#include "instructions/transfer8.h"

Instruction instructions[0x100] = {
    /* instruction-size, menmonic, instruction-fun */
    { 1, "nop", misc::nop },                        // [0x00]
    { 3, "ld bc, d16", tr16::ld_reg_imm<BC> },      // [0x01]
    { 1, "ld (bc), a", tr8::ld_bc_a },              // [0x02]
    { 1, "inc bc", alu16::inc<BC> },                // [0x03]
    { 1, "inc b", alu8::inc<B> },                   // [0x04]
    { 1, "dec b", alu8::dec<B> },                   // [0x05]
    { 2, "ld b, d8", tr8::ld_reg_imm<B> },          // [0x06]
    { 1, "rlca", bits::rlc<A, false> },             // [0x07]
    { 3, "ld (a16), sp", tr16::st_sp },             // [0x08]
    { 1, "add hl, bc", alu16::add_hl<BC> },         // [0x09]
    { 1, "ld a,(bc)", tr8::ld_a_bc },               // [0x0a]
    { 1, "dec bc", alu16::dec<BC> },                // [0x0b]
    { 1, "inc c", alu8::inc<C> },                   // [0x0c]
    { 1, "dec c", alu8::dec<C> },                   // [0x0d]
    { 2, "ld c, d8", tr8::ld_reg_imm<C> },          // [0x0e]
    { 1, "rrca", bits::rrc<A, false> },             // [0x0f]
    { 1, "stop", unimplemented },                   // [0x10]
    { 3, "ld de, d16", tr16::ld_reg_imm<DE> },      //
    { 1, "ld (de), a", tr8::ld_de_a },              //
    { 1, "inc de", alu16::inc<DE> },                //
    { 1, "inc d", alu8::inc<D> },                   //
    { 1, "dec d", alu8::dec<D> },                   //
    { 2, "ld d, d8", tr8::ld_reg_imm<D> },          //
    { 1, "rla", bits::rl<A, false> },               //
    { 2, "jr r8", jp::jump<true> },                 //
    { 1, "add hl, de", alu16::add_hl<DE> },         //
    { 1, "ld a, (de)", tr8::ld_a_de },              //
    { 1, "dec de", alu16::dec<DE> },                //
    { 1, "inc e", alu8::inc<E> },                   //
    { 1, "dec e", alu8::dec<E> },                   //
    { 2, "ld e, d8", tr8::ld_reg_imm<E> },          //
    { 1, "rra", bits::rr<A, false> },               //
    { 2, "jr nz, r8", jp::jump_if<true, FLAG_NZ> }, //
    { 3, "ld hl, d16", tr16::ld_reg_imm<HL> },      //
    { 1, "ld (hl+), a", tr8::ld_hli_a },            //
    { 1, "inc hl", alu16::inc<HL> },                //
    { 1, "inc h", alu8::inc<H> },                   //
    { 1, "dec h", alu8::dec<H> },                   //
    { 2, "ld h, d8", tr8::ld_reg_imm<H> },          //
    { 1, "daa", misc::nop },                        //
    { 2, "jr z, r8", jp::jump_if<true, FLAG_Z> },   //
    { 1, "add hl, hl", alu16::add_hl<HL> },         //
    { 1, "ld a, (hl+)", tr8::ld_a_hli },            //
    { 1, "dec hl", alu16::dec<HL> },                //
    { 1, "inc l", alu8::inc<L> },                   //
    { 1, "dec l", alu8::dec<L> },                   //
    { 2, "ld l, d8", tr8::ld_reg_imm<L> },          //
    { 1, "cpl", misc::cpl },                        //
    { 2, "jr nc, r8", jp::jump_if<true, FLAG_NC> }, //
    { 3, "ld sp, d16", tr16::ld_reg_imm<SP> },      //
    { 1, "ld (hl-), a", tr8::ld_hld_a },            //
    { 1, "inc sp", alu16::inc<SP> },                //
    { 1, "inc (hl)", alu8::inc<HL> },               //
    { 1, "dec (hl)", alu8::dec<HL> },               //
    { 2, "ld (hl), d8", tr8::ld_reg_imm<HL> },      //
    { 1, "scf", misc::scf },                        //
    { 2, "jr c, r8", jp::jump_if<true, FLAG_C> },   //
    { 1, "add hl, sp", alu16::add_hl<SP> },         //
    { 1, "ld a, (hl-)", tr8::ld_a_hld },            //
    { 1, "dec sp", alu16::dec<SP> },                //
    { 1, "inc a", alu8::inc<A> },                   //
    { 1, "dec a", alu8::dec<A> },                   //
    { 2, "ld a, d8", tr8::ld_reg_imm<A> },          //
    { 1, "ccf", misc::ccf },                        //
    { 1, "ld b, b", tr8::ld_reg_reg<B, B> },        //
    { 1, "ld b, c", tr8::ld_reg_reg<B, C> },        //
    { 1, "ld b, d", tr8::ld_reg_reg<B, D> },        //
    { 1, "ld b, e", tr8::ld_reg_reg<B, E> },        //
    { 1, "ld b, h", tr8::ld_reg_reg<B, H> },        //
    { 1, "ld b, l", tr8::ld_reg_reg<B, L> },        //
    { 1, "ld b, (hl)", tr8::ld_reg_reg<B, HL> },    //
    { 1, "ld b, a", tr8::ld_reg_reg<B, A> },        //
    { 1, "ld c, b", tr8::ld_reg_reg<C, B> },        //
    { 1, "ld c, c", tr8::ld_reg_reg<C, C> },        //
    { 1, "ld c, d", tr8::ld_reg_reg<C, D> },        //
    { 1, "ld c, e", tr8::ld_reg_reg<C, E> },        //
    { 1, "ld c, h", tr8::ld_reg_reg<C, H> },        //
    { 1, "ld c, l", tr8::ld_reg_reg<C, L> },        //
    { 1, "ld c, (hl)", tr8::ld_reg_reg<C, HL> },    //
    { 1, "ld c, a", tr8::ld_reg_reg<C, A> },        //
    { 1, "ld d, b", tr8::ld_reg_reg<D, B> },        //
    { 1, "ld d, c", tr8::ld_reg_reg<D, C> },        //
    { 1, "ld d, d", tr8::ld_reg_reg<D, D> },        //
    { 1, "ld d, e", tr8::ld_reg_reg<D, E> },        //
    { 1, "ld d, h", tr8::ld_reg_reg<D, H> },        //
    { 1, "ld d, l", tr8::ld_reg_reg<D, L> },        //
    { 1, "ld d, (hl)", tr8::ld_reg_reg<D, HL> },    //
    { 1, "ld d, a", tr8::ld_reg_reg<D, A> },        //
    { 1, "ld e, b", tr8::ld_reg_reg<E, B> },        //
    { 1, "ld e, c", tr8::ld_reg_reg<E, C> },        //
    { 1, "ld e, d", tr8::ld_reg_reg<E, D> },        //
    { 1, "ld e, e", tr8::ld_reg_reg<E, E> },        //
    { 1, "ld e, h", tr8::ld_reg_reg<E, H> },        //
    { 1, "ld e, l", tr8::ld_reg_reg<E, L> },        //
    { 1, "ld e, (hl)", tr8::ld_reg_reg<E, HL> },    //
    { 1, "ld e, a", tr8::ld_reg_reg<E, A> },        //
    { 1, "ld h, b", tr8::ld_reg_reg<H, B> },        //
    { 1, "ld h, c", tr8::ld_reg_reg<H, C> },        //
    { 1, "ld h, d", tr8::ld_reg_reg<H, D> },        //
    { 1, "ld h, e", tr8::ld_reg_reg<H, E> },        //
    { 1, "ld h, h", tr8::ld_reg_reg<H, H> },        //
    { 1, "ld h, l", tr8::ld_reg_reg<H, L> },        //
    { 1, "ld h, (hl)", tr8::ld_reg_reg<H, HL> },    //
    { 1, "ld h, a", tr8::ld_reg_reg<H, A> },        //
    { 1, "ld l, b", tr8::ld_reg_reg<L, B> },        //
    { 1, "ld l, c", tr8::ld_reg_reg<L, C> },        //
    { 1, "ld l, d", tr8::ld_reg_reg<L, D> },        //
    { 1, "ld l, e", tr8::ld_reg_reg<L, E> },        //
    { 1, "ld l, h", tr8::ld_reg_reg<L, H> },        //
    { 1, "ld l, l", tr8::ld_reg_reg<L, L> },        //
    { 1, "ld l, (hl)", tr8::ld_reg_reg<L, HL> },    //
    { 1, "ld l,a", tr8::ld_reg_reg<L, A> },         //
    { 1, "ld (hl), b", tr8::ld_reg_reg<HL, B> },    //
    { 1, "ld (hl), c", tr8::ld_reg_reg<HL, C> },    //
    { 1, "ld (hl), d", tr8::ld_reg_reg<HL, D> },    //
    { 1, "ld (hl), e", tr8::ld_reg_reg<HL, E> },    //
    { 1, "ld (hl), h", tr8::ld_reg_reg<HL, H> },    //
    { 1, "ld (hl), l", tr8::ld_reg_reg<HL, L> },    //
    { 1, "halt", misc::halt },                      //
    { 1, "ld (hl), a", tr8::ld_reg_reg<HL, A> },      //
    { 1, "ld a, b", tr8::ld_reg_reg<A, B> },          //
    { 1, "ld a, c", tr8::ld_reg_reg<A, C> },          //
    { 1, "ld a, d", tr8::ld_reg_reg<A, D> },          //
    { 1, "ld a, e", tr8::ld_reg_reg<A, E> },          //
    { 1, "ld a, h", tr8::ld_reg_reg<A, H> },          //
    { 1, "ld a, l", tr8::ld_reg_reg<A, L> },          //
    { 1, "ld a, (hl)", tr8::ld_reg_reg<A, HL> },      //
    { 1, "ld a, a", tr8::ld_reg_reg<A, A> },          //
    { 1, "add a, b", alu8::add<false, B> },           //
    { 1, "add a, c", alu8::add<false, C> },           //
    { 1, "add a, d", alu8::add<false, D> },           //
    { 1, "add a, e", alu8::add<false, E> },           //
    { 1, "add a, h", alu8::add<false, H> },           //
    { 1, "add a, l", alu8::add<false, L> },           //
    { 1, "add a, (hl)", alu8::add<false, HL> },       //
    { 1, "add a, a", alu8::add<false, A> },           //
    { 1, "adc a, b", alu8::adc<false, B> },           //
    { 1, "adc a, c", alu8::adc<false, C> },           //
    { 1, "adc a, d", alu8::adc<false, D> },           //
    { 1, "adc a, e", alu8::adc<false, E> },           //
    { 1, "adc a, h", alu8::adc<false, H> },           //
    { 1, "adc a, l", alu8::adc<false, L> },           //
    { 1, "adc a, (hl)", alu8::adc<false, HL> },       //
    { 1, "adc a, a", alu8::adc<false, A> },           //
    { 1, "sub a, b", alu8::sub<false, B> },           //
    { 1, "sub a, c", alu8::sub<false, C> },           //
    { 1, "sub a, d", alu8::sub<false, D> },           //
    { 1, "sub a, e", alu8::sub<false, E> },           //
    { 1, "sub a, h", alu8::sub<false, H> },           //
    { 1, "sub a, l", alu8::sub<false, L> },           //
    { 1, "sub a, (hl)", alu8::sub<false, HL> },       //
    { 1, "sub a, a", alu8::sub<false, A> },           //
    { 1, "sbc a, b", alu8::sbc<false, B> },           //
    { 1, "sbc a, c", alu8::sbc<false, C> },           //
    { 1, "sbc a, d", alu8::sbc<false, D> },           //
    { 1, "sbc a, e", alu8::sbc<false, E> },           //
    { 1, "sbc a, h", alu8::sbc<false, H> },           //
    { 1, "sbc a, l", alu8::sbc<false, L> },           //
    { 1, "sbc a, (hl)", alu8::sbc<false, HL> },       //
    { 1, "sbc a, a", alu8::sbc<false, A> },           //
    { 1, "and a, b", alu8::and_inst<false, B> },      //
    { 1, "and a, c", alu8::and_inst<false, C> },      //
    { 1, "and a, d", alu8::and_inst<false, D> },      //
    { 1, "and a, e", alu8::and_inst<false, E> },      //
    { 1, "and a, h", alu8::and_inst<false, H> },      //
    { 1, "and a, l", alu8::and_inst<false, L> },      //
    { 1, "and a, (hl)", alu8::and_inst<false, HL> },  //
    { 1, "and a, a", alu8::and_inst<false, A> },      //
    { 1, "xor a, b", alu8::xor_inst<false, B> },      //
    { 1, "xor a, c", alu8::xor_inst<false, C> },      //
    { 1, "xor a, d", alu8::xor_inst<false, D> },      //
    { 1, "xor a, e", alu8::xor_inst<false, E> },      //
    { 1, "xor a, h", alu8::xor_inst<false, H> },      //
    { 1, "xor a, l", alu8::xor_inst<false, L> },      //
    { 1, "xor a, (hl)", alu8::xor_inst<false, HL> },  //
    { 1, "xor a, a", alu8::xor_inst<false, A> },      //
    { 1, "or a, b", alu8::or_inst<false, B> },        //
    { 1, "or a, c", alu8::or_inst<false, C> },        //
    { 1, "or a, d", alu8::or_inst<false, D> },        //
    { 1, "or a, e", alu8::or_inst<false, E> },        //
    { 1, "or a, h", alu8::or_inst<false, H> },        //
    { 1, "or a, l", alu8::or_inst<false, L> },        //
    { 1, "or a, (hl)", alu8::or_inst<false, HL> },    //
    { 1, "or a, a", alu8::or_inst<false, A> },        //
    { 1, "cp a, b", alu8::cp<false, B> },             //
    { 1, "cp a, c", alu8::cp<false, C> },             //
    { 1, "cp a, d", alu8::cp<false, D> },             //
    { 1, "cp a, e", alu8::cp<false, E> },             //
    { 1, "cp a, h", alu8::cp<false, H> },             //
    { 1, "cp a, l", alu8::cp<false, L> },             //
    { 1, "cp a, (hl)", alu8::cp<false, HL> },         //
    { 1, "cp a, a", alu8::cp<false, A> },             //
    { 1, "ret nz", call::ret<true, FLAG_NZ> },        //
    { 1, "pop bc", tr16::pop<BC> },                   //
    { 3, "jp nz, a16", jp::jump_if<false, FLAG_NZ> }, //
    { 3, "jp a16", jp::jump<false> },                 //
    { 3, "call nz, a16", call::call<true, FLAG_NZ> }, //
    { 1, "push bc", tr16::push<BC> },                 //
    { 2, "add a, d8", alu8::add<true, REG8_NONE> },   //
    { 1, "rst 00h", call::rst<0b000> },               //
    { 1, "ret z", call::ret<true, FLAG_Z> },          //
    { 1, "ret", call::ret<false, 0b00> },             //
    { 3, "jp z, a16", jp::jump_if<false, FLAG_Z> },   //
    { 1, "prefix cb", unimplemented },                //
    { 3, "call z, a16", call::call<true, FLAG_Z> },   //
    { 3, "call a16", call::call<false, 0> },          //
    { 2, "adc a, d8", alu8::adc<true, REG8_NONE> },   //
    { 1, "rst 08h", call::rst<0b001> },               //
    { 1, "ret nc", call::ret<true, FLAG_NC> },        //
    { 1, "pop de", tr16::pop<DE> },                   //
    { 3, "jp nc, a16", jp::jump_if<false, FLAG_NC> }, //
    { 1, "illegal", unimplemented },                  //
    { 3, "call nc, a16", call::call<true, FLAG_NC> }, //
    { 1, "push de", tr16::push<DE> },                 //
    { 2, "sub d8", alu8::sub<true, REG8_NONE> },      //
    { 1, "rst 10h", call::rst<0b010> },               //
    { 1, "ret c", call::ret<true, FLAG_C> },          //
    { 1, "reti", call::reti },                        //
    { 3, "jp c, a16", jp::jump_if<false, FLAG_C> },   //
    { 1, "illegal", unimplemented },                  //
    { 3, "call c, a16", call::call<true, FLAG_C> },   //
    { 1, "illegal", unimplemented },                  //
    { 2, "sbc a, d8", alu8::sbc<true, REG8_NONE> },   //
    { 1, "rst 18h", call::rst<0b011> },               //
    { 2, "ldh (a8), a", tr8::st_io_imm_a },           //
    { 1, "pop hl", tr16::pop<HL> },                   //
    { 1, "ld (c), a", tr8::st_io_c_a },               //
    { 1, "illegal", unimplemented },                  //
    { 1, "illegal", unimplemented },                  //
    { 1, "push hl", tr16::push<HL> },                 //
    { 2, "and d8", alu8::and_inst<true, REG8_NONE> }, //
    { 1, "rst 20h", call::rst<0b100> },               //
    { 2, "add sp, r8", alu16::add_sp },               //
    { 1, "jp (hl)", jp::jump_hl },                    //
    { 3, "ld (a16), a", tr8::ld_addr_a },             //
    { 1, "illegal", unimplemented },                  //
    { 1, "illegal", unimplemented },                  //
    { 1, "illegal", unimplemented },                  //
    { 2, "xor d8", alu8::xor_inst<true, REG8_NONE> }, //
    { 1, "rst 28h", call::rst<0b101> },               //
    { 2, "ldh a, (a8)", tr8::ld_a_io_imm },           //
    { 1, "pop af", tr16::pop<AF> },                   //
    { 1, "ld a, (c)", tr8::ld_a_io_c },               //
    { 1, "di", misc::di },                            //
    { 1, "illegal", unimplemented },                  //
    { 1, "push af", tr16::push<AF> },                 //
    { 2, "or d8", alu8::or_inst<true, REG8_NONE> },   //
    { 1, "rst 30h", call::rst<0b110> },               //
    { 1, "ldhl", tr16::ldhl },                        //
    { 1, "ld sp, hl", tr16::ld_sp_hl },               //
    { 3, "ld a, (a16)", tr8::ld_a_addr },             //
    { 1, "ei", misc::ei },                            //
    { 1, "illegal", unimplemented },                  //
    { 1, "illegal", unimplemented },                  //
    { 2, "cp d8", alu8::cp<true, REG8_NONE> },        //
    { 1, "rst 38h", call::rst<0b111> },               //
};

Instruction prefix_cb_instructions[0x100] = {
    { 2, "rlc b", bits::rlc<B> },           //
    { 2, "rlc c", bits::rlc<C> },           //
    { 2, "rlc d", bits::rlc<D> },           //
    { 2, "rlc e", bits::rlc<E> },           //
    { 2, "rlc h", bits::rlc<H> },           //
    { 2, "rlc l", bits::rlc<L> },           //
    { 2, "rlc (hl)", bits::rlc<HL> },       //
    { 2, "rlc a", bits::rlc<A> },           //
    { 2, "rrc b", bits::rrc<B> },           //
    { 2, "rrc c", bits::rrc<C> },           //
    { 2, "rrc d", bits::rrc<D> },           //
    { 2, "rrc e", bits::rrc<E> },           //
    { 2, "rrc h", bits::rrc<H> },           //
    { 2, "rrc l", bits::rrc<L> },           //
    { 2, "rrc (hl)", bits::rrc<HL> },       //
    { 2, "rrc a", bits::rrc<A> },           //
    { 2, "rl b", bits::rl<B> },             //
    { 2, "rl c", bits::rl<C> },             //
    { 2, "rl d", bits::rl<D> },             //
    { 2, "rl e", bits::rl<E> },             //
    { 2, "rl h", bits::rl<H> },             //
    { 2, "rl l", bits::rl<L> },             //
    { 2, "rl (hl)", bits::rl<HL> },         //
    { 2, "rl a", bits::rl<A> },             //
    { 2, "rr b", bits::rr<B> },             //
    { 2, "rr c", bits::rr<C> },             //
    { 2, "rr d", bits::rr<D> },             //
    { 2, "rr e", bits::rr<E> },             //
    { 2, "rr h", bits::rr<H> },             //
    { 2, "rr l", bits::rr<L> },             //
    { 2, "rr (hl)", bits::rr<HL> },         //
    { 2, "rr a", bits::rr<A> },             //
    { 2, "sla b", bits::sla<B> },           //
    { 2, "sla c", bits::sla<C> },           //
    { 2, "sla d", bits::sla<D> },           //
    { 2, "sla e", bits::sla<E> },           //
    { 2, "sla h", bits::sla<H> },           //
    { 2, "sla l", bits::sla<L> },           //
    { 2, "sla (hl)", bits::sla<HL> },       //
    { 2, "sla A", bits::sla<A> },           //
    { 2, "sra b", bits::sra<B> },           //
    { 2, "sra c", bits::sra<C> },           //
    { 2, "sra d", bits::sra<D> },           //
    { 2, "sra e", bits::sra<E> },           //
    { 2, "sra h", bits::sra<H> },           //
    { 2, "sra l", bits::sra<L> },           //
    { 2, "sra (hl)", bits::sra<HL> },       //
    { 2, "sra a", bits::sra<A> },           //
    { 2, "swap B", bits::swap<B> },         //
    { 2, "swap C", bits::swap<C> },         //
    { 2, "swap D", bits::swap<D> },         //
    { 2, "swap E", bits::swap<E> },         //
    { 2, "swap H", bits::swap<H> },         //
    { 2, "swap L", bits::swap<L> },         //
    { 2, "swap (HL)", bits::swap<HL> },     //
    { 2, "swap A", bits::swap<A> },         //
    { 2, "srl b", bits::srl<B> },           //
    { 2, "srl c", bits::srl<C> },           //
    { 2, "srl d", bits::srl<D> },           //
    { 2, "srl e", bits::srl<E> },           //
    { 2, "srl h", bits::srl<H> },           //
    { 2, "srl l", bits::srl<L> },           //
    { 2, "srl (hl)", bits::srl<HL> },       //
    { 2, "srl a", bits::srl<A> },           //
    { 2, "bit 0, b", bits::bit<0, B> },     //
    { 2, "bit 0, c", bits::bit<0, C> },     //
    { 2, "bit 0, d", bits::bit<0, D> },     //
    { 2, "bit 0, e", bits::bit<0, E> },     //
    { 2, "bit 0, h", bits::bit<0, H> },     //
    { 2, "bit 0, l", bits::bit<0, L> },     //
    { 2, "bit 0, (hl)", bits::bit<0, HL> }, //
    { 2, "bit 0, a", bits::bit<0, A> },     //
    { 2, "bit 1, b", bits::bit<1, B> },     //
    { 2, "bit 1, c", bits::bit<1, C> },     //
    { 2, "bit 1, d", bits::bit<1, D> },     //
    { 2, "bit 1, e", bits::bit<1, E> },     //
    { 2, "bit 1, h", bits::bit<1, H> },     //
    { 2, "bit 1, l", bits::bit<1, L> },     //
    { 2, "bit 1, (hl)", bits::bit<1, HL> }, //
    { 2, "bit 1, a", bits::bit<1, A> },     //
    { 2, "bit 2, b", bits::bit<2, B> },     //
    { 2, "bit 2, c", bits::bit<2, C> },     //
    { 2, "bit 2, d", bits::bit<2, D> },     //
    { 2, "bit 2, e", bits::bit<2, E> },     //
    { 2, "bit 2, h", bits::bit<2, H> },     //
    { 2, "bit 2, l", bits::bit<2, L> },     //
    { 2, "bit 2, (hl)", bits::bit<2, HL> }, //
    { 2, "bit 2, a", bits::bit<2, A> },     //
    { 2, "bit 3, b", bits::bit<3, B> },     //
    { 2, "bit 3, c", bits::bit<3, C> },     //
    { 2, "bit 3, d", bits::bit<3, D> },     //
    { 2, "bit 3, e", bits::bit<3, E> },     //
    { 2, "bit 3, h", bits::bit<3, H> },     //
    { 2, "bit 3, l", bits::bit<3, L> },     //
    { 2, "bit 3, (hl)", bits::bit<3, HL> }, //
    { 2, "bit 3, a", bits::bit<3, A> },     //
    { 2, "bit 4, b", bits::bit<4, B> },     //
    { 2, "bit 4, c", bits::bit<4, C> },     //
    { 2, "bit 4, d", bits::bit<4, D> },     //
    { 2, "bit 4, e", bits::bit<4, E> },     //
    { 2, "bit 4, h", bits::bit<4, H> },     //
    { 2, "bit 4, l", bits::bit<4, L> },     //
    { 2, "bit 4, (hl)", bits::bit<4, HL> }, //
    { 2, "bit 4, a", bits::bit<4, A> },     //
    { 2, "bit 5, b", bits::bit<5, B> },     //
    { 2, "bit 5, c", bits::bit<5, C> },     //
    { 2, "bit 5, d", bits::bit<5, D> },     //
    { 2, "bit 5, e", bits::bit<5, E> },     //
    { 2, "bit 5, h", bits::bit<5, H> },     //
    { 2, "bit 5, l", bits::bit<5, L> },     //
    { 2, "bit 5, (hl)", bits::bit<5, HL> }, //
    { 2, "bit 5, a", bits::bit<5, A> },     //
    { 2, "bit 6, b", bits::bit<6, B> },     //
    { 2, "bit 6, c", bits::bit<6, C> },     //
    { 2, "bit 6, d", bits::bit<6, D> },     //
    { 2, "bit 6, e", bits::bit<6, E> },     //
    { 2, "bit 6, h", bits::bit<6, H> },     //
    { 2, "bit 6, l", bits::bit<6, L> },     //
    { 2, "bit 6, (hl)", bits::bit<6, HL> }, //
    { 2, "bit 6, a", bits::bit<6, A> },     //
    { 2, "bit 7, b", bits::bit<7, B> },     //
    { 2, "bit 7, c", bits::bit<7, C> },     //
    { 2, "bit 7, d", bits::bit<7, D> },     //
    { 2, "bit 7, e", bits::bit<7, E> },     //
    { 2, "bit 7, h", bits::bit<7, H> },     //
    { 2, "bit 7, l", bits::bit<7, L> },     //
    { 2, "bit 7, (hl)", bits::bit<7, HL> }, //
    { 2, "bit 7, a", bits::bit<7, A> },     //
    { 2, "res 0, b", bits::res<0, B> },     //
    { 2, "res 0, c", bits::res<0, C> },     //
    { 2, "res 0, d", bits::res<0, D> },     //
    { 2, "res 0, e", bits::res<0, E> },     //
    { 2, "res 0, h", bits::res<0, H> },     //
    { 2, "res 0, l", bits::res<0, L> },     //
    { 2, "res 0, (hl)", bits::res<0, HL> }, //
    { 2, "res 0, a", bits::res<0, A> },     //
    { 2, "res 1, b", bits::res<1, B> },     //
    { 2, "res 1, c", bits::res<1, C> },     //
    { 2, "res 1, d", bits::res<1, D> },     //
    { 2, "res 1, e", bits::res<1, E> },     //
    { 2, "res 1, h", bits::res<1, H> },     //
    { 2, "res 1, l", bits::res<1, L> },     //
    { 2, "res 1, (hl)", bits::res<1, HL> }, //
    { 2, "res 1, a", bits::res<1, A> },     //
    { 2, "res 2, b", bits::res<2, B> },     //
    { 2, "res 2, c", bits::res<2, C> },     //
    { 2, "res 2, d", bits::res<2, D> },     //
    { 2, "res 2, e", bits::res<2, E> },     //
    { 2, "res 2, h", bits::res<2, H> },     //
    { 2, "res 2, l", bits::res<2, L> },     //
    { 2, "res 2, (hl)", bits::res<2, HL> }, //
    { 2, "res 2, a", bits::res<2, A> },     //
    { 2, "res 3, b", bits::res<3, B> },     //
    { 2, "res 3, c", bits::res<3, C> },     //
    { 2, "res 3, d", bits::res<3, D> },     //
    { 2, "res 3, e", bits::res<3, E> },     //
    { 2, "res 3, h", bits::res<3, H> },     //
    { 2, "res 3, l", bits::res<3, L> },     //
    { 2, "res 3, (hl)", bits::res<3, HL> }, //
    { 2, "res 3, a", bits::res<3, A> },     //
    { 2, "res 4, b", bits::res<4, B> },     //
    { 2, "res 4, c", bits::res<4, C> },     //
    { 2, "res 4, d", bits::res<4, D> },     //
    { 2, "res 4, e", bits::res<4, E> },     //
    { 2, "res 4, h", bits::res<4, H> },     //
    { 2, "res 4, l", bits::res<4, L> },     //
    { 2, "res 4, (hl)", bits::res<4, HL> }, //
    { 2, "res 4, a", bits::res<4, A> },     //
    { 2, "res 5, b", bits::res<5, B> },     //
    { 2, "res 5, c", bits::res<5, C> },     //
    { 2, "res 5, d", bits::res<5, D> },     //
    { 2, "res 5, e", bits::res<5, E> },     //
    { 2, "res 5, h", bits::res<5, H> },     //
    { 2, "res 5, l", bits::res<5, L> },     //
    { 2, "res 5, (hl)", bits::res<5, HL> }, //
    { 2, "res 5, a", bits::res<5, A> },     //
    { 2, "res 6, b", bits::res<6, B> },     //
    { 2, "res 6, c", bits::res<6, C> },     //
    { 2, "res 6, d", bits::res<6, D> },     //
    { 2, "res 6, e", bits::res<6, E> },     //
    { 2, "res 6, h", bits::res<6, H> },     //
    { 2, "res 6, l", bits::res<6, L> },     //
    { 2, "res 6, (hl)", bits::res<6, HL> }, //
    { 2, "res 6, a", bits::res<6, A> },     //
    { 2, "res 7, b", bits::res<7, B> },     //
    { 2, "res 7, c", bits::res<7, C> },     //
    { 2, "res 7, d", bits::res<7, D> },     //
    { 2, "res 7, e", bits::res<7, E> },     //
    { 2, "res 7, h", bits::res<7, H> },     //
    { 2, "res 7, l", bits::res<7, L> },     //
    { 2, "res 7, (hl)", bits::res<7, HL> }, //
    { 2, "res 7, a", bits::res<7, A> },     //
    { 2, "set 0, b", bits::set<0, B> },     //
    { 2, "set 0, c", bits::set<0, C> },     //
    { 2, "set 0, d", bits::set<0, D> },     //
    { 2, "set 0, e", bits::set<0, E> },     //
    { 2, "set 0, h", bits::set<0, H> },     //
    { 2, "set 0, l", bits::set<0, L> },     //
    { 2, "set 0, (hl)", bits::set<0, HL> }, //
    { 2, "set 0, a", bits::set<0, A> },     //
    { 2, "set 1, b", bits::set<1, B> },     //
    { 2, "set 1, c", bits::set<1, C> },     //
    { 2, "set 1, d", bits::set<1, D> },     //
    { 2, "set 1, e", bits::set<1, E> },     //
    { 2, "set 1, h", bits::set<1, H> },     //
    { 2, "set 1, l", bits::set<1, L> },     //
    { 2, "set 1, (hl)", bits::set<1, HL> }, //
    { 2, "set 1, a", bits::set<1, A> },     //
    { 2, "set 2, b", bits::set<2, B> },     //
    { 2, "set 2, c", bits::set<2, C> },     //
    { 2, "set 2, d", bits::set<2, D> },     //
    { 2, "set 2, e", bits::set<2, E> },     //
    { 2, "set 2, h", bits::set<2, H> },     //
    { 2, "set 2, l", bits::set<2, L> },     //
    { 2, "set 2, (hl)", bits::set<2, HL> }, //
    { 2, "set 2, a", bits::set<2, A> },     //
    { 2, "set 3, b", bits::set<3, B> },     //
    { 2, "set 3, c", bits::set<3, C> },     //
    { 2, "set 3, d", bits::set<3, D> },     //
    { 2, "set 3, e", bits::set<3, E> },     //
    { 2, "set 3, h", bits::set<3, H> },     //
    { 2, "set 3, l", bits::set<3, L> },     //
    { 2, "set 3, (hl)", bits::set<3, HL> }, //
    { 2, "set 3, a", bits::set<3, A> },     //
    { 2, "set 4, b", bits::set<4, B> },     //
    { 2, "set 4, c", bits::set<4, C> },     //
    { 2, "set 4, d", bits::set<4, D> },     //
    { 2, "set 4, e", bits::set<4, E> },     //
    { 2, "set 4, h", bits::set<4, H> },     //
    { 2, "set 4, l", bits::set<4, L> },     //
    { 2, "set 4, (hl)", bits::set<4, HL> }, //
    { 2, "set 4, a", bits::set<4, A> },     //
    { 2, "set 5, b", bits::set<5, B> },     //
    { 2, "set 5, c", bits::set<5, C> },     //
    { 2, "set 5, d", bits::set<5, D> },     //
    { 2, "set 5, e", bits::set<5, E> },     //
    { 2, "set 5, h", bits::set<5, H> },     //
    { 2, "set 5, l", bits::set<5, L> },     //
    { 2, "set 5, (hl)", bits::set<5, HL> }, //
    { 2, "set 5, a", bits::set<5, A> },     //
    { 2, "set 6, b", bits::set<6, B> },     //
    { 2, "set 6, c", bits::set<6, C> },     //
    { 2, "set 6, d", bits::set<6, D> },     //
    { 2, "set 6, e", bits::set<6, E> },     //
    { 2, "set 6, h", bits::set<6, H> },     //
    { 2, "set 6, l", bits::set<6, L> },     //
    { 2, "set 6, (hl)", bits::set<6, HL> }, //
    { 2, "set 6, a", bits::set<6, A> },     //
    { 2, "set 7, b", bits::set<7, B> },     //
    { 2, "set 7, c", bits::set<7, C> },     //
    { 2, "set 7, d", bits::set<7, D> },     //
    { 2, "set 7, e", bits::set<7, E> },     //
    { 2, "set 7, h", bits::set<7, H> },     //
    { 2, "set 7, l", bits::set<7, L> },     //
    { 2, "set 7, (hl)", bits::set<7, HL> }, //
    { 2, "set 7, a", bits::set<7, A> },     //
};
