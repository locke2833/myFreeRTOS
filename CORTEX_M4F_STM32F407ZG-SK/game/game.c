#include "game.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32f429i_discovery_l3gd20.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <r3dfb.h>
#include <r3d.h>
#define BallNum 10
#define CircleR 30
#define DCircleR 900
extern void srand(_UINT);


TP_STATE tp_state;
uint16_t CircleX = LCD_PIXEL_WIDTH-CircleR-5;
uint16_t CircleY = CircleR + 5;
int16_t TouchLen;
static int axes[3] = {0};
//Player1
int16_t player1X = 10;
int16_t player1Y = 10;

uint16_t player1W = 10;
uint16_t player1H = 10;
uint8_t playerMoveButtonX = 0;
int16_t player1VX;
int16_t player1VY;

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
void l3gd20Init(){
	L3GD20_InitTypeDef L3GD20_InitStructure;
	L3GD20_InitStructure.Power_Mode = L3GD20_MODE_ACTIVE;
	L3GD20_InitStructure.Output_DataRate = L3GD20_OUTPUT_DATARATE_1;
	L3GD20_InitStructure.Axes_Enable = L3GD20_AXES_ENABLE;
	L3GD20_InitStructure.Band_Width = L3GD20_BANDWIDTH_4;
	L3GD20_InitStructure.BlockData_Update = L3GD20_BlockDataUpdate_Continous;
	L3GD20_InitStructure.Endianness = L3GD20_BLE_LSB;
	L3GD20_InitStructure.Full_Scale = L3GD20_FULLSCALE_250;
	L3GD20_Init(&L3GD20_InitStructure);
	L3GD20_FilterConfigTypeDef L3GD20_FilterStructure;
	L3GD20_FilterStructure.HighPassFilter_Mode_Selection = L3GD20_HPM_NORMAL_MODE_RES;
	L3GD20_FilterStructure.HighPassFilter_CutOff_Frequency = L3GD20_HPFCF_0;
	L3GD20_FilterConfig(&L3GD20_FilterStructure);
	L3GD20_FilterCmd(L3GD20_HIGHPASSFILTER_ENABLE);



}

void GameInit(){
	int i;
	LCD_SetTextColor (0xFF01);	
	LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
	for(i=0;i<ballnum;i++){
		ballIsRun[i]=0;
	}
	srand(123);
    l3gd20Init();
}
void drawBall(int ballNo){

	LCD_SetTextColor( rand()%0xFFFF );
	LCD_DrawFullRect( ballx[ballNo], bally[ballNo], ballSize, ballSize );
}
void randBallxy(int ballNo){
	
	ballx[ballNo] = ( rand() % (LCD_PIXEL_WIDTH - ballSize ) );

	bally[ballNo] = LCD_PIXEL_HEIGHT - ballSize;
   
	ballvx[ballNo] = (rand()%10)+1;
	ballvy[ballNo] = -((rand()%10)+1);

	ballIsRun[ballNo] = 1;
}
void UpdateBall(int ballNo){

	LCD_SetTextColor (0x0000);	
	LCD_DrawFullRect( ballx[ballNo], bally[ballNo], ballSize, ballSize );
		if( ballIsRun[ballNo] == 1 ){



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
		drawBall(ballNo);
}

void updatel3gd(){
	uint8_t tmp[6] = {0};
	int16_t a[3] = {0};
	uint8_t tmpreg = 0;
	L3GD20_Read(&tmpreg, L3GD20_CTRL_REG4_ADDR, 1);
	L3GD20_Read(tmp, L3GD20_OUT_X_L_ADDR, 6);
	/* check in the control register 4 the data alignment (Big Endian or Little Endian)*/
	if (!(tmpreg & 0x40)) {
		for (int i = 0; i < 3; i++)
			a[i] = (int16_t)(((uint16_t)tmp[2 * i + 1] << 8) | (uint16_t)tmp[2 * i]);
	} else {
		for (int i = 0; i < 3; i++)
			a[i] = (int16_t)(((uint16_t)tmp[2 * i] << 8) | (uint16_t)tmp[2 * i + 1]);
	}
	for(int i=0;i<3;i++){ axes[i] = a[i] / 114.285f ; } 



}
void
GAME_EventHandler1()
{
   	updatel3gd();

}

void
GAME_EventHandler2()
{
	//Player1

    Update_Player_l3gd();
	tp_state = *IOE_TP_GetState();
	if( tp_state.TouchDetected){
		player1VX = tp_state.X-CircleX;
        player1VY = tp_state.Y-CircleY;
    //    TouchLen = sqrt(player1VX*player1VX + player1VY*player1VY);
     //   player1VX = (player1VX*CircleR)/TouchLen;
      //  player1VX = (player1VY*CircleR)/TouchLen;
	
		if(player1VX > CircleR)  { player1VX = CircleR; }
        if(player1VY > CircleR)  { player1VY = CircleR; }
        if(player1VX < -CircleR)  { player1VX = -CircleR; }
        if(player1VY < -CircleR)  { player1VY = -CircleR; }
		LCD_SetTextColor (0x0000);	
		LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
		player1X+=player1VX/6;
	    player1Y+=player1VY/6;

     	if( player1X <= 0 )
			player1X = 0;
		else if( player1X + player1W >= LCD_PIXEL_WIDTH )
			player1X = LCD_PIXEL_WIDTH - player1W;

		if( player1Y <= 0 )
			player1Y = 0;
		else if( player1Y + player1H >= LCD_PIXEL_HEIGHT )
			player1Y = LCD_PIXEL_HEIGHT - player1H;
		LCD_SetTextColor (0xFF01);	
		LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
	}
}

void
GAME_EventHandler3()
{
	int i;
	for(i=0;i<ballnum;i++){
		if( ballIsRun[i] == 0 ){
			//BallReset();
//			randBallxy(i);	
		}
	}
}
void Update_Player_l3gd(){
  
		LCD_SetTextColor (0x0000);	
		LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
		player1X+=axes[1]/5;
	    player1Y+=axes[0]/5;

     	if( player1X <= 0 )
			player1X = 0;
		else if( player1X + player1W >= LCD_PIXEL_WIDTH )
			player1X = LCD_PIXEL_WIDTH - player1W;

		if( player1Y <= 0 )
			player1Y = 0;
		else if( player1Y + player1H >= LCD_PIXEL_HEIGHT )
			player1Y = LCD_PIXEL_HEIGHT - player1H;
		LCD_SetTextColor (0xFF01);	
		LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
}
void
GAME_Update()
{
	//
		
	//Ball	
	int i;
	for(i=0;i<ballnum;i++){
		UpdateBall(i);
	}
	
	

}

	void
GAME_Render()
{
	//LCD_DrawLine( 10, LCD_PIXEL_HEIGHT / 2, LCD_PIXEL_WIDTH - 20, LCD_DIR_HORIZONTAL );
	LCD_SetTextColor (0x1234);
	LCD_DrawFullCircle(CircleX,CircleY,CircleR);	

}
