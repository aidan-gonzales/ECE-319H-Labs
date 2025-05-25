//****************** ECE319K_Lab2H.s ***************
// Your solution to Lab 2 in assembly code
// Author: Aidan Gonzales
// Last Modified: 2/4/25
// ECE319H Spring 2025 (ECE319K students do Lab2)

    .include "../inc/msp.s"

        .data
        .align 2
// Declare global variables here if needed
// with the .space assembly directive

        .text
        .thumb
        .align 2
        .global EID
EID:    .string "AGG3498" // replace ZZZ123 with your EID here
        .align 2
  .equ dot,100
  .equ dash,(3*dot)
  .equ shortgap,(2*dot)  // because it will send an interelement too
  .equ interelement,dot
  Morse:
  .long  dot,  dash,    0,    0, 0 // A
  .long  dash,  dot,  dot,  dot, 0 // B
  .long  dash,  dot, dash,  dot, 0 // C
  .long  dash,  dot,  dot,    0, 0 // D
  .long  dot,     0,    0,    0, 0 // E
  .long  dot,   dot, dash,  dot, 0 // F
  .long  dash, dash,  dot,    0, 0 // G
  .long  dot,   dot,  dot,  dot, 0 // H
  .long  dot,   dot,    0,    0, 0 // I
  .long  dot,  dash, dash, dash, 0 // J
  .long  dash,  dot, dash,    0, 0 // K
  .long  dot,  dash,  dot,  dot, 0 // L
  .long  dash, dash,    0,    0, 0 // M
  .long  dash,  dot,    0,    0, 0 // N
  .long  dash, dash, dash,    0, 0 // O
  .long  dot,  dash, dash,  dot, 0 // P
  .long  dash, dash,  dot, dash, 0 // Q
  .long  dot,  dash,  dot,    0, 0 // R
  .long  dot,   dot,  dot,    0, 0 // S
  .long  dash,    0,    0,    0, 0 // T
  .long  dot,   dot, dash,    0, 0 // U
  .long  dot,   dot,  dot, dash, 0 // V
  .long  dot,  dash, dash,    0, 0 // W
  .long  dash,  dot,  dot, dash, 0 // X
  .long  dash,  dot, dash, dash, 0 // Y
  .long  dash, dash,  dot,  dot, 0 // Z

  .align 2
  .global Lab2Grader
  .global Lab2
  .global Debug_Init // Lab3 programs
  .global Dump       // Lab3 programs

// Switch input: PB2 PB1 or PB0, depending on EID
// LED output:   PB18 PB17 or PB16, depending on EID
// logic analyzer pins PB18 PB17 PB16 PB2 PB1 PB0
// analog scope pin PB20
Lab2:
// Initially the main program will
//   set bus clock at 80 MHz,
//   reset and power enable both Port A and Port B
// Lab2Grader will
//   configure interrupts  on TIMERG0 for grader or TIMERG7 for TExaS
//   initialize ADC0 PB20 for scope,
//   initialize UART0 for grader or TExaS
    MOVS R0,#1  // 0 for info, 1 debug with logic analyzer, 2 debug with scope, 10 for grade
    BL   Lab2Grader
    BL   Lab2Init   // you initialize input pin and output pin
    BL Debug_Init // Lab 3 debug

loop:

    BL Dot
    BL Dot
    BL Dot
    BL Dot

    // BL Dash
    // BL Dash
    // BL Dash

    B loop
/*
waitForRelease:
    LDR R3, =GPIOB_DIN31_0 // Loads the address of the current values of the Port B input pins
    MOVS R1, #8 // Loads the mask for bit 3
    LDR R2, [R3] // Gets the current values of the input
    ANDS R2, R2, R1 // Isolates bit 3
    CMP R2, R1
    BEQ waitForRelease // If bit 3 is set, continue to wait for release

waitForPress:
    LDR R2, [R3] // Gets the current values of the input
    ANDS R2, R2, R1 // Isolates bit 3
    CMP R2, R1
    BNE waitForPress // If bit 3 is 0, continue to wait for press


    MOVS R0, #10
    BL Lab2Grader // Returns R0 = random character from 'A' to 'Z'

    SUBS R0, R0, #0x41 // Gets the index of the character in the morse code list
    MOVS R1, #20 // Five elements per letter, four bytes per element
    MULS R0, R0, R1

    LDR R1, =Morse
    ADDS R1, R1, R0 // Adds the offset to the base memory value of Morse

checkNull:
    LDR R2, [R1] // Loads the Morse Code for this Character
    MOVS R0, #0 // Loads the value of NULL
    CMP R2, R0
    BNE checkDot // If the morse value is not zero, check dot
    B loop // If zero, then wait for button release

checkDot:
    MOVS R0, #100 // Loads the value of dot
    CMP R2, R0
    BNE checkDash // If the morse value is not equal to dot, go to Dash

    BL Dot // Branches to dot subroutine
    ADDS R1, R1, #4 // increments morse code pointer
    B checkNull // Check next character

checkDash:
    BL Dash // Branches to dash subroutine
    ADDS R1, R1, #4 // increments morse code pointer
    B checkNull // Check next character
    
*/



Delay:  SUBS R0, R0, #2
dloop:  SUBS R0, R0, #4
        NOP
        BHS dloop
        BX LR


Dot:
        PUSH {R0, LR} // Saves R0 and LR because they are used in the subroutine
        BL LED_ON
        LDR R0, =8000000 // 100 ms dot timing
        BL Delay
        BL LED_OFF
        LDR R0, =8000000 // 100 ms inter-element timing
        BL Delay
        POP {R0, PC} // Pops R0 and LR

Dash:
        PUSH {R0, LR} // Saves R0 and LR on the stack because they are used in the subroutine
        BL LED_ON
        LDR R0, =24000000 // 300 ms dot timing
        BL Delay
        BL LED_OFF
        LDR R0, =8000000 // 100 ms inter-element timing
        BL Delay
        POP {R0, PC} // Pops R0 and LR


LED_ON:
        PUSH {R1-R3, LR} // Saves R1, R2, R3 , and LR on the stack because they are used in the subroutine
        LDR R3, =GPIOB_DOUT31_0 // Reads the address of the current values of the Port B output pins
        LDR R1, =0x40000 // Loads the mask for bit 18
        ORRS R2, R1, R2 // Friendly set for bit 18
        STR R2, [R3] // Stores the updated value back into the memory address
        BL Dump
        POP {R1-R3, PC} // Pops R1, R2, R3, and PC


LED_OFF:
        PUSH {R1-R3, LR}
        LDR R3, =GPIOB_DOUT31_0 // Reads the address of the current values of the Port B output pins
        LDR R1, =0x40000 // Loads the mask for bit 18
        BICS R2, R1 // Friendly clear for bit 18
        STR R2, [R3] // Stores the updated value back into the memory address
        BL Dump
        POP {R1-R3, PC} // Pops R1, R2, R3, and PC





// make switch an input, LED an output
// PortB is already reset and powered
// Set IOMUX for your input and output
// Set GPIOB_DOE31_0 for your output (be friendly)
Lab2Init:
// ***do not reset/power Port A or Port B, already done****
/*
   LDR R0, =IOMUXPB3 // PINCM for PB3 switch input  PINCM = Pin Control Management
   LDR R1, =0x40081 // Bit 18 for input enable, bit 7 to allow software to input with the pin, and bits 5:0 00001 for GPIO mode
   STR R1, [R0] // Stores the value into PINCM for PB3
*/

   LDR R0, =IOMUXPB18 // PINCM for PB18 LED output  PINCM = Pin Control Management
   MOVS R1, #0x81 // Bit 7 to allow software to output with the pin, and 5:0 00001 for GPIO mode
   STR R1, [R0] // Stores the value into PINCM for PB18

   LDR R0, =GPIOB_DOE31_0 // Loads the address of the Data Output Enable Register
   LDR R1, [R0] // Loads the Data Output Enable Register
   LDR R2, =0x40000 // Loads the mask for bit 18
   ORRS R1, R1, R2 // Friendly set for bit 18
   STR R1, [R0] // Stores the updated value into the address

   BX   LR

   .end
