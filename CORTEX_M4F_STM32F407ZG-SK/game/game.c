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
uint8_t player1IsReversed = 1;

//Player2
int16_t player2X = LCD_PIXEL_WIDTH - 20;
int16_t player2Y = LCD_PIXEL_HEIGHT - 20;
uint16_t player2W = 60;
uint16_t player2H = 10;
uint8_t player2IsReversed = 0;

//Ball
uint16_t ballSize = 5;
/*int16_t ballx[ballNo] = ( LCD_PIXEL_WIDTH - 5 ) / 2;
int16_t bally[ballNo] = ( LCD_PIXEL_HEIGHT - 5 ) / 2;
int16_t ballvx[ballNo] = 5;
int16_t ballvy[ballNo] = 5;
uint8_t ballIsRun = 0;
*/
//balls
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
//    ballx[ballNo] = 0;    

	bally[ballNo] = LCD_PIXEL_HEIGHT - ballSize;
   
//	ballvx[ballNo] = 5;
//    ballvy[ballNo] = 5;
	ballvx[ballNo] = (rand()%10)+1;
	ballvy[ballNo] = -((rand()%10)+1);

	ballIsRun[ballNo] = 1;
}
/*
void
BallReset()
{
	ballx[ballNo] = ( LCD_PIXEL_WIDTH - 5 ) / 2;
	bally[ballNo] = ( LCD_PIXEL_HEIGHT - 5 ) / 2;

	ballvx[ballNo] = 5;
	ballvy[ballNo] = 5;

	ballIsRun = 1;
}*/
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
			//PONG!
			/*if( bally[ballNo] + ballSize >= player2Y ){
				if( ballx[ballNo] + ballSize >= player2X && ballx[ballNo] <= player2X + player2W ){
					if( ballx[ballNo] - ballSize <= player2Y + player2W/4 ){
						ballvy[ballNo] =-3;
						ballvx[ballNo] =-7;
					}
					else if( ballx[ballNo] >= player2Y + player2W - player2W/4 ){
						ballvy[ballNo] =-3;
						ballvx[ballNo] = 7;
					}
					else if( ballx[ballNo] + ballSize < player2Y + player2W/2 ){
						ballvy[ballNo] =-7;
						ballvx[ballNo] =-3;
					}
					else if( ballx[ballNo] > player2Y + player2W/2 ){
						ballvy[ballNo] =-7;
						ballvx[ballNo] = 3;
					}
					else{
						ballvy[ballNo] =-9;
						ballvx[ballNo] = 0;
					}
				}
				else
					BallReset();
			}

			if( bally[ballNo] <= player1Y + player1H ){
					if( ballx[ballNo] + ballSize >= player1X && ballx[ballNo] <= player1X + player1W ){
						if( ballx[ballNo] - ballSize <= player1Y + player1W/4 ){
							ballvy[ballNo] = 3;
							ballvx[ballNo] =-7;
						}
						else if( ballx[ballNo] >= player1Y + player1W - player1W/4 ){
							ballvy[ballNo] = 3;
							ballvx[ballNo] = 7;
						}
						else if( ballx[ballNo] + ballSize < player1Y + player1W/2 ){
							ballvy[ballNo] = 7;
							ballvx[ballNo] =-3;
						}
						else if( ballx[ballNo] > player1Y + player1W/2 ){
							ballvy[ballNo] = 7;
							ballvx[ballNo] = 3;
						}
						else{
							ballvy[ballNo] = 9;
							ballvx[ballNo] = 0;
						}
					}
					else
						BallReset();
			}
		}*/
}
void
GAME_EventHandler1()
{
	if( STM_EVAL_PBGetState( BUTTON_USER ) ){

		player1IsReversed = 0;

		while( STM_EVAL_PBGetState( BUTTON_USER ) );

		player1IsReversed = 1;
	}
}

void
GAME_EventHandler2()
{
	if( IOE_TP_GetState()->TouchDetected ){

		player2IsReversed = 1;

		while( IOE_TP_GetState()->TouchDetected );

		player2IsReversed = 0;
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

		if( player1IsReversed )
			player1X -= 5;
		else
			player1X += 5;

		if( player1X <= 0 )
			player1X = 0;
		else if( player1X + player1W >= LCD_PIXEL_WIDTH )
			player1X = LCD_PIXEL_WIDTH - player1W;

		//Player2
		if( player2IsReversed )
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
	/*else{	//if demoMode == 1

			//Player1 move
			if( ballvy[ballNo] < 0 ){
				if( player1X + player1W/2 < ballx[ballNo] + ballSize/2 ){
					player1X += 8;
					player2X += 2;
				}
				else{
					player1X -= 8;
					player2X -= 2;
				}
			}

			//Player2 move
			if( ballvy[ballNo] > 0 ){
				if( player2X + player2W/2 < ballx[ballNo] + ballSize/2 ){
					player1X += 2;
					player2X += 8;
				}
				else{
					player1X -= 2;
					player2X -= 8;
				}

			}

			if( player1X <= 0 )
				player1X = 0;
			else if( player1X + player1W >= LCD_PIXEL_WIDTH )
				player1X = LCD_PIXEL_WIDTH - player1W;

			if( player2X <= 0 )
				player2X = 0;
			else if( player2X + player2W >= LCD_PIXEL_WIDTH )
				player2X = LCD_PIXEL_WIDTH - player2W;


			//Ball
			if( ballIsRun == 1 ){

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

				//PONG!
				bally[ballNo] += ballvy[ballNo];
				if( bally[ballNo] + ballSize >= player2Y ){
					if( ballx[ballNo] + ballSize >= player2X && ballx[ballNo] <= player2X + player2W ){
					if( ballx[ballNo] - ballSize <= player2Y + player2W/4 ){
						ballvy[ballNo] =-3;
						ballvx[ballNo] =-7;
					}
					else if( ballx[ballNo] >= player2Y + player2W - player2W/4 ){
						ballvy[ballNo] =-3;
						ballvx[ballNo] = 7;
					}
					else if( ballx[ballNo] + ballSize < player2Y + player2W/2 ){
						ballvy[ballNo] =-7;
						ballvx[ballNo] =-3;
					}
					else if( ballx[ballNo] > player2Y + player2W/2 ){
						ballvy[ballNo] =-7;
						ballvx[ballNo] = 3;
					}
					else{
						ballvy[ballNo] =-9;
						ballvx[ballNo] = 0;
					}
				}
				else
					BallReset();
			}

			if( bally[ballNo] <= player1Y + player1H ){
				if( ballx[ballNo] + ballSize >= player1X && ballx[ballNo] <= player1X + player1W ){
					if( ballx[ballNo] - ballSize <= player1Y + player1W/4 ){
						ballvy[ballNo] = 3;
						ballvx[ballNo] =-7;
					}
					else if( ballx[ballNo] >= player1Y + player1W - player1W/4 ){
						ballvy[ballNo] = 3;
						ballvx[ballNo] = 7;
					}
					else if( ballx[ballNo] + ballSize < player1Y + player1W/2 ){
						ballvy[ballNo] = 7;
						ballvx[ballNo] =-3;
					}
					else if( ballx[ballNo] > player1Y + player1W/2 ){
						ballvy[ballNo] = 7;
						ballvx[ballNo] = 3;
					}
					else{
						ballvy[ballNo] = 9;
						ballvx[ballNo] = 0;
					}
				}
				else
					BallReset();
			}
		}
	}*/
}

	void
GAME_Render()
{
	LCD_SetTextColor( LCD_COLOR_WHITE );
	LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
	//LCD_DrawFullRect( player2X, player2Y, player2W, player2H );
	int i;
	for(i=0;i<ballnum;i++){
		LCD_DrawFullRect( ballx[i], bally[i], ballSize, ballSize );
	}
	//LCD_DrawLine( 10, LCD_PIXEL_HEIGHT / 2, LCD_PIXEL_WIDTH - 20, LCD_DIR_HORIZONTAL );
}
