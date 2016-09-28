#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include "Screen.h"

typedef enum _DIRECT { UP, DOWN, LEFT, RIGHT } DIRECT;

typedef struct _PLAYER
{
	int nX, nY;
	int nLife;
	int nMoveTime;
	int nOldMoveTime;
	int nOldFireTime;
	int nFireTime;
	DIRECT nDirect;	
} PLAYER;

PLAYER g_Player;
char *g_pState[4] = { "위쪽", "아래쪽", "왼쪽", "오른쪽" }; 

void Init()
{
	g_Player.nMoveTime = 100;	
    g_Player.nX = 15;
    g_Player.nY = 10;
}

void Update()
{
}

void Render()
{
      char str[100];
      ScreenClear();

      // Note: 렌더링 코드
      ScreenPrint( g_Player.nX*2, g_Player.nY, "◈" );
      sprintf( str, "%s 상태 %d %d", g_pState[g_Player.nDirect], g_Player.nX, g_Player.nY );
      ScreenPrint( 10, 20, str );
      
     ScreenFlipping();
}

void Release()
{
}

int main(void)
{
    int nKey;
    clock_t nCurTime;
   
    ScreenInit();
	Init();
 
    while(1)
    {
		if( _kbhit() )
		{
			nKey = _getch();
			nCurTime = clock();
	            
			switch( nKey )
			{
			case 72: // 위쪽
			   if( nCurTime - g_Player.nOldMoveTime >= g_Player.nMoveTime )
			   {
				   g_Player.nDirect = UP;
				   g_Player.nY--;
				   g_Player.nOldMoveTime = nCurTime;
						   }
			   break;
			case 80: // 아래쪽
			   if( nCurTime - g_Player.nOldMoveTime >= g_Player.nMoveTime )
			   {
				   g_Player.nDirect = DOWN;
				   g_Player.nY++;
				   g_Player.nOldMoveTime = nCurTime;
 			   }
			   break;

			 case 75: // 왼쪽
			   if( nCurTime - g_Player.nOldMoveTime >= g_Player.nMoveTime )
			   {
				   g_Player.nDirect = LEFT;
				   g_Player.nX--;
				   g_Player.nOldMoveTime = nCurTime;
			   }
			   break;

			 case 77: // 오른쪽
			   if( nCurTime - g_Player.nOldMoveTime >= g_Player.nMoveTime )
			   {
				   g_Player.nDirect = RIGHT;
				   g_Player.nX++;
				   g_Player.nOldMoveTime = nCurTime;  
			   }
			   break;
			}
		}

		Update();		
		Render();			
    }

    Release();
	ScreenRelease();	
    return 0;
}
