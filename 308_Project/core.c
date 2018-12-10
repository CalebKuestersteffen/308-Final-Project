#include <gb/gb.h>

void init();
void checkInput();

void main() {

	init();
	
	while(1) {
		
		checkInput();
		
		// Game code here
	}
	
}

void init() {
	
	DISPLAY_ON;		// Turn on the display
	NR52_REG = 0x8F;	// Turn on the sound
	NR51_REG = 0x11;	// Enable the sound channels
	NR50_REG = 0x77;	// Increase the volume to its max
}

void checkInput() {

    if (joypad() & J_B) {
	// The B button was pressed!
    }

}