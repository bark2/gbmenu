#include "instruction.h"

Instruction instructions[0x100] = {
    /* instruction-size, menmonic, instruction-fun */
    // TODO: dont move the pc counter inside the instruction-fun
    { 1, "nop", misc::nop },                            // [0x00]
    { 3, "ld bc, d16", tr16::ld_reg_imm<0b01> },        // [0x01]
    { 1, "ld (bc), a", tr8::st_a<false, 0b000> },       // [0x02]
    { 1, "inc bc", alu16::inc<0b00> },                  // [0x03]
    { 1, "inc b", alu8::inc<false, 0b000> },            // [0x04]
    { 1, "dec b", alu8::dec<false, 0b000> },            // [0x05]
    { 2, "ld b, d8", tr8::ld_reg_imm<0b000> },          // [0x06]
    { 1, "rlca", unimplemented },                       // [0x07]
    { 3, "ld (a16), sp", tr16::st_sp },                 // [0x08]
    { 1, "add hl, bc", alu16::add_hl<0b00> },           //
    { 1, "ld a,(bc)", tr8::ld_a_bc },                   //
    { 1, "dec bc", alu16::dec<0b00> },                  //
    { 1, "inc c", alu8::inc<false, 0b001> },            //
    { 1, "dec c", alu8::dec<false, 0b001> },            //
    { 2, "ld c, d8", tr8::ld_reg_imm<0b001> },          //
    { 1, "rrca", tr8::ld_reg_imm<0b001> },              //
    { 1, "stop", unimplemented },                       //
    { 3, "ld de, d16", tr16::ld_reg_imm<0b01> },        //
    { 1, "ld (de), a", tr8::st_a<false, 0b010> },       //
    { 1, "inc de", alu16::inc<0b01> },                  //
    { 1, "inc d", alu8::inc<false, 0b010> },            //
    { 1, "dec d", alu8::dec<false, 0b011> },            //
    { 2, "ld d, d8", tr8::ld_reg_imm<0b010> },          //
    { 1, "rla", unimplemented },                        //
    { 2, "jr r8", jp::jump<true> },                     //
    { 1, "add hl, de", alu16::add_hl<0b01> },           //
    { 1, "ld a, (de)", tr8::ld_a_de },                  //
    { 1, "dec de", alu16::dec<0b01> },                  //
    { 1, "inc e", alu8::inc<false, 0b011> },            //
    { 1, "dec e", alu8::dec<false, 0b011> },            //
    { 2, "ld e, d8", tr8::ld_reg_imm<0b011> },          //
    { 1, "rra", bits::rr<0b111> },                      //
    { 2, "jr nz, r8", jp::jump_if<true, 0b00> },        //
    { 3, "ld hl, d16", tr16::ld_reg_imm<0b10> },        //
    { 1, "ld (hl+), a", tr8::st_a<false, 0b100> },      //
    { 1, "inc hl", alu16::inc<0b10> },                  //
    { 1, "inc h", alu8::inc<false, 0b100> },            //
    { 1, "dec h", alu8::dec<false, 0b100> },            //
    { 2, "ld h, d8", tr8::ld_reg_imm<0b100> },          //
    { 1, "daa", unimplemented },                        //
    { 2, "jr z, r8", jp::jump_if<true, 0b01> },         //
    { 1, "add hl, hl", alu16::add_hl<0b10> },           //
    { 1, "ld a, (hl+)", tr8::ld_a_hli },                //
    { 1, "dec hl", alu16::dec<0b010> },                 //
    { 1, "inc l", alu8::inc<false, 0b101> },            //
    { 1, "dec l", alu8::dec<false, 0b101> },            //
    { 2, "ld l, d8", tr8::ld_reg_imm<0b101> },          //
    { 1, "cpl", unimplemented },                        //
    { 2, "jr nc, r8", jp::jump_if<true, 0b10> },        //
    { 3, "ld sp, d16", tr16::ld_reg_imm<0b11> },        //
    { 1, "ld (hl-), a", tr8::st_a<false, 0b110> },      //
    { 1, "inc sp", alu16::inc<0b11> },                  //
    { 1, "inc (hl)", alu8::inc<false, 0b110> },         //
    { 1, "dec (hl)", alu8::dec<false, 0b110> },         //
    { 2, "ld (hl), d8", tr8::ld_reg_imm<0b110> },       //
    { 1, "scf", unimplemented },                        //
    { 2, "jr c, r8", jp::jump_if<true, 0b11> },         //
    { 1, "add hl, sp", alu16::add_hl<0b11> },           //
    { 1, "ld a, (hl-)", tr8::ld_a_hld },                //
    { 1, "dec sp", alu16::dec<0b11> },                  //
    { 1, "inc a", alu8::inc<false, 0b111> },            //
    { 1, "dec a", alu8::dec<false, 0b111> },            //
    { 2, "ld a, d8", tr8::ld_reg_imm<0b111> },          //
    { 1, "ccf", unimplemented },                        //
    { 1, "ld b, b", tr8::ld_reg_reg<0b000, 0b000> },    //
    { 1, "ld b, c", tr8::ld_reg_reg<0b000, 0b001> },    //
    { 1, "ld b, d", tr8::ld_reg_reg<0b000, 0b010> },    //
    { 1, "ld b, e", tr8::ld_reg_reg<0b000, 0b011> },    //
    { 1, "ld b, h", tr8::ld_reg_reg<0b000, 0b100> },    //
    { 1, "ld b, l", tr8::ld_reg_reg<0b000, 0b101> },    //
    { 1, "ld b, (hl)", tr8::ld_reg_reg<0b000, 0b110> }, //
    { 1, "ld b, a", tr8::ld_reg_reg<0b000, 0b111> },    //
    { 1, "ld c, b", tr8::ld_reg_reg<0b001, 0b000> },    //
    { 1, "ld c, c", tr8::ld_reg_reg<0b001, 0b001> },    //
    { 1, "ld c, d", tr8::ld_reg_reg<0b001, 0b010> },    //
    { 1, "ld c, e", tr8::ld_reg_reg<0b001, 0b011> },    //
    { 1, "ld c, h", tr8::ld_reg_reg<0b001, 0b100> },    //
    { 1, "ld c, l", tr8::ld_reg_reg<0b001, 0b101> },    //
    { 1, "ld c, (hl)", tr8::ld_reg_reg<0b001, 0b110> }, //
    { 1, "ld c, a", tr8::ld_reg_reg<0b001, 0b111> },    //
    { 1, "ld d, b", tr8::ld_reg_reg<0b010, 0b000> },    //
    { 1, "ld d, c", tr8::ld_reg_reg<0b010, 0b001> },    //
    { 1, "ld d, d", tr8::ld_reg_reg<0b010, 0b010> },    //
    { 1, "ld d, e", tr8::ld_reg_reg<0b010, 0b011> },    //
    { 1, "ld d, h", tr8::ld_reg_reg<0b010, 0b100> },    //
    { 1, "ld d, l", tr8::ld_reg_reg<0b010, 0b101> },    //
    { 1, "ld d, (hl)", tr8::ld_reg_reg<0b010, 0b110> }, //
    { 1, "ld d, a", tr8::ld_reg_reg<0b010, 0b111> },    //
    { 1, "ld e, b", tr8::ld_reg_reg<0b011, 0b000> },    //
    { 1, "ld e, c", tr8::ld_reg_reg<0b011, 0b001> },    //
    { 1, "ld e, d", tr8::ld_reg_reg<0b011, 0b010> },    //
    { 1, "ld e, e", tr8::ld_reg_reg<0b011, 0b011> },    //
    { 1, "ld e, h", tr8::ld_reg_reg<0b011, 0b100> },    //
    { 1, "ld e, l", tr8::ld_reg_reg<0b011, 0b101> },    //
    { 1, "ld e, (hl)", tr8::ld_reg_reg<0b011, 0b110> }, //
    { 1, "ld e, a", tr8::ld_reg_reg<0b011, 0b111> },    //
    { 1, "ld h, b", tr8::ld_reg_reg<0b100, 0b000> },    //
    { 1, "ld h, c", tr8::ld_reg_reg<0b100, 0b001> },    //
    { 1, "ld h, d", tr8::ld_reg_reg<0b100, 0b010> },    //
    { 1, "ld h, e", tr8::ld_reg_reg<0b100, 0b011> },    //
    { 1, "ld h, h", tr8::ld_reg_reg<0b100, 0b100> },    //
    { 1, "ld h, l", tr8::ld_reg_reg<0b100, 0b101> },    //
    { 1, "ld h, (hl)", tr8::ld_reg_reg<0b100, 0b110> }, //
    { 1, "ld h, a", tr8::ld_reg_reg<0b100, 0b111> },    //
    { 1, "ld l, b", tr8::ld_reg_reg<0b101, 0b000> },    //
    { 1, "ld l, c", tr8::ld_reg_reg<0b101, 0b001> },    //
    { 1, "ld l, d", tr8::ld_reg_reg<0b101, 0b010> },    //
    { 1, "ld l, e", tr8::ld_reg_reg<0b101, 0b011> },    //
    { 1, "ld l, h", tr8::ld_reg_reg<0b101, 0b100> },    //
    { 1, "ld l, l", tr8::ld_reg_reg<0b101, 0b101> },    //
    { 1, "ld l, (hl)", tr8::ld_reg_reg<0b101, 0b110> }, //
    { 1, "ld l,a", tr8::ld_reg_reg<0b101, 0b111> },     //
    { 1, "ld (hl), b", tr8::ld_reg_reg<0b110, 0b000> }, //
    { 1, "ld (hl), c", tr8::ld_reg_reg<0b110, 0b001> }, //
    { 1, "ld (hl), d", tr8::ld_reg_reg<0b110, 0b010> }, //
    { 1, "ld (hl), e", tr8::ld_reg_reg<0b110, 0b011> }, //
    { 1, "ld (hl), h", tr8::ld_reg_reg<0b110, 0b100> }, //
    { 1, "ld (hl), l", tr8::ld_reg_reg<0b110, 0b101> }, //
    { 1, "halt", unimplemented },                       //
    { 1, "ld (hl), a", tr8::ld_reg_reg<0b110, 0b111> }, //
    { 1, "ld a, b", tr8::ld_reg_reg<0b111, 0b000> },    //
    { 1, "ld a, c", tr8::ld_reg_reg<0b111, 0b001> },    //
    { 1, "ld a, d", tr8::ld_reg_reg<0b111, 0b010> },    //
    { 1, "ld a, e", tr8::ld_reg_reg<0b111, 0b011> },    //
    { 1, "ld a, h", tr8::ld_reg_reg<0b111, 0b100> },    //
    { 1, "ld a, l", tr8::ld_reg_reg<0b111, 0b101> },    //
    { 1, "ld a, (hl)", tr8::ld_reg_reg<0b111, 0b110> }, //
    { 1, "ld a, a", tr8::ld_reg_reg<0b111, 0b111> },    //
    { 1, "add a, b", alu8::add<false, 0b000> },         //
    { 1, "add a, c", alu8::add<false, 0b001> },         //
    { 1, "add a, d", alu8::add<false, 0b010> },         //
    { 1, "add a, e", alu8::add<false, 0b011> },         //
    { 1, "add a, h", alu8::add<false, 0b100> },         //
    { 1, "add a, l", alu8::add<false, 0b101> },         //
    { 1, "add a, (hl)", alu8::add<false, 0b110> },      //
    { 1, "add a, a", alu8::add<false, 0b111> },         //
    { 1, "adc a, b", alu8::adc<false, 0b000> },         //
    { 1, "adc a, c", alu8::adc<false, 0b001> },         //
    { 1, "adc a, d", alu8::adc<false, 0b010> },         //
    { 1, "adc a, e", alu8::adc<false, 0b011> },         //
    { 1, "adc a, h", alu8::adc<false, 0b100> },         //
    { 1, "adc a, l", alu8::adc<false, 0b101> },         //
    { 1, "adc a, (hl)", alu8::adc<false, 0b110> },      //
    { 1, "adc a, a", alu8::adc<false, 0b111> },         //
    { 1, "sub a, b", alu8::sub<false, 0b000> },         //
    { 1, "sub a, c", alu8::sub<false, 0b001> },         //
    { 1, "sub a, d", alu8::sub<false, 0b010> },         //
    { 1, "sub a, e", alu8::sub<false, 0b011> },         //
    { 1, "sub a, h", alu8::sub<false, 0b100> },         //
    { 1, "sub a, l", alu8::sub<false, 0b101> },         //
    { 1, "sub a, (hl)", alu8::sub<false, 0b110> },      //
    { 1, "sub a, a", alu8::sub<false, 0b111> },         //
    { 1, "sbc a, b", alu8::sbc<false, 0b000> },         //
    { 1, "sbc a, c", alu8::sbc<false, 0b001> },         //
    { 1, "sbc a, d", alu8::sbc<false, 0b010> },         //
    { 1, "sbc a, e", alu8::sbc<false, 0b011> },         //
    { 1, "sbc a, h", alu8::sbc<false, 0b100> },         //
    { 1, "sbc a, l", alu8::sbc<false, 0b101> },         //
    { 1, "sbc a, (hl)", alu8::sbc<false, 0b110> },      //
    { 1, "sbc a, a", alu8::sbc<false, 0b111> },         //
    { 1, "and a, b", alu8::andOp<false, 0b000> },       //
    { 1, "and a, c", alu8::andOp<false, 0b001> },       //
    { 1, "and a, d", alu8::andOp<false, 0b010> },       //
    { 1, "and a, e", alu8::andOp<false, 0b011> },       //
    { 1, "and a, h", alu8::andOp<false, 0b100> },       //
    { 1, "and a, l", alu8::andOp<false, 0b101> },       //
    { 1, "and a, (hl)", alu8::andOp<false, 0b110> },    //
    { 1, "and a, a", alu8::andOp<false, 0b111> },       //
    { 1, "xor a, b", alu8::xorOp<false, 0b000> },       //
    { 1, "xor a, c", alu8::xorOp<false, 0b001> },       //
    { 1, "xor a, d", alu8::xorOp<false, 0b010> },       //
    { 1, "xor a, e", alu8::xorOp<false, 0b011> },       //
    { 1, "xor a, h", alu8::xorOp<false, 0b100> },       //
    { 1, "xor a, l", alu8::xorOp<false, 0b101> },       //
    { 1, "xor a, (hl)", alu8::xorOp<false, 0b110> },    //
    { 1, "xor a, a", alu8::xorOp<false, 0b111> },       //
    { 1, "or a, b", alu8::orOp<false, 0b000> },         //
    { 1, "or a, c", alu8::orOp<false, 0b001> },         //
    { 1, "or a, d", alu8::orOp<false, 0b010> },         //
    { 1, "or a, e", alu8::orOp<false, 0b011> },         //
    { 1, "or a, h", alu8::orOp<false, 0b100> },         //
    { 1, "or a, l", alu8::orOp<false, 0b101> },         //
    { 1, "or a, (hl)", alu8::orOp<false, 0b110> },      //
    { 1, "or a, a", alu8::orOp<false, 0b111> },         //
    { 1, "cp a, b", alu8::cp<false, 0b000> },           //
    { 1, "cp a, c", alu8::cp<false, 0b001> },           //
    { 1, "cp a, d", alu8::cp<false, 0b010> },           //
    { 1, "cp a, e", alu8::cp<false, 0b011> },           //
    { 1, "cp a, h", alu8::cp<false, 0b100> },           //
    { 1, "cp a, l", alu8::cp<false, 0b101> },           //
    { 1, "cp a, (hl)", alu8::cp<false, 0b110> },        //
    { 1, "cp a, a", alu8::cp<false, 0b111> },           //
    { 1, "ret nz", call::ret<true, 0b00> },             //
    { 1, "pop bc", tr16::pop<0b00> },                   //
    { 3, "jp nz, a16", jp::jump_if<false, 0b00> },      //
    { 3, "jp a16", jp::jump<false> },                   //
    { 3, "call nz, a16", call::call<true, 0b00> },      //
    { 1, "push bc", tr16::push<0b00> },                 //
    { 2, "add a, d8", alu8::add<true, 0b111> },         //
    { 1, "rst 00h", call::rst<0b000> },                 //
    { 1, "ret z", call::ret<true, 0b01> },              //
    { 1, "ret", call::ret<false, 0b00> },               //
    { 3, "jp z, a16", jp::jump_if<false, 0b01> },       //
    { 1, "prefix cb", prefix_cb },                      //
    { 3, "call z, a16", call::call<true, 0b01> },       //
    { 3, "call a16", call::call<false, 0b00> },         //
    { 2, "adc a, d8", alu8::adc<true, 0b111> },         //
    { 1, "rst 08h", call::rst<0b001> },                 //
    { 1, "ret nc", call::ret<true, 0b10> },             //
    { 1, "pop de", tr16::pop<0b01> },                   //
    { 3, "jp nc, a16", jp::jump_if<false, 0b10> },      //
    { 1, "unimplemented", unimplemented },              //
    { 3, "call nc, 16", call::call<true, 0b10> },       //
    { 1, "push de", tr16::push<0b10> },                 //
    { 2, "sub d8", alu8::sub<true, 0b111> },            //
    { 1, "rst 10h", call::rst<0b010> },                 //
    { 1, "ret c", call::ret<true, 0b11> },              //
    { 1, "reti", call::reti },                          //
    { 3, "jp c, a16", jp::jump_if<false, 0b11> },       //
    { 1, "unimplemented", unimplemented },              //
    { 3, "call c, a16", call::call<true, 0b11> },       //
    { 1, "unimplemented", unimplemented },              //
    { 2, "sbc a, d8", alu8::sbc<true, 0b111> },         //
    { 1, "rst 18h", call::rst<0b011> },                 //
    { 2, "ldh (a8), a", tr8::st_io_imm_a },             //
    { 1, "pop hl", tr16::pop<0b010> },                  //
    { 1, "ld (c), a", tr8::st_io_c_a },                 //
    { 1, "unimplemented", unimplemented },              //
    { 1, "unimplemented", unimplemented },              //
    { 1, "push hl", tr16::push<0b10> },                 //
    { 2, "and d8", alu8::andOp<true, 0b111> },          //
    { 1, "rst 20h", call::rst<0b100> },                 //
    { 2, "add sp, r8", alu16::add_sp },                 //
    { 1, "jp (hl)", jp::jump_hl },                      //
    { 3, "ld (a16), a", tr8::st_a<true, 0b00> },        //
    { 1, "unimplemented", unimplemented },              //
    { 1, "unimplemented", unimplemented },              //
    { 1, "unimplemented", unimplemented },              //
    { 2, "xor d8", alu8::xorOp<true, 0b000> },          //
    { 1, "rst 28h", call::rst<0b101> },                 //
    { 2, "ldh a, (a8)", tr8::ld_a_io_imm },             //
    { 1, "pop af", tr16::pop<0b11> },                   //
    { 1, "ld a, (c)", tr8::ld_a_io_c },                 //
    { 1, "di", misc::di },                              //
    { 1, "unimplemented", unimplemented },              //
    { 1, "push af", tr16::push<0b11> },                 //
    { 2, "or d8", alu8::orOp<true, 0b000> },            //
    { 1, "rst 30h", call::rst<0b110> },                 //
    { 1, "ldhl", tr16::ldhl },                          //
    { 1, "ld sp, hl", tr16::ld_sp_hl },                 //
    { 3, "ld a, (a16)", tr8::ld_a },                    //
    { 1, "ei", misc::ei },                              //
    { 1, "unimplemented", unimplemented },              //
    { 1, "unimplemented", unimplemented },              //
    { 2, "cp d8", alu8::cp<true, 0b000> },              //
    { 1, "rst 38h", call::rst<0b111> },                 //
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
    { 2, "rr b", bits::rr<0b000> },        //
    { 2, "rr c", bits::rr<0b001> },        //
    { 2, "rr d", bits::rr<0b010> },        //
    { 2, "rr e", bits::rr<0b011> },        //
    { 2, "rr h", bits::rr<0b100> },        //
    { 2, "rr l", bits::rr<0b101> },        //
    { 2, "rr (hl)", bits::rr<0b110> },     //
    { 2, "rr a", bits::rr<0b111> },        //
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
    { 2, "srl b", bits::srl<0b000> },      //
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
