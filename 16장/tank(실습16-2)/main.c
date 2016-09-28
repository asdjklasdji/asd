#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include "Screen.h"

#define MAP_COL   23
#define MAP_ROW   18
#define BULLET_MOVE_TIME 80

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

typedef struct _BULLET
{
	int nX, nY;
	int nLife;
	DIRECT nDirect;
	int nOldMoveTime;
} BULLET;

PLAYER g_Player;
BULLET g_sPlayerBullet[5]; 

char *g_pState[4] = { "����", "�Ʒ���", "����", "������" }; 

void Init()
{
     g_Player.nMoveTime = 100;	
     g_Player.nX = 15;
     g_Player.nY = 17;
}

void Update()
{
     int i;
     clock_t nCurTime = clock();

     for( i = 0 ; i < 5 ; i++ )
     {
		if( g_sPlayerBullet[i].nLife == 1 )
		{ 
		   // Note: �Ѿ� �̵�
		   if( nCurTime - g_sPlayerBullet[i].nOldMoveTime >= BULLET_MOVE_TIME )
		   {
			  switch( g_sPlayerBullet[i].nDirect )
			  {
				case UP :
					g_sPlayerBullet[i].nY -= 1;
					break;
				case DOWN:
					g_sPlayerBullet[i].nY += 1;
					break;
				case LEFT:
					g_sPlayerBullet[i].nX -= 1;
					break;
				case RIGHT:
					g_sPlayerBullet[i].nX += 1;
					break;
			  }		
			  g_sPlayerBullet[i].nOldMoveTime = nCurTime;
		  }			
			
		  // Note: ��� ���� �浹
		  if( g_sPlayerBullet[i].nX < 0 || g_sPlayerBullet[i].nX > MAP_COL - 1 || 
				  g_sPlayerBullet[i].nY > MAP_ROW - 1 || g_sPlayerBullet[i].nY < 0 )
		  {
				 g_sPlayerBullet[i].nLife = 0;				
		  }
       }
     }
}

void Render()
{
     char str[100];
     int i;
     ScreenClear();

     // Note: ������ �ڵ�
     ScreenPrint( g_Player.nX*2, g_Player.nY, "��" );

     // Note: ���ΰ� �Ѿ� �߻�
     for( i = 0 ; i < 5 ; i++ )
     {
		if( g_sPlayerBullet[i].nLife == 1 )
		{
		   if( g_sPlayerBullet[i].nDirect == UP || g_sPlayerBullet[i].nDirect == DOWN )
			  ScreenPrint(g_sPlayerBullet[i].nX*2, g_sPlayerBullet[i].nY, "��" );
		   else
			  ScreenPrint(g_sPlayerBullet[i].nX*2, g_sPlayerBullet[i].nY, "��" );
		}
	}
     sprintf( str, "%s ���� %d %d", g_pState[g_Player.nDirect], g_Player.nX, g_Player.nY );
     ScreenPrint( 10, 20, str );
		
     ScreenFlipping();
}

void Release()
{
}

int main(void)
{
    int nKey, i;
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
			case 72: // ����
			   if( nCurTime - g_Player.nOldMoveTime >= g_Player.nMoveTime )
			   {
				  g_Player.nDirect = UP;
				  g_Player.nY--;
				  g_Player.nOldMoveTime = nCurTime;					   }
			   break;
			case 80: // �Ʒ���
			   if( nCurTime - g_Player.nOldMoveTime >= g_Player.nMoveTime )
			   {
				  g_Player.nDirect = DOWN;
				  g_Player.nY++;
				  g_Player.nOldMoveTime = nCurTime;
						   }
			   break;
			case 75: // ����
			   if( nCurTime - g_Player.nOldMoveTime >= g_Player.nMoveTime )
			   {
				  g_Player.nDirect = LEFT;
				  g_Player.nX--;
				  g_Player.nOldMoveTime = nCurTime;
						   }
			   break;
			case 77: // ������
			   if( nCurTime - g_Player.nOldMoveTime >= g_Player.nMoveTime )
			   {
				  g_Player.nDirect = RIGHT;
				  g_Player.nX++;
				  g_Player.nOldMoveTime = nCurTime;
						   }
			   break;
			case 's' : // �����̽� 
			   if( nCurTime - g_Player.nOldFireTime >= g_Player.nFireTime )
			   {
				  for( i = 0 ; i < 5 ; i++ )
				  {
					if( g_sPlayerBullet[i].nLife == 0 )
					{
						g_sPlayerBullet[i].nLife = 1 ;
						g_sPlayerBullet[i].nDirect = g_Player.nDirect;
						// Note: �ʱ� ��ġ ����
						g_sPlayerBullet[i].nX = g_Player.nX; 
						g_sPlayerBullet[i].nY = g_Player.nY;
						// Note: ���⿡ ���� �ʱ� ��ǥ ������
						switch( g_sPlayerBullet[i].nDirect )
						{
						   case UP :
									g_sPlayerBullet[i].nY -= 1;
									break;
						   case DOWN:
									g_sPlayerBullet[i].nY += 1;
									break;
						   case LEFT:
									g_sPlayerBullet[i].nX -= 1;
									break;
						   case RIGHT:
									g_sPlayerBullet[i].nX += 1;
									break;
						}		
						g_Player.nOldFireTime = g_sPlayerBullet[i].nOldMoveTime = nCurTime;
						break;
				   }
				}
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