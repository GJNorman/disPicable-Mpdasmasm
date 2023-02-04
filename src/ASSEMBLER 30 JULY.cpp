//
//  main.cpp
//  annoyed
//
//  Created by Greg Norman on 19/5/21.
//

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <iterator>
#include <vector>
#include<string.h>
#include <stdarg.h>
#include <sstream>
//#define MAC_OS 1
#define WINDOWS_OS 1
#ifdef WINDOWS_OS
#include<malloc.h>
#endif


using namespace std;// %c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
(byte & 0x80 ? '1' : '0'), \
(byte & 0x40 ? '1' : '0'), \
(byte & 0x20 ? '1' : '0'), \
(byte & 0x10 ? '1' : '0'), \
(byte & 0x08 ? '1' : '0'), \
(byte & 0x04 ? '1' : '0'), \
(byte & 0x02 ? '1' : '0'), \
(byte & 0x01 ? '1' : '0')

#define PIC18f_FDA_CASE 1 //command has parameters f, d and a
#define PIC18f_FA_CASE 2
#define PIC18F_DW_CASE 3
#define PIC18F_DW_CASE2 4
#define PIC18F_BIT_CASE 5
#define PIC18F_CONTROL_CASE 6
#define PIC18F_CONTROL_CASE2 7
#define PIC18F_CALL_CASE 8
#define PIC18F_NO_ARGS 9
#define PIC18F_NOP_CASE 10
#define PIC18F_LIT_CASE 11
#define PIC18F_LIT_CASE2 12
#define PIC18F_RETURN_CASE 13

const char Assembly_Comment_Marker = '/';
class Converted_Assembly_Code{
public:
    std::vector<uint64_t> LABEL_POSITIONS;
    std::vector<string> LABEL_STRINGS;
    std::vector<string> Address;
    std::vector<string> ASSEMBLY_CODE_FULL_PROGRAM;
};
class Thumb_IS{

};
class PIC18F_IS{
private:
    ///otherwise  text too long
    #define FDA_CASE PIC18f_FDA_CASE
    #define FA_CASE PIC18f_FA_CASE
    #define DW_CASE PIC18F_DW_CASE
    #define DW_CASE2 PIC18F_DW_CASE2
    #define BIT_CASE PIC18F_BIT_CASE
    #define CTRL_CASE PIC18F_CONTROL_CASE
    #define CTRL_CASE2 PIC18F_CONTROL_CASE2
    #define CALL_CASE PIC18F_CALL_CASE
    #define NO_ARGS PIC18F_NO_ARGS
    #define NOP_CASE PIC18F_NOP_CASE
    #define LIT_OPS PIC18F_LIT_CASE
    #define LIT_OPS2 PIC18F_LIT_CASE2
    #define RET_CASE PIC18F_RETURN_CASE

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
        "SUBLW",    "XORLW",    "TBLRD*",   "TBLRD*+",  "TBLRD*-",
        "TBLRD+*",  "TBLWT*",   "TBLWT*+",  "TBLWT*-",  "TBLWT+*",
        "SUBWF",    "SUBWFB"
    };
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
        FDA_CASE,   FDA_CASE,   FDA_CASE,   FA_CASE,    FDA_CASE,
        FA_CASE,    FA_CASE,    FA_CASE,    FDA_CASE,   FDA_CASE,
        FDA_CASE,   FDA_CASE,   FDA_CASE,   FDA_CASE,   FDA_CASE,
        FDA_CASE,   DW_CASE,    FA_CASE,    FA_CASE,    FA_CASE,
        FDA_CASE,   FDA_CASE,   FDA_CASE,   FDA_CASE,   FA_CASE,    //5
        FDA_CASE,   FDA_CASE,   FA_CASE,    FDA_CASE,   BIT_CASE,
        BIT_CASE,   BIT_CASE,   BIT_CASE,   BIT_CASE,   CTRL_CASE,
        CTRL_CASE,  CTRL_CASE,  CTRL_CASE,  CTRL_CASE,  CTRL_CASE,
        CTRL_CASE,  CTRL_CASE2, CTRL_CASE,  CALL_CASE,  NO_ARGS,
        NO_ARGS,    DW_CASE2,   NOP_CASE,   NOP_CASE,   NO_ARGS,    //10
        NO_ARGS,    CTRL_CASE2, NO_ARGS,    RET_CASE,   LIT_OPS,
        RET_CASE,   NO_ARGS,    LIT_OPS,    LIT_OPS,    LIT_OPS,
        LIT_OPS2,   LIT_OPS,    LIT_OPS,    LIT_OPS,    LIT_OPS,
        LIT_OPS,    LIT_OPS,    NO_ARGS,    NO_ARGS,    NO_ARGS,
        NO_ARGS,    NO_ARGS,    NO_ARGS,    NO_ARGS,    NO_ARGS, //15
        FDA_CASE,   FDA_CASE
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
//add chip - specific register names
class PIC18F46K22_REG{
public:

    const std::vector<std::string> PIC18F46K22_REGISTERS ={
        "TOSU","TOSH","TOSL","STKPTR","PCLATU","PCLATH","PCL","TBLPTRU","TBLPTRH","TBLPTRL","TABLAT",
        "PRODH","PRODL","INTCON","INTCON2","INTCON3","INDF0","POSTINC0","POSTDEC0","PREINC0","PLUSW0",
        "FSR0H","FSR0L","WREG","INDF1","POSTINC1","POSTDEC1","PREINC1","PLUSW1","FSR1H","FSR1L","BSR",
        "INDF2","POSTINC2","POSTDEC2","PREINC2","PLUSW2","FSR2H","FSR2L","STATUS", "TMR0H","TMR0L",
        "T0CON","U","OSCCON","OSCCON2",

        "WDTCON","RCON","TMR1H","TMR1L","T1CON","T1GCON","SSP1CON3","SSP1MSK","SSP1BUF","SSP1ADD","SSP1STAT",
        "SSP1CON1","SSP1CON2","ADRESH","ADRESL","ADCON0","ADCON1","ADCON2","CCPR1H","CCPR1L","CCP1CON","TMR2","PR2","T2CON",
        "PSTR1CON","BAUDCON1","PWN1CON","ECCP1AS","U","T3GCON","TMR3H","TMR3L","T3CON","SPBRGH1",

        "SPBRG1","RCREG1","TXREG1","TXSTA1","RCSTA1","EEADRH","EEADR","EEDATA","EECON2","EECON1",
        "IPR3","PIR3","PIE3","IPR2","PIR2","PIE2","IPR1","PIR1","PIE1","HLVDCON","OSCTUNE","U","U","U","U",
        "TRISE","TRISD","TRISC","TRISB","TRISA","U","U","U","U","LATE","LATD","LATC","LATB","LATA","U",

        "U","U","U","PORTE","PORTD","PORTC","PORTB","PORTA","IPR5","PIR5","PIE5","IPR4","PIR4","PIE4","CM1CON0",
        "CM2CON0","CM2CON1","SPBRGH2","SPBRG2","RCREG2","TXREG2","TXSTA2","RCSTA2","BAUDCON2","SSP2BUF","SSP2ADD",
        "SSP2STAT","SSP2CON1","SSP2CON2","SSP2MSK","SSP2CON3","CCPR2H","CCPR2L","CCP2CON","PWM2CON","ECCP2AS","PSTR2CON",
        "IOCB","WPUB","SLRCON",

        "CCPR3H","CCPR3L","CCP3CON","PWM3CON","ECCP3AS","PSTR3CON","CCPR4H","CCPR4L","CCP4CON","CCPR5H","CCPR5L","CP5CON",
        "TMR4","PR4","T4CON","TMR5H","TMR5L","T5CON","T5GCON","TMR6","PR6","T6CON","CCPTMRS0","CCPTMRS1","SRCON0","SRCON1",
        "CTMUCONH","CTMUCONL","CTMUICON","VREFCON0","VREFCON1","VREFCON2","PMD0","PMD1","PMD2","ANSELE","ANSELD","ANSELC",
        "ANSELB","ANSELA"
    };
    const uint32_t PIC18F46K22_Config_Address = 0x30;
    const uint32_t PIC18F46K22_device_ID_Address = 0x20;
    const uint32_t PIC18F46K22_Bank_size = 0xFFFF;
};
class PIC18F45K20_REG{
public:

    const std::vector<std::string> PIC18F45K20_REGISTERS ={
        "TOSU","TOSH","TOSL","STKPTR","PCLATU","PCLATH","PCL","TBLPTRU","TBLPTRH","TBLPTRL","TABLAT",
        "PRODH","PRODL","INTCON","INTCON2","INTCON3","INDF0","POSTINC0","POSTDEC0","PREINC0","PLUSW0",
        "FSR0H","FSR0L","WREG","INDF1","POSTINC1","POSTDEC1","PREINC1","PLUSW1","FSR1H","FSR1L","BSR",
        "INDF2","POSTINC2","POSTDEC2","PREINC2","PLUSW2","FSR2H","FSR2L","STATUS", "TMR0H","TMR0L",
        "T0CON","U","OSCCON","HLVDCON",

        "WDTCON","RCON","TMR1H","TMR1L","T1CON","TMR2","PR2","T2CON","SSPBUF","SSPADD","SSPSTAT",
        "SSPCON1","SSPCON2","ADRESH","ADRESL","ADCON0","ADCON1","ADCON2","CCPR1H","CCPR1L","CCP1CON","CCPR2H","CCPR2L","CCP2CON",
        "PSTRCON","BAUDCON","PWN1CON","ECCP1AS","CVRCON","CVRCON2","TMR3H","TMR3L","T3CON","SPBRGH",

        "SPBRG","RCREG","TXREG","TXSTA","RCSTA","EEADRH","EEADR","EEDATA","EECON2","EECON1",
        "U","U","U","IPR2","PIR2","PIE2","IPR1","PIR1","PIE1","U","OSCTUNE","U","U","U","U",
        "TRISE","TRISD","TRISC","TRISB","TRISA","U","U","U","U","LATE","LATD","LATC","LATB","LATA","U",

        "U","U","U","PORTE","PORTD","PORTC","PORTB","PORTA","ANSELH","ANSEL","IOCB","WPUB","CM1CON0",
        "CM2CON0","CM2CON1","SLRCON","SSPMSK","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U",

        "U","U","U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U","U","U",
        "U","U"
    };
    const uint32_t PIC18F45K20_Config_Address = 0x30;
    const uint32_t PIC18F45K20_device_ID_Address = 0x20;
    const uint32_t PIC18F45K20_Bank_size = 0x7FFF;
};
class PIC18F48585_REG{
public:

    const std::vector<std::string> PIC18F8585_REGISTERS ={
        "TOSU","TOSH","TOSL","STKPTR","PCLATU","PCLATH","PCL","TBLPTRU","TBLPTRH","TBLPTRL","TABLAT",
        "PRODH","PRODL","INTCON","INTCON2","INTCON3","INDF0","POSTINC0","POSTDEC0","PREINC0","PLUSW0",
        "FSR0H","FSR0L","WREG","INDF1","POSTINC1","POSTDEC1","PREINC1","PLUSW1","FSR1H","FSR1L","BSR",

        "INDF2","POSTINC2","POSTDEC2","PREINC2","PLUSW2","FSR2H","FSR2L","STATUS", "TMR0H","TMR0L",
        "T0CON","U","OSCCON","LVDCON","WDTCON","RCON","TMR1H","TMR1L","T1CON","TMR2","PR2","T2CON","SSPBUF","SSPADD","SSPSTAT",
        "SSPCON1","SSPCON2","ADRESH","ADRESL","ADCON0","ADCON1","ADCON2",

        "CCPR1H","CCPR1L","CCP1CON","CCPR2H","CCPR2L","CCP2CON",
        "U","U","U","ECCP1AS","CVRCON","CMCON","TMR3H","TMR3L","T3CON","PSPCON",
        "SPBRG","RCREG","TXREG","TXSTA","RCSTA","EEADRH","EEADR","EEDATA","EECON2","EECON1",
        "IPR3","PIR3","PIE3","IPR2","PIR2","PIE2",

        "IPR1","PIR1","PIE1","MEMCON","U","TRISJ","TRISH","TRISG","TRISF",
        "TRISE","TRISD","TRISC","TRISB","TRISA","LATJ","LATH","LATG","LATF","LATE","LATD","LATC","LATB","LATA",
        "PORTJ","PORTH","PORTG","PORTF","PORTE","PORTD","PORTC","PORTB","PORTA",

        "SPBRGH","BAUDCON","U","U","U","U","ECCP1DEL","U","ECANCON",
        "TXERRCNT","RXERRCNT","COMSTAT","CIOCON","BRGCON3","BRGCON2","BRGCON1","CANCON","CANSTAT",
        "RXB0D7","RXB0D6", "RXB0D5","RXB0D4","RXB0D3","RXB0D2","RXB0D1","RXB0D1","RXB0D0",
        "RXB0DLC","RXB0EIDL","RXB0EIDH","RXB0SIDL","RXB0SIDH","RXB0CON",///off by one

        "CANCON_RO0","CANSTAT_RO0","RXB1D7","RXB1D6","RXB1D5","RXB1D4","RXB1D3","RXB1D2","RXB1D1","RXB1D0",
        "RXB1DLC","RXB1EIDL","RXB1EIDH","RXB1SIDL","RXB1SIDH","RXB1CON","CANCON_RO1","CANSTAT_RO1",
        "TXB0D7","TXB0D6", "TXB0D5","TXB0D4","TXB0D3","TXB0D2","TXB0D1","TXB0D0",
        "TXB0DLC","TXB0EIDL","TXB0EIDH","TXB0SIDL","TXB0SIDH","TXB0CON",

        "CANCON_RO2","CANSTAT_RO2","TXB1D7","TXB1D6", "TXB1D5","TXB1D4","TXB1D3","TXB1D2","TXB1D1","TXB1D0",
        "TXB1DLC","TXB1EIDL","TXB1EIDH","TXB1SIDL","TXB1SIDH","TXB1CON","CANCON_RO3","CANSTAT_RO3",
        "TXB2D7","TXB2D6", "TXB2D5","TXB2D4","TXB2D3","TXB2D2","TXB2D1","TXB2D0",
        "TXB2DLC","TXB2EIDL","TXB2EIDH","TXB2SIDL","TXB2SIDH","TXB2CON",

        "RXM1EIDL","RXM1EIDH","RXM1SIDL","RXM1SIDH","RXM0EIDL","RXM0EIDH","RXM0SIDL","RXM0SIDH",
        "RXF5EIDL","RXF5EIDH","RXF5SIDL","RXF5SIDH","RXF4EIDL","RXF4EIDH","RXF4SIDL","RXF4SIDH",
        "RXF3EIDL","RXF3EIDH","RXF3SIDL","RXF3SIDH","RXF2EIDL","RXF2EIDH","RXF2SIDL","RXF2SIDH",
        "RXF1EIDL","RXF1EIDH","RXF1SIDL","RXF1SIDH","RXF0EIDL","RXF0EIDH","RXF0SIDL","RXF0SIDH",

        "U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U",
        "U","U",

        "U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U",
        "U","U",

        "U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U",
        "U","U",

        "U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U",
        "U","U",

        "CANCON_RO4","CANSTAT_RO4","B5D7","B5D6","B5D5","B5D4","B5D3","B5D2","B5D1","B5D0","B5DLC","B5EIDL",
        "B5EIDH","B5SIDL","B5SIDH","B5CON","CANCON_RO5","CANSTAT_RO5",
        "B4D7","B4D6","B4D5","B4D4","B4D3","B4D2","B4D1","B4D0","B4DLC","B4EIDL",
        "B4EIDH","B4SIDL","B4SIDH","B4CON",

        "CANCON_RO6","CANSTAT_RO6",

        "B3D7","B3D6","B3D5","B3D4","B3D3","B3D2","B3D1","B3D0","B3DLC","B3EIDL",
        "B3EIDH","B3SIDL","B3SIDH","B3CON","CANCON_RO7","CANSTAT_RO7",

        "B2D7","B2D6","B2D5","B2D4","B2D3","B2D2","B2D1","B2D0","B2DLC","B2EIDL",
        "B2EIDH","B2SIDL","B2SIDH","B2CON","CANCON_RO8","CANSTAT_RO8",

        "B1D7","B1D6","B1D5","B1D4","B1D3","B1D2","B1D1","B1D0","B1DLC","B1EIDL",
        "B1EIDH","B1SIDL","B1SIDH","B1CON","CANCON_RO9","CANSTAT_RO9",

        "B0D7","B0D6","B0D5","B0D4","B0D3","B0D2","B0D1","B0D0","B0DLC","B0EIDL",
        "B0EIDH","B0SIDL","B0SIDH","B0CON",

        "U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U",
        "U","U",

        "U","U","U","TXBIE","U","BIE0","U","BSEL0","U","U","U","U","MSEL3","MSEL2","MSEL1","MSEL0",
        "U","U","U","U","U","U","U","U","RXFBCON7","RXFBCON6","RXFBCON5","RXFBCON4","RXFBCON3","RXFBCON2","RXFBCON1","RXFBCON0",

        "U","U","U","U","U","U","U","SDFLC","U","U","RXFCON1","RFXCON0",
        "U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U",

        "U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U",
        "U","U","U","U","U","U","U","U","U","U",
        "U","U",

        "U","U","U","U","U","U","U","U","U","U",
        "U","U","RXF15EIDL","RXF15EIDH","RXF15SIDL","RXF15SIDH","U","U","U","U",
        "RXF14EIDL","RXF14EIDH","RXF14SIDL","RXF14SIDH","RXF13EIDL","RXF13EIDH","RXF13SIDL","RXF13SIDH",
        "RXF12EIDL","RXF12EIDH","RXF12SIDL","RXF12SIDH","U","U","U","U",

        "RXF11EIDL","RXF11EIDH","RXF11SIDL","RXF11SIDH","RXF10EIDL","RXF10EIDH","RXF10SIDL","RXF10SIDH",
        "RXF9EIDL","RXF9EIDH","RXF9SIDL","RXF9SIDH","U","U","U","U",
        "RXF8EIDL","RXF8EIDH","RXF8SIDL","RXF8SIDH","RXF7EIDL","RXF7EIDH","RXF7SIDL","RXF7SIDH",
        "RXF6EIDL","RXF6EIDH","RXF6SIDL","RXF6SIDH"
    };
    const uint32_t PIC18F8585_Config_Address = 0x30;
    const uint32_t PIC18F8585_device_ID_Address = 0x20;
    const uint32_t PIC18F8585_Bank_size = 0xBFFF;
};
class PIC18F_FULL_IS:   public PIC18F45K20_REG,
                        public PIC18F46K22_REG,
                        public PIC18F_IS,
                        public PIC18F48585_REG
{
public:
    std::vector<std::string> PIC18F_REGS;
    uint32_t Config_Address = 0x300000;
    uint32_t Device_ID_Address = 0x200000;
    uint32_t Bank_size = 0xFFFF;
    uint32_t LUP_ADDR_START = 0x7a4f;
    uint32_t LUP_ADDR_END = 0x7c4c;
    void REG_NAMES_SETUP(const std::vector<std::string> Device_REGISTERs, uint32_t device_id_addr, uint32_t device_config_addr,uint32_t device_Bank_size)
    {
        for(uint16_t counter=0;counter<Device_REGISTERs.size();counter++)
            this->PIC18F_REGS.push_back( Device_REGISTERs[counter]);

        Device_ID_Address = device_id_addr;
        Config_Address = device_config_addr;
        Bank_size = device_Bank_size;

        std::cout << "Device Memory: " << Bank_size << "\n";
    }
    void SetupRegisterNames(const char MCU_TYPE[])
    {
        std::cout << MCU_TYPE << " Selected - ";

        if(strstr(MCU_TYPE, "PIC18F45K20"))
        {
            REG_NAMES_SETUP(PIC18F45K20_REGISTERS,PIC18F45K20_device_ID_Address,PIC18F45K20_Config_Address,PIC18F45K20_Bank_size);
        }
        if(strstr(MCU_TYPE, "PIC18F46K22"))
        {
            REG_NAMES_SETUP(PIC18F46K22_REGISTERS,PIC18F46K22_device_ID_Address,PIC18F46K22_Config_Address,PIC18F46K22_Bank_size);
        }
        if(strstr(MCU_TYPE, "PIC18F8585"))
        {
            REG_NAMES_SETUP(PIC18F8585_REGISTERS,PIC18F8585_device_ID_Address,PIC18F8585_Config_Address,PIC18F8585_Bank_size);
        }
    }

};
class MyChar
{
    char *mychar;
public:
    MyChar()
    {
        mychar = (char*)malloc(1);
    }
    ~MyChar()
    {
        free(mychar);
    }
};
#ifdef WINDOWS_OS
const char *Global_working_directory = "C:/Users/Administrator/Desktop/Assembler/Assembler";
#endif
#ifdef MAC_OS
const char *Global_working_directory ="/Users/gregnorman/Desktop/annoyed";
#endif
    void print_as_bits(uint8_t OutputBytes);
    uint32_t Convert_num_to_hex( std::vector<unsigned char> input,uint64_t &vec_start, uint8_t return_len);
    void Copy_Vector_to_JPEG_FILE(std::vector<unsigned char> buffer, const char *Output_file_name);
    void Copy_FIRMWARE_FILE_to_Buffer(const char *Input_File_dir,std::vector<unsigned char> &OutputFileContents);
    void generate_asm(std::vector<uint8_t> &MachineCode, uint8_t number_of_bytes, uint32_t &address,std::vector<unsigned char> &HEX_FILE_CONTENTS,uint64_t file_position, bool &final_byte_is_double_word, Converted_Assembly_Code &OutputAssemblyCode,PIC18F_FULL_IS &Instruction_Set, bool bdisplayASM, bool bDisplayBinContents,uint16_t Address_Upper_16_bits);
    void Disassemble(std::vector<unsigned char> &OutputFileContents, bool bDisplayBinContents,bool bDisplayAssembly,PIC18F_FULL_IS &Instruction_Set);
    void Assemble(const char *Input_File_dir, bool bDisplayBinContents,bool bDisplayAssembly,PIC18F_FULL_IS &Instruction_Set);
    uint16_t check_n(int16_t n, uint16_t bitmask,uint16_t negative_sign_mask, uint32_t program_counter);
    uint32_t check_n_for_assembly(int32_t f, uint32_t program_counter);
    char *translate_label_to_mem_address(const char *Label_instruction,PIC18F_FULL_IS &Instruction_Set, Converted_Assembly_Code &AssemblyCode, bool CALL_USES_EXTERNAL_MEM, uint32_t start_address, uint32_t Instruction_address);
    void compare_generated_and_original_hex_files(const char *OrignialFileDir,const char *GeneratedFileDir);
    void increment_assembler_address(uint16_t &upper_16_bits, uint32_t &lower_32_bits, FILE *fp,uint16_t &check_sum, bool &check_sum_required, uint32_t Bank_size, bool bDisplayBinContents);
    bool check_against_list_of_instructions(std::vector<std::string> Instruction_list, const char *Input_string,  char *&OutputString);
    void output_Machine_Code(bool display_on_screen, FILE *fp, const char *Format_Specifier, ...);
    char * hex_to_ascii(const char *Input);
int main()
{
    PIC18F_FULL_IS Instruction_Set;

    bool bDisplayBinContents = false;
    bool bDisplayAssembly = false;

    const char *SourceFiles[] ={ "CAR_II_Version_3.46.hex","GLP_FIRMWARE619_UPDATE.hex","CAR_III_Version_4.06.hex","ChargeLane_Version_2.12_production.hex"};
    const char *Device_Types[] ={"PIC18F45K20","PIC18F8585","PIC18F46K22","PIC18F46K22"};

    int file_selector = 0;

    for(uint8_t counter=0;counter<4;counter++)
    {
        std::cout<<file_selector++ <<" -> " << SourceFiles[counter] <<std::endl;
    }
    std::cout << "Please Select File: ";
    scanf("%d",&file_selector);

    Instruction_Set.SetupRegisterNames(Device_Types[file_selector]);

    printf("Source File: %s/%s\n",Global_working_directory,SourceFiles[file_selector]);

    printf("Display Binary:     %s\n", bDisplayBinContents ? "true" : "false");
    printf("Display Assembly:   %s\n", bDisplayAssembly ? "true" : "false");

    char filedir[300]="";
    int function_selector = 0;
    std::cout << "Select Mode: "<< function_selector++ <<" -> Convert Hex File to PIC Assembly\n";
    std::cout << "             "<< function_selector++ <<" -> Convert PIC Assembly to Hex File\n";
    std::cout << "             "<< function_selector++ <<" -> Compare Original and Generated Hex Files\n";
    std::cout << "             "<< function_selector++ <<" -> Quit\n";
    std::cout << "             "<< function_selector++ <<" -> Change Settings\n";
    while(1)
    {
        printf("Select Mode: ");
        scanf("%u",&function_selector);

        if(function_selector==0)
        {
            std::vector<unsigned char> FileContents;
            sprintf(filedir,"%s/%s",Global_working_directory,SourceFiles[file_selector]);

            Copy_FIRMWARE_FILE_to_Buffer(filedir,FileContents);
            Disassemble(FileContents,  bDisplayBinContents, bDisplayAssembly,Instruction_Set);

            FileContents.clear();
        }
        if(function_selector==1)
        {
            sprintf(filedir,"%s/code.txt",Global_working_directory);
            Assemble(filedir, bDisplayBinContents, bDisplayAssembly,Instruction_Set);
        }

        if(function_selector==2)
        {
            sprintf(filedir,"%s/%s",Global_working_directory,SourceFiles[file_selector]);
            char generatedFileDir[300]="";
            sprintf(generatedFileDir,"%s/code.hex",Global_working_directory);
            compare_generated_and_original_hex_files(filedir,generatedFileDir);
        }
        if(function_selector==3)
        {
            break;
        }
        if(function_selector==4)
        {
            char temp;
            printf("Display Assembly Code (y/n)? ");

            scanf(" %c",&temp);

            if((temp=='Y')||(temp=='y'))
            {
                bDisplayAssembly=true;
            }
            else
            {
                bDisplayAssembly=false;
            }

            printf("Display Binary Code (y/n)? ");
            scanf(" %c",&temp);
            if((temp=='Y')||(temp=='y'))
            {
                bDisplayBinContents=true;
            }
            else
            {
                bDisplayBinContents=false;
            }
            printf("Display Binary:     %s\n", bDisplayBinContents ? "true" : "false");
            printf("Display Assembly:   %s\n", bDisplayAssembly ? "true" : "false");
        }
    }
    return 0;
}
char *copy_out_substring(int p1, int p2, const char* parent)
{
    char *temp = (char*)malloc(sizeof(char) * (p2-p1+1));
    strncpy(temp,parent+p1,p2-p1);
    temp[p2-p1]=0;

    return temp;
}
void compare_generated_and_original_hex_files(const char *OrignialFileDir,const char *GeneratedFileDir)
{
    FILE *f_org = fopen(OrignialFileDir,"rb");
    FILE *f_gen = fopen(GeneratedFileDir,"rb");

    char line_org[1024]="";
    char line_gen[1024]="";

    int total_errors=0;
    while(fgets(line_org,1000,f_org))
    {
        if(fgets(line_gen,1000,f_gen)==NULL)
        {
            printf("Generated Hex file is shorter\n");
            break;
        }
        if(strcmp(line_gen,line_org)!=0)
        {
            printf("Error: \nGenLine%sOrgLine%s",line_gen,line_org);
            total_errors++;
         //   exit(0);
        }
    }
    printf("There are %d total errors\n",total_errors);

    fclose(f_org);
    fclose(f_gen);
}
char* return_substring(const char *parent_String, const char *searchTerm1,const char *searchTerm2, unsigned long search_start_offset, size_t search_end_offset, size_t search_end_offset2)
{
    if(strstr(&parent_String[search_start_offset],searchTerm1)==NULL)
    {
        return NULL;
    }
    unsigned long p1 = (int) (strstr(&parent_String[search_start_offset],searchTerm1)-parent_String)+search_end_offset;
    unsigned long p2= (int)strlen(parent_String);

    if(searchTerm2!=NULL)
    {
        if(strstr(&parent_String[p1],searchTerm2)==NULL)
        {
            return NULL;
        }
        p2 = (int) (strstr(&parent_String[p1],searchTerm2)-parent_String)+search_end_offset2;
    }
    if(p2-p1<0)
    {
        printf("ERROR PARSING: %s\n",parent_String);
        return NULL;
    }
    char *temp = (char*)malloc(sizeof(char)*(p2-p1+1));
    strncpy(temp,&parent_String[p1],p2-p1);
    temp[p2-p1]=0;

    return temp;
}
uint32_t convert_FSR_for_assembler(const char *FSR_REG,PIC18F_FULL_IS & Instruction_Set, uint32_t offset)
{
    char security[strlen(FSR_REG)+5];//to avoid aliasing
    sprintf(security,"  %s  ",FSR_REG);
    uint32_t f=0;
    uint16_t counter=0;
    for(string it: Instruction_Set.PIC18F_REGS)
    {
        char security2[strlen(it.c_str())+3];//to avoid aliasing
        sprintf(security2," %s ",it.c_str());
        if(strstr(security,security2))
        {
            f = 0xff - counter +offset;
            break;
        }
        counter++;
    }

    return f;
}
uint32_t find_f_value_for_assembly(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, uint32_t address)
{
    uint32_t f=0;
    char *f_prm = NULL;
    if(strstr(Assembly_Instruction,"0x"))
    {
        int p1 = (int)(strstr(Assembly_Instruction,"0x")-Assembly_Instruction);

        if(strstr(Assembly_Instruction+p1,","))
        {
            f_prm = return_substring(Assembly_Instruction,"0x", ",", 0,strlen( "0x"), 0);
        }
        else
        {
            f_prm = return_substring(Assembly_Instruction,"0x", NULL, 0,strlen( "0x"), 0);
        }
    }

    if(f_prm==NULL) //FSRs
    {
        f_prm = return_substring(Assembly_Instruction,Instruction_Set.PIC18F_MNEMONICS[pos].c_str(), ",", 0,strlen(Instruction_Set.PIC18F_MNEMONICS[pos].c_str()), 0);

        if(f_prm==NULL)
        {
            f_prm = return_substring(Assembly_Instruction,Instruction_Set.PIC18F_MNEMONICS[pos].c_str(), NULL, 0,strlen(Instruction_Set.PIC18F_MNEMONICS[pos].c_str()), 0);
        }
        if(strtol(f_prm,NULL,16)==1)
        {
            f=1;
        }
        else

        f=convert_FSR_for_assembler(f_prm,Instruction_Set,0);

    }
    else //f is a raw value
    {
        f = (strtol(f_prm, NULL, 16)&0xffff);
    }
    if(f_prm!=NULL)
    free(f_prm);
    //N needs futher processing
    if((Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos] == PIC18F_CONTROL_CASE)||(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos] == PIC18F_CONTROL_CASE2))
    {

        uint32_t n = check_n_for_assembly(f, address&0xffff);
        if((Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos] == PIC18F_CONTROL_CASE))
        {
            return n&0xff;
        }
        return n&0x7ff;//Used by BRA
    }

    return f;
}
uint8_t find_d_value_for_assembly(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos)
{
    uint8_t d_or_b=0;
    char *d_prm = return_substring(Assembly_Instruction, ",", ",", 0, strlen(","), 0);
    if(d_prm!=NULL)
    {
        if(strstr(d_prm,"0x")==NULL)
        {
            d_or_b = strtol(d_prm, NULL, 16);
        }
        else
        {
            char *d_val = return_substring(d_prm, "0x",NULL, 0, strlen("0x"), 0);

            if(d_val!=NULL)
            {
                d_or_b= strtol(d_val, NULL, 16);
                free(d_val);
            }
        }
        free(d_prm);
    }
    return d_or_b;
}
uint8_t find_a_value_for_assembly(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, size_t a_offset)
{
    uint8_t a=0;
    char *a_prm = return_substring(Assembly_Instruction, ",", NULL, (strstr(&Assembly_Instruction[0],",")-Assembly_Instruction)+a_offset, strlen(","), 0);
    if(a_prm!=NULL)
    {
        //a can be either 1 or 0;
        if(strstr(a_prm,"ACCESS"))
        {
            a=0;
        }
        else if(strstr(a_prm,"BANK"))
        {
            a=1;
        }
        else if(strstr(a_prm,"A"))
        {
            a=0;
        }
        else if(strstr(a_prm,"B"))
        {
            a=1;
        }
        else
        {
            a = strtol(a_prm, NULL, 16);//che sera sera
        }
        free(a_prm);
    }


    return a;
}
uint8_t find_LFSR_f_value(const char *Assembly_Instruction)
{
    char *f_prm = return_substring(Assembly_Instruction, "LFSR", ",", 0, strlen("LFSR"), 0);
    if(f_prm==NULL)
    {
        return 0;
    }
    uint8_t f = strtol(f_prm,NULL,16);
    free(f_prm);

    return f;
}
void add_to_checksum(uint16_t OPCODE, uint16_t &checksum)
{
    checksum+=(OPCODE&0xff)&0xff;
    checksum+=((0xff00&OPCODE)>>8)&0xff;
}
uint16_t check_if_FSR_or_RAW_REG(const char *REGISTER,PIC18F_FULL_IS & Instruction_Set)
{
    uint16_t value = 0;

    if(strstr(REGISTER,"0x"))
    {
        value = strtol(REGISTER,NULL,16);
    }
    else
    {
        value = convert_FSR_for_assembler(REGISTER,Instruction_Set,0xf00);
    }
    return value;
}
//used for MOVFF fs,fd
void find_FILE_values_for_assembly(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, uint32_t address, uint16_t &fs, uint16_t &fd)
{
    //convert_FSR_for_assembler(const char *FSR_REG,PIC18F_FULL_IS & Instruction_Set)
    char* FD = return_substring(Assembly_Instruction, ",",NULL, 0, strlen(","), 0);
    char temp[strlen(Assembly_Instruction)+1];
    sprintf(temp,"%s",Assembly_Instruction);
    strtok(temp,",");

    int p1 = (int)(strstr(temp,"MOVFF")-temp)+strlen("MOVFF");

    char *FS =copy_out_substring(p1,(int)strlen(temp),temp);


    fs = check_if_FSR_or_RAW_REG(FS,Instruction_Set);
    fd = check_if_FSR_or_RAW_REG(FD,Instruction_Set);
    free(FD);
    free(FS);
    return ;
}
void check_if_single_word_branch_will_work(uint32_t k, uint32_t address,const char *Assembly_Instruction,uint32_t &OPCODE_UPPER, uint32_t &OPCODE_LOWER)
{
    int32_t check_if_rcall_can_be_used = (int32_t ) (k - address);
    if(abs(check_if_rcall_can_be_used ) < 512)
    {
        printf("Shorter Branch can be used: %s; Range = %d\n",Assembly_Instruction, check_if_rcall_can_be_used);
    }
}
char * assemble_double_word_case(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, uint32_t address, uint16_t &checksum, FILE *fp)
{
    //MOVFF Fs, Fd  PIC18F_DW_CASE
    //      1100 ffff ffff ffff
    //      1111 ffff ffff ffff

    //CALL n,s  PIC18F_CALL_CASE
    //      1110 000s kkkk kkkk
    //      1111 kkkk kkkk kkkk

    //GOTO n PIC18F_DW_CASE2
    //      1110 1111 kkkk kkkk
    //      1111 kkkk kkkk kkkk

    //LFSR f,k PIC18F_LIT_CASE2
    //      1110 1110 00ff kkkk
    //      1111 0000 kkkk kkkk
    char *ReturnWord=NULL;

    uint32_t k=0;
    uint32_t OPCODE_UPPER = Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos];
    uint32_t OPCODE_LOWER = 0b1111000000000000;                                 //same value used for all double word instructions
    if(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos] == PIC18F_DW_CASE)
    {
        uint16_t fs=0,fd=0;
        find_FILE_values_for_assembly(Assembly_Instruction,Instruction_Set, pos,address&0xffff,fs,fd);
        OPCODE_UPPER+=fs;
        OPCODE_LOWER+=fd;
    }
    if(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos] == PIC18F_CALL_CASE)
    {
        uint8_t s=0;
        k = find_f_value_for_assembly(Assembly_Instruction,Instruction_Set, pos,address&0xffff);
        s = find_a_value_for_assembly(Assembly_Instruction,Instruction_Set, pos, strlen(","));
        if(s==0)
            check_if_single_word_branch_will_work(k,address,Assembly_Instruction,OPCODE_UPPER,OPCODE_LOWER);
        k>>=1;
        OPCODE_UPPER+= (k&0xff) + (s<<8);
        OPCODE_LOWER+= (k>>8)&0xfff;
    }
    if(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos] ==PIC18F_DW_CASE2 )
    {
        k = find_f_value_for_assembly(Assembly_Instruction,Instruction_Set, pos,address&0xffff);
        check_if_single_word_branch_will_work(k,address,Assembly_Instruction,OPCODE_UPPER,OPCODE_LOWER);
        k>>=1;
        OPCODE_UPPER+= (k&0xff);
        OPCODE_LOWER+= (k>>8)&0xfff;
    }
    if(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos] == PIC18F_LIT_CASE2)
    {
        uint8_t f=0;
        f = find_LFSR_f_value(Assembly_Instruction);
        k = find_f_value_for_assembly(Assembly_Instruction,Instruction_Set, pos,address&0xffff);//find_a_value_for_assembly(Assembly_Instruction,Instruction_Set, pos, strlen(","));

        OPCODE_UPPER+= ((k>>8)&0x0f) + (f<<4);
        OPCODE_LOWER+= (k)&0xff;
    }
    uint32_t temp = address&0xffff;

    while(temp>=16)
    {
        temp-=16;
    }
    add_to_checksum(OPCODE_UPPER,checksum);
    if(temp+4<=16)
    {
        fprintf(fp,"%.2X%.2X%.2X%.2X",(OPCODE_UPPER&0xff),((0xff00&OPCODE_UPPER)>>8),((OPCODE_LOWER&0xff)),((0xff00&OPCODE_LOWER)>>8));  //little endian

        add_to_checksum(OPCODE_LOWER,checksum);
    }
    else    //Second word out of bounds, return it to main and add to next entry
    {
        fprintf(fp,"%.2X%.2X",((OPCODE_UPPER&0xff)),((0xff00&OPCODE_UPPER)>>8));
        ReturnWord = (char*)malloc(sizeof(char)*5);
        sprintf(ReturnWord,"%.2X%.2X",(OPCODE_LOWER&0xff),(0xff00&OPCODE_LOWER)>>8);
    }

    if(OPCODE_UPPER > Instruction_Set.PIC18F_OPCODES_UPPER_VAL[pos])
    {
        printf("ERROR: UPPER OPCODE OUT OF LIMITS: %s, %x%x\n",Assembly_Instruction,OPCODE_UPPER,OPCODE_LOWER);
    }
    return ReturnWord;
}
void assemble_single_word_case(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set, uint64_t pos, bool find_d_or_b_parameter, bool find_a_or_s_parameter, uint32_t address, uint16_t &checksum, FILE *fp)
{

    /*-------------------
     Find F (or N)
     -------------------*/
    uint16_t f=0;
    uint8_t d_or_b=0, a=0;
    f = find_f_value_for_assembly(Assembly_Instruction,Instruction_Set, pos,address&0xffff);

    /*-------------------
     (optional) Find D or B
     -------------------*/
    size_t a_offset=0;
    if(find_d_or_b_parameter == true)
    {
        a_offset=strlen(",");
        d_or_b=find_d_value_for_assembly(Assembly_Instruction,Instruction_Set, pos);
    }
    /*-------------------
     (optional) Find a
     -------------------*/
    if(find_a_or_s_parameter == true)
    {
        a = find_a_value_for_assembly(Assembly_Instruction,Instruction_Set, pos, a_offset);
    }

    uint16_t OPCODE = Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos] + (d_or_b<<9) + (a<<8) + f;

    fprintf(fp,"%.2X%.2X",(OPCODE&0xff),((0xff00&OPCODE)>>8));  //little endian

    add_to_checksum(OPCODE,checksum);

    if(OPCODE > Instruction_Set.PIC18F_OPCODES_UPPER_VAL[pos])
    {
        printf("ERROR: OPCODE OUT OF LIMITS: INSTR = {%s},OPCODE = {%x}, case = {%d}\n",Assembly_Instruction,OPCODE,Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos]);
    }
}
void Assemble_PIC18f_SingleWord(const char *Assembly_Instruction,PIC18F_FULL_IS & Instruction_Set,uint64_t pos,uint32_t address,uint16_t &checksum, FILE *fp)
{
    switch(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos])
    {
        case PIC18f_FDA_CASE:
            assemble_single_word_case(Assembly_Instruction, Instruction_Set,  pos,true, true,address&0xffff,checksum,fp);
            break;
        case PIC18f_FA_CASE:
            assemble_single_word_case(Assembly_Instruction, Instruction_Set,  pos,false, true,address&0xffff,checksum,fp);
            break;
        case PIC18F_CONTROL_CASE:
            assemble_single_word_case(Assembly_Instruction, Instruction_Set,  pos,false, false,address&0xffff,checksum,fp);
            break;
        case PIC18F_CONTROL_CASE2:
            assemble_single_word_case(Assembly_Instruction, Instruction_Set,  pos,false, false,address&0xffff,checksum,fp);
            break;
        case PIC18F_BIT_CASE:
            assemble_single_word_case(Assembly_Instruction, Instruction_Set,  pos,true, true,address&0xffff,checksum,fp);
            break;
        case PIC18F_LIT_CASE:
            assemble_single_word_case(Assembly_Instruction, Instruction_Set,  pos,false, false,address&0xffff,checksum,fp);
            break;
        case PIC18F_RETURN_CASE:
            assemble_single_word_case(Assembly_Instruction, Instruction_Set,  pos,false, false,address&0xffff,checksum,fp);
            break;
    }
}
void Write_to_debug(string debugstring)
{
    char debugdir[300]="";
    sprintf(debugdir,"%s/debug.txt",Global_working_directory);
    FILE *fd = fopen(debugdir,"a");
    fprintf(fd,"%s",debugstring.c_str());
    fclose(fd);
}
//change GOTO LABEL_0x100
//  into GOTO 0x100
char *convert_label(const char *Label_instruction, uint32_t address, uint32_t START_ADDRESS)
{
    char *LABEL = (char*)malloc(sizeof(char)*(strlen(Label_instruction)+1));

    int p1 = (int)(strstr(Label_instruction,"LABEL")-Label_instruction);

    char *temp = copy_out_substring(p1,(int)strlen(Label_instruction),Label_instruction);


    int p2=(int)(strlen(Label_instruction));
    if(strstr(Label_instruction,","))           //CALL instruction
    {
        p2 = (int)(strstr(Label_instruction,",")-Label_instruction);
    }

    char *post_label =copy_out_substring(p2,(int)strlen(Label_instruction),Label_instruction);

    char *pre_label=copy_out_substring(0,p1,Label_instruction);


    sprintf(LABEL, "%s0x%x%s",pre_label,address,post_label);

    free(temp);
    free(post_label);
    free(pre_label);
    return LABEL;
}
uint32_t translate_external_label_call_into_address(const char *Label_instruction)
{
    uint32_t address=0;

    char *ADDR = return_substring(Label_instruction, "0x", NULL, 0, strlen("0x"), 0);

    address = strtol (ADDR,NULL,16) & 0xffffffff;

    free(ADDR);

    return address;
}
char *translate_label_to_mem_address(const char *Label_instruction,PIC18F_FULL_IS &Instruction_Set, Converted_Assembly_Code &MachineCode, bool CALL_USES_EXTERNAL_MEM, uint32_t Start_Address, uint32_t Instruction_address)
{
    char *LABEL = return_substring(Label_instruction,"LABEL", NULL, 0,0, 0);    //holds LABEL_DELAY etc

    if(LABEL==NULL)
    {
        std::cout << "Could Not Parse Label on Instruction: " << Label_instruction << std::endl;
        exit(0);
    }
    strtok(LABEL,",");                  //for CALL instructions
    uint32_t address=Start_Address;
    if(CALL_USES_EXTERNAL_MEM == true)
    {
        free(LABEL);
        address=translate_external_label_call_into_address(Label_instruction);
        LABEL = convert_label(Label_instruction,address,Start_Address);
        return LABEL;
    }

    strtok(LABEL," ");

    //find the address of the actual label
    for(auto it = 0; it<MachineCode.LABEL_POSITIONS.size();it++)
    {
        if(strstr(MachineCode.LABEL_STRINGS[it].c_str(),LABEL))
        {
           // .Address
            free(LABEL);

            //we've found the address
            LABEL = convert_label(Label_instruction,(uint32_t)MachineCode.LABEL_POSITIONS[it],Start_Address);
          /*  int32_t offset = (int32_t)(Instruction_address - MachineCode.LABEL_POSITIONS[it]);

            if(offset == -2)
            {
                printf("Check Branch Instruction: %s; Range = %d; %s\n",MachineCode.ASSEMBLY_CODE_FULL_PROGRAM[Instruction_address].c_str(), offset,MachineCode.LABEL_STRINGS[it].c_str());
            }*/
            return LABEL;
        }
    }

    free(LABEL);
    return NULL;
}
char *copy_assembly_file_to_buffer(const char *inputfiledir)
{
    FILE *fp = fopen(inputfiledir,"rb");
    if(!fp)
    {
        printf("File not found: %s\n",inputfiledir);
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    uint64_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *ASM_BUFFER = (char*)calloc(sizeof(char),file_size*2+1);

    fread(ASM_BUFFER,sizeof(char) , file_size, fp);

    fclose(fp);

    return ASM_BUFFER;
}
string copy_assembly_instruction_from_text_buffer2(uint64_t &file_pos, Converted_Assembly_Code &AssemblyCode,PIC18F_FULL_IS &Instruction_Set, uint32_t START_ADDRESS, uint32_t Instruction_address)
{
    std::string Assembly_Instruction= AssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[file_pos] +" ";

    bool CALL_USES_EXTERNAL_MEM = false;
    if(strstr(Assembly_Instruction.c_str(),"//External Memory"))
    {
        CALL_USES_EXTERNAL_MEM = true;
    }

    //this is the label being called from the branch command
    if((strstr(Assembly_Instruction.c_str()," LABEL")))
    {
        char *temp = translate_label_to_mem_address(Assembly_Instruction.c_str(),Instruction_Set,AssemblyCode,CALL_USES_EXTERNAL_MEM,START_ADDRESS,Instruction_address);

        if(temp==NULL)
        {
            printf("Couldn't find label position: %s\n",Assembly_Instruction.c_str());
            exit(0);
        }

        Assembly_Instruction = temp;
        free(temp);
    }

    return Assembly_Instruction;
}
void assemble_non_program_data(uint16_t REG_VALUE, const char *Assembly_Instruction, FILE *fp, uint32_t &address,uint16_t &address_upper_16bits, uint16_t &checksum,bool &check_sum_required, uint32_t Bank_size, bool bDisplayBinContents)
{
    if(check_sum_required==true)
    while(address % 16 !=0)
    {
        increment_assembler_address(address_upper_16bits,address,fp,checksum,check_sum_required, Bank_size,bDisplayBinContents);//address+=2;
        output_Machine_Code(bDisplayBinContents,fp,"%s","FFFF");
        checksum+=0xff+0xff;
    }

    uint16_t check_sum = 0x2 +0x4 + REG_VALUE;

    if(check_sum_required==true)
    {
        output_Machine_Code(bDisplayBinContents,fp,"%.2X\r\n:0200000400%.2X%.2X",(1+(~checksum)) & 0xff,REG_VALUE,(1+(~check_sum)) & 0xff);
        checksum=0;
    }

    else
        output_Machine_Code(bDisplayBinContents,fp,"\r\n:0200000400%.2X%.2X",REG_VALUE,(1+(~check_sum)) & 0xff);

    int p1= (int)(strstr(Assembly_Instruction,":")-Assembly_Instruction)+1;
    size_t p2 = strlen(Assembly_Instruction);

    if(strstr(Assembly_Instruction," "))
    {
        p2 = (int)(strstr(Assembly_Instruction," ")-Assembly_Instruction);
    }

    char *temp =copy_out_substring(p1,(int)p2,Assembly_Instruction);
    check_sum=0;
    for(size_t counter=0; counter<strlen(temp);counter+=2)
    {
        char temp2[5]="";
        sprintf(temp2,"%c%c",temp[counter],temp[counter+1]);

        check_sum+=strtol(temp2,NULL,16);
    }
    check_sum+=(unsigned int)strlen(temp)/2;
    output_Machine_Code(bDisplayBinContents,fp,"\r\n:%.2X000000%s%.2X",(unsigned int)strlen(temp)/2,temp,(1+(~check_sum)) & 0xff);
    check_sum_required=false;

    free(temp);

    return;
}
void increment_assembler_address(uint16_t &upper_16_bits, uint32_t &lower_32_bits, FILE *fp,uint16_t &check_sum, bool &check_sum_required, uint32_t bank_size, bool bDisplayBinContents)
{
    if(lower_32_bits <= bank_size - 2)
    {
        lower_32_bits+=2;
    }
    else
    {
        upper_16_bits+=0xF0;
        lower_32_bits=0;

        uint16_t EEPROM_checksum = 0x2 +0x4;
        add_to_checksum(upper_16_bits,EEPROM_checksum);
        output_Machine_Code(bDisplayBinContents,fp,"%.2X\r\n:02000004%.4X%.2X",(1+(~check_sum)) & 0xff,upper_16_bits,(1+(~EEPROM_checksum)) & 0xff);
        check_sum_required=false;
        check_sum=0;
    }
}
void output_Machine_Code(bool display_on_screen, FILE *fp, const char *Format_Specifier, ...)
{
    va_list args;
    va_start(args, Format_Specifier);

    if(display_on_screen == true)
    {
        vprintf(Format_Specifier,args);
    }

    vfprintf(fp,Format_Specifier,args);

    va_end(args);
}
void Convert_Databyte_into_hex_data(string line,Converted_Assembly_Code &Machine_Code_File , uint32_t &File_Position_Counter)
{
    char * data_bytes = return_substring(line.c_str(), "\"", "\"", 0, strlen("\""), 0);

    if(data_bytes!=NULL)
    {
        char temp[20]="";
        uint8_t oddness_factor=1;
        if(strlen(data_bytes) % 2 == 0)
        {
            oddness_factor = 0;
        }
        for(uint8_t counter=0; counter < strlen(data_bytes)-1; counter+=2)
        {
            //if(counter  +1< strlen(data_bytes))
            {
                sprintf(temp,"UNKNOWN[%.2X%.2X]",data_bytes[counter+1],data_bytes[counter]);
                Machine_Code_File.ASSEMBLY_CODE_FULL_PROGRAM.push_back(temp);
                File_Position_Counter+=2;

            }
         /*   else
            {
            //    if(counter <strlen(data_bytes)){
                if(oddness_factor==1)
                    sprintf(temp,"UNKNOWN[00%.2X]",data_bytes[counter]);
                else
                    sprintf(temp,"UNKNOWN[00]");
            //    }
              //  else
                {
                //    sprintf(temp,"UNKNOWN[00]");
                }
                Machine_Code_File.ASSEMBLY_CODE_FULL_PROGRAM.push_back(temp);
                File_Position_Counter+=2;
            }*/
            sprintf(temp, "%.4X",File_Position_Counter);
            Machine_Code_File.Address.push_back(temp);

        }
        if(line.c_str(),"\0")
        {
            sprintf(temp,"UNKNOWN[00%.2X]",data_bytes[strlen(data_bytes)-1]);

            Machine_Code_File.ASSEMBLY_CODE_FULL_PROGRAM.push_back(temp);
            File_Position_Counter+=2;
            sprintf(temp, "%.4X",File_Position_Counter);
            Machine_Code_File.Address.push_back(temp);
        }
     /*   if(oddness_factor==1)
        {
            sprintf(temp,"UNKNOWN[00%.2X]",data_bytes[strlen(data_bytes)-1]);

            Machine_Code_File.ASSEMBLY_CODE_FULL_PROGRAM.push_back(temp);
            File_Position_Counter+=2;
            sprintf(temp, "%.4X",File_Position_Counter);
            Machine_Code_File.Address.push_back(temp);
        }
        else
        {
            sprintf(temp,"UNKNOWN[00]");

            Machine_Code_File.ASSEMBLY_CODE_FULL_PROGRAM.push_back(temp);
            File_Position_Counter+=1;
            sprintf(temp, "%.4X",File_Position_Counter);
            Machine_Code_File.Address.push_back(temp);
        }*/

        free(data_bytes);
    }
}
Converted_Assembly_Code Copy_Over_Binary_File(const char *inputfiledir,PIC18F_FULL_IS Instruction_Set)
{
    Converted_Assembly_Code Machine_Code_File;

    std::ifstream input_file;
    input_file.open(inputfiledir);
    std::string line;

    uint32_t File_Position_Counter = 0 ;
    while (getline(input_file, line,'\n'))
    {
        if((strstr(line.c_str(),"#")==NULL)&&(strstr(line.c_str(),"$")==NULL))//ignore commented out line
        {

            if(strstr(line.c_str()," db ") == NULL)
            {
                //remove any inline comments, unless they are ASCII DATA!
                std::istringstream iss(line);
                std::getline(iss, line,Assembly_Comment_Marker);
            }

            if((strstr(line.c_str(),"CONFIG_BITS"))||((strstr(line.c_str(),"DEVICE_ID"))))
            {
                Machine_Code_File.ASSEMBLY_CODE_FULL_PROGRAM.push_back(line);
                Machine_Code_File.Address.push_back("0000");
            }
            else if(strstr(line.c_str(),":"))
                 //reserved for labels
            {
                Machine_Code_File.LABEL_POSITIONS.push_back(File_Position_Counter);
                Machine_Code_File.LABEL_STRINGS.push_back(line);
            }
            else
            {
                //check that there is an instruction on the line
                uint32_t pos =0 ;
                bool found = false;
                for(auto it =  Instruction_Set.PIC18F_MNEMONICS.begin(); it != Instruction_Set.PIC18F_MNEMONICS.end(); it++)
                {
                    if((strstr(line.c_str(),Instruction_Set.PIC18F_MNEMONICS[pos].c_str()))
                       || (strstr(line.c_str(), "ORG"))     //Memory origin
                       || (strstr(line.c_str(),"UNKNOWN"))  //unknown command
                       || (strstr(line.c_str()," db ")))    //data bytes
                    {
                        if(strstr(line.c_str()," db "))
                        {
                            Convert_Databyte_into_hex_data(line,Machine_Code_File,File_Position_Counter);
                        }
                        else
                        {
                            Machine_Code_File.ASSEMBLY_CODE_FULL_PROGRAM.push_back(line);

                            //store the memory address
                            char temp[10]="";
                            sprintf(temp, "%.4X",File_Position_Counter);
                            Machine_Code_File.Address.push_back(temp);
                            File_Position_Counter+=2;
                            char *dummy;
                            //check for double word instructions
                            if(check_against_list_of_instructions({" MOVFF ", " CALL ", " GOTO ", " LFSR "}, line.c_str(),  dummy))
                            {
                                File_Position_Counter+=2;
                                free(dummy);
                            }

                            if(strstr(line.c_str(),"ORG"))
                            {
                                char *ORG_address = return_substring(line.c_str(), "ORG", NULL, 0,strlen("ORG"), 0);
                                if(ORG_address!=NULL)
                                {
                                    File_Position_Counter = strtol(ORG_address,NULL,16)&0xffff;     ///copy ORG address into working address
                                    free(ORG_address);
                                }
                            }
                        }

                        found=true;
                        break;

                    }
                    pos++;
                }
                if(found==false)
                {
                    std::cout << line<<"\n";
                }
            }
        }
    }
    return Machine_Code_File;
}
void Assemble(const char *inputfiledir, bool bDisplayBinContents,bool bDisplayAssembly,PIC18F_FULL_IS &Instruction_Set)
{
    Converted_Assembly_Code OutputAssemblyCode = Copy_Over_Binary_File(inputfiledir,Instruction_Set);

    if(OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size() == 0)
    {
        std::cout << "Error: File Not Found: " << inputfiledir << "\n";
        return;
    }

    uint32_t address=0x00;
    uint16_t address_upper_16bits=0;
    char *ReturnWord=NULL;
    uint16_t check_sum=0x00;


    char output_dir[320]="";
    sprintf(output_dir,"%s/code.hex",Global_working_directory);

    FILE *fp = fopen(output_dir,"wb");

    bool check_sum_required=true;
    uint32_t START_ADDRESS=0;

    output_Machine_Code(bDisplayBinContents,fp,"%s",":020000040000FA");

    for(uint64_t file_position =0; file_position<OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size(); file_position++)
    {

        std::string Assembly_Instruction=copy_assembly_instruction_from_text_buffer2(file_position, OutputAssemblyCode,Instruction_Set,START_ADDRESS,address);

        uint64_t pos=0;

        if(bDisplayAssembly==true)
        {
            printf("%.4x  ",address);
            std::cout <<Assembly_Instruction << "\n";
        }
        bool instruction_found = false;
        {

            if(strstr(Assembly_Instruction.c_str(),"ORG"))
            {
                uint32_t temp_address = address;

                char *ORG_address = return_substring(Assembly_Instruction.c_str(), "ORG", NULL, 0,strlen("ORG"), 0);
                if(ORG_address!=NULL)
                {
                    address = strtol(ORG_address,NULL,16)&0xffff;     ///copy ORG address into working address
                    START_ADDRESS = address;
                    free(ORG_address);
                }
                else
                {
                    std::cout << "Unable to Parse ORG \n";
                }
                if(temp_address > address)
                {
                    std::cout <<"Error on ORG - " <<Assembly_Instruction << ", whose address: " << address << " is less than the current Address: " <<temp_address << std::endl;
                    exit(0);
                }
                while(temp_address !=address)
                {
                    if(temp_address % 16 ==0)
                    {
                        output_Machine_Code(bDisplayBinContents,fp,"%.2X\r\n",(1+(~check_sum)) & 0xff);
                        output_Machine_Code(bDisplayBinContents,fp,":10%.4X00",temp_address&0xffff);
                        check_sum=0x10;
                        add_to_checksum(temp_address, check_sum); //add NOP instructions
                    }
                    output_Machine_Code(bDisplayBinContents,fp,"0000");
                    temp_address+=2;
                }
            }
            else
            {

                for(auto it =  Instruction_Set.PIC18F_MNEMONICS.begin(); it != Instruction_Set.PIC18F_MNEMONICS.end(); it++)
                {
                    char security[strlen(Instruction_Set.PIC18F_MNEMONICS[pos].c_str())+10];
                    sprintf(security," %s ",Instruction_Set.PIC18F_MNEMONICS[pos].c_str());

                    if(strstr(Assembly_Instruction.c_str(),security))
                    {
                        instruction_found=true;
                        char header[45]="";
                        if(address!=0)
                        {
                            if(check_sum_required==true)
                                sprintf(header,"%.2X\r\n:10%.4X00",(1+(~check_sum))&0xff,address&0xffff);
                            else
                              sprintf(header,"\r\n:10%.4X00",address&0xffff);

                            check_sum_required=true;
                        }

                        else    //no checksum
                            sprintf(header,"\r\n:10%.4X00",address&0xffff);

                        if(ReturnWord!=NULL)
                        {
                            output_Machine_Code(bDisplayBinContents,fp,"%s%s",header,ReturnWord);
                            check_sum=0x10;
                            add_to_checksum(address&0xffff, check_sum);

                            increment_assembler_address(address_upper_16bits,address,fp,check_sum,check_sum_required,Instruction_Set.Bank_size,bDisplayBinContents);

                            uint16_t OPCODE = strtol(ReturnWord,NULL,16);

                            add_to_checksum(OPCODE, check_sum);

                            free(ReturnWord);
                            ReturnWord=NULL;
                        }
                        else
                        {
                            if((address %16 == 0) || (address==0))
                            {
                                output_Machine_Code(bDisplayBinContents,fp,"%s",header);
                                check_sum=0x10;
                                add_to_checksum(address, check_sum);
                            }
                        }

                        switch(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[pos])
                        {
                            //double word instructions
                            case PIC18F_DW_CASE:
                            case PIC18F_DW_CASE2:
                            case PIC18F_CALL_CASE:
                            case PIC18F_LIT_CASE2:
                                ReturnWord = assemble_double_word_case(Assembly_Instruction.c_str(), Instruction_Set,  pos, address&0xffff,check_sum,fp);

                                if(ReturnWord==NULL)
                                {
                                     increment_assembler_address(address_upper_16bits,address,fp,check_sum,check_sum_required,Instruction_Set.Bank_size,bDisplayBinContents);//address+=2;
                                }
                                break;

                            //no argument instructions
                            case PIC18F_NOP_CASE:
                                output_Machine_Code(bDisplayBinContents,fp,"%s","FFFF");
                                add_to_checksum(0xffff, check_sum);
                                break;
                            case PIC18F_NO_ARGS:
                                output_Machine_Code(bDisplayBinContents,fp,"%.2X%.2X",Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos]&0xff,(Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos]>>8)&0xff);
                                add_to_checksum(Instruction_Set.PIC18F_OPCODES_LOWER_VAL[pos], check_sum);
                                break;

                            //single word statements
                            default:
                                Assemble_PIC18f_SingleWord(Assembly_Instruction.c_str(),Instruction_Set,pos,address&0xffff,check_sum,fp);
                                break;
                        }
                        increment_assembler_address(address_upper_16bits,address,fp,check_sum,check_sum_required,Instruction_Set.Bank_size,bDisplayBinContents);//address+=2;

                        break;
                    }
                    pos++;
                }
                if(instruction_found==false)//UNKNOWN command
                {
                    if(strstr(Assembly_Instruction.c_str(),"UNKNOWN"))
                    {
                        char *unknown_command = return_substring(Assembly_Instruction.c_str(),"UNKNOWN[","]",0,strlen("UNKNOWN["),0);
                        uint16_t value = strtol(unknown_command,NULL,16);

                        if(address % 16 !=0)
                        {
                            if(strlen(unknown_command)==4)
                                output_Machine_Code(bDisplayBinContents,fp,"%.2X%.2X",value&0xff,(value>>8)&0xff);
                            else{
                                output_Machine_Code(bDisplayBinContents,fp,"%.2X",value&0xff);
                               // address--;
                            }

                        }
                        else
                        {
                            if(check_sum_required==true)
                            {
                                output_Machine_Code(bDisplayBinContents,fp,"%.2X\r\n:10%.4X00",(1+(~check_sum))&0xff,address&0xffff);
                            }

                            else
                                output_Machine_Code(bDisplayBinContents,fp,"\r\n:10%.4X00",address&0xffff);

                            check_sum_required=true;
                            check_sum=0x10;
                            add_to_checksum(address, check_sum);
                            fprintf(fp,"%.2X%.2X",value&0xff,(value>>8)&0xff);
                        }
                        add_to_checksum(value, check_sum);
                        increment_assembler_address(address_upper_16bits,address,fp,check_sum,check_sum_required,Instruction_Set.Bank_size,bDisplayBinContents);

                        free(unknown_command);
                    }
                    if(strstr(Assembly_Instruction.c_str(),"DEVICE_ID"))
                    {
                        assemble_non_program_data(Instruction_Set.Device_ID_Address, Assembly_Instruction.c_str(), fp,address,address_upper_16bits, check_sum,check_sum_required,Instruction_Set.Bank_size,bDisplayBinContents);
                    }
                    if(strstr(Assembly_Instruction.c_str(),"CONFIG"))
                    {
                        assemble_non_program_data(Instruction_Set.Config_Address, Assembly_Instruction.c_str(), fp,address,address_upper_16bits, check_sum,check_sum_required,Instruction_Set.Bank_size,bDisplayBinContents);
                    }
                }
            }
        }
    }
    //End of FILE
    output_Machine_Code(bDisplayBinContents,fp,"%s","\r\n:00000001FF\r\n");
    fclose(fp);
}
void deal_with_non_program_data(std::vector<uint8_t> Data_bytes, uint8_t number_of_bytes,const char *Tag, uint32_t address, uint16_t Address_Upper_16_bits,Converted_Assembly_Code &OutputAssemblyCode )
{
    char temp[number_of_bytes+1+strlen(Tag)];

    sprintf(temp,"%s",Tag);
    for(int counter=0;counter<number_of_bytes;counter++)
    {
        char super_temp[3]="";
        sprintf(super_temp,"%c",Data_bytes[counter]);
        strcat(temp,super_temp);
        temp[counter+1+strlen(Tag)]=0;
    }
    OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(temp);

    char print_addr[10]="";
    sprintf(print_addr,"%.4X%.4X",Address_Upper_16_bits,(address&0xffff));
    OutputAssemblyCode.Address.push_back(print_addr);
}
void add_ORG_to_asm_file(uint32_t address,Converted_Assembly_Code &OutputAssemblyCode)
{
    char temp[strlen("ORG ")+1+6]="";
    sprintf(temp,"ORG 0x%.4X",address);
    OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(temp);

    sprintf(temp,"0x0000%.4X",address);
    OutputAssemblyCode.Address.push_back(temp);
}
void Disassemble(std::vector<unsigned char> &OutputFileContents, bool bDisplayBinContents,bool bDisplayAssembly,PIC18F_FULL_IS &Instruction_Set)
{
    Converted_Assembly_Code OutputAssemblyCode;
    uint64_t file_pos=0;
    uint32_t line_counter=0;
    uint16_t Address_Upper_16_bits =0x00;
    uint8_t binary_newline_counter=0;
    bool final_byte_is_double_word=false;
    bool first_byte=true;
    bool display_raw_file_data=false;
    for (int n : OutputFileContents)
    {
        if(display_raw_file_data == true)
        {
            std::cout << (char)n <<" " ;
            binary_newline_counter++;
            if(binary_newline_counter ==10 )
            {
                binary_newline_counter=0;
                std::cout << "\n" ;
            }
        }
        if(n==':')
        {
            uint64_t temp =file_pos+1;
            uint8_t number_of_bytes=       0xff &   Convert_num_to_hex(OutputFileContents,temp,2);
            uint32_t address =                      Convert_num_to_hex(OutputFileContents,temp,4);
            uint8_t type =                 0xff &   Convert_num_to_hex(OutputFileContents,temp,2);

            if(type==0x00)
            {
                if(first_byte==true)
                {
                    first_byte=false;
                    add_ORG_to_asm_file(address,OutputAssemblyCode);
                }

                line_counter+=number_of_bytes/2;
                number_of_bytes*=2;

                std::vector<uint8_t> Data_bytes(number_of_bytes);
                for(int counter=0;counter<number_of_bytes;counter++)
                {
                    Data_bytes[counter] =OutputFileContents[temp];temp++;
                }

                if(Address_Upper_16_bits==Instruction_Set.Device_ID_Address)
                {
                    deal_with_non_program_data(Data_bytes,number_of_bytes,"DEVICE_ID:",address,Address_Upper_16_bits,OutputAssemblyCode);
                }
                else if(Address_Upper_16_bits==Instruction_Set.Config_Address)
                {
                    deal_with_non_program_data(Data_bytes,number_of_bytes,"CONFIG_BITS:",address,Address_Upper_16_bits,OutputAssemblyCode);
                }
                else
                {
                    generate_asm(Data_bytes,number_of_bytes,address,OutputFileContents,temp, final_byte_is_double_word,OutputAssemblyCode,Instruction_Set,bDisplayAssembly,bDisplayBinContents,Address_Upper_16_bits);
                }
            }
            if(type==0x01)  //End of File
            {
                break;
            }
            if(type==0x04)  //Extended Linear Address
            {
                Address_Upper_16_bits = Convert_num_to_hex(OutputFileContents,temp,4);
            }
        }
        file_pos++;
    }

    for (uint64_t  pos = 0;pos<OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size();pos++)
    {
        char *Addr=copy_out_substring(0,8,OutputAssemblyCode.Address[pos].c_str());

        uint64_t Address = strtol(Addr, NULL, 16);

        free(Addr);
        Address=Address/2;

     //   char *double_word_instruction=NULL;
     //   bool last_instruction_was_a_double_word=false;

        ///because of the stupid bullshit piccola having branch instructions that point to the second word of a double word instruction for some autistic reason
       /* if(pos>0)
        if(check_against_list_of_instructions({"MOVFF", " CALL ", " GOTO ", " LFSR "}, OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos-1].c_str(),  double_word_instruction))
            {
              //  last_instruction_was_a_double_word=true;
                free(double_word_instruction);
            }*/
        for (uint64_t  n = 0;n<OutputAssemblyCode.LABEL_POSITIONS.size();n++)
        {
            if(OutputAssemblyCode.LABEL_POSITIONS[n] == Address)
            {
                string temp = OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos];

                OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.at(pos) = OutputAssemblyCode.LABEL_STRINGS[n] + "\n" + temp;

                break;
            }
          /*  else
            {
                if(last_instruction_was_a_double_word==true)
                {
                    if(OutputAssemblyCode.LABEL_POSITIONS[n] == Address-2)
                    {
                        string temp = OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos];

                        OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.at(pos) = OutputAssemblyCode.LABEL_STRINGS[n] + "\n" + temp;

                        break;
                    }
                }
            }*/
        }
    }

    char output_dir[320]="";
    sprintf(output_dir,"%s/code.txt",Global_working_directory);

    FILE *fp = fopen(output_dir,"w");
    bool C_Output=false;

    for (uint64_t  pos = 0;pos<OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size();pos++)
    {
        if(C_Output==true)  //inline assembly
        {
            if(bDisplayAssembly)
            std::cout<<"asm(\""<<OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos]<<"\");"<<std::endl;
        }
        else    //raw assembly
        {
            if(bDisplayBinContents)
            {
                std::cout<<OutputAssemblyCode.Address[pos] << " ";
            }
            if(bDisplayAssembly)
            std::cout<< OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos]<<std::endl;

            fprintf(fp,"%s\n",OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos].c_str());

            if(strstr(OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos].c_str(),"RET"))//RETFIE, RETURN
            {
                fprintf(fp,"################################################################################################\n#\n################################################################################################\n");
            }
        }
    }

    fclose(fp);
}
int16_t check_what_reg_f_is(uint16_t f, const std::vector<string> &PIC18F_REGISTERS)
{
    if((unsigned)0xfff-f<PIC18F_REGISTERS.size())
    {
        return 0xfff-f;
    }
    return -1;
}
bool deal_with_PIC18F_DoubleWord(uint16_t MachineCode_byte ,uint16_t MachineCode_byte2, std::string opcode_mnem, uint8_t CASE_TYPE, char*& command_for_prompt, uint64_t program_counter, const std::vector<std::string> &PIC18F_REGISTERS)
{
    //the second word of every double word command will begin with an "0xF000"
    //sometimes ascii data will alias a double word instruction
    if((MachineCode_byte2&0xf000) != 0xf000)
    {
        char temp[5+strlen("UNKNOWN[]")]="";
        sprintf(temp,"UNKNOWN[%.4X]",MachineCode_byte);
        strcat(command_for_prompt,temp);
        return false;
    }
    //source and destination
    uint16_t fs=0,fd=0;

    uint16_t fs_bitmask=0x0fff;
    uint16_t fd_bitmask=0x0fff;

    if((CASE_TYPE==PIC18F_DW_CASE2)||(CASE_TYPE==PIC18F_CALL_CASE))
    {
        fs_bitmask=0x00ff;///lower 8 bits
    }
    if(CASE_TYPE==PIC18F_LIT_CASE2)
    {
        fs_bitmask=0x000f;///lower 4 bits
        fd_bitmask=0x00ff;///lower 8 bits
    }
    fs = MachineCode_byte & fs_bitmask;///lower 12 bits

    fd = MachineCode_byte2 & fd_bitmask;
    char temp[100]="";
    char Comment[100]="";
    if(CASE_TYPE==PIC18F_DW_CASE)
    {
        char fs_stand_in[20]="";
        char fd_stand_in[20]="";

        sprintf(fs_stand_in,"0x%x",fs);
        sprintf(fd_stand_in,"0x%x",fd);

        if(check_what_reg_f_is(fs,PIC18F_REGISTERS)!=-1)
        {
            if(strcmp(PIC18F_REGISTERS[0xfff-fs].c_str(),"U")!=0)
            {
                sprintf(fs_stand_in,"%s",PIC18F_REGISTERS[0xfff-fs].c_str());
            }
            else
            {
                sprintf(fs_stand_in,"0x%X",fs);
                printf("Unimplemented Register Used 0x%X, Instruction = %s\n",fs,opcode_mnem.c_str());
                sprintf(Comment,"\t\t%cUnimplemented Register",Assembly_Comment_Marker);
            }
        }
        if(check_what_reg_f_is(fd,PIC18F_REGISTERS)!=-1)
        {
            if(strcmp(PIC18F_REGISTERS[0xfff-fd].c_str(),"U")!=0)
            {
                sprintf(fd_stand_in,"%s",PIC18F_REGISTERS[0xfff-fd].c_str());
            }
            else
            {
                sprintf(fd_stand_in,"0x%X",fd);
                printf("Unimplemented Register Used 0x%X, Instruction = %s\n",fd,opcode_mnem.c_str());
                sprintf(Comment,"\t\t%cUnimplemented Register",Assembly_Comment_Marker);
            }
        }

        sprintf(temp,"%s %s, %s",opcode_mnem.c_str(),fs_stand_in,fd_stand_in);
    }
    if(CASE_TYPE==PIC18F_DW_CASE2)//applies to goto command
    {
        uint32_t n = (fd<<8) + fs;
        sprintf(temp,"%s 0x%.4X",opcode_mnem.c_str(),n<<1);
    }
    if(CASE_TYPE==PIC18F_CALL_CASE)
    {
        uint32_t n = (fd<<8) + fs;
        uint8_t s = (MachineCode_byte & 0b0000000100000000)>>8;
        sprintf(temp,"%s 0x%.4X, %d",opcode_mnem.c_str(),n<<1,s);
    }
    if(CASE_TYPE==PIC18F_LIT_CASE2)
    {
        uint32_t n = fd + (fs<<8);
        uint8_t s = (MachineCode_byte & 0b110000)>>4;
        sprintf(temp,"%s %u, 0x%x", opcode_mnem.c_str(),s,n);
    }
    strcat(command_for_prompt,temp);

    return true;
}
void find_opcode_parameters(uint16_t MachineCode_byte,uint8_t &prm1, uint8_t &prm2, uint16_t &prm3, uint8_t prm1_operand,uint8_t prm2_operand,uint8_t prm3_operand, uint8_t prm_bitshift, char &Access_or_bank_RAM)
{
    uint8_t upper_byte = (MachineCode_byte&0xff00)>>8;

    prm2 = upper_byte &prm2_operand;                //access or bank

    if(prm2==0)
    {
        Access_or_bank_RAM = 'A';
    }
    else
    {
        Access_or_bank_RAM = 'B';
    }
    prm1 =(upper_byte &prm1_operand)>>prm_bitshift;

    prm3 = MachineCode_byte&prm3_operand;
}
uint32_t check_n_for_assembly(int32_t f, uint32_t program_counter)
{
    int32_t n=f-program_counter-2;

    return (n>>1);
}
uint16_t check_n(int16_t n, uint16_t bitmask,uint16_t negative_sign_mask, uint32_t program_counter)
{
    if(n&negative_sign_mask) //negative sign
    {
        n=((~n)&bitmask)+1;
        n=program_counter-n*2;
    }
    else
    {
        n=program_counter+n*2;
    }
    return (n);//&0xffff);
}
void deal_with_PIC18f_SingleWord(uint16_t MachineCode_byte, PIC18F_FULL_IS &Instruction_Set,uint8_t IS_Pos,char*&command_for_prompt, uint32_t program_counter)
{
    uint8_t CASE_TYPE = Instruction_Set.PIC18F_OPCODES_DECODE_CASE[IS_Pos];
    string opcode_mnem = Instruction_Set.PIC18F_MNEMONICS[IS_Pos];

    //final two bits of byte1 are d and a, byte2 is f
    uint8_t d=0,a=0;
    uint16_t f=0;
    char Access_or_bank_RAM='A';

    find_opcode_parameters(MachineCode_byte,d, a, f, 0b10,0b1,0xff, 1,Access_or_bank_RAM);

    char f_stand_in[20]="";
    char Comment[100]="";
    sprintf(f_stand_in,"0x%x",f);
    if(a==0)    //check for Special Function Registers
    {
        if((CASE_TYPE==PIC18f_FDA_CASE)||(CASE_TYPE==PIC18f_FA_CASE)||(CASE_TYPE==PIC18F_BIT_CASE))
            if(f>0x5f)
            {
                f+=0xf00;
                f= f & 0xffff;
                sprintf(f_stand_in,"0x%x",f);
                if(check_what_reg_f_is(f,Instruction_Set.PIC18F_REGS)!=-1)
                {
                    if(strcmp(Instruction_Set.PIC18F_REGS[0xfff-f].c_str(),"U")!=0)
                    {
                        sprintf(f_stand_in,"%s",Instruction_Set.PIC18F_REGS[0xfff-f].c_str());
                    }
                    else
                    {
                        sprintf(f_stand_in,"0x%X",f - 0xf00);
                        printf("Unimplemented Register Used 0x%X, Instruction = %s;\t\t%s\n",f,opcode_mnem.c_str(),Instruction_Set.PIC18F_DESCRIPTIONS[IS_Pos].c_str());
                        sprintf(Comment,"\t\t%cUnimplemented Register",Assembly_Comment_Marker);
                    }
                }
            }
    }
    int32_t n =0;
    uint8_t b=0, s=0;
    char temp[100]="";
    switch(CASE_TYPE)
    {
        case PIC18f_FDA_CASE:
            sprintf(temp,"%s %s, %u, %u", opcode_mnem.c_str(),f_stand_in,d,a);//for whatever reason we have to use 0 or 1 for "a" on FDA commands, but "A" or "B" for FA commands
            break;
        case PIC18f_FA_CASE:
            sprintf(temp,"%s %s, %c", opcode_mnem.c_str(),f_stand_in,Access_or_bank_RAM);
            break;
        case PIC18F_CONTROL_CASE:
            n = check_n((MachineCode_byte)&0xff,0xff,0x80, program_counter);
            sprintf(temp,"%s 0x%.4X", opcode_mnem.c_str(),n);
            break;
        case PIC18F_CONTROL_CASE2:
            n = check_n((MachineCode_byte)&0x7ff,0x7ff,0x400, program_counter);
            sprintf(temp,"%s 0x%.4X", opcode_mnem.c_str(),n);
            break;
        case PIC18F_BIT_CASE:
            find_opcode_parameters(MachineCode_byte,b, a, f, 0b1110,0b1,0xff, 1,Access_or_bank_RAM);
            sprintf(temp,"%s %s, %u, %c", opcode_mnem.c_str(),f_stand_in,b,Access_or_bank_RAM);
            break;
        case PIC18F_LIT_CASE:
            sprintf(temp,"%s %s", opcode_mnem.c_str(),f_stand_in);
            break;
        case PIC18F_RETURN_CASE:
            s = MachineCode_byte & 0x1;
            sprintf(temp,"%s %u", opcode_mnem.c_str(),s);
            break;
        default :
            sprintf(temp,"UNKNOWN[%d]",MachineCode_byte);
    }

    strcat(command_for_prompt,temp);
    strcat(command_for_prompt,Comment);
}
bool check_against_list_of_instructions(std::vector<std::string> Instruction_list, const char *Input_string,  char *&OutputString)
{
    for(uint8_t counter=0;counter<Instruction_list.size();counter++)
    {
        if(strstr(Input_string,Instruction_list[counter].c_str()))
        {
            OutputString=(char*)malloc(sizeof(char)*(1+strlen(Instruction_list[counter].c_str())));
            sprintf(OutputString,"%s",Instruction_list[counter].c_str());
            return true;
        }
    }
    return false;
}
void finalise_command(char *&command_for_prompt,Converted_Assembly_Code &OutputAssemblyCode, const char *Description, uint16_t Address_Upper_16_bits,uint32_t device_mem_size)
{
    bool include_comments=false;
    uint8_t end_of_meta_data=16;

    char *Branching_Command;
    if(check_against_list_of_instructions({"GOTO "," BC ","BN ","BNN ","BNC ","BNOV ","BNZ ","BOV ","BRA ","BZ ","CALL "}, command_for_prompt,  Branching_Command))
    {
        //find the position to move to
        long unsigned int p1 = (int)(strstr(command_for_prompt,Branching_Command)-command_for_prompt)+strlen(Branching_Command);
        long unsigned int p2 = strlen(command_for_prompt);
        long unsigned int x = (int)(strstr(command_for_prompt,"x")-command_for_prompt)+1;//mem address

        if(strstr(&command_for_prompt[p1],","))
        {
            p2 = (int)(strstr(command_for_prompt,",")-command_for_prompt);
        }

        char *pre_label = copy_out_substring((int)end_of_meta_data,(int)p1,command_for_prompt);

        char *label_address = copy_out_substring((int)p1,(int)p2,command_for_prompt);

        char *comment = copy_out_substring((int)p2,(int)strlen(command_for_prompt),command_for_prompt);

        char *Addr= copy_out_substring((int)x,(int)p2,command_for_prompt);

        OutputAssemblyCode.LABEL_POSITIONS.push_back(strtol(Addr, NULL, 16)/2);

        char label[strlen(label_address)+strlen("LABEL_:")+strlen(command_for_prompt)+1+100];
        sprintf(label,"%sLABEL_%s %s",pre_label,label_address,comment);

        //the label may be refering to external memory
        if((unsigned)strtol(label_address,NULL,16)>device_mem_size)
        {
            sprintf(label,"%sLABEL_%s %s//External Memory",pre_label,label_address,comment);
            std::cout <<"Address (" << label_address  << ") Exceeds Internal Storage (" << device_mem_size << ")\n";
        }

        if(include_comments==true)
        {
            size_t temp_len =strlen(label);
            while(strlen(label)<40)
            {
                strcat(label," ");
                temp_len++;
                label[temp_len]=0;
            }
            char temp_comment_marker[5];
            sprintf(temp_comment_marker,"%c",Assembly_Comment_Marker);
            strcat(label,temp_comment_marker);
            label[temp_len + strlen(temp_comment_marker)]=0;
            strcat(label,Description);
            label[temp_len + strlen(temp_comment_marker)+strlen(Description)]=0;
        }
        OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(label);

        strtok(label_address," ");
        sprintf(label,"LABEL_0x%.4X:",(unsigned)strtol(label_address,NULL,16));
        OutputAssemblyCode.LABEL_STRINGS.push_back(label);

        free(Branching_Command);
        free(label_address);
        free(comment);
        free(Addr);
        free(pre_label);
    }
    else
    {
        if(strstr(command_for_prompt,"TBLPTR"))
        {
           // printf("\t\t\t\t%s\n%s\n",OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size()-1].c_str(),command_for_prompt);
        }
        if(include_comments==true)
        {
            while(strlen(command_for_prompt)<52)
                strcat(command_for_prompt," ");

            char temp_comment_marker[5];
            sprintf(temp_comment_marker,"%c",Assembly_Comment_Marker);
            strcat(command_for_prompt,temp_comment_marker);
            strcat(command_for_prompt,Description);
        }

        char *temp =copy_out_substring((int)end_of_meta_data,(int)strlen(command_for_prompt),command_for_prompt);

        OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(temp);

        free(temp);
    }

}
void generate_asm(std::vector<uint8_t> &MachineCode,
                  uint8_t number_of_bytes,
                  uint32_t &address,
                  std::vector<unsigned char> &HEX_FILE_CONTENTS,
                  uint64_t file_position,
                  bool &final_byte_is_double_word,
                  Converted_Assembly_Code &OutputAssemblyCode,
                  PIC18F_FULL_IS &Instruction_Set,
                  bool bdisplayASM,bool bDisplayBinContents,
                  uint16_t Address_Upper_16_bits)
{

    char temp[5]="";
    bool Instruction_found = false;
    char *command_for_prompt=(char*)calloc(sizeof(char),1024);
    int start_point =0 ;
    if(final_byte_is_double_word==true)
    {
        start_point=4;
        final_byte_is_double_word=false;
        address+=2;
    }
    for(uint8_t counter=start_point;counter<MachineCode.size();counter+=4)
    {
        sprintf(temp,"%c%c%c%c",MachineCode[counter+2],MachineCode[counter+3],MachineCode[counter+0],MachineCode[counter+1]);
        uint16_t n = (int)strtol(temp, NULL, 16);

        Instruction_found = false;

        sprintf(command_for_prompt,"%.4x%.4x    %s    ",Address_Upper_16_bits,0xffff & address,temp);


        address+=2;
        uint16_t array_pos=0;
       /* if((address >= Instruction_Set.LUP_ADDR_START)&&(address <= Instruction_Set.LUP_ADDR_END))
        {
           // char *ascii = hex_to_ascii(temp);

            OutputAssemblyCode.Address.push_back(command_for_prompt);

            sprintf(command_for_prompt,"    UNKNOWN[%s]",temp);
            OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(command_for_prompt);
            Instruction_found = true;
           // free(ascii);
        }
        else*/
        for(auto it =  Instruction_Set.PIC18F_OPCODES_UPPER_VAL.begin(); it != Instruction_Set.PIC18F_OPCODES_UPPER_VAL.end(); it++)
        {
            if((n>=Instruction_Set.PIC18F_OPCODES_LOWER_VAL[array_pos]) && (n<=Instruction_Set.PIC18F_OPCODES_UPPER_VAL[array_pos]))
            {
                OutputAssemblyCode.Address.push_back(command_for_prompt);

                char temp2[5]="";
                switch(Instruction_Set.PIC18F_OPCODES_DECODE_CASE[array_pos])
                {
                    //double word instructions
                    case PIC18F_DW_CASE:
                    case PIC18F_DW_CASE2:
                    case PIC18F_CALL_CASE:
                    case PIC18F_LIT_CASE2:

                        if(counter+(unsigned)7<MachineCode.size())//sometimes double word instructions are spread accross two file records
                        {
                            sprintf(temp2,"%c%c%c%c",MachineCode[counter+6],MachineCode[counter+7],MachineCode[counter+4],MachineCode[counter+5]);
                        }
                        else //data is inside the next record
                        {
                            //find the next entry
                            while(HEX_FILE_CONTENTS[file_position]!=':')
                            {
                                file_position++;
                            }
                            //and copy over the bytes
                            sprintf(temp2,"%c%c%c%c",HEX_FILE_CONTENTS[file_position+11],HEX_FILE_CONTENTS[file_position+12],HEX_FILE_CONTENTS[file_position+9],HEX_FILE_CONTENTS[file_position+10]);
                            final_byte_is_double_word=true;
                        }
                        if(deal_with_PIC18F_DoubleWord(n,(int)strtol(temp2, NULL, 16),Instruction_Set.PIC18F_MNEMONICS[array_pos],Instruction_Set.PIC18F_OPCODES_DECODE_CASE[array_pos], command_for_prompt,address&0xffff,Instruction_Set.PIC18F_REGS)==true)
                        {
                            counter+=4;//double word
                            address+=2;
                        }

                        break;

                    //no argument instructions
                    case PIC18F_NOP_CASE:

                        strcat(command_for_prompt,"UNKNOWN[");
                        strcat(command_for_prompt,temp);
                        strcat(command_for_prompt,"]");

                        break;
                    case PIC18F_NO_ARGS:
                        strcat(command_for_prompt,Instruction_Set.PIC18F_MNEMONICS[array_pos].c_str());
                        break;

                    //single word statements
                    default:
                        deal_with_PIC18f_SingleWord(n,Instruction_Set,array_pos, command_for_prompt,address&0xffff);
                        break;
                }

                finalise_command(command_for_prompt,OutputAssemblyCode,Instruction_Set.PIC18F_DESCRIPTIONS[array_pos].c_str(),Address_Upper_16_bits,Instruction_Set.Bank_size);
                Instruction_found = true;
                break;
            }

            array_pos++;
        }
        if(Instruction_found==false)
        {
            OutputAssemblyCode.Address.push_back(command_for_prompt);
            sprintf(command_for_prompt,"    UNKNOWN[%s]",temp);
            OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.push_back(command_for_prompt);
        }
    }
    free(command_for_prompt);
    return;
}

void print_as_bits(uint8_t OutputBytes)
{
    printf(" " BYTE_TO_BINARY_PATTERN,BYTE_TO_BINARY(  OutputBytes));
    printf("\n");
}
void Copy_Vector_to_JPEG_FILE(std::vector<string> buffer, const char *Output_file_name)
{
  /*  std::ofstream output( Output_file_name, std::ios::binary );

    std::copy
    (
        buffer.begin(),
        buffer.end(),
        ostream_iterator<char>(output)
    );*/
}
uint32_t Convert_num_to_hex( std::vector<unsigned char> input,uint64_t &vec_start, uint8_t return_len)
{
    uint32_t return_value=0;
    for(int counter=0;counter<return_len;counter++)
    {
        char tempstring[100]="";
        sprintf(tempstring,"%c",input[vec_start+counter]);

        return_value=return_value*0x10;
        return_value+=strtol(tempstring,NULL,16);
    }
    vec_start+=return_len;

    return return_value;

}
void Copy_FIRMWARE_FILE_to_Buffer(const char *Input_File_dir,std::vector<unsigned char> &OutputFileContents)
{
    std::ifstream input( Input_File_dir, std::ios::binary );

    if(!input)
    {
        std::cout << "Could not find file\n";
    }
    std::copy
    (
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>( ),
        std::back_inserter(OutputFileContents)
    );

}
