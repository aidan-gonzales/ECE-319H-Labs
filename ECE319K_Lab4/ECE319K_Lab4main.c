/* ECE319K_Lab4main.c
 * Traffic light FSM
 * ECE319H students must use pointers for next state
 * ECE319K students can use indices or pointers for next state
 * Names: Aidan Gonzales and Aditya Goyal
  */

#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "../inc/UART.h"
#include "../inc/Timer.h"
#include "../inc/Dump.h"  // student's Lab 3
#include <stdio.h>
#include <string.h>
// put both EIDs in the next two lines
const char EID1[] = "AGG3498"; //  ;replace abc123 with your EID
const char EID2[] = "AG79267"; //  ;replace abc123 with your EID
// Hint implement Traffic_Out before creating the struct, make struct match your Traffic_Out

struct State {
    uint32_t WestOut;
    uint32_t SouthOut;
    uint32_t WalkOut;
    uint32_t Time;
    uint32_t number;
    const struct State *Next[8];
};
typedef const struct State State_t;
#define goS &FSM[0]
#define waitS &FSM[1]
#define allRed1 &FSM[2]
#define goW &FSM[3]
#define waitW &FSM[4]
#define allRed2 &FSM[5]
#define SWaitW &FSM[6]
#define WWaitW &FSM[7]
#define allRed3 &FSM[8]
#define goWalk &FSM[9]
#define walkFlash1 &FSM[10]
#define walkOff1 &FSM[11]
#define walkFlash2 &FSM[12]
#define walkOff2 &FSM[13]
#define allRed4 &FSM[14]
State_t FSM[15] = {
    {4, 1, 1<<26, 500, 0, {goS, waitS, goS, waitS, SWaitW, SWaitW, SWaitW, SWaitW}}, //goS
    {4, 2, 1<<26, 200, 1, {allRed1, allRed1, allRed1, allRed1, allRed1, allRed1, allRed1, allRed1}}, //waitS
    {4, 4, 1<<26, 200, 2, {goW, goW, goW, goW, goW, goW, goW, goW}}, //allRed1
    {1, 4, 1<<26, 500, 3, {goW, goW, waitW, waitW, WWaitW, WWaitW, waitW, waitW}}, //goW
    {2, 4, 1<<26, 200, 4, {allRed2, allRed2, allRed2, allRed2, allRed2, allRed2, allRed2, allRed2}}, //waitW
    {4, 4, 1<<26, 200, 5, {goS, goS, goS, goS, goS, goS, goS, goS}}, //allRed2
    {4, 2, 1<<26, 200, 6, {allRed3, allRed3, allRed3, allRed3, allRed3, allRed3, allRed3, allRed3}}, //SWaitW
    {2, 4, 1<<26, 200, 7, {allRed3, allRed3, allRed3, allRed3, allRed3, allRed3, allRed3, allRed3}}, //WWaitW
    {4, 4, 1<<26, 200, 8, {goWalk, goWalk, goWalk, goWalk, goWalk, goWalk, goWalk, goWalk}}, //allRed3
    {4, 4, 0x31<<22, 500, 9, {walkFlash1, walkFlash1, walkFlash1, walkFlash1, goWalk, walkFlash1, walkFlash1, walkFlash1}}, //goWalk
    {4, 4, 1<<26, 50, 10, {walkOff1, walkOff1, walkOff1, walkOff1, walkOff1, walkOff1, walkOff1, walkOff1}}, //walkFlash1
    {4, 4, 0, 50, 11, {walkFlash2, walkFlash2, walkFlash2, walkFlash2, walkFlash2, walkFlash2, walkFlash2, walkFlash2}}, //walkOff1
    {4, 4, 1<<26, 50, 12, {walkOff2, walkOff2, walkOff2, walkOff2, walkOff2, walkOff2, walkOff2, walkOff2}}, //walkFlash2
    {4, 4, 0, 50, 13, {allRed4, allRed4, allRed4, allRed4, allRed4, allRed4, allRed4, allRed4}}, //walkOff2
    {4, 4, 1<<26, 200, 14, {goW, goW, goS, goW, goW, goW, goS, goW}} //allRed4
    };
    State_t *CPt;

// initialize all 6 LED outputs and 3 switch inputs
// assumes LaunchPad_Init resets and powers A and B
void Traffic_Init(void){ // assumes LaunchPad_Init resets and powers A and B
    //Input Initialization
    IOMUX->SECCFG.PINCM[PB15INDEX] = 0x40081;
    IOMUX->SECCFG.PINCM[PB16INDEX] = 0x40081;
    IOMUX->SECCFG.PINCM[PB17INDEX] = 0x40081;
    
    //Output Initialization
    IOMUX->SECCFG.PINCM[PB0INDEX] = 0x81;
    IOMUX->SECCFG.PINCM[PB1INDEX] = 0x81;
    IOMUX->SECCFG.PINCM[PB2INDEX] = 0x81;
    IOMUX->SECCFG.PINCM[PB6INDEX] = 0x81;
    IOMUX->SECCFG.PINCM[PB7INDEX] = 0x81;
    IOMUX->SECCFG.PINCM[PB8INDEX] = 0x81;
    IOMUX->SECCFG.PINCM[PB22INDEX] = 0x81;
    IOMUX->SECCFG.PINCM[PB26INDEX] = 0x81;
    IOMUX->SECCFG.PINCM[PB27INDEX] = 0x81;
    GPIOB->DOE31_0 |= 0xC4001C7;

  
}
/* Activate LEDs
* Inputs: west is 3-bit value to three east/west LEDs
*         south is 3-bit value to three north/south LEDs
*         walk is 3-bit value to 3-color positive logic LED on PB22,PB26,PB27
* Output: none
* - west =1 sets west green
* - west =2 sets west yellow
* - west =4 sets west red
* - south =1 sets south green
* - south =2 sets south yellow
* - south =4 sets south red
* - walk=0 to turn off LED
* - walk bit 22 sets blue color
* - walk bit 26 sets red color
* - walk bit 27 sets green color
* Feel free to change this. But, if you change the way it works, change the test programs too
* Be friendly*/
void Traffic_Out(uint32_t west, uint32_t south, uint32_t walk){
    GPIOB->DOUTCLR31_0 = 0xC4001C7;
    GPIOB->DOUTSET31_0 = (west<<6) | (south) | (walk);
    uint32_t temp = 0;
}
/* Read sensors
 * Input: none
 * Output: sensor values
 * - bit 0 is west car sensor
 * - bit 1 is south car sensor
 * - bit 2 is walk people sensor
* Feel free to change this. But, if you change the way it works, change the test programs too
 */
uint32_t Traffic_In(void){
    uint32_t temp = (GPIOB->DIN31_0 & 0x38000) >> 15;
    return temp;
}
// use main1 to determine Lab4 assignment
void Lab4Grader(int mode);
void Grader_Init(void);
int main1(void){ // main1
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Lab4Grader(0); // print assignment, no grading
  while(1){
  }
}
// use main2 to debug LED outputs
// at this point in ECE319K you need to be writing your own test functions
// modify this program so it tests your Traffic_Out  function
int main2(void){ // main2
  uint32_t last = 0, now;
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Grader_Init(); // execute this line before your code
  LaunchPad_LED1off();
  Traffic_Init(); // your Lab 4 initialization
  if((GPIOB->DOE31_0 & 0x20)==0){
    UART_OutString("access to GPIOB->DOE31_0 should be friendly.\n\r");
  }
  UART_Init();
  UART_OutString("Lab 4, Spring 2025, Step 1. Debug LEDs\n\r");
  UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
  UART_OutString("EID2= "); UART_OutString((char*)EID2); UART_OutString("\n\r");
  while(1){
    Traffic_Out(0, 1, 0);
    Debug_Dump(GPIOB->DOUT31_0 & 0xC4001C7);
    Traffic_Out(0, 2, 0);
    Debug_Dump(GPIOB->DOUT31_0 & 0xC4001C7);
    Traffic_Out(0, 4, 0);
    Debug_Dump(GPIOB->DOUT31_0 & 0xC4001C7);
    Traffic_Out(1, 0, 0);
    Debug_Dump(GPIOB->DOUT31_0 & 0xC4001C7);
    Traffic_Out(2, 0, 0);
    Debug_Dump(GPIOB->DOUT31_0 & 0xC4001C7);
    Traffic_Out(4, 0, 0);
    Debug_Dump(GPIOB->DOUT31_0 & 0xC4001C7);
    Traffic_Out(0, 0, 1<<26);
    Debug_Dump(GPIOB->DOUT31_0 & 0xC4001C7);
    Traffic_Out(0, 0, 0x31<<22);
    Debug_Dump(GPIOB->DOUT31_0 & 0xC4001C7);
    if((GPIOB->DOUT31_0&0x20) == 0){
      UART_OutString("DOUT not friendly\n\r");
    }
  }
}
// use main3 to debug the three input switches
// at this point in ECE319K you need to be writing your own test functions
// modify this program so it tests your Traffic_In  function
int main3(void){ // main3
  uint32_t last=0,now;
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Traffic_Init(); // your Lab 4 initialization
  Debug_Init();   // Lab 3 debugging
  UART_Init();
  __enable_irq(); // UART uses interrupts
  UART_OutString("Lab 4, Spring 2025, Step 2. Debug switches\n\r");
  UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
  UART_OutString("EID2= "); UART_OutString((char*)EID2); UART_OutString("\n\r");
  while(1){
    now = Traffic_In(); // Your Lab4 input
    if(now != last){ // change
      UART_OutString("Switch= 0x"); UART_OutUHex(now); UART_OutString("\n\r");
      Debug_Dump(now);
    }
    last = now;
    Clock_Delay(800000); // 10ms, to debounce switch
  }
}
// use main4 to debug using your dump
// proving your machine cycles through all states
int main4(void){// main4
uint32_t input;
  Clock_Init80MHz(0);
  LaunchPad_Init();
  LaunchPad_LED1off();
  Traffic_Init(); // your Lab 4 initialization
  CPt = goS; // set initial state
  uint32_t debugVar;
  Debug_Init();   // Lab 3 debugging
  UART_Init();
  __enable_irq(); // UART uses interrupts
  UART_OutString("Lab 4, Spring 2025, Step 3. Debug FSM cycle\n\r");
  UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
  UART_OutString("EID2= "); UART_OutString((char*)EID2); UART_OutString("\n\r");
// initialize your FSM
  SysTick_Init();   // Initialize SysTick for software waits

  while(1){
      Traffic_Out(CPt->WestOut, CPt->SouthOut, CPt->WalkOut); //Output based on current state

      //the following lines of code set debugVar up correctly
      if ((CPt->WalkOut & (1<<26)) && (CPt->WalkOut & (1<<27)) && (CPt->WalkOut & (1<<22))) {
        debugVar = 7;
      } else if (CPt->WalkOut & (1<<26)) {
        debugVar = 2;
      } else {
        debugVar = 0;
      }
      debugVar |= CPt->number<<24;
      debugVar |= CPt->WestOut<<16;
      debugVar |= CPt->SouthOut<<8;

      Debug_Dump(debugVar); //Calls dump with state in bits 31-24, WestOut in bits 23-16, SouthOut in bits 15-8, and WalkOut in bits 7-0
      SysTick_Wait10ms(CPt->Time); //Delays based on the delay time of the current state

      input = 7; //Hard coded traffic in so that all three switches are pressed
      CPt = CPt->Next[input]; //Next state based on current state and input
  }
}
// use main5 to grade
int main(void){// main5
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Grader_Init(); // execute this line before your code
  LaunchPad_LED1off();
  Traffic_Init(); // your Lab 4 initialization
  SysTick_Init();   // Initialize SysTick for software waits
  CPt = goS;
  uint32_t input;
  Lab4Grader(1); // activate UART, grader and interrupts
  while(1){
      Traffic_Out(CPt->WestOut, CPt->SouthOut, CPt->WalkOut); //output depending on state
      SysTick_Wait10ms(CPt->Time); // wait depending on state
      input = Traffic_In(); // read input from switches
      CPt = CPt->Next[input]; // next state depends on current state and input
  }
}

