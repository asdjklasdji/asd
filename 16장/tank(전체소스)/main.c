#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <conio.h>
#include <fmod.h>
#include <time.h>
#include "Screen.h"

#define MAP_COL   23
#define MAP_ROW   18

#define BULLET_MOVE_TIME 100

typedef enum _DIRECT { UP, DOWN, LEFT, RIGHT } DIRECT;
typedef enum _STATE { ENEMY_STOP, ENEMY_RUN, ENEMY_ROTATION } STATE;
typedef enum _GAME_STATE { INIT, READY, RUNNING, SUCCESS, FAILED, STOP, RESULT } GAME_STATE;
typedef struct _POS_XY
{
	int nX, nY;
}POS_XY;

typedef struct _STAGE
{
	int nEnemyCount;				// �� ĳ���� ��
	int nEnemyPositionCount;		// �� ���� ��ġ ����
	POS_XY *pEnemyPosition;			// �� ���� ��ġ �迭
	int nBossX, nBossY;				// ���� �ʱ� ��ġ
	int nPlayerX, nPlayerY;			// �÷��̾� �ʱ� ��ġ
	int nEnemyTypeCount;			// �� ĳ���� ���� ����
} STAGE;

typedef struct _ENEMY_TYPE
{	
	int	nLife;		// �����
	int nMoveTime;	// �ӷ�
	int nFireTime;  // �̻��� �߻� �ð� ����	
} ENEMY_TYPE;

typedef struct _ENEMY
{
	int nTypeIndex;	 // Ÿ�� �ε���
	int nPosIndex;	 // ���� �ʱ� ��ġ �ε���
	int nX, nY;      // �̵� ��ǥ
	STATE nState;	 // ���� ����
	int nLife;		 // �����
	int nDirect;	 // ����
	int nMoveTime;	// �̵� �ð�
	int nFireTime;  // �̻��� �߻� �ð� ����
	int nAppearanceTime; // ���� �ð�
	int nOldTime; // ���� �̵� �ð�
	int nOldFireTime;  // �̻��� �߻� ���� �ð� ����
} ENEMY;

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

typedef struct _BOSS
{
	int nX, nY;
	int nLife;
} BOSS;
	
PLAYER g_Player;
ENEMY_TYPE g_EnemyType[10]; // ��ũ ������ �ִ� 10������ ���� �Ѵ�.
ENEMY g_Enemy[30];
BOSS g_Boss;
STAGE g_Stage;

int g_nMap[MAP_ROW][MAP_COL];
char g_Title[6][3] = { "  ", "��", "��", "��", "��", "��" };
char *g_MapFileName[3] = { "map1.txt", "map1.txt", "map1.txt" };
GAME_STATE	g_nGameState = INIT;
BULLET g_sPlayerBullet[5]; 
BULLET g_sEnemyBullet[100];

int		g_nEnemyIndex = 0;
int     g_nDeadEnemy;
int		g_nStage = -1;
int		g_nGrade = 0; // ���� 
int		g_nTotalGrade = 0; // ����
clock_t	g_GameStartTime, g_UpdateOldTime, g_RemainTime;

// Note: ���� ����
FMOD_SYSTEM *g_System;
FMOD_SOUND  *g_Sound[7];
FMOD_CHANNEL *g_Channel[7];

void Box()
{
	int i;

	ScreenPrint(0, 0,  "������������������������������������������������������������");
	for( i = 1 ; i <= 19 ; i++ )
		ScreenPrint(0, i,  "��                                                        ��");
	ScreenPrint(0, 20, "������������������������������������������������������������");	
}

void InitScreen()
{
	ScreenPrint(0, 0,  "������������������������������������������������������������");
	ScreenPrint(0, 1,  "��                                                        ��");
	ScreenPrint(0, 2,  "��         -----                                          ��");
	ScreenPrint(0, 3,  "��        /    |                                       /  ��");
	ScreenPrint(0, 4,  "��       /       |    ��ũ ����                       /   ��");
	ScreenPrint(0, 5,  "��      /          |                            /-----/   ��");
	ScreenPrint(0, 6,  "��     /            |---------------|         /           ��");
	ScreenPrint(0, 7,  "��    /                               |      /            ��");
	ScreenPrint(0, 8,  "��   /              ��������������      |   /             ��");
	ScreenPrint(0, 9,  "��  /               ��          ��       |/               ��");
	ScreenPrint(0, 10, "��/                 ��    ��    ��                        ��");
	ScreenPrint(0, 11, "��     ( (    ��������������������������     ) )          ��");
	ScreenPrint(0, 12, "��            ��                      ��                  ��");
	ScreenPrint(0, 13, "��           �� ��                   �� ��                ��");
	ScreenPrint(0, 14, "��           �� �� �������������������� ��                ��");
	ScreenPrint(0, 15, "��           �� ��                   �� ��                ��");
	ScreenPrint(0, 16, "��  ~ ^ ~ $~ ~ ^ ~ ~ ~ ^ ~ ~ ~ ^ ~ ~ ~ ^~ ~ ~ ~ ~ $ ~    ~��");
	ScreenPrint(0, 17, "��                                                        ��");
	ScreenPrint(0, 18, "��                  Press Space Key !!                    ��");
	ScreenPrint(0, 19, "��                                                        ��");
	ScreenPrint(0, 20, "������������������������������������������������������������");	
}

void ReadyScreen()
{
	char string[100];

	Box();
	sprintf( string, "%d ��������", g_nStage+1 );
	ScreenPrint( 25, 9, string );
}

void SuccessScreen()
{	
	ScreenPrint(0, 0,  "������������������������������������������������������������");
	ScreenPrint(0, 1,  "��                                                        ��");
	ScreenPrint(0, 2,  "��                                                        ��");
	ScreenPrint(0, 3,  "��                                                        ��");
	ScreenPrint(0, 4,  "��                                                        ��");
	ScreenPrint(0, 5,  "��                                                        ��");
	ScreenPrint(0, 6,  "��                                                        ��");
	ScreenPrint(0, 7,  "��                                                        ��");
	ScreenPrint(0, 8,  "��                     ////����                           ��");
	ScreenPrint(0, 9,  "��                    q ��. �� p (^)@                     ��");
	ScreenPrint(0, 10, "��                    (��������) //                       ��");
	ScreenPrint(0, 11, "��                   �� �̼� ���� ��                      ��");
	ScreenPrint(0, 12, "��                                                        ��");
	ScreenPrint(0, 13, "��                                                        ��");
	ScreenPrint(0, 14, "��                                                        ��");
	ScreenPrint(0, 15, "��                                                        ��");
	ScreenPrint(0, 16, "��                                                        ��");
	ScreenPrint(0, 17, "��                                                        ��");
	ScreenPrint(0, 18, "��                                                        ��");
	ScreenPrint(0, 19, "��                                                        ��");
	ScreenPrint(0, 20, "������������������������������������������������������������");
}

void FailureScreen()
{	
	ScreenPrint(0, 0, "������������������������������������������������������������");
	ScreenPrint(0, 1, "��                                                        ��");
	ScreenPrint(0, 2, "��                                                        ��");
	ScreenPrint(0, 3, "��                                                        ��");
	ScreenPrint(0, 4, "��                                                        ��");
	ScreenPrint(0, 5, "��                                                        ��");
	ScreenPrint(0, 6, "��                                                        ��");
	ScreenPrint(0, 7, "��                    �̼� ���� !!!!                      ��");
	ScreenPrint(0, 8, "��                                                        ��");
	ScreenPrint(0, 9, "��                                                        ��");
	ScreenPrint(0, 10,"��                               �ܦ�����                 ��");
	ScreenPrint(0, 11,"��                                 ��  ��                 ��");
	ScreenPrint(0, 12,"��                                �����                ��");
	ScreenPrint(0, 13,"��                                                        ��");
	ScreenPrint(0, 14,"��                                                        ��");
	ScreenPrint(0, 15,"��                                                        ��");
	ScreenPrint(0, 16,"��                ��� �Ͻðڽ��ϱ�? (y/n)                ��");
	ScreenPrint(0, 17,"��                                                        ��");
	ScreenPrint(0, 18,"��                                                        ��");
	ScreenPrint(0, 19,"��                                                        ��");
	ScreenPrint(0, 20,"������������������������������������������������������������");	
}


void ResultScreen()
{
	char string[100];

	Box();

	ScreenPrint( 3,  3, "                     ��ũ ���� ��" );	
	ScreenPrint( 3,  6, "                    ��������������" );
	ScreenPrint( 3,  7, "               ~~   ��          ��" );
	ScreenPrint( 3,  8, "        ~~          ��    ��    ��" );
	ScreenPrint( 3,  9, "    ��         ��������������������������" );
	ScreenPrint( 3, 10, "  ������       ��                      ��" );
	ScreenPrint( 3, 11, "   ����      �� ��                   �� ��" );
	ScreenPrint( 3, 12, "             �� �� �������������������� ��" );
	ScreenPrint( 3, 13, "             �� ��                   �� ��" );
	ScreenPrint( 3, 14, "~ ~ ^ ~ $~ ~ ^ ~ ~ ~ ^ ~ ~ ~ ^ ~ ~ ~ ^~ ~ ~ ~ /~ $ ~~" );
	sprintf( string, "Score : %d", g_nTotalGrade ); 
	ScreenPrint( 39, 7, string );	
}

void SoundInit()
{
	FMOD_System_Create( &g_System );
	FMOD_System_Init( g_System, 32, FMOD_INIT_NORMAL, NULL );

	// loop sound
	FMOD_System_CreateSound( g_System, "intro.MP3", FMOD_LOOP_NORMAL, 0, &g_Sound[0] ); // �Ұ� 
	FMOD_System_CreateSound( g_System, "running.ogg", FMOD_LOOP_NORMAL, 0, &g_Sound[1] ); // ���� ���� ��
	FMOD_System_CreateSound( g_System, "failed.wav", FMOD_LOOP_NORMAL, 0, &g_Sound[2] ); // ����
	
	// effect sound 
	FMOD_System_CreateSound( g_System, "ready.wav",	FMOD_DEFAULT, 0, &g_Sound[3] ); // �غ�
	FMOD_System_CreateSound( g_System, "fire.wav", FMOD_DEFAULT, 0, &g_Sound[4] ); // ���� �߻� 
	FMOD_System_CreateSound( g_System, "collision.wav", FMOD_DEFAULT, 0, &g_Sound[5] ); // �浹 
	FMOD_System_CreateSound( g_System, "success.wav", FMOD_DEFAULT, 0, &g_Sound[6] ); // �浹 	
}

void SoundRelease()
{
	int i;
	for( i = 0 ; i < 6 ; i++ )
		FMOD_Sound_Release( g_Sound[i] );

	FMOD_System_Close( g_System );
	FMOD_System_Release( g_System );
}

void Init()
{

	FILE* fp;
	int i, j;

	if( g_nStage == -1 ) // 1���� �ʱ�ȭ�� �Ǵ� �κп� ���� ó�� ����
	{		
		SoundInit();		
		g_nStage = 0;		
		FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[0], 0, &g_Channel[0] );			
	}

	fp = fopen( g_MapFileName[g_nStage], "rt" );
	fscanf( fp, "�� ĳ���ͼ� : %d\n", &g_Stage.nEnemyCount );
	fscanf( fp, "�� ĳ���Ͱ� ������ ��ġ ���� : %d\n", &g_Stage.nEnemyPositionCount );
			
	if( g_Stage.pEnemyPosition != NULL )
		free( g_Stage.pEnemyPosition );

	g_Stage.pEnemyPosition = (POS_XY*)malloc( sizeof( POS_XY )*g_Stage.nEnemyPositionCount );
	for( i = 0 ; i < g_Stage.nEnemyPositionCount ; i++ )
	{
		fscanf( fp, "%d %d\n", &g_Stage.pEnemyPosition[i].nX, &g_Stage.pEnemyPosition[i].nY );		
	}
	fscanf( fp, "���� �ʱ� ��ġ : %d %d\n", &g_Stage.nBossX, &g_Stage.nBossY );	
	fscanf( fp, "�÷��̾� �ʱ� ��ġ : %d %d\n", &g_Stage.nPlayerX , &g_Stage.nPlayerY );	
	fscanf( fp, "�� ĳ���� ���� ���� : %d\n", &g_Stage.nEnemyTypeCount );	

	for( i = 0 ; i < g_Stage.nEnemyTypeCount ; i++ )
		fscanf( fp, "�����: %d �̵��ð� ����: %d �̻���: %d\n", &g_EnemyType[i].nLife, &g_EnemyType[i].nMoveTime, &g_EnemyType[i].nFireTime );

	for( i = 0 ; i < g_Stage.nEnemyCount ; i++ )
		fscanf( fp, "Ÿ���ε���: %d ��ĳ���� ���� �ε���: %d ���� �ð�: %d\n", &g_Enemy[i].nTypeIndex, &g_Enemy[i].nPosIndex, &g_Enemy[i].nAppearanceTime );

	for( i = 0 ; i < MAP_ROW ; i++ )
	{
		for( j = 0 ; j < MAP_COL ; j++ )
		{
			fscanf( fp, "%d ", &g_nMap[i][j] );			
		}
	}
	
	fclose( fp );

	// Note: ���� ����
	g_Boss.nX = g_Stage.nBossX;
	g_Boss.nY = g_Stage.nBossY;
	g_Boss.nLife = 2;

	// Note: ���ΰ� ����
	g_Player.nOldMoveTime = clock();
	g_Player.nOldFireTime = clock();
	g_Player.nMoveTime = 200;
	g_Player.nX = g_Stage.nPlayerX;
	g_Player.nY = g_Stage.nPlayerY;
	g_Player.nLife = 3;

	// Note: �̻��� �ʱ�ȭ
	memset( g_sPlayerBullet, 0, sizeof( g_sPlayerBullet ) ); 
	memset( g_sEnemyBullet, 0, sizeof( g_sEnemyBullet ) );
	srand( (unsigned int)time(NULL) );

	for( i = 0 ; i < g_Stage.nEnemyCount ; i++ )
	{
		g_Enemy[i].nLife = g_EnemyType[ g_Enemy[i].nTypeIndex ].nLife;
		g_Enemy[i].nX = g_Stage.pEnemyPosition[ g_Enemy[i].nPosIndex ].nX;
		g_Enemy[i].nY = g_Stage.pEnemyPosition[ g_Enemy[i].nPosIndex ].nY;
		g_Enemy[i].nMoveTime =  g_EnemyType[ g_Enemy[i].nTypeIndex ].nMoveTime;
		g_Enemy[i].nFireTime =  g_EnemyType[ g_Enemy[i].nTypeIndex ].nFireTime;
		g_Enemy[i].nOldTime = clock();
		g_Enemy[i].nDirect = rand() % 4; // Note: ������ ���ǰ� ����
		g_Enemy[i].nState = ENEMY_STOP;
	}		

	g_nEnemyIndex = 0;
	g_nGrade = 0;	// ������������ ����
	g_nDeadEnemy = 0;	// �� ĳ���� ���� ����	
	g_UpdateOldTime = clock();
}

// Note: ���ΰ� �̻��ϰ� �浹 üũ
int IsPlayerBulletCollision( int nX, int nY ) 
{
	int i ;

	for( i = 0 ; i < 5 ; i++ )
	{
		if( g_sPlayerBullet[i].nLife == 1 )
		{
			if( g_sPlayerBullet[i].nX == nX && g_sPlayerBullet[i].nY == nY ) 
			{
				g_sPlayerBullet[i].nLife = 0;
				return 1; // �浹			
			}
		}
	}

	return 0;  // ���浹
}
//---------------------------------------------------------------------------
// �� ĳ���Ϳ� �浹 üũ                                               
// in :  nX, nY                                                                   
// out : �浹�� �� ĳ���� �ε���
// return : �浹 1 , ���浹 0
//---------------------------------------------------------------------------                     
int IsEnemyCollision( int nX, int nY, int *nIndex ) 
{
	int i;

	for( i = 0 ; i < g_nEnemyIndex ; i++ )
	{
		if( g_Enemy[i].nLife > 0 )
		{
			if( g_Enemy[i].nX == nX && g_Enemy[i].nY == nY )
			{				
				*nIndex = i;
			    return 1; // �浹
			} 
		}
	}

	return 0; // ���浹
}

// Note: ���ΰ��� �浹 üũ
int IsPalyerCollision( int nX, int nY )
{
	if( g_Player.nX == nX && g_Player.nY == nY )
		return 1;
	else
		return 0;	
}

// Note: �� �̻��ϰ� �浹 üũ
int IsEnemyBulletCollision( int nX, int nY ) // ���ΰ�, ���ΰ� �̻���  
{
	int i ;

	for( i = 0 ; i < 100 ; i++ )
	{
		if( g_sEnemyBullet[i].nLife == 1 )
		{
			if( g_sEnemyBullet[i].nX == nX && g_sEnemyBullet[i].nY == nY )
			{
				g_sEnemyBullet[i].nLife = 0;
				return 1;  // �浹			
			}
		}
	}

	return 0; //�� �浹 
}

void Update()
{
	int i, j, nTemp, nIndex;
	clock_t CurTime, PassTime;

	CurTime = clock();
	switch( g_nGameState )
	{
	case READY :
				if( CurTime - g_UpdateOldTime > 3000 )  // 2��
				{					
					g_nGameState = RUNNING;
					g_GameStartTime = CurTime;	
					FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[1], 0, &g_Channel[1] ); // ���� ���� ����
				}				
				break;
	case RUNNING:
				// Note: �� ĳ���� ����
				for( i = g_nEnemyIndex ; i < g_Stage.nEnemyCount ; i++ )
				{		
					if( g_Enemy[i].nState == ENEMY_STOP )
					{
						if( CurTime - g_GameStartTime >= g_Enemy[i].nAppearanceTime )
						{
							g_Enemy[i].nState = ENEMY_RUN;
							g_Enemy[i].nOldTime = CurTime;
							g_nEnemyIndex++;
						}else{
							break;
						}
					}
				}

				//------------------ ���ΰ� �̻��� -------------------------------//
				for( i = 0 ; i < 5 ; i++ )
				{
					if( g_sPlayerBullet[i].nLife )
					{
						// Note: �̻��� �̵�
						if( CurTime - g_sPlayerBullet[i].nOldMoveTime >= BULLET_MOVE_TIME )
						{
							g_sPlayerBullet[i].nOldMoveTime = CurTime;

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
						}
						
						//------------------------------------------------ �浹 -----------------------------------------------------------//
						// Note: ��� ���� �浹
						if( g_sPlayerBullet[i].nY > MAP_ROW - 1 || g_sPlayerBullet[i].nY < 0 )
						{
							g_sPlayerBullet[i].nLife = 0;
							continue;
						}

						// ���ΰ� �̻��ϰ� �� �̻��� �浹			
						if( IsEnemyBulletCollision( g_sPlayerBullet[i].nX, g_sPlayerBullet[i].nY ) == 1 )
						{
							g_sPlayerBullet[i].nLife = 0;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );
							continue;
						}

						// ���ΰ� �̻��ϰ� �� ĳ������ �浹
						if( IsEnemyCollision( g_sPlayerBullet[i].nX, g_sPlayerBullet[i].nY, &nIndex ) == 1 )
						{
							g_sPlayerBullet[i].nLife = 0;
							g_Enemy[nIndex].nLife--;
							if( g_Enemy[nIndex].nLife == 0 ) // �Ҹ� ����
							{
								g_Enemy[nIndex].nState = ENEMY_STOP;
								g_nGrade += 10; // ���� ����
								g_nDeadEnemy++;
							}

							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );

							if( g_nDeadEnemy == g_Stage.nEnemyCount )
							{	// Note: ���� ����
								g_nGameState = STOP;														
								return ;
							}
							continue;
						}


						// ��ȣ�� �浹
						if( g_nMap[g_sPlayerBullet[i].nY][g_sPlayerBullet[i].nX] == 2 )
						{
							g_sPlayerBullet[i].nLife = 0;
							continue;
						}

						// ��� �浹
						if( g_nMap[g_sPlayerBullet[i].nY][g_sPlayerBullet[i].nX] == 1 )
						{
							g_sPlayerBullet[i].nLife = 0;
							g_nMap[g_sPlayerBullet[i].nY][g_sPlayerBullet[i].nX] = 0; // ���� ó��
						}					
					}		
				}

				//---------------------------- �� ĳ���� �̵��� �浹 ----------------------------------//
				for( i = 0 ; i < g_nEnemyIndex ; i++ )
				{
					if( g_Enemy[i].nLife ) // ��� �־�� �浹 üũ�� �Ѵ�.
					{
						switch( g_Enemy[i].nState )
						{
						case ENEMY_RUN : // �̵� �� ���� �浹 ���θ� �ľ�
									if( CurTime - g_Enemy[i].nOldTime > g_Enemy[i].nMoveTime )
									{
										g_Enemy[i].nOldTime = CurTime;
										switch( g_Enemy[i].nDirect )
										{
										case UP :   // ��� ���� ó���� ��ϰ� ��ȣ���� �浹�ϸ� ���� ��ȯ
													if( g_Enemy[i].nY - 1 < 0 || g_nMap[ g_Enemy[i].nY - 1 ][g_Enemy[i].nX] == 1 || g_nMap[ g_Enemy[i].nY - 1 ][g_Enemy[i].nX] == 2 )
														g_Enemy[i].nState = ENEMY_ROTATION;
													else
														g_Enemy[i].nY--;												
													
													break;
										case DOWN :
													if( g_Enemy[i].nY + 1 > MAP_ROW - 1 || g_nMap[ g_Enemy[i].nY + 1 ][g_Enemy[i].nX] == 1 || g_nMap[ g_Enemy[i].nY + 1 ][g_Enemy[i].nX] == 2 )
														g_Enemy[i].nState = ENEMY_ROTATION;
													else
														g_Enemy[i].nY++;												

													break;
										case LEFT :
													if( g_Enemy[i].nX - 1 < 0 || g_nMap[g_Enemy[i].nY][g_Enemy[i].nX-1] == 1 || g_nMap[g_Enemy[i].nY][g_Enemy[i].nX-1] == 2 )
														g_Enemy[i].nState = ENEMY_ROTATION;
													else
														g_Enemy[i].nX--;													
													
													break;
										case RIGHT:
													if( g_Enemy[i].nX + 1 > MAP_COL - 1 || g_nMap[g_Enemy[i].nY][g_Enemy[i].nX+1] == 1 || g_nMap[g_Enemy[i].nY][g_Enemy[i].nX+1] == 2 )
														g_Enemy[i].nState = ENEMY_ROTATION;
													else
														g_Enemy[i].nX++;														
													
													break;
										}
									}					
									break;

						case ENEMY_ROTATION : // �¿���� �����ؼ� ���� ���� , ���� ���� ����� �������� �ϵ� ���� ������ ����, ���� �ҽ��� �����ϰ� �ϴ� ����̴�.
								while( 1 )
								{
									nTemp = rand() % 4;
									if( nTemp != g_Enemy[i].nDirect ) // ���� ������ �ƴϸ�
									{	
										g_Enemy[i].nDirect = nTemp;
										g_Enemy[i].nState = ENEMY_RUN;
										g_Enemy[i].nOldTime = CurTime;
										break;
									}
								}
								break;
						}

						// Note: �� ĳ���Ϳ� ���ΰ� �浹 üũ 
						if( IsPalyerCollision( g_Enemy[i].nX, g_Enemy[i].nY ) == 1 )
						{
							g_Player.nLife--;
							g_Enemy[i].nLife--;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );
							
							if( g_Enemy[i].nLife ==  0 )
							{
								g_Enemy[i].nState = ENEMY_STOP;
								g_nDeadEnemy++;
								g_nGrade += 10;
							}							
							
							// ���ΰ��� �Ҹ�� �� ĳ���� ���� üũ 
							if( g_Player.nLife == 0 || g_nDeadEnemy == g_Stage.nEnemyCount )
							{
								g_nGameState = STOP;
								return ;					
							}								
							continue;
						}

						// Note: �� ĳ���Ϳ� ���ΰ� �̻��� �浹 üũ 
						if( IsPlayerBulletCollision( g_Enemy[i].nX, g_Enemy[i].nY ) == 1 )
						{
							g_Enemy[i].nLife--;

							if( g_Enemy[i].nLife ==  0 )
							{
								g_Enemy[i].nState = ENEMY_STOP;
								g_nDeadEnemy++;
								g_nGrade += 10;
							}	

							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );
							 
							// �� ĳ���� ���� üũ
							if( g_nDeadEnemy == g_Stage.nEnemyCount )
							{
								g_nGameState = STOP;								
								return ;
							}
							continue;
						}

						// Note: �� ĳ���Ͱ� ������ �浹�� ���
						if( g_Enemy[i].nX == g_Boss.nX && g_Enemy[i].nY == g_Boss.nY )
						{
							g_Enemy[i].nLife--;
							if( g_Enemy[i].nLife == 0 )
							{
								g_Enemy[i].nState = ENEMY_STOP;
								g_nDeadEnemy++;
								g_nGrade += 10;
							}	

							g_Boss.nLife--;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );
							if( g_Boss.nLife == 0 )
							{
								g_nGameState = STOP;
								return ;
							}
						}
					}
				}

				//------------------------------------------------ �� �̻��� ------------------------------------------------//	
				// Note: �� ĳ������ �̻��� �߻� �κ� 
				//       RUN ���°� ������ ������ �͸� �̻����� �߻� �Ѵ�. 
				for( i = 0 ; i < g_nEnemyIndex ; i++ )
				{
					if( g_Enemy[i].nState == ENEMY_RUN )
					{
						if( CurTime - g_Enemy[i].nOldFireTime > g_Enemy[i].nFireTime )
						{
							g_Enemy[i].nOldFireTime = CurTime;
							// 100���� �̻��Ͽ��� ã�� �κ��̴�.
							for( j = 0 ; j < 100 ; j++ )
							{
								if( g_sEnemyBullet[j].nLife == 0 )
								{
									g_sEnemyBullet[j].nDirect = g_Enemy[i].nDirect;

									switch( g_Enemy[i].nDirect )
									{
									case LEFT:
											g_sEnemyBullet[j].nX = g_Enemy[i].nX-1;
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
									g_sEnemyBullet[j].nOldMoveTime = CurTime;
									FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[4], 0, &g_Channel[4] );
									break;
								}
							}
						}
					}
				}

				//------------------------------------------------ �� �̻��� �̵��� �浹-------------------------------------------//

				// Note: �̻��� ��ǥ ������Ʈ 
				for( i = 0 ; i < 100 ; i++ )
				{
					if( g_sEnemyBullet[i].nLife == 1 )
					{	
						if( CurTime - g_sEnemyBullet[i].nOldMoveTime > 120 ) // �� �̻����� �̵� �ð� ������ 120���� ����
						{
							switch( g_sEnemyBullet[i].nDirect )
							{
							case UP:
									if( g_sEnemyBullet[i].nY - 1 < 0 )  // ��� ������ ���� �浹
									{
										g_sEnemyBullet[i].nLife = 0;
										continue;
									}else{
										g_sEnemyBullet[i].nY--;
										g_sEnemyBullet[i].nOldMoveTime = CurTime;
									}
									break;
							case DOWN:
									if( g_sEnemyBullet[i].nY + 1 > MAP_ROW - 1 )
									{
										g_sEnemyBullet[i].nLife = 0;
										continue;
									}else{
										g_sEnemyBullet[i].nY++;
										g_sEnemyBullet[i].nOldMoveTime = CurTime;
									}
									break;
							case LEFT:
									if( g_sEnemyBullet[i].nX - 1 < 0 )
									{
										g_sEnemyBullet[i].nLife = 0;
										continue;
									}else{
										g_sEnemyBullet[i].nX--;
										g_sEnemyBullet[i].nOldMoveTime = CurTime;
									}
									break;
							case RIGHT:
									if( g_sEnemyBullet[i].nX + 1 > MAP_COL - 1 )
									{
										g_sEnemyBullet[i].nLife = 0;
										continue;
									}else{
										g_sEnemyBullet[i].nX++;
										g_sEnemyBullet[i].nOldMoveTime = CurTime;
									}
									break;
							}							
						}
					
						// Note: �� �̻��ϰ� ���ΰ�
						if( IsPalyerCollision( g_sEnemyBullet[i].nX, g_sEnemyBullet[i].nY ) == 1 )
						{
							g_sEnemyBullet[i].nLife = 0;
							g_Player.nLife--;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );

							// note: ���ΰ� ���� üũ
							if( g_Player.nLife == 0 )
							{	
								 g_nGameState = STOP;
								 return ;
							}
							continue;
						}

						// Note: �� �̻��ϰ� ���ΰ� �̻���
						if( IsPlayerBulletCollision( g_sEnemyBullet[i].nX, g_sEnemyBullet[i].nY ) == 1 )
						{
							g_sEnemyBullet[i].nLife = 0;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );
							continue;
						}
						
						// Note: �� �̻��ϰ� ���
						if( g_nMap[g_sEnemyBullet[i].nY][g_sEnemyBullet[i].nX] == 1  )
						{
							g_nMap[g_sEnemyBullet[i].nY][g_sEnemyBullet[i].nX] = 0;
							g_sEnemyBullet[i].nLife = 0;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );
							continue;
						}

						// Note: �� �̻��ϰ� ��ȣ��
						if( g_nMap[g_sEnemyBullet[i].nY][g_sEnemyBullet[i].nX] == 2 )
						{
							g_sEnemyBullet[i].nLife = 0;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );
							continue;
						}

						// Note: ������ �浹
						if( g_sEnemyBullet[i].nX == g_Boss.nX && g_sEnemyBullet[i].nY == g_Boss.nY )
						{
							g_sEnemyBullet[i].nLife = 0;
							g_Boss.nLife--;

							if( g_Boss.nLife == 0 )
							{
								g_nGameState = STOP;
								return ;
							}							
						}			
					}
				}	
				break;
	case STOP:
				if( g_nDeadEnemy == g_Stage.nEnemyCount && g_Boss.nLife > 0 && g_Player.nLife > 0 )
				{
					g_UpdateOldTime = CurTime;
					g_nGameState = SUCCESS;		
					g_nTotalGrade += g_nGrade;
					FMOD_Channel_Stop( g_Channel[1] ); // ����� ����
					FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[6], 0, &g_Channel[6] ); // �̼� ���� ����
				}else{
					g_nGameState = FAILED;
					FMOD_Channel_Stop( g_Channel[1] ); // ����� ����
					FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[2], 0, &g_Channel[2] ); // �̼� ���� ����
				}
				break;
	case SUCCESS :
				if( CurTime - g_UpdateOldTime > 3000 )
				{
					g_UpdateOldTime = CurTime;		
					++g_nStage;		
					Init();
					g_nGameState = READY;
					FMOD_Channel_Stop( g_Channel[6] );  // �̼� ���� ���� ��� ����					
					FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[3], 0, &g_Channel[3]); // READY ���� ���
				}				
				break;
	}	
}

void Render()
{
	int i, j;
	char string[100];		
	ScreenClear();

	switch( g_nGameState )
	{
	case INIT :
				if( g_nStage == 0 )
				   InitScreen();	
				break;
	case READY:					
				ReadyScreen();
				break;
	case RUNNING:
				for( i = 0 ; i < MAP_ROW ; i++ )
				{
					for( j = 0 ; j < MAP_COL ; j++ )
					{
						if( g_nMap[i][j] == 3 ||  g_nMap[i][j] == 5 )
							ScreenPrint( j*2, i, g_Title[0] );		
						else	
							ScreenPrint( j*2, i, g_Title[ g_nMap[i][j] ] );		
					}
				}

				// Note: ���ΰ� �̻��� �߻�
				for( i = 0 ; i < 5 ; i++ )
				{
					if( g_sPlayerBullet[i].nLife )
					{
						if( g_sPlayerBullet[i].nDirect == UP || g_sPlayerBullet[i].nDirect == DOWN )
							ScreenPrint(g_sPlayerBullet[i].nX*2, g_sPlayerBullet[i].nY, "��" );
						else
							ScreenPrint(g_sPlayerBullet[i].nX*2, g_sPlayerBullet[i].nY, "��" );
					}
				}

				// Note: �� ĳ���� �̻��� �߻�
				for( i = 0 ;  i < 100 ; i++ )
				{
					if( g_sEnemyBullet[i].nLife )
					{
						switch( g_sEnemyBullet[i].nDirect )
						{
						case UP :
									ScreenPrint( g_sEnemyBullet[i].nX*2, g_sEnemyBullet[i].nY, "��" );	
									break;
						case DOWN :
									ScreenPrint( g_sEnemyBullet[i].nX*2, g_sEnemyBullet[i].nY, "��" );	
									break;
						case LEFT:
									ScreenPrint( g_sEnemyBullet[i].nX*2, g_sEnemyBullet[i].nY, "��" );		
									break;
						case RIGHT:
									ScreenPrint( g_sEnemyBullet[i].nX*2, g_sEnemyBullet[i].nY, "��" );	
									break;
						}
					}
				}

				for( i = 0 ; i < g_nEnemyIndex ; i++ )
				{
					if( g_Enemy[i].nState != ENEMY_STOP )
					{
						ScreenPrint( g_Enemy[i].nX*2, g_Enemy[i].nY, "��" );
					}
				}	

				ScreenPrint( g_Player.nX*2, g_Player.nY, "��" );

				sprintf( string, "���ΰ� �����: %d", g_Player.nLife );
				ScreenPrint( 50, 2, string );
				sprintf( string, "�� ��ũ ��: %d", g_Stage.nEnemyCount );
				ScreenPrint( 50, 4, string );
				sprintf( string, "�ı��� ��ũ ��: %d", g_nDeadEnemy );
				ScreenPrint( 50, 6, string );
				sprintf( string, "����: %d", g_nGrade );
				ScreenPrint( 50, 8, string );				
				break;
	case SUCCESS:		
				SuccessScreen();					
				break;
	case FAILED:
				FailureScreen();		
				break;
	case RESULT :
				ResultScreen();				
				break;
	}

	ScreenFlipping();
}

void Release()
{
	if( g_Stage.pEnemyPosition != NULL )
		free( g_Stage.pEnemyPosition );

	SoundRelease();
}

int main(void)
{	
	int nKey, i;	
	clock_t CurTime, OldTime;
	
	ScreenInit();
	Init();

	OldTime = clock();
	while( 1 )
	{	
		CurTime = clock();	

		if( _kbhit() )
		{
			if( g_nGameState == RESULT )  // ��� �ڿ��� �����ϴ� ����
				break;

			nKey = _getch();

			switch( nKey )
			{
			case ' ' :
						if( g_nGameState == INIT && g_nStage == 0 )
						{							
							g_nGameState = READY;
							FMOD_Channel_Stop( g_Channel[0] ); // ����� ����
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[3], 0, &g_Channel[3]); // �������� ���
							g_UpdateOldTime = clock();  // ready�� �����ð� ������ �ֱ� ���� 							
						}
						break;

			case 72 :	// ����
						if( CurTime - g_Player.nOldMoveTime >= g_Player.nMoveTime )
						{
							if( g_nMap[g_Player.nY-1][g_Player.nX] == 0 || g_nMap[g_Player.nY-1][g_Player.nX] == 3 || g_nMap[g_Player.nY-1][g_Player.nX] == 5 )
							{
								if( g_Player.nY - 1 >= 0 )
								{
									g_Player.nY--;
									g_Player.nDirect = UP;
									g_Player.nOldMoveTime = CurTime;
								}
							}													
						}
						break;
			case 80 :   //�Ʒ���
						if( CurTime - g_Player.nOldMoveTime >= g_Player.nMoveTime )
						{
							if( g_nMap[g_Player.nY+1][g_Player.nX] == 0 || g_nMap[g_Player.nY+1][g_Player.nX] == 3 || g_nMap[g_Player.nY+1][g_Player.nX] == 5)
							{
								if( g_Player.nY + 1 <= MAP_ROW - 1 )
								{
									g_Player.nY++;
									g_Player.nDirect = DOWN;
									g_Player.nOldMoveTime = CurTime;	
								}
							}													
						}
						break;
			case 75 :  // ����
						if( CurTime - g_Player.nOldMoveTime >= g_Player.nMoveTime )
						{		
							if( g_nMap[g_Player.nY][g_Player.nX-1] == 0 || g_nMap[g_Player.nY][g_Player.nX-1] == 3 || g_nMap[g_Player.nY][g_Player.nX-1] == 5 )
							{
								if( g_Player.nX - 1 >= 0 )
								{
									g_Player.nX--;
									g_Player.nDirect = LEFT;
									g_Player.nOldMoveTime = CurTime;		
								}
							}												
						}
						break;
			case 77 : //������
						if( CurTime - g_Player.nOldMoveTime >= g_Player.nMoveTime )
						{
							if( g_nMap[g_Player.nY][g_Player.nX+1] == 0 || g_nMap[g_Player.nY][g_Player.nX+1] == 3 || g_nMap[g_Player.nY][g_Player.nX+1] == 5)
							{
								if( g_Player.nX + 1 <= MAP_COL - 1 )
								{
									g_Player.nX++;
									g_Player.nDirect = RIGHT;
									g_Player.nOldMoveTime = CurTime;
								}
							}														
						}
						break;
			case 's' : // ���ΰ� �̻��� �߻�
						if( CurTime - g_Player.nOldFireTime >= g_Player.nFireTime )
						{
							g_Player.nOldFireTime = CurTime;
							for( i = 0 ; i < 5 ; i++ )
							{
								if( g_sPlayerBullet[i].nLife == 0 )
								{
									g_sPlayerBullet[i].nLife = 1 ;
									g_sPlayerBullet[i].nDirect = g_Player.nDirect;

									// �ʱ� ��ġ ����
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

									g_sPlayerBullet[i].nOldMoveTime = CurTime;		
									FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[4], 0, &g_Channel[4] );
									break;
								}
							}
						}
						break;
			case 'y' :
			case 'Y' :
						if( g_nGameState == FAILED ) 
						{													
							Init();
							g_nGameState = READY;	
							FMOD_Channel_Stop( g_Channel[2] );  // �̼� ���� ���� ��� ���� 
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[3], 0, &g_Channel[3]); // �������� ���
							g_UpdateOldTime = clock();
 						}						
							break;
			case 'n' :
			case 'N' :
						if( g_nGameState == FAILED )
						{							
							g_nGameState = RESULT;							
							FMOD_Channel_Stop( g_Channel[2] );  // �̼� ���� ���� ��� ����							
						}
						break;	
			}
		}
		Update();		
		Render();	
		FMOD_System_Update( g_System );	
	}

	Release();
	ScreenRelease();

	return 0;
}
