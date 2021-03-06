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
	int nEnemyCount;				// 瞳 議葛攪 熱
	int nEnemyPositionCount;		// 瞳 儅撩 嬪纂 偃熱
	POS_XY *pEnemyPosition;			// 瞳 儅撩 嬪纂 寡翮
	int nBossX, nBossY;				// 爾蝶 蟾晦 嬪纂
	int nPlayerX, nPlayerY;			// Ы溯檜橫 蟾晦 嬪纂
	int nEnemyTypeCount;			// 瞳 議葛攪 謙盟 偃熱
} STAGE;

typedef struct _ENEMY_TYPE
{	
	int	nLife;		// 儅貲溘
	int nMoveTime;	// 樓溘
	int nFireTime;  // 嘐餌橾 嫦餌 衛除 除問	
} ENEMY_TYPE;

typedef struct _ENEMY
{
	int nTypeIndex;	 // 顫殮 檣策蝶
	int nPosIndex;	 // 儅撩 蟾晦 嬪纂 檣策蝶
	int nX, nY;      // 檜翕 謝ル
	STATE nState;	 // 鼻鷓 瞪檜
	int nLife;		 // 儅貲溘
	int nDirect;	 // 寞щ
	int nMoveTime;	// 檜翕 衛除
	int nFireTime;  // 嘐餌橾 嫦餌 衛除 除問
	int nAppearanceTime; // 轎⑷ 衛除
	int nOldTime; // 檜瞪 檜翕 衛除
	int nOldFireTime;  // 嘐餌橾 嫦餌 檜瞪 衛除 除問
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
ENEMY_TYPE g_EnemyType[10]; // 戀觼 謙盟朝 譆渠 10偃梱雖 薯и и棻.
ENEMY g_Enemy[30];
BOSS g_Boss;
STAGE g_Stage;

int g_nMap[MAP_ROW][MAP_COL];
char g_Title[6][3] = { "  ", "﹥", "〦", "≡", "卄", "Ⅹ" };
char *g_MapFileName[3] = { "map1.txt", "map1.txt", "map1.txt" };
GAME_STATE	g_nGameState = INIT;
BULLET g_sPlayerBullet[5]; 
BULLET g_sEnemyBullet[100];

int		g_nEnemyIndex = 0;
int     g_nDeadEnemy;
int		g_nStage = -1;
int		g_nGrade = 0; // 薄熱 
int		g_nTotalGrade = 0; // 識薄
clock_t	g_GameStartTime, g_UpdateOldTime, g_RemainTime;

// Note: 餌遴萄 婦溼
FMOD_SYSTEM *g_System;
FMOD_SOUND  *g_Sound[7];
FMOD_CHANNEL *g_Channel[7];

void Box()
{
	int i;

	ScreenPrint(0, 0,  "旨收收收收收收收收收收收收收收收收收收收收收收收收收收收收旬");
	for( i = 1 ; i <= 19 ; i++ )
		ScreenPrint(0, i,  "早                                                        早");
	ScreenPrint(0, 20, "曲收收收收收收收收收收收收收收收收收收收收收收收收收收收收旭");	
}

void InitScreen()
{
	ScreenPrint(0, 0,  "旨收收收收收收收收收收收收收收收收收收收收收收收收收收收收旬");
	ScreenPrint(0, 1,  "早                                                        早");
	ScreenPrint(0, 2,  "早         -----                                          早");
	ScreenPrint(0, 3,  "早        /    |                                       /  早");
	ScreenPrint(0, 4,  "早       /       |    戀觼 啪歜                       /   早");
	ScreenPrint(0, 5,  "早      /          |                            /-----/   早");
	ScreenPrint(0, 6,  "早     /            |---------------|         /           早");
	ScreenPrint(0, 7,  "早    /                               |      /            早");
	ScreenPrint(0, 8,  "早   /              旨收收收收收旬      |   /             早");
	ScreenPrint(0, 9,  "早  /               早          早       |/               早");
	ScreenPrint(0, 10, "早/                 早    ≡    早                        早");
	ScreenPrint(0, 11, "早     ( (    旨收收收收收收收收收收收旬     ) )          早");
	ScreenPrint(0, 12, "早            早                      早                  早");
	ScreenPrint(0, 13, "早           〥 〥                   〥 〥                早");
	ScreenPrint(0, 14, "早           〥 〥 收收收收收收收收收〥 〥                早");
	ScreenPrint(0, 15, "早           〥 〥                   〥 〥                早");
	ScreenPrint(0, 16, "早  ~ ^ ~ $~ ~ ^ ~ ~ ~ ^ ~ ~ ~ ^ ~ ~ ~ ^~ ~ ~ ~ ~ $ ~    ~早");
	ScreenPrint(0, 17, "早                                                        早");
	ScreenPrint(0, 18, "早                  Press Space Key !!                    早");
	ScreenPrint(0, 19, "早                                                        早");
	ScreenPrint(0, 20, "曲收收收收收收收收收收收收收收收收收收收收收收收收收收收收旭");	
}

void ReadyScreen()
{
	char string[100];

	Box();
	sprintf( string, "%d 蝶纔檜雖", g_nStage+1 );
	ScreenPrint( 25, 9, string );
}

void SuccessScreen()
{	
	ScreenPrint(0, 0,  "旨收收收收收收收收收收收收收收收收收收收收收收收收收收收收旬");
	ScreenPrint(0, 1,  "早                                                        早");
	ScreenPrint(0, 2,  "早                                                        早");
	ScreenPrint(0, 3,  "早                                                        早");
	ScreenPrint(0, 4,  "早                                                        早");
	ScreenPrint(0, 5,  "早                                                        早");
	ScreenPrint(0, 6,  "早                                                        早");
	ScreenPrint(0, 7,  "早                                                        早");
	ScreenPrint(0, 8,  "早                     ////′′                           早");
	ScreenPrint(0, 9,  "早                    q ∥. ∥ p (^)@                     早");
	ScreenPrint(0, 10, "早                    (戌式式戎) //                       早");
	ScreenPrint(0, 11, "早                   Ｏ 嘐暮 撩奢 Ｎ                      早");
	ScreenPrint(0, 12, "早                                                        早");
	ScreenPrint(0, 13, "早                                                        早");
	ScreenPrint(0, 14, "早                                                        早");
	ScreenPrint(0, 15, "早                                                        早");
	ScreenPrint(0, 16, "早                                                        早");
	ScreenPrint(0, 17, "早                                                        早");
	ScreenPrint(0, 18, "早                                                        早");
	ScreenPrint(0, 19, "早                                                        早");
	ScreenPrint(0, 20, "曲收收收收收收收收收收收收收收收收收收收收收收收收收收收收旭");
}

void FailureScreen()
{	
	ScreenPrint(0, 0, "旨收收收收收收收收收收收收收收收收收收收收收收收收收收收收旬");
	ScreenPrint(0, 1, "早                                                        早");
	ScreenPrint(0, 2, "早                                                        早");
	ScreenPrint(0, 3, "早                                                        早");
	ScreenPrint(0, 4, "早                                                        早");
	ScreenPrint(0, 5, "早                                                        早");
	ScreenPrint(0, 6, "早                                                        早");
	ScreenPrint(0, 7, "早                    嘐暮 褒ぬ !!!!                      早");
	ScreenPrint(0, 8, "早                                                        早");
	ScreenPrint(0, 9, "早                                                        早");
	ScreenPrint(0, 10,"早                               ≒有收旬                 早");
	ScreenPrint(0, 11,"早                                 旭  曲                 早");
	ScreenPrint(0, 12,"早                                ﹥﹥﹥﹥                早");
	ScreenPrint(0, 13,"早                                                        早");
	ScreenPrint(0, 14,"早                                                        早");
	ScreenPrint(0, 15,"早                                                        早");
	ScreenPrint(0, 16,"早                啗樓 ж衛啊蝗棲梱? (y/n)                早");
	ScreenPrint(0, 17,"早                                                        早");
	ScreenPrint(0, 18,"早                                                        早");
	ScreenPrint(0, 19,"早                                                        早");
	ScreenPrint(0, 20,"曲收收收收收收收收收收收收收收收收收收收收收收收收收收收收旭");	
}


void ResultScreen()
{
	char string[100];

	Box();

	ScreenPrint( 3,  3, "                     戀觼 啪歜 部" );	
	ScreenPrint( 3,  6, "                    旨收收收收收旬" );
	ScreenPrint( 3,  7, "               ~~   早          早" );
	ScreenPrint( 3,  8, "        ~~          早    ≡    早" );
	ScreenPrint( 3,  9, "    ≒         旨收收收收收收收收收收收旬" );
	ScreenPrint( 3, 10, "  曲早旭       早                      早" );
	ScreenPrint( 3, 11, "   旨旬      〥 〥                   〥 〥" );
	ScreenPrint( 3, 12, "             〥 〥 收收收收收收收收收〥 〥" );
	ScreenPrint( 3, 13, "             〥 〥                   〥 〥" );
	ScreenPrint( 3, 14, "~ ~ ^ ~ $~ ~ ^ ~ ~ ~ ^ ~ ~ ~ ^ ~ ~ ~ ^~ ~ ~ ~ /~ $ ~~" );
	sprintf( string, "Score : %d", g_nTotalGrade ); 
	ScreenPrint( 39, 7, string );	
}

void SoundInit()
{
	FMOD_System_Create( &g_System );
	FMOD_System_Init( g_System, 32, FMOD_INIT_NORMAL, NULL );

	// loop sound
	FMOD_System_CreateSound( g_System, "intro.MP3", FMOD_LOOP_NORMAL, 0, &g_Sound[0] ); // 模偃 
	FMOD_System_CreateSound( g_System, "running.ogg", FMOD_LOOP_NORMAL, 0, &g_Sound[1] ); // 啪歜 霞ч 醞
	FMOD_System_CreateSound( g_System, "failed.wav", FMOD_LOOP_NORMAL, 0, &g_Sound[2] ); // 褒ぬ
	
	// effect sound 
	FMOD_System_CreateSound( g_System, "ready.wav",	FMOD_DEFAULT, 0, &g_Sound[3] ); // 遽綠
	FMOD_System_CreateSound( g_System, "fire.wav", FMOD_DEFAULT, 0, &g_Sound[4] ); // 渠ん 嫦餌 
	FMOD_System_CreateSound( g_System, "collision.wav", FMOD_DEFAULT, 0, &g_Sound[5] ); // 醱給 
	FMOD_System_CreateSound( g_System, "success.wav", FMOD_DEFAULT, 0, &g_Sound[6] ); // 醱給 	
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

	if( g_nStage == -1 ) // 1廓虜 蟾晦�降� 腎朝 睡碟縑 渠и 籀葬 餌о
	{		
		SoundInit();		
		g_nStage = 0;		
		FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[0], 0, &g_Channel[0] );			
	}

	fp = fopen( g_MapFileName[g_nStage], "rt" );
	fscanf( fp, "瞳 議葛攪熱 : %d\n", &g_Stage.nEnemyCount );
	fscanf( fp, "瞳 議葛攪陛 轎⑷й 嬪纂 偃熱 : %d\n", &g_Stage.nEnemyPositionCount );
			
	if( g_Stage.pEnemyPosition != NULL )
		free( g_Stage.pEnemyPosition );

	g_Stage.pEnemyPosition = (POS_XY*)malloc( sizeof( POS_XY )*g_Stage.nEnemyPositionCount );
	for( i = 0 ; i < g_Stage.nEnemyPositionCount ; i++ )
	{
		fscanf( fp, "%d %d\n", &g_Stage.pEnemyPosition[i].nX, &g_Stage.pEnemyPosition[i].nY );		
	}
	fscanf( fp, "爾蝶 蟾晦 嬪纂 : %d %d\n", &g_Stage.nBossX, &g_Stage.nBossY );	
	fscanf( fp, "Ы溯檜橫 蟾晦 嬪纂 : %d %d\n", &g_Stage.nPlayerX , &g_Stage.nPlayerY );	
	fscanf( fp, "瞳 議葛攪 謙盟 偃熱 : %d\n", &g_Stage.nEnemyTypeCount );	

	for( i = 0 ; i < g_Stage.nEnemyTypeCount ; i++ )
		fscanf( fp, "儅貲溘: %d 檜翕衛除 除問: %d 嘐餌橾: %d\n", &g_EnemyType[i].nLife, &g_EnemyType[i].nMoveTime, &g_EnemyType[i].nFireTime );

	for( i = 0 ; i < g_Stage.nEnemyCount ; i++ )
		fscanf( fp, "顫殮檣策蝶: %d 瞳議葛攪 儅撩 檣策蝶: %d 轎⑷ 衛除: %d\n", &g_Enemy[i].nTypeIndex, &g_Enemy[i].nPosIndex, &g_Enemy[i].nAppearanceTime );

	for( i = 0 ; i < MAP_ROW ; i++ )
	{
		for( j = 0 ; j < MAP_COL ; j++ )
		{
			fscanf( fp, "%d ", &g_nMap[i][j] );			
		}
	}
	
	fclose( fp );

	// Note: 爾蝶 撲薑
	g_Boss.nX = g_Stage.nBossX;
	g_Boss.nY = g_Stage.nBossY;
	g_Boss.nLife = 2;

	// Note: 輿檣奢 撲薑
	g_Player.nOldMoveTime = clock();
	g_Player.nOldFireTime = clock();
	g_Player.nMoveTime = 200;
	g_Player.nX = g_Stage.nPlayerX;
	g_Player.nY = g_Stage.nPlayerY;
	g_Player.nLife = 3;

	// Note: 嘐餌橾 蟾晦��
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
		g_Enemy[i].nDirect = rand() % 4; // Note: 寞щ擎 歜曖高 撲薑
		g_Enemy[i].nState = ENEMY_STOP;
	}		

	g_nEnemyIndex = 0;
	g_nGrade = 0;	// 蝶纔檜雖葆棻 薄熱
	g_nDeadEnemy = 0;	// 瞳 議葛攪 撰資 薄熱	
	g_UpdateOldTime = clock();
}

// Note: 輿檣奢 嘐餌橾婁 醱給 羹觼
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
				return 1; // 醱給			
			}
		}
	}

	return 0;  // 綠醱給
}
//---------------------------------------------------------------------------
// 瞳 議葛攪諦 醱給 羹觼                                               
// in :  nX, nY                                                                   
// out : 醱給脹 瞳 議葛攪 檣策蝶
// return : 醱給 1 , 綠醱給 0
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
			    return 1; // 醱給
			} 
		}
	}

	return 0; // 綠醱給
}

// Note: 輿檣奢婁 醱給 羹觼
int IsPalyerCollision( int nX, int nY )
{
	if( g_Player.nX == nX && g_Player.nY == nY )
		return 1;
	else
		return 0;	
}

// Note: 瞳 嘐餌橾婁 醱給 羹觼
int IsEnemyBulletCollision( int nX, int nY ) // 輿檣奢, 輿檣奢 嘐餌橾  
{
	int i ;

	for( i = 0 ; i < 100 ; i++ )
	{
		if( g_sEnemyBullet[i].nLife == 1 )
		{
			if( g_sEnemyBullet[i].nX == nX && g_sEnemyBullet[i].nY == nY )
			{
				g_sEnemyBullet[i].nLife = 0;
				return 1;  // 醱給			
			}
		}
	}

	return 0; //綠 醱給 
}

void Update()
{
	int i, j, nTemp, nIndex;
	clock_t CurTime, PassTime;

	CurTime = clock();
	switch( g_nGameState )
	{
	case READY :
				if( CurTime - g_UpdateOldTime > 3000 )  // 2蟾
				{					
					g_nGameState = RUNNING;
					g_GameStartTime = CurTime;	
					FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[1], 0, &g_Channel[1] ); // 啪歜 霞ч 餌遴萄
				}				
				break;
	case RUNNING:
				// Note: 瞳 議葛攪 轎⑷
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

				//------------------ 輿檣奢 嘐餌橾 -------------------------------//
				for( i = 0 ; i < 5 ; i++ )
				{
					if( g_sPlayerBullet[i].nLife )
					{
						// Note: 嘐餌橾 檜翕
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
						
						//------------------------------------------------ 醱給 -----------------------------------------------------------//
						// Note: 唳啗 艙羲 醱給
						if( g_sPlayerBullet[i].nY > MAP_ROW - 1 || g_sPlayerBullet[i].nY < 0 )
						{
							g_sPlayerBullet[i].nLife = 0;
							continue;
						}

						// 輿檣奢 嘐餌橾婁 瞳 嘐餌橾 醱給			
						if( IsEnemyBulletCollision( g_sPlayerBullet[i].nX, g_sPlayerBullet[i].nY ) == 1 )
						{
							g_sPlayerBullet[i].nLife = 0;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );
							continue;
						}

						// 輿檣奢 嘐餌橾婁 瞳 議葛攪曖 醱給
						if( IsEnemyCollision( g_sPlayerBullet[i].nX, g_sPlayerBullet[i].nY, &nIndex ) == 1 )
						{
							g_sPlayerBullet[i].nLife = 0;
							g_Enemy[nIndex].nLife--;
							if( g_Enemy[nIndex].nLife == 0 ) // 模資 鼻鷓
							{
								g_Enemy[nIndex].nState = ENEMY_STOP;
								g_nGrade += 10; // 薄熱 陛骯
								g_nDeadEnemy++;
							}

							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );

							if( g_nDeadEnemy == g_Stage.nEnemyCount )
							{	// Note: 啪歜 撩奢
								g_nGameState = STOP;														
								return ;
							}
							continue;
						}


						// 寞�ㄩ� 醱給
						if( g_nMap[g_sPlayerBullet[i].nY][g_sPlayerBullet[i].nX] == 2 )
						{
							g_sPlayerBullet[i].nLife = 0;
							continue;
						}

						// 綰煙 醱給
						if( g_nMap[g_sPlayerBullet[i].nY][g_sPlayerBullet[i].nX] == 1 )
						{
							g_sPlayerBullet[i].nLife = 0;
							g_nMap[g_sPlayerBullet[i].nY][g_sPlayerBullet[i].nX] = 0; // 奢寥 籀葬
						}					
					}		
				}

				//---------------------------- 瞳 議葛攪 檜翕婁 醱給 ----------------------------------//
				for( i = 0 ; i < g_nEnemyIndex ; i++ )
				{
					if( g_Enemy[i].nLife ) // 髦嬴 氈橫撿 醱給 羹觼蒂 и棻.
					{
						switch( g_Enemy[i].nState )
						{
						case ENEMY_RUN : // 檜翕 醞 漁婁 醱給 罹睡蒂 だ學
									if( CurTime - g_Enemy[i].nOldTime > g_Enemy[i].nMoveTime )
									{
										g_Enemy[i].nOldTime = CurTime;
										switch( g_Enemy[i].nDirect )
										{
										case UP :   // 唳啗 艙羲 籀葬諦 綰煙婁 寞�ㄩ挪� 醱給ж賊 寞щ 瞪��
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

						case ENEMY_ROTATION : // 謝辦鼻ж 褻餌п憮 寞щ 唸薑 , 寞щ 唸薑 寞徹擎 楠渾戲煎 ж腎 漁曖 嶸鼠煎 唸薑, 雖旎 模蝶朝 楠渾ж啪 ж朝 寞衝檜棻.
								while( 1 )
								{
									nTemp = rand() % 4;
									if( nTemp != g_Enemy[i].nDirect ) // 偽擎 寞щ檜 嬴棲賊
									{	
										g_Enemy[i].nDirect = nTemp;
										g_Enemy[i].nState = ENEMY_RUN;
										g_Enemy[i].nOldTime = CurTime;
										break;
									}
								}
								break;
						}

						// Note: 瞳 議葛攪諦 輿檣奢 醱給 羹觼 
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
							
							// 輿檣奢婁 模資脹 瞳 議葛攪 鼻鷓 羹觼 
							if( g_Player.nLife == 0 || g_nDeadEnemy == g_Stage.nEnemyCount )
							{
								g_nGameState = STOP;
								return ;					
							}								
							continue;
						}

						// Note: 瞳 議葛攪諦 輿檣奢 嘐餌橾 醱給 羹觼 
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
							 
							// 瞳 議葛攪 鼻鷓 羹觼
							if( g_nDeadEnemy == g_Stage.nEnemyCount )
							{
								g_nGameState = STOP;								
								return ;
							}
							continue;
						}

						// Note: 瞳 議葛攪陛 爾蝶諦 醱給и 唳辦
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

				//------------------------------------------------ 瞳 嘐餌橾 ------------------------------------------------//	
				// Note: 瞳 議葛攪曖 嘐餌橾 嫦餌 睡碟 
				//       RUN 鼻鷓陛 諫瞪�� 唸薑脹 匙虜 嘐餌橾擊 嫦餌 и棻. 
				for( i = 0 ; i < g_nEnemyIndex ; i++ )
				{
					if( g_Enemy[i].nState == ENEMY_RUN )
					{
						if( CurTime - g_Enemy[i].nOldFireTime > g_Enemy[i].nFireTime )
						{
							g_Enemy[i].nOldFireTime = CurTime;
							// 100偃曖 嘐餌橾縑憮 瓊朝 睡碟檜棻.
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

				//------------------------------------------------ 瞳 嘐餌橾 檜翕婁 醱給-------------------------------------------//

				// Note: 嘐餌橾 謝ル 機等檜お 
				for( i = 0 ; i < 100 ; i++ )
				{
					if( g_sEnemyBullet[i].nLife == 1 )
					{	
						if( CurTime - g_sEnemyBullet[i].nOldMoveTime > 120 ) // 瞳 嘐餌橾曖 檜翕 衛除 除問擎 120戲煎 堅薑
						{
							switch( g_sEnemyBullet[i].nDirect )
							{
							case UP:
									if( g_sEnemyBullet[i].nY - 1 < 0 )  // 唳啗 艙羲縑 渠и 醱給
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
					
						// Note: 瞳 嘐餌橾婁 輿檣奢
						if( IsPalyerCollision( g_sEnemyBullet[i].nX, g_sEnemyBullet[i].nY ) == 1 )
						{
							g_sEnemyBullet[i].nLife = 0;
							g_Player.nLife--;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );

							// note: 輿檣奢 鼻鷓 羹觼
							if( g_Player.nLife == 0 )
							{	
								 g_nGameState = STOP;
								 return ;
							}
							continue;
						}

						// Note: 瞳 嘐餌橾婁 輿檣奢 嘐餌橾
						if( IsPlayerBulletCollision( g_sEnemyBullet[i].nX, g_sEnemyBullet[i].nY ) == 1 )
						{
							g_sEnemyBullet[i].nLife = 0;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );
							continue;
						}
						
						// Note: 瞳 嘐餌橾婁 綰煙
						if( g_nMap[g_sEnemyBullet[i].nY][g_sEnemyBullet[i].nX] == 1  )
						{
							g_nMap[g_sEnemyBullet[i].nY][g_sEnemyBullet[i].nX] = 0;
							g_sEnemyBullet[i].nLife = 0;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );
							continue;
						}

						// Note: 瞳 嘐餌橾婁 寞�ㄩ�
						if( g_nMap[g_sEnemyBullet[i].nY][g_sEnemyBullet[i].nX] == 2 )
						{
							g_sEnemyBullet[i].nLife = 0;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );
							continue;
						}

						// Note: 爾蝶諦 醱給
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
					FMOD_Channel_Stop( g_Channel[1] ); // 寡唳擠 醞雖
					FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[6], 0, &g_Channel[6] ); // 嘐暮 撩奢 餌遴萄
				}else{
					g_nGameState = FAILED;
					FMOD_Channel_Stop( g_Channel[1] ); // 寡唳擠 醞雖
					FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[2], 0, &g_Channel[2] ); // 嘐暮 褒ぬ 餌遴萄
				}
				break;
	case SUCCESS :
				if( CurTime - g_UpdateOldTime > 3000 )
				{
					g_UpdateOldTime = CurTime;		
					++g_nStage;		
					Init();
					g_nGameState = READY;
					FMOD_Channel_Stop( g_Channel[6] );  // 嘐暮 撩奢 餌遴萄 轎溘 醞雖					
					FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[3], 0, &g_Channel[3]); // READY 餌遴萄 轎溘
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

				// Note: 輿檣奢 嘐餌橾 嫦餌
				for( i = 0 ; i < 5 ; i++ )
				{
					if( g_sPlayerBullet[i].nLife )
					{
						if( g_sPlayerBullet[i].nDirect == UP || g_sPlayerBullet[i].nDirect == DOWN )
							ScreenPrint(g_sPlayerBullet[i].nX*2, g_sPlayerBullet[i].nY, "早" );
						else
							ScreenPrint(g_sPlayerBullet[i].nX*2, g_sPlayerBullet[i].nY, "收" );
					}
				}

				// Note: 瞳 議葛攪 嘐餌橾 嫦餌
				for( i = 0 ;  i < 100 ; i++ )
				{
					if( g_sEnemyBullet[i].nLife )
					{
						switch( g_sEnemyBullet[i].nDirect )
						{
						case UP :
									ScreenPrint( g_sEnemyBullet[i].nX*2, g_sEnemyBullet[i].nY, "∟" );	
									break;
						case DOWN :
									ScreenPrint( g_sEnemyBullet[i].nX*2, g_sEnemyBullet[i].nY, "⊿" );	
									break;
						case LEFT:
									ScreenPrint( g_sEnemyBullet[i].nX*2, g_sEnemyBullet[i].nY, "∠" );		
									break;
						case RIGHT:
									ScreenPrint( g_sEnemyBullet[i].nX*2, g_sEnemyBullet[i].nY, "⊥" );	
									break;
						}
					}
				}

				for( i = 0 ; i < g_nEnemyIndex ; i++ )
				{
					if( g_Enemy[i].nState != ENEMY_STOP )
					{
						ScreenPrint( g_Enemy[i].nX*2, g_Enemy[i].nY, "≠" );
					}
				}	

				ScreenPrint( g_Player.nX*2, g_Player.nY, "Ⅹ" );

				sprintf( string, "輿檣奢 儅貲溘: %d", g_Player.nLife );
				ScreenPrint( 50, 2, string );
				sprintf( string, "瞳 戀觼 熱: %d", g_Stage.nEnemyCount );
				ScreenPrint( 50, 4, string );
				sprintf( string, "だ惚脹 戀觼 熱: %d", g_nDeadEnemy );
				ScreenPrint( 50, 6, string );
				sprintf( string, "薄熱: %d", g_nGrade );
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
			if( g_nGameState == RESULT )  // 賅萇 濠錳擊 п薯ж朝 鼻鷓
				break;

			nKey = _getch();

			switch( nKey )
			{
			case ' ' :
						if( g_nGameState == INIT && g_nStage == 0 )
						{							
							g_nGameState = READY;
							FMOD_Channel_Stop( g_Channel[0] ); // 寡唳擠 醞雖
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[3], 0, &g_Channel[3]); // 蝶纔檜雖 轎溘
							g_UpdateOldTime = clock();  // ready蒂 橾薑衛除 雖樓п 輿晦 嬪п 							
						}
						break;

			case 72 :	// 嬪薹
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
			case 80 :   //嬴楚薹
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
			case 75 :  // 豭薹
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
			case 77 : //螃艇薹
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
			case 's' : // 輿檣奢 嘐餌橾 嫦餌
						if( CurTime - g_Player.nOldFireTime >= g_Player.nFireTime )
						{
							g_Player.nOldFireTime = CurTime;
							for( i = 0 ; i < 5 ; i++ )
							{
								if( g_sPlayerBullet[i].nLife == 0 )
								{
									g_sPlayerBullet[i].nLife = 1 ;
									g_sPlayerBullet[i].nDirect = g_Player.nDirect;

									// 蟾晦 嬪纂 撲薑
									g_sPlayerBullet[i].nX = g_Player.nX; 
									g_sPlayerBullet[i].nY = g_Player.nY;

									// Note: 寞щ縑 評艇 蟾晦 謝ル 營褻薑
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
							FMOD_Channel_Stop( g_Channel[2] );  // 嘐暮 褒ぬ 餌遴萄 轎溘 醞雖 
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[3], 0, &g_Channel[3]); // 蝶纔檜雖 轎溘
							g_UpdateOldTime = clock();
 						}						
							break;
			case 'n' :
			case 'N' :
						if( g_nGameState == FAILED )
						{							
							g_nGameState = RESULT;							
							FMOD_Channel_Stop( g_Channel[2] );  // 嘐暮 褒ぬ 餌遴萄 轎溘 醞雖							
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
