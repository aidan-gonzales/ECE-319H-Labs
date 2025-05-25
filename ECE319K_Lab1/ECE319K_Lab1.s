//****************** ECE319K_Lab1.s ***************
// Your solution to Lab 1 in assembly code
// Author: Aidan Gonzales
// Last Modified: 1/30/25
// Spring 2025
        .data
        .align 2
// Declare global variables here if needed
// with the .space assembly directive

        .text
        .thumb
        .align 2
        .global EID
EID:    .string "AGG3498"

        .global Phase
        .align 2
Phase:  .long 10
// Phase= 0 will display your objective and some of the test cases, 
// Phase= 1 to 5 will run one test case (the ones you have been given)
// Phase= 6 to 7 will run one test case (the inputs you have not been given)
// Phase=10 will run the grader (all cases 1 to 7)
        .global Lab1
// Input: R0 points to the list
// Return: R0 as specified in Lab 1 assignment and terminal window
// According to AAPCS, you must save/restore R4-R7
// If your function calls another function, you must save/restore LR
Lab1: PUSH {R4-R7,LR}

CHECK:
        LDR R1, [R0] // loads the EID string pointer
        CMP R1, #0 // Compares EID pointer to 0
        BEQ FAILED // If 0, then no match found

        LDR R3, =MYEID // loads R3 with the pointer to my EID

COMPARE:
        LDR R4, [R3] // loads my EID
        LDR R2, [R1] // loads this EID


        CMP R2, R4
        BEQ MATCH // if R2 = R4, then match found

        B INNERFAIL // else, fail and go to next EID


INNERFAIL:
        ADDS R0, R0, #8 // increments to the next EID
        B CHECK

FAILED:
        MOVS R0, #1 // loads R0 with 1
        RSBS R0, R0, #0 // Negates R0
        B RETURN

MATCH:
        ADDS R1, R0, #4 // increments to the associated score
        LDR R0, [R1] // loads R0 with the associated score
        B RETURN
       
RETURN:
      POP  {R4-R7,PC} // return

        .align 2
        .global MYEID
MYEID:  .string "AGG3498"

        .align 2
        .global myClass
myClass: .long pAB123  // pointer to EID
         .long 95      // Score
         .long pXYZ1   // pointer to EID
         .long 96      // Score
         .long pAB5549 // pointer to EID
         .long 94      // Score
         .long 0       // null pointer means end of list
         .long 0
pAB123:  .string "AB123"
pXYZ1:   .string "XYZ1"
pAB5549: .string "AB5549"
        .end
