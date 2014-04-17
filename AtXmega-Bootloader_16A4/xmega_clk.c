
#include <avr/io.h>


// System Clocks initialization
void system_clocks_init(void)
{
unsigned char n,s;

// Optimize for speed

// Save interrupts enabled/disabled state
s=SREG;
// Disable interrupts
asm("cli");

// External 32,768 kHz oscillator initialization
// External 32.768 kHz crystal oscillator low power mode: Off
OSC.XOSCCTRL=OSC_XOSCSEL_32KHz_gc;
// Enable the external oscillator
OSC.CTRL|=OSC_XOSCEN_bm;

// Wait for the external oscillator to stabilize
while ((OSC.STATUS & OSC_XOSCRDY_bm)==0);

// Internal 32 MHz RC oscillator initialization
// Enable the internal 32 MHz RC oscillator
OSC.CTRL|=OSC_RC32MEN_bm;

// System Clock prescaler A division factor: 1
// System Clock prescalers B & C division factors: B:1, C:1
// ClkPer4: 32000,000 kHz
// ClkPer2: 32000,000 kHz
// ClkPer:  32000,000 kHz
// ClkCPU:  32000,000 kHz
n=(CLK.PSCTRL & (~(CLK_PSADIV_gm | CLK_PSBCDIV1_bm | CLK_PSBCDIV0_bm))) |
	CLK_PSADIV_1_gc | CLK_PSBCDIV_1_1_gc;
CCP=CCP_IOREG_gc;
CLK.PSCTRL=n;

// Internal 32 MHz RC osc. calibration reference clock source: 32.768 kHz Ext. Crystal Osc.
OSC.DFLLCTRL=(OSC.DFLLCTRL & (~OSC_RC2MCREF_bm)) | OSC_RC32MCREF_bm;
// Enable the autocalibration of the internal 32 MHz RC oscillator
DFLLRC32M.CTRL|=DFLL_ENABLE_bm;

// Wait for the internal 32 MHz RC oscillator to stabilize
while ((OSC.STATUS & OSC_RC32MRDY_bm)==0);

// Select the system clock source: 32 MHz Internal RC Osc.
n=(CLK.CTRL & (~CLK_SCLKSEL_gm)) | CLK_SCLKSEL_RC32M_gc;
CCP=CCP_IOREG_gc;
CLK.CTRL=n;

// Disable the unused oscillators: 2 MHz, internal 32 kHz, PLL
OSC.CTRL&= ~(OSC_RC2MEN_bm | OSC_RC32KEN_bm | OSC_PLLEN_bm);

// ClkPer output: Disabled bit 7
PORTCFG.CLKEVOUT=(PORTCFG.CLKEVOUT & (~PORTCFG_CLKOUT_gm)) | PORTCFG_CLKOUT_OFF_gc;

// Restore interrupts enabled/disabled state
SREG=s;
// Restore optimization for size if needed

}
