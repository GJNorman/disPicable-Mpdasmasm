# disPicable-Mpdasmasm
An Assembler / disassembler for Pic18 microcontrollers

The name is light-hearted; 'disPicable' meaning its a disassembler for PIC MCUs, 'Mpdasmasm' meaning Microchip Disassembler-Assembler

The main issue with the MPLAB diassembler, and other projects, is that they do not produce usable code. This is due to a number of reasons
  - Hard coded addresses for branching instructions
  
  - Aliasing of data bytes with instructions which produces non-sense code
  
  - Double word instructions produce redundant NOP instructions for the second word

## Goals of the project

  - Disassemble a PIC hex file into a functional and readable .asm file

  - Reassemble the same file with or without modifications into a usable hex file

### This can be achieved through the following

  - Add symbolic labels for branching/Call instructions. Otherwise any modification will break the program
  
  - Add symbolic labels for TBLRD instructions. for the same reason as above
  
### Some challenges

  - Frequently table read address are passed as arguments to functions, which can make tracking them difficult. 
  It's necessary to do this, so that we can differentiate between data and code
  
  - Another issue, is that sometimes a program will read the entire program memory (for example, after an update) for verification, which adds another layer of ambiguity

  - I'm trying to limit the amount of hard-coding and external resources needed. currently the program can (optionally) read in a ".inc" file for whatever the target processor is. This file mostly contains information on the chip's SFRs (special function registers), it doesn't really give any information on the chip, only the number of RAM banks and whether EEPROM is present
## Where the project currently is

### Disassembler
Symbolic labels work for Branch/Call instruction, but not table reads. although the data being read by the table read is indicated in line with the instruction

Apart from decoding instructions, the program will also 

#### Highlight input arguments used for function calls

<img width="585" alt="Screenshot 2023-02-10 at 8 32 13 pm" src="https://user-images.githubusercontent.com/113757511/218055949-4b190753-12c8-4ba4-8ae6-ccacaedd9a4c.png">


The displayed values are in hex and ascii

This can be helpful for quickly estimating the purpose of a function

It works by monitoring the use of software stacks (i.e.  POSTINCx SFRs). If the code uses other register for passing arguments, this is not detected

If the function being called uses any SFRs, this will also be indicated

<img width="726" alt="Screenshot 2023-02-11 at 12 19 07 pm" src="https://user-images.githubusercontent.com/113757511/218230426-626b892e-61a2-46e7-ba9d-4b7a01a72c51.png">

you can see here that the function must be transmitting '\n' over UART (TXREG)

#### Highlight the value's being read by TBLRD instructions

<img width="548" alt="Screenshot 2023-02-11 at 12 36 26 pm" src="https://user-images.githubusercontent.com/113757511/218231794-0c27df40-8520-432c-ae71-efbcd1feea40.png">

you can see that these nonsense commands are actually ascii data '3.' 

### Assembler
The Assembler can successfully re-assemble the disassembled hex file. It can also process brand new .asm files, but is not yet fully comptaible with MPLAB's assembler. 
  
### basic example

Here's a short program that will just loop until it has finished reading a "DB" string

<img width="348" alt="Screenshot 2023-02-05 at 2 51 14 pm" src="https://user-images.githubusercontent.com/113757511/216800552-e907e679-192f-4759-b1fc-778bd8fd0974.png">

which generates the hex file

<img width="332" alt="Screenshot 2023-02-05 at 2 51 32 pm" src="https://user-images.githubusercontent.com/113757511/216800589-7adbcaee-16f9-4dee-8e8e-08d89d05745b.png">


which can then be re-assembled into

<img width="869" alt="Screenshot 2023-02-05 at 2 49 57 pm" src="https://user-images.githubusercontent.com/113757511/216800598-01d462e7-4cec-48be-a6a3-6f195419fb32.png">


Note - while we lost the symbolic label for the table pointer, we retained the Branching label. However the label has lost meaning and just takes the memory address that it is stored in



### supported operations

#### Instruction set

The program is compatible with all 75 PIC18 single and souble word instructions; including with optional arguments. 

It does not have support for the extended IS or triple word instructions (i.e. MOVFFL)


#### Number systems

 - Hex notation is supported, examples 0x37, h'37

 - decimal notation is supported, examples 0d37, d'37
 
 - binary notaiton is supported, examples 0b1101, b'1101

#### ORG directive

- any number of ORG directives can be placed into the Program as so 'ORG 0x10f5'

#### Lables

- lables must appear on a line by themselves and contain a colon. There are no other limitations

#### DB 

      label    DB "hello"

               DB "World\0"
               
      

if you want a null terminator, you need to include it in the string itself as "World\0"

this is an artifact of the disassembling process

so if you were to read "label" until TABLAT == 0, you would read "Hello World"

#### Preprocessor

limited support for C-Style Macros

    #define SUM(x,y)  MOVFW x, BANKSEL(x)\
 
                     ADDFW y, f, BANKSEL(y)

including files

you will need to include a header file from the relevant microcontroller

#include 'pic18xyz.inc'

#### EQU

MyVar  equ    0x100

#### Comments

The program supports inline comments using either ';' or '//'

and supports multiline comments using /*  */ 

### How to use

you can either use command line arguments to specificy files/functionality or else have the program present a prompt

#### Command line arguments

To disassemble a file called "file.hex" that uses a pic18f45k50 MCU

    disPicable pic18f45k50.inc dasm file.hex

To assemble a file called "file.asm" that uses a pic18f45k50 MCU

    disPicable pic18f45k50.inc asm file.asm

note: you will need to provide the "i.inc" file yourself (it can be aquired from MPLAB for example)

#### Prompt

The program will search the working directory for files that have extensions ".inc", ".hex" and ".asm". It will then ask you which ones you wish to use


#### Output

The program will create a file called "DisassembledCode.asm" or "AssembledCode.hex" in the working directory

