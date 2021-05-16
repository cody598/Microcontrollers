#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#define lowcutoff  153 /* ~3.0V (2.988V) 8-bit converted reading. */
#define highcutoff 256 /* ~3.333V (3.339V) 8-bit converted reading. */

// Function prototypes here (any function that is CALLED)

void cpuinit(void);
void mtiminit(void);
void pininit(void);
void flaginit(void);
void TurnOnLight(void);
void TurnOffLight(void);
void TOFdisable (void);
void TOFenable (void);
void SetCounter(void);
void resetCoco(void);

// Global variable declarations go here
unsigned int counter;
unsigned int analogRead;
int LightOffFlag;
void main(void) 
{
  // Variable declarations go here
  
  counter = 248; //Counter for 2 seconds of on time

  // Initialization routines called here
  cpuinit();
  pininit();
  mtiminit();
   
  EnableInterrupts;
   
  // Main loop 
  for(;;)
  {

  }
  /* loop forever */
	
  /* never leave main */

}
void cpuinit(void)
{   /* Turn off watchdog and trim oscillator */
	SOPT1 = 0x53;                                               
	ICSTRM = NVICSTRM;
}

/* Sets up MTimer specificaitons. */
void mtiminit(void)
{
	// Enables MTIM overflow interrupt.
	MTIMSC_TOIE = 1;
	MTIMSC_TSTP = 0;
	
	// MTIM clock uses bus clock.
	MTIMCLK_CLKS = 0;
	
	// Clock source prescaler  is 256
	MTIMCLK_PS = 8;
	
	// Modulus for 2 seconds of PTAD_PTAD1 high time.
	MTIMMOD = 248; 
}

/* Set up all pins, including the ADC pins. */
void pininit(void)
{
  // Initializes PTAD direction and data..
  PTADD_PTADD1 = 1;
  PTAD_PTAD1 = 0;
  	
  // Sets input channel to 0.
  ADCSC1_ADCH = 0;
  
  // Initiates conversion via software.
  ADCSC2_ADTRG = 0;
  
  // Disables I/O control for ADP0 (pin 16).
  APCTL1_ADPC0= 1;
  
  // Sets mode to 8- bit conversion.
  ADCCFG_MODE = 0; 
  
}

/* Light off flag to prevent ADC conversions */
void flaginit (void)
{
   LightOffFlag == 1;
}
    
/* Turns relay on */
void TurnOnLight(void)
{
	PTAD_PTAD1 = 1;
}

/* Turns relay off */
void TurnOffLight(void)
{
	PTAD_PTAD1 = 0;
}

/* TOF flag set to 0 */
void TOFdisable (void)
{
	MTIMSC_TOF = 0;	
}

/* TOF flag set to 1 */
void TOFenable (void)
{
	MTIMSC_TOF = 1;
}

/* Sets counter to match MTIMMOD for 2 sec interrupt */
void SetCounter(void)
{
	counter = 248;
}

/* Sets Coco flag to low */
void resetCoco(void) 
{
  ADCSC1_COCO = 0;
}

  
  

/* Interrupt occurs once every 8 ms */
interrupt VectorNumber_Vmtim void mtim_tof_isr(void)
{  
  MTIMSC;
  
  // Clears TOF flag to low.
  TOFdisable();
        
  // Conversion with prevention check.
  if (ADCSC1_COCO && LightOffFlag == 1) 
  { 
     analogRead = ADCRL;   
  }   
  // Checks to see if PIR sensor detected anything post conversion.
  if (analogRead >= lowcutoff && analogRead <= highcutoff) 
  { 	    
    LightOffFlag = 0;
  	TurnOnLight();
  }
  // Decrements counter.
  if(PTAD_PTAD1 == 1 && counter > 0) 
  {
    counter--;
  }	  
   
  else
  {
    // Sets TOF Flag high
    TOFenable();
    	
    // Turns relay off.
   	TurnOffLight();
    	
   	// Resets counter to initial value.
   	SetCounter();
   	
   	// Lowers Conversion complete flag.
   	resetCoco();
   	
    // Sets light on flag to starting state.	
   	flaginit();
   
  }  
  
}








