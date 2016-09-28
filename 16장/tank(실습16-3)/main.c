#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include "Screen.h"

#define BULLET_MOVE_TIME 100

typedef enum _DIRECT { UP, DOWN, LEFT, RIGHT } DIRECT;
typedef enum _STATE { STOP, RUN, ROTATION } STATE;

typedef struct _ENEMY
{
	int nTypeIndex;	          // 타입 인덱스
	int nPosIndex;	          // 생성 초기 위치 인덱스
	int nX, nY;               // 이동 좌표
	STATE nState;	          // 상태 전이
	int nLife;	              // 생명
	int nDirect;	          // 이동 방향
	int nMoveTime;	          // 이동 시간 간격
	int nFireTime;            // 포탄 발사 시간 간격
	int nAppearanceTime;      // 출현 시간
	int nOldTime;             // 이전 이동 시각
	int nOldFireTime;         // 포탄 발사 이전 시각
} ENEMY;

typedef struct _BULLET
{
	int nX, nY;
	int nLife;
	DIRECT nDirect;
	int nOldMoveTime;
} BULLET;

int	g_nEnemyIndex = 0;
unsigned int g_nStartTime;
BULLET g_sEnemyBullet[100];
ENEMY g_Enemy[30];

void Init()
{
    int i;
    for( i = 0 ; i < 30 ; i++ )
    {
	g_Enemy[i].nAppearanceTime = 800*(i+1);
	g_Enemy[i].nDirect = rand() % 4;
	g_Enemy[i].nFireTime = 700;
	g_Enemy[i].nMoveTime = 400;
	g_Enemy[i].nState = STOP;
	g_Enemy[i].nX = rand() % 35;
	g_Enemy[i].nY = rand() % 23;
	g_Enemy[i].nLife = 1;
    }	
}

void Update()
{
     int nCurTime, nPassTime;
     int i, j, nTemp;

     nCurTime = clock();
     nPassTime = nCurTime - g_nStartTime;

     // Note: 적 탱크 출현
     for( i = g_nEnemyIndex ; i < 30 ; i++ )
     {		
		if( g_Enemy[i].nState == STOP )
		{
		   if( g_Enemy[i].nAppearanceTime <= nPassTime )
		   {
			  g_Enemy[i].nState = RUN;
			  g_nEnemyIndex++;
		   }else{
			   break;
		   }
		}
      }

     // Note: 적 탱크 이동
     for( i = 0 ; i < g_nEnemyIndex ; i++ )
     {
		if( g_Enemy[i].nLife == 1 ) 
		{
		   switch( g_Enemy[i].nState )
		   {
		   case RUN : 
					 if( nCurTime - g_Enemy[i].nOldTime > g_Enemy[i].nMoveTime )
					 {
						switch( g_Enemy[i].nDirect )
						{
						case UP :   // 경계 영역 처리
							if( g_Enemy[i].nY - 1 < 0 )
							{	
								g_Enemy[i].nState = ROTATION;
							}else{
								g_Enemy[i].nY--;
								g_Enemy[i].nOldTime = nCurTime;
							}
							break;
						case DOWN :
							if( g_Enemy[i].nY + 1 > 23 )
							{	
	 							g_Enemy[i].nState = ROTATION;
							}else{
								g_Enemy[i].nY++;
								g_Enemy[i].nOldTime = nCurTime;
							}
							break;
						case LEFT :
							if( g_Enemy[i].nX - 1 < 1 )
							{	
							   g_Enemy[i].nState = ROTATION;
							}else{
							   g_Enemy[i].nX--;
							   g_Enemy[i].nOldTime = nCurTime;
							}
							break;
						case RIGHT:
							if( g_Enemy[i].nX + 1 > 39 )
							{	
							   g_Enemy[i].nState = ROTATION;
							}else{
							   g_Enemy[i].nX++;
							   g_Enemy[i].nOldTime = nCurTime;
							}
							break;
						}
					 }					
					 break;
			case ROTATION : 
					 while( 1 )
					 {
							nTemp = rand() % 4;
							if( nTemp != g_Enemy[i].nDirect ) //다른 방향이 나올 때까지 
							{	
							   g_Enemy[i].nDirect = nTemp;
							   g_Enemy[i].nState = RUN;
							   g_Enemy[i].nOldTime = nCurTime;
							   break;
							}
					  }
					  break;
			}
		}
     }

     // Note: 포탄 발사
     for( i = 0 ; i < g_nEnemyIndex ; i++ )
     {
		if( g_Enemy[i].nState == RUN )
		{
			if( nCurTime - g_Enemy[i].nOldFireTime > g_Enemy[i].nFireTime )
			{
			   // 100개의 총알에서 찾는 부분이다.
			   for( j = 0 ; j < 100 ; j++ )
			   {
					if( g_sEnemyBullet[j].nLife == 0 )
					{
					   g_sEnemyBullet[j].nDirect = g_Enemy[i].nDirect;
								   switch( g_Enemy[i].nDirect )
					   {
						case LEFT:
							g_sEnemyBullet[j].nX = g_Enemy[i].nX - 1;
							g_sEnemyBullet[j].nY = g_Enemy[i].nY;
							break;
						case RIGHT:
								   g_sEnemyBullet[j].nX = g_Enemy[i].nX + 1;
							 g_sEnemyBullet[j].nY = g_Enemy[i].nY;
							 break;
						case UP:
								   g_sEnemyBullet[j].nY = g_Enemy[i].nY - 1;
							 g_sEnemyBullet[j].nX = g_Enemy[i].nX;
							 break;
						case DOWN:
							 g_sEnemyBullet[j].nY = g_Enemy[i].nY + 1;
							 g_sEnemyBullet[j].nX = g_Enemy[i].nX;
							 break;
					   }
					   g_sEnemyBullet[j].nLife = 1;
					   g_sEnemyBullet[j].nOldMoveTime = nCurTime;
					   g_Enemy[i].nOldFireTime = nCurTime;	
					   break;
					}
			   }
			}
		}
     }

     // Note: 포탄 이동
     for( i = 0 ; i < 100 ; i++ )
     {
		if( g_sEnemyBullet[i].nLife == 1 )
		{	// 포탄의 이동 시간을 120으로 고정
			if( nCurTime - g_sEnemyBullet[i].nOldMoveTime > 120 ) 
			{
				   switch( g_sEnemyBullet[i].nDirect )
				   {
				   case UP:
					   if( g_sEnemyBullet[i].nY - 1 < 0 )
					   {
							g_sEnemyBullet[i].nLife = 0;
							continue;
					   }else{
							g_sEnemyBullet[i].nY--;
					   }
					   break;
					case DOWN:
					   if( g_sEnemyBullet[i].nY + 1 > 23 )
					   {
							g_sEnemyBullet[i].nLife = 0;
							continue;
					   }else{
							g_sEnemyBullet[i].nY++;
					   }
					   break;
					case LEFT:
					if( g_sEnemyBullet[i].nX - 1 < 1 )
					{
						g_sEnemyBullet[i].nLife = 0;
						continue;
					}else{
						g_sEnemyBullet[i].nX--;
					}
					break;
				case RIGHT:
					if( g_sEnemyBullet[i].nX + 1 > 39 )
					{
					   g_sEnemyBullet[i].nLife = 0;
					   continue;
					}else{
					   g_sEnemyBullet[i].nX++;
					}
					break;
				}
				g_sEnemyBullet[i].nOldMoveTime = nCurTime;
			}
		}
     }
}

void Render()
{
     int i;
     
     ScreenClear();
	
      // Note: 렌더링 부분 & 적 탱크 총알 발사
     for( i = 0 ;  i < 100 ; i++ )
     {
		 if( g_sEnemyBullet[i].nLife == 1 )
		 {
			 switch( g_sEnemyBullet[i].nDirect )
			 {
			  case UP :
						ScreenPrint( g_sEnemyBullet[i].nX*2, g_sEnemyBullet[i].nY, "↑" );
						break;
			  case DOWN :
						ScreenPrint( g_sEnemyBullet[i].nX*2, g_sEnemyBullet[i].nY, "↓" );	
						break;
			  case LEFT:
						ScreenPrint( g_sEnemyBullet[i].nX*2, g_sEnemyBullet[i].nY, "←" );		              
						break;
			  case RIGHT:
						ScreenPrint( g_sEnemyBullet[i].nX*2, g_sEnemyBullet[i].nY, "→" );	
						break;
			 }
		 }
      } 

      for( i = 0 ; i < g_nEnemyIndex ; i++ )
      {
		  if( g_Enemy[i].nState != STOP )
		  {
			  ScreenPrint( g_Enemy[i].nX*2, g_Enemy[i].nY, "★" );			
		  }
      }	
      ScreenFlipping();
}

void Release()
{
}

int main(void)
{ 
    ScreenInit();
    Init();
    
    g_nStartTime = clock();

    while( 1 )
    {		
 		Update();		
		Render();	        
    }
    Release();
    ScreenRelease();
    return 0;
}
