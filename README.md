# disPicable-Mpdasmasm
A disassembler / Assembler for Pic18 microcontrollers

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





