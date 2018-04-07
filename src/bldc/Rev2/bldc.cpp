#include "bldc.h"

#include <propeller.h>
#include <cstdio>
#include <cstdlib>


int main() {
	DIRA |= 1 << 16;
	while(1){
		OUTA |= 1 << 16;
	}
 }