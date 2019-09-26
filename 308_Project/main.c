#include <gb/gb.h> 
#include <rand.h>
#include "Initial_Load.c"
#include "Tiles.c"
#include "Sprites.c"// contains all of the sprites

typedef struct Asteroid {
	UINT8 x;
	UINT8 y;
}Asteroid;

typedef enum bool{
	False,
	True
}bool;

typedef struct Player {
	UINT8 x;
	UINT8 y;
	bool alive;
}Player;

typedef struct Laser {
	UINT8 x;
	UINT8 y;
	bool fired;
}Laser;

typedef struct Explosion {
	UINT8 delay;
	bool flag;
}Explosion;

void init();
void checkInput();
void moveLaser();
void moveAstroids();
void checkCollisions();
void updateSwitches();
bool collisionCheck(UINT8, UINT8, UINT8, UINT8, UINT8);

//Globals
Player player;// the x/y of the ship
Laser laser;// the x/y of the laser
Explosion explosion;
UINT8 Asteroid_Sprites = 0;// Use this for keeping sprite total under 20
UINT8 i = 0;
UINT8 j = 0;
UINT8 score = 0;
UINT8 upDown = 0;
UINT8 tick = 0;
Asteroid Asteroids[16];

// working
void main() {
	init();	
	while(1) {		
		checkInput();
		updateSwitches();
		wait_vbl_done();
		moveAstroids();
		moveLaser();
		checkCollisions();
	}// end of while
}// end of main

//working (don't change)
void init() {
	DISPLAY_ON;		// Turn on the display
	//NR52_REG = 0x8F;	// Turn on the sound
	//NR51_REG = 0x11;	// Enable the sound channels
	//NR50_REG = 0x77;	// Increase the volume to its max

	set_bkg_data(0,3,Tiles);// loads the tiles into memory	
	set_bkg_tiles(0,0,20,18,Initial_Load);// updates the tiles	
	set_sprite_data(0,10,Sprites);// loads the sprite(s) into memory	
	set_sprite_tile(0,0);// sets the top part of the ship to sprite 0
	set_sprite_tile(1,1);// sets the middle of the ship to sprite 1
	set_sprite_tile(2,2);// sets the bottom of the ship to sprite 2
	set_sprite_tile(3,3);// sets the laser to be sprite 3

	player.x = 160;// player spawn position
	player.y = 80;
	laser.fired = False;// if the laser has been laser.fired
	player.alive = True;// players lives
	explosion.flag = False;
	explosion.delay = 0;

	i = 4;
	while (i < 20) {// makes all the remaining sprites into asteroids (sprites 4-19 specifically)
		set_sprite_tile(i, 6);// i is the sprite number and 6 is the asteroid tile

		//	======> add to an array of asteroids here possibly <=======
		Asteroids[i - 4].x = ((rand() % 12) + 16);
		Asteroids[i - 4].y = ((rand() % 112) + 32);
		move_sprite(i, Asteroids[i - 4].x, Asteroids[i - 4].y);
		i++;
	}// end of for

}// end of initialization

//Don't change
void updateSwitches() {
	
	HIDE_WIN;
	SHOW_SPRITES;
	SHOW_BKG;
	
}

//working (don't change)
void checkInput() {

	if (joypad() & J_B && player.alive == True) {// The B button was pressed!
		if (laser.fired == False) {
			laser.fired = True;// the laser is laser.fired
			set_sprite_tile(3, 3);
			laser.x = player.x + 1;
			laser.y = player.y;
		}
	}
	//UP
	if (joypad() & J_UP && player.y > 20 && player.alive == True) {// makes sure its not touching the top
		player.y -= 1;	
		
	}

	// DOWN
	if (joypad() & J_DOWN && player.y < 148 && player.alive == True) {// makes sure its not touching the bottom
			player.y += 1;
		
	}
	
	if (player.alive == True) {
		move_sprite(0, player.x, (player.y - 8));
		move_sprite(1, player.x, player.y);
		move_sprite(2, player.x, (player.y + 8));
	}
}

//working (don't change)
void moveLaser() {
	if (laser.fired == True && laser.x > 3) {// if the laser has been laser.fired and its not off the screen and it isnt hitting an asteroid
		laser.x -= 3;
	}
	else if (laser.fired == True && laser.x <= 3) {// if the laser has been laser.fired and its going off screen
		laser.x = 0;// remove the laser
		laser.y = 0;// remove the laser
		set_sprite_tile(3, 5);
		laser.fired = False;
	}

	move_sprite(3, laser.x, laser.y);
}
//working (don't change)
void moveAstroids() {
	
	if (score > 30)
		j = 0;
	else
		j = 30 - score;
	if (player.alive == True && tick > j) {
		i = 4;
		while (i < 20) {
			Asteroids[i - 4].x += 1;
			move_sprite(i, Asteroids[i - 4].x, Asteroids[i - 4].y);
			i++;
		}
		tick = 0;
	}
	else
	{
		tick = tick + 1;
	}
	
}
// working (don't change)
void checkCollisions()
{
	i = 0;
	while (i < 16) {// checks all asteroids (20 - (3 for ship and 1 for laser) leaves 16 because 20 is the sprite max)
		if (collisionCheck(player.x, (player.y), 14, Asteroids[i].x, Asteroids[i].y) == True) {
			set_sprite_tile(0, 7);
			set_sprite_tile(1, 4);// ship goes boom
			set_sprite_tile(2, 8);
			player.alive = False;// stops player movement and shooting
		}// end of if
		else if ((collisionCheck(laser.x, laser.y, 8, Asteroids[i].x, Asteroids[i].y) == True)) {
			score++;
			Asteroids[i].x = (rand() & 12) + 8;
			Asteroids[i].y = (rand() & 112) + 32;
			move_sprite((i + 4), Asteroids[i].x, Asteroids[i].y);// respawns asteroid
			set_sprite_tile(3, 4);//changes the laser to a blown up asteroid
			laser.fired = False;// allows laser to be fired again
			explosion.flag = True;
		}// end of else if	
		i++;
	}// end of while

	if (explosion.delay == 3 && explosion.flag == True) {// clears laser from screen
		laser.x = 0;
		laser.y = 0;
		set_sprite_tile(3, 5);// makes the laser black (invisible)
		explosion.delay = 0;
		explosion.flag = False;
	}// end of if
	else if (explosion.flag == True) {
		explosion.delay++;
	}// end of else if

}

// Check if the ship/laser (Sprite_X,Sprite_Y, and extending out 8, 8) 
//  overlap with an Asteroid (Asteroid_X,Asteroid_Y, and extending out 8, 8)
bool collisionCheck(UINT8 Sprite_X, UINT8 Sprite_Y, UINT8 Sprite_height, UINT8 Asteroid_X, UINT8 Asteroid_Y) {
	j = 8;
	//	within radius to the right		 within radius to left			 within radius below			  within radius above
	if ( (Sprite_X < (Asteroid_X + 8)) && ((Sprite_X + 8) > Asteroid_X) && (Sprite_Y < (Asteroid_Y + Sprite_height)) && ((Sprite_Y + Sprite_height) > Asteroid_Y) ) {
		return True;// hit registered
	}
	else {
		return False;// no hit
	}
}
