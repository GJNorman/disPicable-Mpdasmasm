# disPicable-Mpdasmasm
A disassembler / Assembler for Pic18 microcontrollers

Goals of the project

-Disassemble a PIC hex file into a functional and readable .asm file

-Reassemble the same file with or without modifications into a usable hex file

-I don't currently intend full compatibility with Microchips offical Assembler (atleast for now). Just the minimum required to achieve the above

-To do this we need to achieve the following

  -Add symbolic labels for branching/Call instructions. Otherwise any modification will break the program
  
  -Add symbolic labels for TBLRD instructions. for the same reason as above
  
some challenges

-Frequenctly table read address are passed as arguments to functions, which can make tracking them difficult. 
It's necessary to do this, so that we can differentiate between data and code


Where the projet currently is

Symbolic labels work for Branch/Call instruction, but not table reads. although the data being read by the table read is indicated in line with the instruction
  
  
  
