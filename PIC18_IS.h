//
//  PIC18_IS.h
//  
//
//  Created by Greg Norman on 26/1/2023.
//

#ifndef PIC18_IS_h
#define PIC18_IS_h


enum {
    PIC18F_FDA_CASE = 1,
    PIC18F_FA_CASE,
    PIC18F_DW_CASE,
    PIC18F_DW_CASE2,
    PIC18F_BIT_CASE,
    PIC18F_CTRL_CASE,
    PIC18F_CTRL_CASE2,
    PIC18F_CALL_CASE,
    PIC18F_NO_ARGS,
    PIC18F_NOP_CASE,
    PIC18F_LIT_CASE,
    PIC18F_LIT_CASE2,
    PIC18F_RET_CASE,
    PIC18F_TBLRD_CASE  // table read is normally a NO_ARG, but the compiler will look at the address to be read and treat it as ascii
                            
};

#include <stdint.h>
#include <vector>
#include <string>
#include <iostream>



class PIC18F_IS{

public:


    const std::vector<std::string> PIC18F_MNEMONICS ={
        "ADDWF",    "ADDWFC",   "ANDWF",    "CLRF",     "COMF",                 ///1
        "CPFSEQ",   "CPFSGT",   "CPFSLT",   "DECF",     "DECFSZ",
        "DCFSNZ",   "INCF",     "INCFSZ",   "INFSNZ",   "IORWF",
        "MOVF",     "MOVFF",    "MOVWF",    "MULWF",    "NEGF",
        "RLCF",     "RLNCF",    "RRCF",     "RRNCF",    "SETF", //5
        "SUBFWB",   "SWAPF",    "TSTFSZ",   "XORWF",    "BCF",
        "BSF",      "BTFSC",    "BTFSS",    "BTG",      "BC",
        "BN",       "BNC",      "BNN",      "BNOV",     "BNZ",
        "BOV",      "BRA",      "BZ",       "CALL",     "CLRWDT",
        "DAW",      "GOTO",     "NOP",      "NOP",      "POP",        //10
        "PUSH",     "RCALL",    "RESET",    "RETFIE",   "RETLW",
        "RETURN",   "SLEEP",    "ADDLW",    "ANDLW",    "IORLW",
        "LFSR",     "MOVLB",    "MOVLW",    "MULLW",    "RETLW",
        "SUBLW",    "XORLW",    "TBLRD*",   "TBLRD*+",  "TBLRD*-",  //14
        "TBLRD+*",  "TBLWT*",   "TBLWT*+",  "TBLWT*-",  "TBLWT+*",
        "SUBWF",    "SUBWFB"
    };
    //0b01101110 ffff ffff
    //0b0110111000000000
    const std::vector<uint16_t> PIC18F_OPCODES_LOWER_VAL =
    {
        0b0010010000000000,0b0010000000000000,0b0001010000000000,0b0110101000000000,0b0001110000000000,  ///1
        0b0110001000000000,0b0110010000000000,0b0110000000000000,0b0000010000000000,0b0010110000000000,
        0b0100110000000000,0b0010100000000000,0b0011110000000000,0b0100100000000000,0b0001000000000000,
        0b0101000000000000,0b1100000000000000,0b0110111000000000,0b0000001000000000,0b0110110000000000,
        0b0011010000000000,0b0100010000000000,0b0011000000000000,0b0100000000000000,0b0110100000000000, //5
        0b0101010000000000,0b0011100000000000,0b0110011000000000,0b0001100000000000,0b1001000000000000,
        0b1000000000000000,0b1011000000000000,0b1010000000000000,0b0111000000000000,0b1110001000000000,
        0b1110011000000000,0b1110001100000000,0b1110011100000000,0b1110010100000000,0b1110000100000000,
        0b1110010000000000,0b1101000000000000,0b1110000000000000,0b1110110000000000,0b0000000000000100,
        0b0000000000000111,0b1110111100000000,0b0000000000000000,0b1111000000000000,0b0000000000000110,    //10
        0b0000000000000101,0b1101100000000000,0b0000000011111111,0b0000000000010000,0b0000110000000000,
        0b0000000000010010,0b0000000000000011,0b0000111100000000,0b0000101100000000,0b0000100100000000,
        0b1110111000000000,0b0000000100000000,0b0000111000000000,0b0000110100000000,0b0000110000000000,
        0b0000100000000000,0b0000101000000000,0b0000000000001000,0b0000000000001001,0b0000000000001010,
        0b0000000000001011,0b0000000000001100,0b0000000000001101,0b0000000000001110,0b0000000000001111,
        0b0101110000000000,0b0101100000000000
    };
    const std::vector<uint16_t> PIC18F_OPCODES_UPPER_VAL =
    {
        0b0010011111111111,0b0010001111111111,0b0001011111111111,0b0110101111111111,0b0001111111111111,
        0b0110001111111111,0b0110010111111111,0b0110000111111111,0b0000011111111111,0b0010111111111111,
        0b0100111111111111,0b0010101111111111,0b0011111111111111,0b0100101111111111,0b0001001111111111,
        0b0101001111111111,0b1100111111111111,0b0110111111111111,0b0000001111111111,0b0110110111111111,
        0b0011011111111111,0b0100011111111111,0b0011001111111111,0b0100001111111111,0b0110100111111111, //5
        0b0101011111111111,0b0011101111111111,0b0110011111111111,0b0001101111111111,0b1001111111111111,
        0b1000111111111111,0b1011111111111111,0b1010111111111111,0b0111111111111111,0b1110001011111111,
        0b1110011011111111,0b1110001111111111,0b1110011111111111,0b1110010111111111,0b1110000111111111,
        0b1110010011111111,0b1101011111111111,0b1110000011111111,0b1110110111111111,0b0000000000000100,
        0b0000000000000111,0b1110111111111111,0b0000000000000000,0b1111111111111111,0b0000000000000110,    //10
        0b0000000000000101,0b1101111111111111,0b0000000011111111,0b0000000000010001,0b0000110011111111,
        0b0000000000010011,0b0000000000000011,0b0000111111111111,0b0000101111111111,0b0000100111111111,
        0b1110111011111111,0b0000000100001111,0b0000111011111111,0b0000110111111111,0b0000110011111111,
        0b0000100011111111,0b0000101011111111,0b0000000000001000,0b0000000000001001,0b0000000000001010,
        0b0000000000001011,0b0000000000001100,0b0000000000001101,0b0000000000001110,0b0000000000001111,
        0b0101111111111111,0b0101101111111111
    };
    const std::vector<unsigned char> PIC18F_OPCODES_DECODE_CASE =
    {
        PIC18F_FDA_CASE,   PIC18F_FDA_CASE,   PIC18F_FDA_CASE,   PIC18F_FA_CASE,    PIC18F_FDA_CASE,
        PIC18F_FA_CASE,    PIC18F_FA_CASE,    PIC18F_FA_CASE,    PIC18F_FDA_CASE,   PIC18F_FDA_CASE,
        PIC18F_FDA_CASE,   PIC18F_FDA_CASE,   PIC18F_FDA_CASE,   PIC18F_FDA_CASE,   PIC18F_FDA_CASE,
        PIC18F_FDA_CASE,   PIC18F_DW_CASE,    PIC18F_FA_CASE,    PIC18F_FA_CASE,    PIC18F_FA_CASE,
        PIC18F_FDA_CASE,   PIC18F_FDA_CASE,   PIC18F_FDA_CASE,   PIC18F_FDA_CASE,   PIC18F_FA_CASE,    //5
        PIC18F_FDA_CASE,   PIC18F_FDA_CASE,   PIC18F_FA_CASE,    PIC18F_FDA_CASE,   PIC18F_BIT_CASE,
        PIC18F_BIT_CASE,   PIC18F_BIT_CASE,   PIC18F_BIT_CASE,   PIC18F_BIT_CASE,   PIC18F_CTRL_CASE,
        PIC18F_CTRL_CASE,  PIC18F_CTRL_CASE,  PIC18F_CTRL_CASE,  PIC18F_CTRL_CASE,  PIC18F_CTRL_CASE,
        PIC18F_CTRL_CASE,  PIC18F_CTRL_CASE2, PIC18F_CTRL_CASE,  PIC18F_CALL_CASE,  PIC18F_NO_ARGS,
        PIC18F_NO_ARGS,    PIC18F_DW_CASE2,   PIC18F_NOP_CASE,   PIC18F_NOP_CASE,   PIC18F_NO_ARGS,    //10
        PIC18F_NO_ARGS,    PIC18F_CTRL_CASE2, PIC18F_NO_ARGS,    PIC18F_RET_CASE,   PIC18F_LIT_CASE,
        PIC18F_RET_CASE,   PIC18F_NO_ARGS,    PIC18F_LIT_CASE,   PIC18F_LIT_CASE,   PIC18F_LIT_CASE,
        PIC18F_LIT_CASE2,  PIC18F_LIT_CASE,   PIC18F_LIT_CASE,   PIC18F_LIT_CASE,   PIC18F_LIT_CASE,
        PIC18F_LIT_CASE,   PIC18F_LIT_CASE,   PIC18F_NO_ARGS,    PIC18F_NO_ARGS,    PIC18F_NO_ARGS,
        PIC18F_NO_ARGS,    PIC18F_NO_ARGS,    PIC18F_NO_ARGS,    PIC18F_NO_ARGS,    PIC18F_NO_ARGS, //15
        PIC18F_FDA_CASE,   PIC18F_FDA_CASE
    };
    const std::vector<std::string> PIC18F_DESCRIPTIONS ={
        "ADDWF f,d,a; Add WREG and f",                      "ADDWFC f,d,a; Add WREG and CARRY bit to f",    "ANDWF f,d,a; AND WREG with f",             "CLRF f,a; Clear f",                        "COMF f,d,a; Complement f",                 ///1
        "CPFSEQ f,a; Compare f with WREG, skip =",          "CPFSGT f,a; Compare f with WREG, skip >",      "CPFSLT f,a; Compare f with WREG, skip <",  "DECF f,d,a; Decrement f",                  "DECFSZ f,d,a; Decrement f, skip if 0",
        "DCFSNZ f,d,a; Decrement f, skip if NOT 0",         "INCF f,d,a; Increment f",                      "INCFSZ f,d,a; Increment f, skip if 0",     "INFSNZ f,d,a; Increment f, skip if NOT 0", "IORWF f,d,a; Inclusive IR WREG with f",
        "MOVF f,d,a; Move f",                               "MOVFF fs,fd; Move fs to fd",                   "MOVWF f,a; Move WREG to f",                "MULWF f,a; Multiply WREG with f",          "NEGF f,a; Negate f",
        "RLCF f,d,a; Rotate left f through Carry",          "RLNCF f,d,a; Rotate left f (No Carry)",        "RRCF f,d,a; Rotate right f through Carry", "RRNCF f,d,a; Rotate right f (No Carry)",   "SETF f,a; Set f", //5
        "SUBFWB f,d,a; Subtract f from WREG with borrow",   "SWAPF f,d,a; Subtract WREG from f with borrow","TSTFSZ f,a;Test f, skip if 0",             "XORWF f,d,a; Exclusive OR WREG with f",    "BCF f,b,a; Bit Clear f",
        "BSF f,b,a; bit set f",                             "BTFSC f,b,a; Bit test f, skip if clear",       "BTFSS f,b,a; Bit test f, skip if set",     "BTG f,d,a; Bit toggle f" ,                 "BC n; Branch if Carry",
        "BN n; Branch if Negative",                         "BNC n; Brach if Not Carry",                    "BNN n; Branch if Not Negative",            "BNOV n; Branch if Not Overflow",           "BNZ n; Branch if not Zero",
        "BOV n; Branch if Overflow",                        "BRA n; Branch",                                "BZ n; Branch if 0",                        "CALL n,s; Call Subroutine",                "CLRWDT; Clear Watchdog Timer",
        "DAW; Decimal Adjust WREG",                         "GOTO n; Go to Address",                        "NOP; No Operation",                        "NOP; No Operation",                        "POP; Pop top of return Stack",        //10
        "PUSH; Push off return Stack",                      "RCALL n; Relative Call",                       "RESET; MCU software reset",                "RETFIE s; Return from Interrupt Enable",   "RETLW k; Return with literal in WREG",
        "RETURN s; Return from Subroutine",                 "SLEEP; Go to Standby Mode",                    "ADDLW k; add literal and WREG",            "ANDLW k; AND literal with WREG",           "IORLW k; Inclusive OR with WREG",
        "LFSR f,k; Move Literal (12-bit) to FSR(f)",        "MOVLB k; Move literal to BSR<3:0>",            "MOVLW k; Move literal to WREG",            "MULLW k; Multiply literal with WREG",      "RETLW k; Return with literal in WREG",
        "SUBLW k; Subtract WREG from literal",              "XORLW; Exclusive OR literal with WREG",        "TBLRD*; Table Read",                       "TBLRD*+; Table Read with post-increment",  "TBLRD*-; Table Read with post-decrement",
        "TBLRD+*; Table Read with pre-increment",           "TBLWT*; Table Write",                          "TBLWT*+; Table Write with post-increment", "TBLWT*-; Table Write with post-decrement", "TBLWT+*; Table Write with pre-increment",
        "SUBWF f,d,a; Subtract WREG from f",                "SUBWFB f,d,a; Subtract WREG from f with borrow"
    };

};

class PIC18F_FULL_IS: public PIC18F_IS
{
public:
    uint32_t Config_Address = 0x30;
    uint32_t Device_ID_Address = 0x20;
    uint32_t FLASH_size = 0xFFFF;
    uint32_t EEPROM_START_ADDR = 0xF0;
    std::vector<std::string> Define_Byte_Tags;
    std::vector<uint32_t> Define_byte_tag_positions;
    std::string MCU;

    // the mcu_type contains the name of the header file
    // we record this so that it can be included into the disassembled file
    void SetupRegisterNames(const char MCU_TYPE[])
    {
        std::cout << MCU_TYPE << " Selected - ";
        
        this->MCU.clear();
        
        const char *StartOfFileName = strrchr(MCU_TYPE,'/')+1;
        size_t Dot = strrchr(MCU_TYPE, '.')-StartOfFileName;
        
        char temp[Dot + 1];
        
        strncpy(temp,&StartOfFileName[0],Dot);
        temp[Dot] = 0;
        this->MCU = temp;

    }

};
#endif /* PIC18_IS_h */
