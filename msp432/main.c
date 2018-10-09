/*
 *
 * Timing:
 *     1. Driving timer from the SMCLK at default 3 MHz (approx)
 *     2. Divisor = 3 (ID=0, EX0=2)
 * so that the timer goes at about 1MHz.
 *
 */

#include "msp.h"

//12 LEDS
#define LED0_PORT P5
#define LED0_BIT BIT6
#define LED1_PORT P2
#define LED1_BIT BIT4
#define LED2_PORT P6
#define LED2_BIT BIT1
#define LED3_PORT P4
#define LED3_BIT BIT0
#define LED4_PORT P6
#define LED4_BIT BIT6
#define LED5_PORT P6
#define LED5_BIT BIT7
#define LED6_PORT P4
#define LED6_BIT BIT2
#define LED7_PORT P4
#define LED7_BIT BIT4
#define LED8_PORT P2
#define LED8_BIT BIT3
#define LED9_PORT P5
#define LED9_BIT BIT1
#define LED10_PORT P5
#define LED10_BIT BIT4
#define LED11_PORT P4
#define LED11_BIT BIT5

//Button
#define BUTTON_PORT P3
#define BUTTON_BIT  BIT7
#define DEBOUNCE_COUNT 40 // 2 seconds to debounce!

volatile unsigned int debounce_counter = DEBOUNCE_COUNT; // down counter for resetting button_flag
volatile unsigned int flag = 3;
// two distinct periods and duty cycles... Round up
volatile unsigned int modulation_freq;  //Period = 1000000/frequency
volatile unsigned int state_change_freq; //overhead of 3.84ms -> state change ~260Hz

volatile unsigned int bright_duty = 0; // #ticks = duty % * Period
volatile unsigned int dim_duty = 0;
volatile unsigned int state_duty = 0;
//led_on is a state that controls which LEDS are on
volatile unsigned int state = 0;
//row = LED
//col = state
//volatile unsigned int matrix[13][12] = {
//                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //state 1 = control
//                {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //state 2
//                {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //state 3
//                {1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //state 4
//                {1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1}, //state 5
//                {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1}, //state 6
//                {1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1}, //state 7
//                {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1}, //state 8
//                {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1}, //state 9
//                {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1}, //state 10
//                {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1}, //state 11
//                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, //state 12
//                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0} //state 13
//                };

volatile unsigned int matrix[13][12] = {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //state 1 = control
                {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //state 2
                {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //state 3
                {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //state 4
                {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, //state 5
                {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, //state 6
                {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, //state 7
                {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}, //state 8
                {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, //state 9
                {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}, //state 10
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, //state 11
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}, //state 12
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} //state 13
                };

// declare functions defined below

void init_timer(void);   // routine to setup the timer
void init_button(void);  // routine to setup the button

void set_frequency(){

  //the frequency of the square wave outputted for each led is actually 1/12 * state_change_freq.
  //might only need 1 timer total
  //130Hz -> 7.7ms
  state_change_freq = 60000; //

  state_duty = 30000; //state_change_freq/2
}
// ++++++++++++++++++++++++++

void main(){
  WDT_A->CTL = WDTPW + WDTHOLD; // Stop watchdog timer
  LED0_PORT->DIR |= LED0_BIT;
  LED0_PORT->OUT &= ~LED0_BIT;
    LED1_PORT->DIR |= LED1_BIT;   // Direction is output
    LED1_PORT->OUT &= ~LED1_BIT;  // LED off to start with
    LED2_PORT->DIR |= LED2_BIT;   // Direction is output
    LED2_PORT->OUT &= ~LED2_BIT;  // LED off to start with
    LED3_PORT->DIR |= LED3_BIT;   // Direction is output
    LED3_PORT->OUT &= ~LED3_BIT;  // LED off to start with
    LED4_PORT->DIR |= LED4_BIT;   // Direction is output
    LED4_PORT->OUT &= ~LED4_BIT;  // LED off to start with
    LED5_PORT->DIR |= LED5_BIT;   // Direction is output
    LED5_PORT->OUT &= ~LED5_BIT;  // LED off to start with
    LED6_PORT->DIR |= LED6_BIT;   // Direction is output
    LED6_PORT->OUT &= ~LED6_BIT;  // LED off to start with
    LED7_PORT->DIR |= LED7_BIT;   // Direction is output
    LED7_PORT->OUT &= ~LED7_BIT;  // LED off to start with
    LED8_PORT->DIR |= LED8_BIT;   // Direction is output
    LED8_PORT->OUT &= ~LED8_BIT;  // LED off to start with
    LED9_PORT->DIR |= LED9_BIT;   // Direction is output
    LED9_PORT->OUT &= ~LED9_BIT;  // LED off to start with
    LED10_PORT->DIR |= LED10_BIT;   // Direction is output
    LED10_PORT->OUT &= ~LED10_BIT;  // LED off to start with
    LED11_PORT->DIR |= LED11_BIT;   // Direction is output
    LED11_PORT->OUT &= ~LED11_BIT;  // LED off to start with

    //These pins used to check frequency of state change is as expected.
    P6->DIR |= BIT4;
    P6->OUT &= ~BIT4;
    P3->DIR |= BIT6;
    P3->OUT &= ~BIT6;
  init_timer();  // initialize timer
  init_button();
  // setup NVIC and Interrupts
  SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;   // Specify that after an interrupt, the CPU wakes up

  __enable_interrupt();// unmask IRQ interrupts toallow the CPU to respond.
  // enable the interrupt for this program (this is done after unmasking CPU)
  // ISER = Interrupt System Enable Register
  // A single 1 bit flag for each of the 64 theoretically possible interrupts
  // Notation: ISER[0] is for 0..31
  //           ISER[1] is for 31...63
  // Interrupt numbers are logical names in the include file.
  // Common numbers we have used in the early part of the course are:
    //     WDT_A_IRQn  =  3 (thus uses NVIC->ISER[0] to enable
  //     PORT3_IRQn  = 37 (uses NVIC->ISER[1])
  //     TA0_0_IRQn  =  8 (uses NVIC->ISER[0])
  //     TA0_N_IRQn  =  9 (uses NVIC->ISER[0])
  NVIC->ISER[1] = 1 << ((PORT3_IRQn) & 31); // enable P3 to send interrupt signals
  NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31); // enable TA0CCR0 to send interrupt signals
  NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31); // enable TA0CCRN to send interrupt signals

  NVIC->ISER[0] = 1 << ((TA1_0_IRQn) & 31); // enable TA1CCR0 to send interrupt signals
  NVIC->ISER[0] = 1 << ((TA1_N_IRQn) & 31); // enable TA1CCR0 to send interrupt signals

    while (1)
    {
        /* Go to LPM0 mode (Low power mode with CPU powered off */
        __sleep();      //
        __no_operation(); //  For debugger
    }

}


/*
 *
 * The timer clock source is SMCLK (3Mhz), prescaled by 1, then divided by 3.
 * Thus it is about 1MHz.
 * Note: possible prescale factors are 1,2,4,8.
 *       possible additional EX0 dividers are 1,2,3,4,5,6,7,8
 *
 */

void init_timer(){              // initialization and start of timer
  set_frequency();
  TIMER_A0->CTL |= TIMER_A_CTL_CLR ;// reset clock
  TIMER_A0->CTL =  TIMER_A_CTL_TASSEL_2  // clock source = SMCLK
                  +TIMER_A_CTL_ID_1      // clock divider=1
          +TIMER_A_CTL_MC_1;     // Up Mode
  TIMER_A0->EX0 = TIMER_A_EX0_TAIDEX_4;  // additional divisor=5
  //Timer A0 counts up to 3000000/2/8 = 187500

  // set CCR0 compare mode, output mode 7 (reset/set), interrupt enabled, flag clear
  TIMER_A0->CCTL[0]=TIMER_A_CCTLN_OUTMOD_7; //reset/set mode
  TIMER_A0->CCTL[1]=TIMER_A_CCTLN_OUTMOD_7; //reset/set mode
  TIMER_A0->CCR[0] = state_change_freq;
  TIMER_A0->CCR[1] = state_duty;

  TIMER_A1->CTL |= TIMER_A_CTL_CLR ;// reset clock
  TIMER_A1->CTL =  TIMER_A_CTL_TASSEL_2  // clock source = SMCLK
          +TIMER_A_CTL_ID_0      // clock prescale=1
          +TIMER_A_CTL_MC_2;     // Continuous Mode
  TIMER_A1->EX0 = TIMER_A_EX0_TAIDEX_2;  // additional divisor=3

  // set CCR0 compare mode, output mode 4 (toggle), interrupt enabled, flag clear
  TIMER_A1->CCTL[0]=TIMER_A_CCTLN_OUTMOD_4+TIMER_A_CCTLN_CCIE;
  TIMER_A1->CCR[0] = 50000; //5ms period
}


void leds(){
    LED0_PORT->OUT = (matrix[state][0]) ? LED0_PORT->OUT | LED0_BIT : LED0_PORT->OUT & ~LED0_BIT;
    LED1_PORT->OUT = (matrix[state][1]) ? LED1_PORT->OUT | LED1_BIT : LED1_PORT->OUT & ~LED1_BIT;
    LED2_PORT->OUT = (matrix[state][2]) ? LED2_PORT->OUT | LED2_BIT : LED2_PORT->OUT & ~LED2_BIT;
    LED3_PORT->OUT = (matrix[state][3]) ? LED3_PORT->OUT | LED3_BIT : LED3_PORT->OUT & ~LED3_BIT;
    LED4_PORT->OUT = (matrix[state][4]) ? LED4_PORT->OUT | LED4_BIT : LED4_PORT->OUT & ~LED4_BIT;
    LED5_PORT->OUT = (matrix[state][5]) ? LED5_PORT->OUT | LED5_BIT : LED5_PORT->OUT & ~LED5_BIT;
    LED6_PORT->OUT = (matrix[state][6]) ? LED6_PORT->OUT | LED6_BIT : LED6_PORT->OUT & ~LED6_BIT;
    LED7_PORT->OUT = (matrix[state][7]) ? LED7_PORT->OUT | LED7_BIT : LED7_PORT->OUT & ~LED7_BIT;
    LED8_PORT->OUT = (matrix[state][8]) ? LED8_PORT->OUT | LED8_BIT : LED8_PORT->OUT & ~LED8_BIT;
    LED9_PORT->OUT = (matrix[state][9]) ? LED9_PORT->OUT | LED9_BIT : LED9_PORT->OUT & ~LED9_BIT;
    LED10_PORT->OUT = (matrix[state][10]) ? LED10_PORT->OUT | LED10_BIT : LED10_PORT->OUT & ~LED10_BIT;
    LED11_PORT->OUT = (matrix[state][11]) ? LED11_PORT->OUT | LED11_BIT : LED11_PORT->OUT & ~LED11_BIT;
}
//TIMER 0 - > STATE CHANGE FREQUENCY (fundamental frequency)
void TA0_0_IRQHandler(){
  TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear flag
  //CLOCK SIG OFF
  P6->OUT &= ~BIT4;
  P3->OUT &= ~BIT6;
  state = (state+1) % 13;

}
void TA0_N_IRQHandler(){
  TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear flag
  leds();
  //CLOCK SIG ON
  P6->OUT |= BIT4;
  P3->OUT |= BIT6;
}
void TA1_0_IRQHandler(){
  TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear flag

  debounce_counter++;
//  printf("%d\n", debounce_counter);
}
void init_button(){
// All GPIO's are already inputs if we are coming in after a reset
  BUTTON_PORT->DIR &= ~BUTTON_BIT; //input
  BUTTON_PORT->OUT |= BUTTON_BIT; // pullup
  BUTTON_PORT->REN |= BUTTON_BIT; // enable resistor
  BUTTON_PORT->IES &= ~BUTTON_BIT; // set for 0->1 transition
  BUTTON_PORT->IFG &= ~BUTTON_BIT;// clear interrupt flag
  BUTTON_PORT->IE  |= BUTTON_BIT; // enable interrupt
}
void PORT3_IRQHandler(){
  if ((BUTTON_PORT->IN & BUTTON_BIT) != BUTTON_BIT){ // check that button is pressed
    if(debounce_counter >= DEBOUNCE_COUNT){
      BUTTON_PORT->IFG &= ~BUTTON_BIT; // clear the flag to allow for another interrupt later.
      debounce_counter = 0;
      TIMER_A0->CCTL[0] ^= TIMER_A_CCTLN_CCIE;
      TIMER_A0->CCTL[1] ^= TIMER_A_CCTLN_CCIE;
      LED0_PORT->OUT = LED0_PORT->OUT & ~LED0_BIT;
      LED1_PORT->OUT = LED1_PORT->OUT & ~LED1_BIT;
      LED2_PORT->OUT = LED2_PORT->OUT & ~LED2_BIT;
      LED3_PORT->OUT = LED3_PORT->OUT & ~LED3_BIT;
      LED4_PORT->OUT = LED4_PORT->OUT & ~LED4_BIT;
      LED5_PORT->OUT = LED5_PORT->OUT & ~LED5_BIT;
      LED6_PORT->OUT = LED6_PORT->OUT & ~LED6_BIT;
      LED7_PORT->OUT = LED7_PORT->OUT & ~LED7_BIT;
      LED8_PORT->OUT = LED8_PORT->OUT & ~LED8_BIT;
      LED9_PORT->OUT = LED9_PORT->OUT & ~LED9_BIT;
      LED10_PORT->OUT = LED10_PORT->OUT & ~LED10_BIT;
      LED11_PORT->OUT = LED11_PORT->OUT & ~LED11_BIT;
    }
  }
}
