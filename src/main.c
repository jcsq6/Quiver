#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <graphx.h>
#include <keypadc.h>
#include <debug.h>

#include <compression.h>

#include "gfx/gfx.h"

bool isTouching(enum bodyParts part, bool checkForPad, int yVelocity, int xVelocity);

enum direction{
	right,
	left,
	up,
	down,
	upLeft,
	upRight,
	downLeft,
	downRight,
	nothing
};

enum bodyParts{
	leftSide,
	rightSide,
	topHead,
	bottomFeet
};

gfx_point_t hero;

void main(void) {

	int x, y, i, mapNum, velocity;

	gfx_point_t lastStill;
	gfx_point_t arrow, arrowInit;

	gfx_sprite_t *map;

	gfx_sprite_t *HeroStill, *HeroLeft0, *HeroLeft1, *HeroLeft2, *HeroRight0, *HeroRight1, *HeroRight2,*HeroJumpRight0, *HeroJumpRight1, *HeroJumpRight2, *HeroJumpUpRight, *HeroJumpUpLeft, *HeroJumpLeft0, *HeroJumpLeft1, *HeroJumpLeft2;

	double distance, arrowDistance;
	unsigned int step;

	enum direction jumpingDir;
	enum direction shootingDir;

	bool doubleJumped, keyIsReleased, inLadder, facingRight, ascending, key, prevkey, isBoosted, tempBool1, tempBool2;

	gfx_sprite_t *heroRunRight[3];
	gfx_sprite_t *heroRunLeft[3];

	gfx_sprite_t *heroJumpRight[3];

	gfx_sprite_t *behind_sprite;

	gfx_sprite_t *tempPerson;

	map = gfx_MallocSprite(80, 60);

	HeroStill = gfx_MallocSprite(20, 40);
	HeroLeft0 = gfx_MallocSprite(20, 40);
	HeroLeft1 = gfx_MallocSprite(20, 40);
	HeroLeft2 = gfx_MallocSprite(20, 40);
	HeroRight0 = gfx_MallocSprite(20, 40);
	HeroRight1 = gfx_MallocSprite(20, 40);
	HeroRight2 = gfx_MallocSprite(20, 40);
	HeroJumpRight0 = gfx_MallocSprite(20, 40);
	HeroJumpRight1 = gfx_MallocSprite(20, 40);
	HeroJumpRight2 = gfx_MallocSprite(20, 40);
	HeroJumpUpRight = gfx_MallocSprite(20, 40);
	HeroJumpUpLeft = gfx_MallocSprite(20, 40);
	HeroJumpLeft0 = gfx_MallocSprite(20, 40);
	HeroJumpLeft1 = gfx_MallocSprite(20, 40);
	HeroJumpLeft2 = gfx_MallocSprite(20, 40);


	zx7_Decompress( HeroStill, HeroStill_compressed );
	zx7_Decompress( HeroRight0, HeroRight0_compressed );
	zx7_Decompress( HeroRight1, HeroRight1_compressed );
	zx7_Decompress( HeroRight2, HeroRight2_compressed );
	zx7_Decompress( HeroJumpRight0, HeroJumpRight0_compressed );
	zx7_Decompress( HeroJumpRight1, HeroJumpRight1_compressed );
	zx7_Decompress( HeroJumpRight2, HeroJumpRight2_compressed );
	zx7_Decompress( HeroJumpUpRight, HeroJumpUpRight_compressed );

	gfx_FlipSpriteY(HeroRight0, HeroLeft0);
	gfx_FlipSpriteY(HeroRight1, HeroLeft1);
	gfx_FlipSpriteY(HeroRight2, HeroLeft2);
	gfx_FlipSpriteY(HeroJumpRight0, HeroJumpLeft0);
	gfx_FlipSpriteY(HeroJumpRight1, HeroJumpLeft1);
	gfx_FlipSpriteY(HeroJumpRight2, HeroJumpLeft2);
	gfx_FlipSpriteY(HeroJumpUpRight, HeroJumpUpLeft);

	velocity = 2;

	jumpingDir = nothing;

	ascending = doubleJumped = inLadder = isBoosted = keyIsReleased = false;

	facingRight = true;

	heroRunRight[0] = HeroRight0;
	heroRunRight[1] = HeroRight1;
	heroRunRight[2] = HeroRight2;

	heroRunLeft[0] = HeroLeft0;
	heroRunLeft[1] = HeroLeft1;
	heroRunLeft[2] = HeroLeft2;

	heroJumpRight[0] = HeroJumpRight0;
	heroJumpRight[1] = HeroJumpRight1;
	heroJumpRight[2] = HeroJumpRight2;

	behind_sprite = gfx_MallocSprite(20, 40);

	lastStill.x = hero.x;
	lastStill.y = hero.y;

	shootingDir = nothing;

	srand(rtc_Time());

	gfx_Begin();

	gfx_SetPalette(global_palette, sizeof_global_palette, 0);

	mapNum = 1;

	gfx_FillScreen(1);

	while(!(kb_Data[6] & kb_Enter)){
		kb_Scan();

   		key = kb_Data[7] == kb_Down;

		if(key && !prevkey) {
			if(mapNum == 7) mapNum = 0;
			mapNum++;
			gfx_SetColor(1);

			gfx_FillRectangle(0, 0, 4, 4);
			gfx_FillRectangle(0, 20, 4, 4);
			gfx_FillRectangle(0, 40, 4, 4);
			gfx_FillRectangle(0, 60, 4, 4);
			gfx_FillRectangle(0, 80, 4, 4);


			gfx_SetColor(0);
		}

		gfx_PrintStringXY("Map1", 20, 0);
		gfx_PrintStringXY("Map2", 20, 20);
		gfx_PrintStringXY("Map3", 20, 40);
		gfx_PrintStringXY("Map4", 20, 60);
		gfx_PrintStringXY("Map5", 20, 80);

		if(mapNum == 1)	gfx_FillRectangle(0, 0, 4, 4);
		else if(mapNum == 2) gfx_FillRectangle(0, 20, 4, 4);			
		else if(mapNum == 3) gfx_FillRectangle(0, 40, 4, 4);
		else if(mapNum == 4) gfx_FillRectangle(0, 60, 4, 4);
		else if(mapNum == 5) gfx_FillRectangle(0, 80, 4, 4);

		prevkey = key;

		dbg_ClearConsole();
		dbg_sprintf(dbgout, "%d", mapNum);
	}

	if(mapNum == 1)	{
		zx7_Decompress( map, map1_compressed );
		hero.x = 0;
		hero.y = 150;
	}
	else if(mapNum == 2) {
		zx7_Decompress( map, map2_compressed );		
		hero.x = 0;
		hero.y = 180;
	}
	else if(mapNum == 3) {
		zx7_Decompress( map, map3_compressed );
		hero.x = 0;
		hero.y = 180;
	}
	else if(mapNum == 4) {
		zx7_Decompress( map, map4_compressed );
		hero.x = 50;
		hero.y = 100;
	}		
	else if(mapNum == 5) {
		zx7_Decompress( map, map5_compressed );
		hero.x = 80;
		hero.y = 180;
	}

	gfx_ScaledSprite_NoClip(map, 0, 0, 4, 4);

	gfx_SetDrawBuffer();
	
	gfx_SetTransparentColor(1);

	gfx_ScaledSprite_NoClip(map, 0, 0, 4, 4);

	gfx_BlitBuffer();

	gfx_GetSprite(behind_sprite, hero.x, hero.y);
	gfx_TransparentSprite_NoClip(HeroStill, hero.x, hero.y);

	while(!(kb_Data[6] & kb_Clear)){

		kb_Scan();

		if(shootingDir == nothing && kb_Data[7] & kb_Right) {
			shootingDir = right;
			arrow.x = hero.x + 15;
			arrow.y = hero.y + 4;
			arrowInit.x = hero.x + 15;
			arrowInit.y = hero.y + 4;
		}
		if(shootingDir == nothing && kb_Data[7] & kb_Left) {
			shootingDir = left;
			arrow.x = hero.x;
			arrow.y = hero.y + 4;
			arrowInit.x = hero.x;
			arrowInit.y = hero.y + 4;
		}
		if(shootingDir == nothing && kb_Data[7] & kb_Up && kb_Data[7] & kb_Left){
			shootingDir = upLeft;
			arrow.x = hero.x;
			arrow.y = hero.y + 4;
			arrowInit.x = hero.x;
			arrowInit.y = hero.y + 4;
		}
		if(shootingDir == nothing && kb_Data[7] & kb_Up && kb_Data[7] & kb_Right) {
			shootingDir = upRight;
			arrow.x = hero.x + 15;
			arrow.y = hero.y + 4;
			arrowInit.x = hero.x + 15;
			arrowInit.y = hero.y + 4;
		}
		if(shootingDir == nothing && kb_Data[7] & kb_Down && kb_Data[7] & kb_Left) {
			shootingDir = downLeft;
			arrow.x = hero.x;
			arrow.y = hero.y + 4;
			arrowInit.x = hero.x;
			arrowInit.y = hero.y + 4;
		}
		if(shootingDir == nothing && kb_Data[7] & kb_Down && kb_Data[7] & kb_Right) {
			shootingDir = downRight;
			arrow.x = hero.x + 15;
			arrow.y = hero.y + 4;
			arrowInit.x = hero.x + 15;
			arrowInit.y = hero.y + 4;
		}

		if(shootingDir == right){
			arrow.x += 5;
			arrowDistance = arrow.x - arrowInit.x;
			if(arrowDistance < 10) arrow.y = arrowInit.y + floor(.1*(arrowDistance - 10)*(arrowDistance - 10)+10);
			if(arrowDistance >= 10 && arrowDistance < 50) y = arrowInit.y + 10;
			if(arrowDistance >= 50) arrow.y = arrowInit.y + floor(.002*(arrowDistance - 50)*(arrowDistance - 50) + 10);
			gfx_FillRectangle(arrow.x, arrow.y, 2, 2);
		}


		tempBool1 = isTouching(bottomFeet, false, velocity - 1, 0);
		tempBool2 = isTouching(bottomFeet, false, 0, 0);
		velocity = (hero.y - lastStill.y ) / 50 + 2;

		dbg_ClearConsole();
		dbg_sprintf(dbgout, "hero = (%d, %d)\n", hero.x, hero.y);
		dbg_sprintf(dbgout, "velocity = %d\n", velocity);
		dbg_sprintf(dbgout, "keyIsReleased = %d\n", keyIsReleased);
		dbg_sprintf(dbgout, "ascending = %d\n", ascending);
		dbg_sprintf(dbgout, "inLadder = %d\n", inLadder);
		dbg_sprintf(dbgout, "doubleJumped = %d\n", doubleJumped);
		dbg_sprintf(dbgout, "jumpingDir = %d\n", jumpingDir);
		dbg_sprintf(dbgout, "tempBool1 = %d\n", tempBool1);
		dbg_sprintf(dbgout, "tempBool2 = %d\n", tempBool2);
		gfx_BlitBuffer();
		//while(!os_GetCSC());

		if(((kb_Data[1] & kb_Mode && !isTouching(rightSide, false, 0, 1)) || (kb_Data[1] & kb_2nd && !isTouching(leftSide, false, 0, -1))) && !((kb_Data[1] & kb_Mode) && (kb_Data[1] & kb_2nd)) && jumpingDir == nothing && !(inLadder && hero.y < 59 && hero.y > 41 && mapNum == 1) && !isBoosted && !ascending)  {
			gfx_Sprite_NoClip(behind_sprite, hero.x, hero.y);

			//IF RIGHT IS PRESSED
			if(kb_Data[1] & kb_Mode) {
				facingRight = true;
				if(((kb_Data[7] & kb_Up && !(kb_Data[7] & kb_Left) && !(kb_Data[7] & kb_Right))) && (!inLadder || (mapNum == 1 && hero.y == 40) || (mapNum == 2 && hero.y == 120))) {
					jumpingDir = right;
					ascending = true;
					keyIsReleased = false;
					lastStill.x = hero.x;
					lastStill.y = hero.y;
					if(!tempBool2) doubleJumped = true;
				}
				hero.x += 2;
			};
			//IF LEFT IS PRESSED
			if(kb_Data[1] & kb_2nd){ 
				facingRight = false;
				if(((kb_Data[7] & kb_Up && !(kb_Data[7] & kb_Left) && !(kb_Data[7] & kb_Right))) && (!inLadder || (mapNum == 1 && hero.y == 40) || (mapNum == 2 && hero.y == 120))) {
					jumpingDir = left;
					ascending = true;
					keyIsReleased = false;

					lastStill.x = hero.x;
					lastStill.y = hero.y;
					if(!tempBool2) doubleJumped = true;
				}				
				hero.x -= 2;
			}

			//IF IS IN AIR
			if(!tempBool2 && !inLadder){
					if(!tempBool1)	hero.y += velocity;
					else if(!tempBool2 && tempBool1) hero.y++;

					if(isTouching(rightSide, false, 0, 0)) hero.x--;
					if(isTouching(leftSide, false, 0, 0)) hero.x++;
			}
			else if(!inLadder && tempBool2){
				lastStill.y = hero.y;
			}

			if(isTouching(bottomFeet, true, 0, 0)) {
				isBoosted = true;
				ascending = true;
				lastStill.x = hero.x;
				lastStill.y = hero.y;
			}

			inLadder = false;
			if((hero.x >= 106 && hero.x <= 136 && hero.y >= 39 && hero.y <= 140 && mapNum == 1) || (hero.x >= 271 && mapNum == 2 && hero.y >= 120)) inLadder = true;

			if(hero.x < 0) hero.x = 0;
			if(hero.x > 300) hero.x = 300;

			gfx_GetSprite(behind_sprite, hero.x, hero.y);

			if(kb_Data[1] & kb_Mode){
				distance = hero.x - lastStill.x;
				step = floor(distance/6);

				gfx_TransparentSprite_NoClip(heroRunRight[ step % 3 ], hero.x, hero.y);
			}
			if(kb_Data[1] & kb_2nd){
				distance = lastStill.x - hero.x;
				step = floor(distance/6);

				gfx_TransparentSprite_NoClip(heroRunLeft[ step % 3 ], hero.x, hero.y);
			}

		}
		//IF NOTHING IS PRESSED
		else{

			gfx_Sprite_NoClip(behind_sprite, hero.x, hero.y);


			//IF IS IN AIR
			if(!tempBool2 && !inLadder && !ascending){

					if(!tempBool1)	hero.y += velocity;
					else if(!tempBool2 && tempBool1) hero.y++;

					if(isTouching(rightSide, false, 0, 0)) hero.x--;
					if(isTouching(leftSide, false, 0, 0)) hero.x++;
					gfx_GetSprite(behind_sprite, hero.x, hero.y);
			}
			else if(tempBool2 && !inLadder && !ascending){
				jumpingDir = nothing;
				doubleJumped = false;
				keyIsReleased = false;
				if(isTouching(bottomFeet, true, 0, 0)) {
					isBoosted = true;
					ascending = true;
					jumpingDir = nothing;
					lastStill.x = hero.x;
					lastStill.y = hero.y;
					doubleJumped = false;
				}	
			}

			//IF ASCENDING
			if(ascending){
				inLadder = false;
				if(!isBoosted) velocity = -2;
				else velocity = -10;

				if(!isTouching(topHead, false, velocity + 1, 0)) hero.y += velocity;
				if(((lastStill.y - hero.y > 25 && !isBoosted) || (lastStill.y - hero.y > 160 && isBoosted)) || isTouching(topHead, false, velocity + 1, 0)) {
					ascending = false;
					isBoosted = false;
					lastStill.x = hero.x;
					lastStill.y = hero.y;
				}
				gfx_GetSprite(behind_sprite, hero.x, hero.y);
			}

			//MOVING LADDER
			if(inLadder && (((kb_Data[7] & kb_Up && !(kb_Data[7] & kb_Left) && !(kb_Data[7] & kb_Right))) && !(kb_Data[7] & kb_Left) && !(kb_Data[7] & kb_Right))){
				gfx_Sprite_NoClip(behind_sprite, hero.x, hero.y);
				hero.y -= 2;
				gfx_GetSprite(behind_sprite, hero.x, hero.y);
			}
			else if(inLadder && ((kb_Data[7] & kb_Down && !(kb_Data[7] & kb_Left) && !(kb_Data[7] & kb_Right)))){
				gfx_Sprite_NoClip(behind_sprite, hero.x, hero.y);
				if(hero.y != 140 && mapNum == 1 || mapNum != 1 && hero.y != 200) hero.y += 2;
				gfx_GetSprite(behind_sprite, hero.x, hero.y);
			}

			if(!((((kb_Data[7] & kb_Up && !(kb_Data[7] & kb_Left) && !(kb_Data[7] & kb_Right))) && !(kb_Data[7] & kb_Left) && !(kb_Data[7] & kb_Right))) && !keyIsReleased  && !doubleJumped && jumpingDir != nothing) keyIsReleased = true;

			
			//CHECK FOR JUMPING UP
			if(((((kb_Data[7] & kb_Up && !(kb_Data[7] & kb_Left) && !(kb_Data[7] & kb_Right))) && !(kb_Data[7] & kb_Left) && !(kb_Data[7] & kb_Right))) && !doubleJumped && !inLadder && ((keyIsReleased && jumpingDir != nothing) || (!keyIsReleased && jumpingDir == nothing))) {
				jumpingDir = up;
				isBoosted = false;
				ascending = true;

				lastStill.x = hero.x;
				lastStill.y = hero.y;

				keyIsReleased = false;

				if(!tempBool2) doubleJumped = true;
				
			}
			//STANDING STILL ANIMATION
			if(jumpingDir == nothing && !isBoosted){

				inLadder = false;
				if((hero.x >= 106 && hero.x <= 136 && hero.y >= 39 && hero.y <= 140 && mapNum == 1) || (hero.x >= 271 && mapNum == 2 && hero.y >= 120) && jumpingDir == nothing) inLadder = true;

				if(isTouching(bottomFeet, true, 0, 0)) {
					isBoosted = true;
					ascending = true;
					lastStill.x = hero.x;
					lastStill.y = hero.y;
				}

				gfx_TransparentSprite_NoClip(HeroStill, hero.x, hero.y);
			}

			//JUMPING RIGHT ANIMATION
			else if(jumpingDir == right){
				gfx_Sprite_NoClip(behind_sprite, hero.x, hero.y);

				if(hero.x - lastStill.x <= 50 && !isTouching(rightSide, false, 0, 1)) hero.x += 2;
				else if(hero.x - lastStill.x > 50 && hero.x - lastStill.x <= 65 && !isTouching(rightSide, false, 0, 0)) hero.x++;

				if(hero.x > 300) hero.x = 300;

				gfx_GetSprite(behind_sprite, hero.x, hero.y);

				if(hero.x - lastStill.x <= 12) gfx_TransparentSprite_NoClip(HeroJumpRight0, hero.x, hero.y);
				else if(hero.x - lastStill.x > 12 && hero.x - lastStill.x <= 24) gfx_TransparentSprite_NoClip(HeroJumpRight1, hero.x, hero.y);
				else if(hero.x - lastStill.x > 24) gfx_TransparentSprite_NoClip(HeroJumpRight2, hero.x, hero.y);

				
			}
			//JUMPING LEFT ANIMATION
			else if(jumpingDir == left){
				gfx_Sprite_NoClip(behind_sprite, hero.x, hero.y);

				if(lastStill.x - hero.x <= 50 && !isTouching(leftSide, false, 0, -1)) hero.x -= 2;
				else if(lastStill.x - hero.x > 50 && lastStill.x - hero.x <= 65 && !isTouching(leftSide, false, 0, 0)) hero.x--;

				if(hero.x < 0) hero.x = 0;

				gfx_GetSprite(behind_sprite, hero.x, hero.y);

				if(lastStill.x - hero.x <= 12) gfx_TransparentSprite_NoClip(HeroJumpLeft0, hero.x, hero.y);
				else if(lastStill.x - hero.x > 12 && lastStill.x - hero.x <= 24) gfx_TransparentSprite_NoClip(HeroJumpLeft1, hero.x, hero.y);
				else if(lastStill.x - hero.x > 24) gfx_TransparentSprite_NoClip(HeroJumpLeft2, hero.x, hero.y);
			}
			//JUMPING UP ANIMATION
			else if(jumpingDir == up){
				gfx_Sprite_NoClip(behind_sprite, hero.x, hero.y);

				if((kb_Data[1] & kb_2nd) && !isTouching(leftSide, false, 0, 0)) {
					hero.x--;
					facingRight = false;
				}
				if(kb_Data[1] & kb_Mode && !isTouching(rightSide, false, 0, 0)) {
					hero.x++;
					facingRight = true;
				}

				gfx_GetSprite(behind_sprite, hero.x, hero.y);
				
				if(facingRight) gfx_TransparentSprite_NoClip(HeroJumpUpRight, hero.x, hero.y);
				else gfx_TransparentSprite_NoClip(HeroJumpUpLeft, hero.x, hero.y);
				
			}
			else if(isBoosted){
				gfx_Sprite_NoClip(behind_sprite, hero.x, hero.y);

				if((kb_Data[1] & kb_2nd) && !isTouching(leftSide, false, 0, 0)) {
					hero.x--;
					facingRight = false;
				}
				if(kb_Data[1] & kb_Mode && !isTouching(rightSide, false, 0, 0)) {
					hero.x++;
					facingRight = true;
				}

				gfx_GetSprite(behind_sprite, hero.x, hero.y);
				
				if(facingRight) gfx_TransparentSprite_NoClip(HeroJumpUpRight, hero.x, hero.y);
				else gfx_TransparentSprite_NoClip(HeroJumpUpLeft, hero.x, hero.y);
				
			}
		}

	}

	gfx_End();
}

bool isTouching(enum bodyParts part, bool checkForPad, int yVelocity, int xVelocity){
	int f;

	bool check;
	check = true;
	if(part == leftSide){
		for(f = 3; f <= 40; f += 3) {
			if(gfx_GetPixel(hero.x + 2 + xVelocity, hero.y + f + yVelocity) == 0 || hero.x + xVelocity == 0) return true;
		}
	}
	else if(part == rightSide){
		for(f = 3; f <= 40; f += 3) {
			if(gfx_GetPixel(hero.x + 17 + xVelocity, hero.y + f + yVelocity) == 0 || hero.x + 20 + xVelocity== 320) return true;
		}
	}
	else if(part == topHead){
		for(f = 3; f <= 16; f += 3) {
			if(gfx_GetPixel(hero.x + f + xVelocity, hero.y + yVelocity) == 0 || hero.y + yVelocity < 0) return true;
		}
	}
	else if(part == bottomFeet && !checkForPad){
		for(f = 6; f <= 14; f += 3) {
			if(gfx_GetPixel(hero.x + f + xVelocity, hero.y + 40 + yVelocity) == 0 || hero.y + 41 + yVelocity > 240 || gfx_GetPixel(hero.x + f + xVelocity, hero.y + 40 + yVelocity) == 3 || gfx_GetPixel(hero.x + f + xVelocity, hero.y + 40 + yVelocity) == 4) return true;
		}
	}
	else if(part == bottomFeet && checkForPad){
		for(f = 6; f <= 14; f += 3) {
			if(gfx_GetPixel(hero.x + f + xVelocity, hero.y + 41 + yVelocity) != 3 || gfx_GetPixel(hero.x + f + xVelocity, hero.y + 40 + yVelocity) == 4) check = false;
		}
		return check;
	}
	return false;
}	