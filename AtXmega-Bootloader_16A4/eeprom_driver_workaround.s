;NVM-workaround for ATxmega256A3revB for handling EEPROM command execution
/*MODULE _NVM_EXEC_workaround
PUBLIC NVM_EXEC_workaround
RSEG CODE*/
#include <avr/io.h>		

//NVM_EXEC_workaround:

	push	r20
	push	r21
	push	r22
	push	r23
	push	r16
	
	/*Make sure that the high level interrupt is enabled, so that the device wakes.
	This is just a safety feature to prevent the device from going to permenanly sleep,
	if it is forgotten to do the correct settings before calling this function*/
	ldi	r22, PMIC_HILVLEN_bm
	sts	PMIC_CTRL, r22
	
	ldi	r20, NVM_EELVL0_bm | NVM_EELVL1_bm
	ldi	r23, NVM_CMDEX_bm
	/* Set sleep enabled */
	ldi	r21,SLEEP_SEN_bm 
	sts	SLEEP_CTRL,r21
	ldi	r16, CCP_IOREG_gc  ; Prepare Protected IO signature in R16.
	sts	CCP, r16         ; Enable IO operation (this disables interrupts for 4 cycles).
	
	sts	NVM_CTRLA, r23         ; Execute EEPROM command
	
	sts	NVM_INTCTRL,r20  
	/* Sleep before 2.5uS has passed */
	SLEEP
	
	pop	r16
	pop	r23
	pop	r22
	pop	r21
	pop	r20
	ret

//ENDMOD

//END
