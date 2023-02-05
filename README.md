# disPicable-Mpdasmasm
A disassembler / Assembler for Pic18 microcontrollers

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

  - Frequenctly table read address are passed as arguments to functions, which can make tracking them difficult. 
  It's necessary to do this, so that we can differentiate between data and code

## Where the project currently is

### Disassembler
Symbolic labels work for Branch/Call instruction, but not table reads. although the data being read by the table read is indicated in line with the instruction

### Assembler
The Assembler can successfully re-assemble the disassembled hex file. It can also process brand new .asm files, but is not yet fully comptaible with MPLAB's assembler. 
  

