#include <propeller.h>
#include "bldc.h"

extern unsigned int _load_start_bldc_cog[];
static unsigned stack_bldc[BLDC_STACK_SIZE + 1];
volatile struct bldc_mb motorPitch; 


int main() {
	stack_bldc[BLDC_STACK_SIZE] = (unsigned)&motorPitch;
	cognew(_load_start_bldc_cog, &stack_bldc[BLDC_STACK_SIZE]);
}