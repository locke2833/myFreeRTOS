#include "game.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BallNum 10
extern void srand(_UINT);
//Player1
int16_t player1X = 10;
int16_t player1Y = 10;
uint16_t player1W = 10;
uint16_t player1H = 10;
uint8_t playerMoveButtonX = 0;

//Player2
int16_t player2X = LCD_PIXEL_WIDTH - 20;
int16_t player2Y = LCD_PIXEL_HEIGHT - 20;
uint16_t player2W = 60;
uint16_t player2H = 10;
uint8_t playerMoveButtonY = 0;

//Ball
uint16_t ballSize = 5;
int16_t ballnum = BallNum;
int16_t ballx[BallNum];
int16_t bally[BallNum];
int16_t ballvx[BallNum];
int16_t ballvy[BallNum];
int16_t ballIsRun[BallNum];
//Mode
uint8_t demoMode = 0;
void GameInit(){
	int i;
	for(i=0;i<ballnum;i++){
		ballIsRun[i]=0;
	}
	srand(123);
}
void randBallxy(int ballNo){
	
	ballx[ballNo] = ( rand() % (LCD_PIXEL_WIDTH - ballSize ) );

	bally[ballNo] = LCD_PIXEL_HEIGHT - ballSize;
   
	ballvx[ballNo] = (rand()%10)+1;
	ballvy[ballNo] = -((rand()%10)+1);

	ballIsRun[ballNo] = 1;
}
void UpdateBall(int ballNo){

		if( ballIsRun[ballNo] == 1 ){


			LCD_SetTextColor( LCD_COLOR_BLACK );
			LCD_DrawFullRect( ballx[ballNo], bally[ballNo], ballSize, ballSize );

			//Touch wall
			ballx[ballNo] += ballvx[ballNo];
			if( ballx[ballNo] <= 0 ){
				ballx[ballNo] = 0;
				ballvx[ballNo] *= -1;
			}
			else if( ballx[ballNo] + ballSize >= LCD_PIXEL_WIDTH ){
				ballx[ballNo] = LCD_PIXEL_WIDTH - ballSize;
				ballvx[ballNo] *= -1;
			}

			bally[ballNo] += ballvy[ballNo];
			if(bally[ballNo]<0){
				ballIsRun[ballNo] =0;
				randBallxy(ballNo);
			}
		}
		else{
	    	randBallxy(ballNo);		
		}
}
void
GAME_EventHandler1()
{
	if( STM_EVAL_PBGetState( BUTTON_USER ) ){

		playerMoveButtonX = 1;

		while( STM_EVAL_PBGetState( BUTTON_USER ) );

		playerMoveButtonX = 0;
	}
}

void
GAME_EventHandler2()
{
	if( IOE_TP_GetState()->TouchDetected ){

		playerMoveButtonY = 1;

		while( IOE_TP_GetState()->TouchDetected );

		playerMoveButtonY = 0;
	}
}

void
GAME_EventHandler3()
{
	int i;
	for(i=0;i<ballnum;i++){
		if( ballIsRun[i] == 0 ){
			//BallReset();
			randBallxy(i);	
		}
	}
}

void
GAME_Update()
{
	//Player1
	LCD_SetTextColor( LCD_COLOR_BLACK );
	LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
	LCD_DrawFullRect( player2X, player2Y, player2W, player2H );

	if( demoMode == 0 ){

		if( playerMoveButtonX )
			player1X += 5;
		else
			player1X -= 5;

		if( player1X <= 0 )
			player1X = 0;
		else if( player1X + player1W >= LCD_PIXEL_WIDTH )
			player1X = LCD_PIXEL_WIDTH - player1W;

		if( playerMoveButtonY )
			player1Y += 5;
		else
			player1Y -= 5;

		if( player1Y <= 0 )
			player1Y = 0;
		else if( player1Y + player1H >= LCD_PIXEL_HEIGHT )
			player1Y = LCD_PIXEL_HEIGHT - player1H;

		//Ball	
		int i;
		for(i=0;i<ballnum;i++){
			UpdateBall(i);
		}
	}
}

	void
GAME_Render()
{
	LCD_SetTextColor (0xFF01);	
	LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
	LCD_SetTextColor( rand()%0xFFFF );
	//LCD_DrawFullRect( player2X, player2Y, player2W, player2H );
	int i;
	for(i=0;i<ballnum;i++){
		LCD_DrawFullRect( ballx[i], bally[i], ballSize, ballSize );
	}
	//LCD_DrawLine( 10, LCD_PIXEL_HEIGHT / 2, LCD_PIXEL_WIDTH - 20, LCD_DIR_HORIZONTAL );
}
