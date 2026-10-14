#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define W 256
#define H 192
#define ROAD_L 47
#define ROAD_R 209
#define C(r,g,b) RGB15(r,g,b)
static u16 *screen;
static PrintConsole lower;
static int px=128, speed=2, score=0, lives=3, ticks=0, paused=0, over=0;
static int enemyX[4]={75,170,112,190};
static int enemyY[4]={-20,-90,-160,-230};
static int enemyColor[4]={0,1,2,3};
static unsigned int seed=2463534242u;
static unsigned int rnd(void){seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;}
static void rect(int x,int y,int w,int h,u16 color){
  int x0=x<0?0:x,y0=y<0?0:y,x1=x+w>W?W:x+w,y1=y+h>H?H:y+h;
  if(x1<=x0||y1<=y0)return;
  for(int j=y0;j<y1;j++)for(int i=x0;i<x1;i++)screen[j*256+i]=color;
}
static void car(int x,int y,u16 body,int player){
  rect(x-9,y-14,18,28,C(3,3,5));
  rect(x-7,y-12,14,24,body);
  rect(x-5,y-7,10,8,C(7,15,19));
  rect(x-12,y-10,4,8,C(1,1,2));rect(x+8,y-10,4,8,C(1,1,2));
  rect(x-12,y+7,4,8,C(1,1,2));rect(x+8,y+7,4,8,C(1,1,2));
  rect(x-11,y-13,22,3,player?C(29,25,7):C(24,24,25));
  rect(x-10,y+12,20,3,C(2,2,3));
}
static void reset(void){px=128;speed=2;score=0;lives=3;ticks=0;paused=0;over=0;
  for(int i=0;i<4;i++){enemyY[i]=-40-i*58;enemyX[i]=70+(rnd()%115);}}
static void draw(void){
  rect(0,0,W,H,C(9,17,15));
  rect(ROAD_L-5,0,ROAD_R-ROAD_L+10,H,C(22,20,15));
  rect(ROAD_L,0,ROAD_R-ROAD_L,H,C(8,10,13));
  rect(ROAD_L+3,0,2,H,C(27,27,22));rect(ROAD_R-5,0,2,H,C(27,27,22));
  for(int y=((ticks*speed*2)%44)-44;y<H;y+=44){
    rect(ROAD_L+54,y,3,22,C(19,19,17));rect(ROAD_L+107,y,3,22,C(19,19,17));
  }
  for(int y=((ticks*speed)%32)-32;y<H;y+=32){
    rect(ROAD_L-5,y,5,16,C(24,6,5));rect(ROAD_R,y,5,16,C(24,6,5));
  }
  const u16 colors[4]={C(27,8,6),C(6,15,28),C(27,21,5),C(18,8,24)};
  for(int i=0;i<4;i++)if(enemyY[i]>-18&&enemyY[i]<H+18)car(enemyX[i],enemyY[i],colors[enemyColor[i]],0);
  car(px,158,C(6,25,20),1);
  if(paused||over){
    rect(40,67,176,54,C(2,5,7));
    rect(43,70,170,48,over?C(14,5,6):C(5,13,15));
    // Text status appears on the lower screen to avoid font assets.
  }
}
static void hud(void){
  consoleSelect(&lower);consoleClear();
  iprintf("      FORMULA RETRO\n");
  iprintf("  ------------------------\n\n");
  iprintf("  SCORE   %08d\n\n",score);
  iprintf("  SPEED   %d / 8\n\n",speed);
  iprintf("  LIVES   %d\n\n",lives);
  if(over)iprintf("  GAME OVER - START RESTART\n");
  else if(paused)iprintf("  PAUSED - START RESUME\n");
  else iprintf("  OVERTAKE THE TRAFFIC!\n");
  iprintf("\n  D-PAD : STEER\n  A/B   : SPEED UP / DOWN\n  START : PAUSE / RESTART\n");
}
int main(void){
  powerOn(POWER_ALL_2D);
  videoSetMode(MODE_5_2D);
  vramSetBankA(VRAM_A_MAIN_BG);
  int bg=bgInit(3,BgType_Bmp16,BgSize_B16_256x256,0,0);
  screen=(u16*)bgGetGfxPtr(bg);
  videoSetModeSub(MODE_0_2D);
  vramSetBankC(VRAM_C_SUB_BG);
  consoleInit(&lower,3,BgType_Text4bpp,BgSize_T_256x256,31,0,false,true);
  reset();int lastHud=-1;
  while(1){
    scanKeys();int down=keysDown(), held=keysHeld();
    if(down&KEY_START){if(over)reset();else paused=!paused;}
    if(!paused&&!over){
      if((held&KEY_LEFT)&&px>ROAD_L+14)px-=3;
      if((held&KEY_RIGHT)&&px<ROAD_R-14)px+=3;
      if((down&KEY_A)&&speed<8)speed++;
      if((down&KEY_B)&&speed>1)speed--;
      ticks++;score+=speed;
      for(int i=0;i<4;i++){
        enemyY[i]+=speed+1;
        if(enemyY[i]>H+20){enemyY[i]=-35-(rnd()%110);enemyX[i]=ROAD_L+19+(rnd()%(ROAD_R-ROAD_L-38));score+=100;}
        if(abs(enemyY[i]-158)<24&&abs(enemyX[i]-px)<19){
          lives--;enemyY[i]=-65-(rnd()%90);enemyX[i]=ROAD_L+19+(rnd()%(ROAD_R-ROAD_L-38));
          if(lives<=0)over=1;
        }
      }
    }
    draw();
    if(ticks/8!=lastHud||down||paused||over){hud();lastHud=ticks/8;}
    swiWaitForVBlank();
  }
  return 0;
}
