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
    { 1, "nop", misc::nop },                          // [0x00]
    { 3, "ld bc, d16", tr16::ld_reg_imm<BC> },        // [0x01]
    { 1, "ld (bc), a", tr8::ld_bc_a },                // [0x02]
    { 1, "inc bc", alu16::inc<BC> },                  // [0x03]
    { 1, "inc b", alu8::inc<B> },                     // [0x04]
    { 1, "dec b", alu8::dec<B> },                     // [0x05]
    { 2, "ld b, d8", tr8::ld_reg_imm<B> },            // [0x06]
    { 1, "rlca", unimplemented },                     // [0x07]
    { 3, "ld (a16), sp", tr16::st_sp },               // [0x08]
    { 1, "add hl, bc", alu16::add_hl<BC> },           //
    { 1, "ld a,(bc)", tr8::ld_a_bc },                 //
    { 1, "dec bc", alu16::dec<BC> },                  //
    { 1, "inc c", alu8::inc<C> },                     //
    { 1, "dec c", alu8::dec<C> },                     //
    { 2, "ld c, d8", tr8::ld_reg_imm<C> },            //
    { 1, "rrca", unimplemented },                     //
    { 1, "stop", unimplemented },                     //
    { 3, "ld de, d16", tr16::ld_reg_imm<DE> },        //
    { 1, "ld (de), a", tr8::ld_de_a },                //
    { 1, "inc de", alu16::inc<DE> },                  //
    { 1, "inc d", alu8::inc<D> },                     //
    { 1, "dec d", alu8::dec<D> },                     //
    { 2, "ld d, d8", tr8::ld_reg_imm<D> },            //
    { 1, "rla", unimplemented },                      //
    { 2, "jr r8", jp::jump<true> },                   //
    { 1, "add hl, de", alu16::add_hl<DE> },           //
    { 1, "ld a, (de)", tr8::ld_a_de },                //
    { 1, "dec de", alu16::dec<DE> },                  //
    { 1, "inc e", alu8::inc<E> },                     //
    { 1, "dec e", alu8::dec<E> },                     //
    { 2, "ld e, d8", tr8::ld_reg_imm<E> },            //
    { 1, "rra", bits::rr<A> },                        //
    { 2, "jr nz, r8", jp::jump_if<true, FLAG_NZ> },   //
    { 3, "ld hl, d16", tr16::ld_reg_imm<HL> },        //
    { 1, "ld (hl+), a", tr8::ld_hli_a },              //
    { 1, "inc hl", alu16::inc<HL> },                  //
    { 1, "inc h", alu8::inc<H> },                     //
    { 1, "dec h", alu8::dec<H> },                     //
    { 2, "ld h, d8", tr8::ld_reg_imm<H> },            //
    { 1, "daa", unimplemented },                      //
    { 2, "jr z, r8", jp::jump_if<true, FLAG_Z> },     //
    { 1, "add hl, hl", alu16::add_hl<HL> },           //
    { 1, "ld a, (hl+)", tr8::ld_a_hli },              //
    { 1, "dec hl", alu16::dec<HL> },                  //
    { 1, "inc l", alu8::inc<L> },                     //
    { 1, "dec l", alu8::dec<L> },                     //
    { 2, "ld l, d8", tr8::ld_reg_imm<L> },            //
    { 1, "cpl", unimplemented },                      //
    { 2, "jr nc, r8", jp::jump_if<true, FLAG_NC> },   //
    { 3, "ld sp, d16", tr16::ld_reg_imm<SP> },        //
    { 1, "ld (hl-), a", tr8::ld_hld_a },              //
    { 1, "inc sp", alu16::inc<SP> },                  //
    { 1, "inc (hl)", alu8::inc<HL> },                 //
    { 1, "dec (hl)", alu8::dec<HL> },                 //
    { 2, "ld (hl), d8", tr8::ld_reg_imm<HL> },        //
    { 1, "scf", unimplemented },                      //
    { 2, "jr c, r8", jp::jump_if<true, FLAG_C> },     //
    { 1, "add hl, sp", alu16::add_hl<SP> },           //
    { 1, "ld a, (hl-)", tr8::ld_a_hld },              //
    { 1, "dec sp", alu16::dec<SP> },                  //
    { 1, "inc a", alu8::inc<A> },                     //
    { 1, "dec a", alu8::dec<A> },                     //
    { 2, "ld a, d8", tr8::ld_reg_imm<A> },            //
    { 1, "ccf", unimplemented },                      //
    { 1, "ld b, b", tr8::ld_reg_reg<B, B> },          //
    { 1, "ld b, c", tr8::ld_reg_reg<B, C> },          //
    { 1, "ld b, d", tr8::ld_reg_reg<B, D> },          //
    { 1, "ld b, e", tr8::ld_reg_reg<B, E> },          //
    { 1, "ld b, h", tr8::ld_reg_reg<B, H> },          //
    { 1, "ld b, l", tr8::ld_reg_reg<B, L> },          //
    { 1, "ld b, (hl)", tr8::ld_reg_reg<B, HL> },      //
    { 1, "ld b, a", tr8::ld_reg_reg<B, A> },          //
    { 1, "ld c, b", tr8::ld_reg_reg<C, B> },          //
    { 1, "ld c, c", tr8::ld_reg_reg<C, C> },          //
    { 1, "ld c, d", tr8::ld_reg_reg<C, D> },          //
    { 1, "ld c, e", tr8::ld_reg_reg<C, E> },          //
    { 1, "ld c, h", tr8::ld_reg_reg<C, H> },          //
    { 1, "ld c, l", tr8::ld_reg_reg<C, L> },          //
    { 1, "ld c, (hl)", tr8::ld_reg_reg<C, HL> },      //
    { 1, "ld c, a", tr8::ld_reg_reg<C, A> },          //
    { 1, "ld d, b", tr8::ld_reg_reg<D, B> },          //
    { 1, "ld d, c", tr8::ld_reg_reg<D, C> },          //
    { 1, "ld d, d", tr8::ld_reg_reg<D, D> },          //
    { 1, "ld d, e", tr8::ld_reg_reg<D, E> },          //
    { 1, "ld d, h", tr8::ld_reg_reg<D, H> },          //
    { 1, "ld d, l", tr8::ld_reg_reg<D, L> },          //
    { 1, "ld d, (hl)", tr8::ld_reg_reg<D, HL> },      //
    { 1, "ld d, a", tr8::ld_reg_reg<D, A> },          //
    { 1, "ld e, b", tr8::ld_reg_reg<E, B> },          //
    { 1, "ld e, c", tr8::ld_reg_reg<E, C> },          //
    { 1, "ld e, d", tr8::ld_reg_reg<E, D> },          //
    { 1, "ld e, e", tr8::ld_reg_reg<E, E> },          //
    { 1, "ld e, h", tr8::ld_reg_reg<E, H> },          //
    { 1, "ld e, l", tr8::ld_reg_reg<E, L> },          //
    { 1, "ld e, (hl)", tr8::ld_reg_reg<E, HL> },      //
    { 1, "ld e, a", tr8::ld_reg_reg<E, A> },          //
    { 1, "ld h, b", tr8::ld_reg_reg<H, B> },          //
    { 1, "ld h, c", tr8::ld_reg_reg<H, C> },          //
    { 1, "ld h, d", tr8::ld_reg_reg<H, D> },          //
    { 1, "ld h, e", tr8::ld_reg_reg<H, E> },          //
    { 1, "ld h, h", tr8::ld_reg_reg<H, H> },          //
    { 1, "ld h, l", tr8::ld_reg_reg<H, L> },          //
    { 1, "ld h, (hl)", tr8::ld_reg_reg<H, HL> },      //
    { 1, "ld h, a", tr8::ld_reg_reg<H, A> },          //
    { 1, "ld l, b", tr8::ld_reg_reg<L, B> },          //
    { 1, "ld l, c", tr8::ld_reg_reg<L, C> },          //
    { 1, "ld l, d", tr8::ld_reg_reg<L, D> },          //
    { 1, "ld l, e", tr8::ld_reg_reg<L, E> },          //
    { 1, "ld l, h", tr8::ld_reg_reg<L, H> },          //
    { 1, "ld l, l", tr8::ld_reg_reg<L, L> },          //
    { 1, "ld l, (hl)", tr8::ld_reg_reg<L, HL> },      //
    { 1, "ld l,a", tr8::ld_reg_reg<L, A> },           //
    { 1, "ld (hl), b", tr8::ld_reg_reg<HL, B> },      //
    { 1, "ld (hl), c", tr8::ld_reg_reg<HL, C> },      //
    { 1, "ld (hl), d", tr8::ld_reg_reg<HL, D> },      //
    { 1, "ld (hl), e", tr8::ld_reg_reg<HL, E> },      //
    { 1, "ld (hl), h", tr8::ld_reg_reg<HL, H> },      //
    { 1, "ld (hl), l", tr8::ld_reg_reg<HL, L> },      //
    { 1, "halt", unimplemented },                     //
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
    { 2, "add a, d8", alu8::add<true, A> },           //
    { 1, "rst 00h", call::rst<0b000> },               //
    { 1, "ret z", call::ret<true, FLAG_Z> },          //
    { 1, "ret", call::ret<false, 0b00> },             //
    { 3, "jp z, a16", jp::jump_if<false, FLAG_Z> },   //
    { 1, "prefix cb", unimplemented },                //
    { 3, "call z, a16", call::call<true, FLAG_Z> },   //
    { 3, "call a16", call::call<false, 0> },          //
    { 2, "adc a, d8", alu8::adc<true, A> },           //
    { 1, "rst 08h", call::rst<0b001> },               //
    { 1, "ret nc", call::ret<true, FLAG_NC> },        //
    { 1, "pop de", tr16::pop<DE> },                   //
    { 3, "jp nc, a16", jp::jump_if<false, FLAG_NC> }, //
    { 1, "unimplemented", unimplemented },            //
    { 3, "call nc, 16", call::call<true, FLAG_NC> },  //
    { 1, "push de", tr16::push<DE> },                 //
    { 2, "sub d8", alu8::sub<true, A> },              //
    { 1, "rst 10h", call::rst<0b010> },               //
    { 1, "ret c", call::ret<true, FLAG_C> },          //
    { 1, "reti", call::reti },                        //
    { 3, "jp c, a16", jp::jump_if<false, FLAG_C> },   //
    { 1, "unimplemented", unimplemented },            //
    { 3, "call c, a16", call::call<true, FLAG_C> },   //
    { 1, "unimplemented", unimplemented },            //
    { 2, "sbc a, d8", alu8::sbc<true, A> },           //
    { 1, "rst 18h", call::rst<0b011> },               //
    { 2, "ldh (a8), a", tr8::st_io_imm_a },           //
    { 1, "pop hl", tr16::pop<HL> },                   //
    { 1, "ld (c), a", tr8::st_io_c_a },               //
    { 1, "unimplemented", unimplemented },            //
    { 1, "unimplemented", unimplemented },            //
    { 1, "push hl", tr16::push<HL> },                 //
    { 2, "and d8", alu8::and_inst<true, A> },         //
    { 1, "rst 20h", call::rst<0b100> },               //
    { 2, "add sp, r8", alu16::add_sp },               //
    { 1, "jp (hl)", jp::jump_hl },                    //
    { 3, "ld (a16), a", tr8::ld_addr_a },             //
    { 1, "unimplemented", unimplemented },            //
    { 1, "unimplemented", unimplemented },            //
    { 1, "unimplemented", unimplemented },            //
    { 2, "xor d8", alu8::xor_inst<true, REG8_NONE> }, //
    { 1, "rst 28h", call::rst<0b101> },               //
    { 2, "ldh a, (a8)", tr8::ld_a_io_imm },           //
    { 1, "pop af", tr16::pop<AF> },                   //
    { 1, "ld a, (c)", tr8::ld_a_io_c },               //
    { 1, "di", misc::di },                            //
    { 1, "unimplemented", unimplemented },            //
    { 1, "push af", tr16::push<AF> },                 //
    { 2, "or d8", alu8::or_inst<true, REG8_NONE> },   //
    { 1, "rst 30h", call::rst<0b110> },               //
    { 1, "ldhl", tr16::ldhl },                        //
    { 1, "ld sp, hl", tr16::ld_sp_hl },               //
    { 3, "ld a, (a16)", tr8::ld_a_addr },             //
    { 1, "ei", misc::ei },                            //
    { 1, "unimplemented", unimplemented },            //
    { 1, "unimplemented", unimplemented },            //
    { 2, "cp d8", alu8::cp<true, REG8_NONE> },        //
    { 1, "rst 38h", call::rst<0b111> },               //
};

Instruction prefix_cb_instructions[0x100] = {
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "rr b", bits::rr<B> },            //
    { 2, "rr c", bits::rr<C> },            //
    { 2, "rr d", bits::rr<D> },            //
    { 2, "rr e", bits::rr<E> },            //
    { 2, "rr h", bits::rr<H> },            //
    { 2, "rr l", bits::rr<L> },            //
    { 2, "rr (hl)", bits::rr<HL> },        //
    { 2, "rr a", bits::rr<A> },            //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "swap A", bits::swap<A> },        //
    { 2, "srl b", bits::srl<B> },          //
    { 2, "srl c", unimplemented },         //
    { 2, "srl d", unimplemented },         //
    { 2, "srl e", unimplemented },         //
    { 2, "srl h", unimplemented },         //
    { 2, "srl l", unimplemented },         //
    { 2, "srl (hl)", unimplemented },      //
    { 2, "srl a", unimplemented },         //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
    { 2, "unimplemented", unimplemented }, //
};
