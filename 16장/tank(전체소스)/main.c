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
	int nEnemyCount;				// 적 캐릭터 수
	int nEnemyPositionCount;		// 적 생성 위치 개수
	POS_XY *pEnemyPosition;			// 적 생성 위치 배열
	int nBossX, nBossY;				// 보스 초기 위치
	int nPlayerX, nPlayerY;			// 플레이어 초기 위치
	int nEnemyTypeCount;			// 적 캐릭터 종류 개수
} STAGE;

typedef struct _ENEMY_TYPE
{	
	int	nLife;		// 생명력
	int nMoveTime;	// 속력
	int nFireTime;  // 미사일 발사 시간 간격	
} ENEMY_TYPE;

typedef struct _ENEMY
{
	int nTypeIndex;	 // 타입 인덱스
	int nPosIndex;	 // 생성 초기 위치 인덱스
	int nX, nY;      // 이동 좌표
	STATE nState;	 // 상태 전이
	int nLife;		 // 생명력
	int nDirect;	 // 방향
	int nMoveTime;	// 이동 시간
	int nFireTime;  // 미사일 발사 시간 간격
	int nAppearanceTime; // 출현 시간
	int nOldTime; // 이전 이동 시간
	int nOldFireTime;  // 미사일 발사 이전 시간 간격
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
ENEMY_TYPE g_EnemyType[10]; // 탱크 종류는 최대 10개까지 제한 한다.
ENEMY g_Enemy[30];
BOSS g_Boss;
STAGE g_Stage;

int g_nMap[MAP_ROW][MAP_COL];
char g_Title[6][3] = { "  ", "■", "▥", "◎", "♨", "◈" };
char *g_MapFileName[3] = { "map1.txt", "map1.txt", "map1.txt" };
GAME_STATE	g_nGameState = INIT;
BULLET g_sPlayerBullet[5]; 
BULLET g_sEnemyBullet[100];

int		g_nEnemyIndex = 0;
int     g_nDeadEnemy;
int		g_nStage = -1;
int		g_nGrade = 0; // 점수 
int		g_nTotalGrade = 0; // 총점
clock_t	g_GameStartTime, g_UpdateOldTime, g_RemainTime;

// Note: 사운드 관련
FMOD_SYSTEM *g_System;
FMOD_SOUND  *g_Sound[7];
FMOD_CHANNEL *g_Channel[7];

void Box()
{
	int i;

	ScreenPrint(0, 0,  "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	for( i = 1 ; i <= 19 ; i++ )
		ScreenPrint(0, i,  "┃                                                        ┃");
	ScreenPrint(0, 20, "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");	
}

void InitScreen()
{
	ScreenPrint(0, 0,  "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	ScreenPrint(0, 1,  "┃                                                        ┃");
	ScreenPrint(0, 2,  "┃         -----                                          ┃");
	ScreenPrint(0, 3,  "┃        /    |                                       /  ┃");
	ScreenPrint(0, 4,  "┃       /       |    탱크 게임                       /   ┃");
	ScreenPrint(0, 5,  "┃      /          |                            /-----/   ┃");
	ScreenPrint(0, 6,  "┃     /            |---------------|         /           ┃");
	ScreenPrint(0, 7,  "┃    /                               |      /            ┃");
	ScreenPrint(0, 8,  "┃   /              ┏━━━━━┓      |   /             ┃");
	ScreenPrint(0, 9,  "┃  /               ┃          ┃       |/               ┃");
	ScreenPrint(0, 10, "┃/                 ┃    ◎    ┃                        ┃");
	ScreenPrint(0, 11, "┃     ( (    ┏━━━━━━━━━━━┓     ) )          ┃");
	ScreenPrint(0, 12, "┃            ┃                      ┃                  ┃");
	ScreenPrint(0, 13, "┃           ▤ ▤                   ▤ ▤                ┃");
	ScreenPrint(0, 14, "┃           ▤ ▤ ━━━━━━━━━▤ ▤                ┃");
	ScreenPrint(0, 15, "┃           ▤ ▤                   ▤ ▤                ┃");
	ScreenPrint(0, 16, "┃  ~ ^ ~ $~ ~ ^ ~ ~ ~ ^ ~ ~ ~ ^ ~ ~ ~ ^~ ~ ~ ~ ~ $ ~    ~┃");
	ScreenPrint(0, 17, "┃                                                        ┃");
	ScreenPrint(0, 18, "┃                  Press Space Key !!                    ┃");
	ScreenPrint(0, 19, "┃                                                        ┃");
	ScreenPrint(0, 20, "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");	
}

void ReadyScreen()
{
	char string[100];

	Box();
	sprintf( string, "%d 스테이지", g_nStage+1 );
	ScreenPrint( 25, 9, string );
}

void SuccessScreen()
{	
	ScreenPrint(0, 0,  "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	ScreenPrint(0, 1,  "┃                                                        ┃");
	ScreenPrint(0, 2,  "┃                                                        ┃");
	ScreenPrint(0, 3,  "┃                                                        ┃");
	ScreenPrint(0, 4,  "┃                                                        ┃");
	ScreenPrint(0, 5,  "┃                                                        ┃");
	ScreenPrint(0, 6,  "┃                                                        ┃");
	ScreenPrint(0, 7,  "┃                                                        ┃");
	ScreenPrint(0, 8,  "┃                     ////＼＼                           ┃");
	ScreenPrint(0, 9,  "┃                    q ∧. ∧ p (^)@                     ┃");
	ScreenPrint(0, 10, "┃                    (└──┘) //                       ┃");
	ScreenPrint(0, 11, "┃                   ♬ 미션 성공 ♪                      ┃");
	ScreenPrint(0, 12, "┃                                                        ┃");
	ScreenPrint(0, 13, "┃                                                        ┃");
	ScreenPrint(0, 14, "┃                                                        ┃");
	ScreenPrint(0, 15, "┃                                                        ┃");
	ScreenPrint(0, 16, "┃                                                        ┃");
	ScreenPrint(0, 17, "┃                                                        ┃");
	ScreenPrint(0, 18, "┃                                                        ┃");
	ScreenPrint(0, 19, "┃                                                        ┃");
	ScreenPrint(0, 20, "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
}

void FailureScreen()
{	
	ScreenPrint(0, 0, "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	ScreenPrint(0, 1, "┃                                                        ┃");
	ScreenPrint(0, 2, "┃                                                        ┃");
	ScreenPrint(0, 3, "┃                                                        ┃");
	ScreenPrint(0, 4, "┃                                                        ┃");
	ScreenPrint(0, 5, "┃                                                        ┃");
	ScreenPrint(0, 6, "┃                                                        ┃");
	ScreenPrint(0, 7, "┃                    미션 실패 !!!!                      ┃");
	ScreenPrint(0, 8, "┃                                                        ┃");
	ScreenPrint(0, 9, "┃                                                        ┃");
	ScreenPrint(0, 10,"┃                               ●┳━┓                 ┃");
	ScreenPrint(0, 11,"┃                                 ┛  ┗                 ┃");
	ScreenPrint(0, 12,"┃                                ■■■■                ┃");
	ScreenPrint(0, 13,"┃                                                        ┃");
	ScreenPrint(0, 14,"┃                                                        ┃");
	ScreenPrint(0, 15,"┃                                                        ┃");
	ScreenPrint(0, 16,"┃                계속 하시겠습니까? (y/n)                ┃");
	ScreenPrint(0, 17,"┃                                                        ┃");
	ScreenPrint(0, 18,"┃                                                        ┃");
	ScreenPrint(0, 19,"┃                                                        ┃");
	ScreenPrint(0, 20,"┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");	
}


void ResultScreen()
{
	char string[100];

	Box();

	ScreenPrint( 3,  3, "                     탱크 게임 끝" );	
	ScreenPrint( 3,  6, "                    ┏━━━━━┓" );
	ScreenPrint( 3,  7, "               ~~   ┃          ┃" );
	ScreenPrint( 3,  8, "        ~~          ┃    ◎    ┃" );
	ScreenPrint( 3,  9, "    ●         ┏━━━━━━━━━━━┓" );
	ScreenPrint( 3, 10, "  ┗┃┛       ┃                      ┃" );
	ScreenPrint( 3, 11, "   ┏┓      ▤ ▤                   ▤ ▤" );
	ScreenPrint( 3, 12, "             ▤ ▤ ━━━━━━━━━▤ ▤" );
	ScreenPrint( 3, 13, "             ▤ ▤                   ▤ ▤" );
	ScreenPrint( 3, 14, "~ ~ ^ ~ $~ ~ ^ ~ ~ ~ ^ ~ ~ ~ ^ ~ ~ ~ ^~ ~ ~ ~ /~ $ ~~" );
	sprintf( string, "Score : %d", g_nTotalGrade ); 
	ScreenPrint( 39, 7, string );	
}

void SoundInit()
{
	FMOD_System_Create( &g_System );
	FMOD_System_Init( g_System, 32, FMOD_INIT_NORMAL, NULL );

	// loop sound
	FMOD_System_CreateSound( g_System, "intro.MP3", FMOD_LOOP_NORMAL, 0, &g_Sound[0] ); // 소개 
	FMOD_System_CreateSound( g_System, "running.ogg", FMOD_LOOP_NORMAL, 0, &g_Sound[1] ); // 게임 진행 중
	FMOD_System_CreateSound( g_System, "failed.wav", FMOD_LOOP_NORMAL, 0, &g_Sound[2] ); // 실패
	
	// effect sound 
	FMOD_System_CreateSound( g_System, "ready.wav",	FMOD_DEFAULT, 0, &g_Sound[3] ); // 준비
	FMOD_System_CreateSound( g_System, "fire.wav", FMOD_DEFAULT, 0, &g_Sound[4] ); // 대포 발사 
	FMOD_System_CreateSound( g_System, "collision.wav", FMOD_DEFAULT, 0, &g_Sound[5] ); // 충돌 
	FMOD_System_CreateSound( g_System, "success.wav", FMOD_DEFAULT, 0, &g_Sound[6] ); // 충돌 	
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

	if( g_nStage == -1 ) // 1번만 초기화가 되는 부분에 대한 처리 사항
	{		
		SoundInit();		
		g_nStage = 0;		
		FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[0], 0, &g_Channel[0] );			
	}

	fp = fopen( g_MapFileName[g_nStage], "rt" );
	fscanf( fp, "적 캐릭터수 : %d\n", &g_Stage.nEnemyCount );
	fscanf( fp, "적 캐릭터가 출현할 위치 개수 : %d\n", &g_Stage.nEnemyPositionCount );
			
	if( g_Stage.pEnemyPosition != NULL )
		free( g_Stage.pEnemyPosition );

	g_Stage.pEnemyPosition = (POS_XY*)malloc( sizeof( POS_XY )*g_Stage.nEnemyPositionCount );
	for( i = 0 ; i < g_Stage.nEnemyPositionCount ; i++ )
	{
		fscanf( fp, "%d %d\n", &g_Stage.pEnemyPosition[i].nX, &g_Stage.pEnemyPosition[i].nY );		
	}
	fscanf( fp, "보스 초기 위치 : %d %d\n", &g_Stage.nBossX, &g_Stage.nBossY );	
	fscanf( fp, "플레이어 초기 위치 : %d %d\n", &g_Stage.nPlayerX , &g_Stage.nPlayerY );	
	fscanf( fp, "적 캐릭터 종류 개수 : %d\n", &g_Stage.nEnemyTypeCount );	

	for( i = 0 ; i < g_Stage.nEnemyTypeCount ; i++ )
		fscanf( fp, "생명력: %d 이동시간 간격: %d 미사일: %d\n", &g_EnemyType[i].nLife, &g_EnemyType[i].nMoveTime, &g_EnemyType[i].nFireTime );

	for( i = 0 ; i < g_Stage.nEnemyCount ; i++ )
		fscanf( fp, "타입인덱스: %d 적캐릭터 생성 인덱스: %d 출현 시간: %d\n", &g_Enemy[i].nTypeIndex, &g_Enemy[i].nPosIndex, &g_Enemy[i].nAppearanceTime );

	for( i = 0 ; i < MAP_ROW ; i++ )
	{
		for( j = 0 ; j < MAP_COL ; j++ )
		{
			fscanf( fp, "%d ", &g_nMap[i][j] );			
		}
	}
	
	fclose( fp );

	// Note: 보스 설정
	g_Boss.nX = g_Stage.nBossX;
	g_Boss.nY = g_Stage.nBossY;
	g_Boss.nLife = 2;

	// Note: 주인공 설정
	g_Player.nOldMoveTime = clock();
	g_Player.nOldFireTime = clock();
	g_Player.nMoveTime = 200;
	g_Player.nX = g_Stage.nPlayerX;
	g_Player.nY = g_Stage.nPlayerY;
	g_Player.nLife = 3;

	// Note: 미사일 초기화
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
		g_Enemy[i].nDirect = rand() % 4; // Note: 방향은 임의값 설정
		g_Enemy[i].nState = ENEMY_STOP;
	}		

	g_nEnemyIndex = 0;
	g_nGrade = 0;	// 스테이지마다 점수
	g_nDeadEnemy = 0;	// 적 캐릭터 섬멸 점수	
	g_UpdateOldTime = clock();
}

// Note: 주인공 미사일과 충돌 체크
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
				return 1; // 충돌			
			}
		}
	}

	return 0;  // 비충돌
}
//---------------------------------------------------------------------------
// 적 캐릭터와 충돌 체크                                               
// in :  nX, nY                                                                   
// out : 충돌된 적 캐릭터 인덱스
// return : 충돌 1 , 비충돌 0
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
			    return 1; // 충돌
			} 
		}
	}

	return 0; // 비충돌
}

// Note: 주인공과 충돌 체크
int IsPalyerCollision( int nX, int nY )
{
	if( g_Player.nX == nX && g_Player.nY == nY )
		return 1;
	else
		return 0;	
}

// Note: 적 미사일과 충돌 체크
int IsEnemyBulletCollision( int nX, int nY ) // 주인공, 주인공 미사일  
{
	int i ;

	for( i = 0 ; i < 100 ; i++ )
	{
		if( g_sEnemyBullet[i].nLife == 1 )
		{
			if( g_sEnemyBullet[i].nX == nX && g_sEnemyBullet[i].nY == nY )
			{
				g_sEnemyBullet[i].nLife = 0;
				return 1;  // 충돌			
			}
		}
	}

	return 0; //비 충돌 
}

void Update()
{
	int i, j, nTemp, nIndex;
	clock_t CurTime, PassTime;

	CurTime = clock();
	switch( g_nGameState )
	{
	case READY :
				if( CurTime - g_UpdateOldTime > 3000 )  // 2초
				{					
					g_nGameState = RUNNING;
					g_GameStartTime = CurTime;	
					FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[1], 0, &g_Channel[1] ); // 게임 진행 사운드
				}				
				break;
	case RUNNING:
				// Note: 적 캐릭터 출현
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

				//------------------ 주인공 미사일 -------------------------------//
				for( i = 0 ; i < 5 ; i++ )
				{
					if( g_sPlayerBullet[i].nLife )
					{
						// Note: 미사일 이동
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
						
						//------------------------------------------------ 충돌 -----------------------------------------------------------//
						// Note: 경계 영역 충돌
						if( g_sPlayerBullet[i].nY > MAP_ROW - 1 || g_sPlayerBullet[i].nY < 0 )
						{
							g_sPlayerBullet[i].nLife = 0;
							continue;
						}

						// 주인공 미사일과 적 미사일 충돌			
						if( IsEnemyBulletCollision( g_sPlayerBullet[i].nX, g_sPlayerBullet[i].nY ) == 1 )
						{
							g_sPlayerBullet[i].nLife = 0;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );
							continue;
						}

						// 주인공 미사일과 적 캐릭터의 충돌
						if( IsEnemyCollision( g_sPlayerBullet[i].nX, g_sPlayerBullet[i].nY, &nIndex ) == 1 )
						{
							g_sPlayerBullet[i].nLife = 0;
							g_Enemy[nIndex].nLife--;
							if( g_Enemy[nIndex].nLife == 0 ) // 소멸 상태
							{
								g_Enemy[nIndex].nState = ENEMY_STOP;
								g_nGrade += 10; // 점수 가산
								g_nDeadEnemy++;
							}

							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );

							if( g_nDeadEnemy == g_Stage.nEnemyCount )
							{	// Note: 게임 성공
								g_nGameState = STOP;														
								return ;
							}
							continue;
						}


						// 방호벽 충돌
						if( g_nMap[g_sPlayerBullet[i].nY][g_sPlayerBullet[i].nX] == 2 )
						{
							g_sPlayerBullet[i].nLife = 0;
							continue;
						}

						// 블록 충돌
						if( g_nMap[g_sPlayerBullet[i].nY][g_sPlayerBullet[i].nX] == 1 )
						{
							g_sPlayerBullet[i].nLife = 0;
							g_nMap[g_sPlayerBullet[i].nY][g_sPlayerBullet[i].nX] = 0; // 공백 처리
						}					
					}		
				}

				//---------------------------- 적 캐릭터 이동과 충돌 ----------------------------------//
				for( i = 0 ; i < g_nEnemyIndex ; i++ )
				{
					if( g_Enemy[i].nLife ) // 살아 있어야 충돌 체크를 한다.
					{
						switch( g_Enemy[i].nState )
						{
						case ENEMY_RUN : // 이동 중 벽과 충돌 여부를 파악
									if( CurTime - g_Enemy[i].nOldTime > g_Enemy[i].nMoveTime )
									{
										g_Enemy[i].nOldTime = CurTime;
										switch( g_Enemy[i].nDirect )
										{
										case UP :   // 경계 영역 처리와 블록과 방호벽에 충돌하면 방향 전환
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

						case ENEMY_ROTATION : // 좌우상하 조사해서 방향 결정 , 방향 결정 방법은 랜덤으로 하되 벽의 유무로 결정, 지금 소스는 랜덤하게 하는 방식이다.
								while( 1 )
								{
									nTemp = rand() % 4;
									if( nTemp != g_Enemy[i].nDirect ) // 같은 방향이 아니면
									{	
										g_Enemy[i].nDirect = nTemp;
										g_Enemy[i].nState = ENEMY_RUN;
										g_Enemy[i].nOldTime = CurTime;
										break;
									}
								}
								break;
						}

						// Note: 적 캐릭터와 주인공 충돌 체크 
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
							
							// 주인공과 소멸된 적 캐릭터 상태 체크 
							if( g_Player.nLife == 0 || g_nDeadEnemy == g_Stage.nEnemyCount )
							{
								g_nGameState = STOP;
								return ;					
							}								
							continue;
						}

						// Note: 적 캐릭터와 주인공 미사일 충돌 체크 
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
							 
							// 적 캐릭터 상태 체크
							if( g_nDeadEnemy == g_Stage.nEnemyCount )
							{
								g_nGameState = STOP;								
								return ;
							}
							continue;
						}

						// Note: 적 캐릭터가 보스와 충돌한 경우
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

				//------------------------------------------------ 적 미사일 ------------------------------------------------//	
				// Note: 적 캐릭터의 미사일 발사 부분 
				//       RUN 상태가 완전히 결정된 것만 미사일을 발사 한다. 
				for( i = 0 ; i < g_nEnemyIndex ; i++ )
				{
					if( g_Enemy[i].nState == ENEMY_RUN )
					{
						if( CurTime - g_Enemy[i].nOldFireTime > g_Enemy[i].nFireTime )
						{
							g_Enemy[i].nOldFireTime = CurTime;
							// 100개의 미사일에서 찾는 부분이다.
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

				//------------------------------------------------ 적 미사일 이동과 충돌-------------------------------------------//

				// Note: 미사일 좌표 업데이트 
				for( i = 0 ; i < 100 ; i++ )
				{
					if( g_sEnemyBullet[i].nLife == 1 )
					{	
						if( CurTime - g_sEnemyBullet[i].nOldMoveTime > 120 ) // 적 미사일의 이동 시간 간격은 120으로 고정
						{
							switch( g_sEnemyBullet[i].nDirect )
							{
							case UP:
									if( g_sEnemyBullet[i].nY - 1 < 0 )  // 경계 영역에 대한 충돌
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
					
						// Note: 적 미사일과 주인공
						if( IsPalyerCollision( g_sEnemyBullet[i].nX, g_sEnemyBullet[i].nY ) == 1 )
						{
							g_sEnemyBullet[i].nLife = 0;
							g_Player.nLife--;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );

							// note: 주인공 상태 체크
							if( g_Player.nLife == 0 )
							{	
								 g_nGameState = STOP;
								 return ;
							}
							continue;
						}

						// Note: 적 미사일과 주인공 미사일
						if( IsPlayerBulletCollision( g_sEnemyBullet[i].nX, g_sEnemyBullet[i].nY ) == 1 )
						{
							g_sEnemyBullet[i].nLife = 0;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );
							continue;
						}
						
						// Note: 적 미사일과 블록
						if( g_nMap[g_sEnemyBullet[i].nY][g_sEnemyBullet[i].nX] == 1  )
						{
							g_nMap[g_sEnemyBullet[i].nY][g_sEnemyBullet[i].nX] = 0;
							g_sEnemyBullet[i].nLife = 0;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );
							continue;
						}

						// Note: 적 미사일과 방호벽
						if( g_nMap[g_sEnemyBullet[i].nY][g_sEnemyBullet[i].nX] == 2 )
						{
							g_sEnemyBullet[i].nLife = 0;
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[5], 0, &g_Channel[5] );
							continue;
						}

						// Note: 보스와 충돌
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
					FMOD_Channel_Stop( g_Channel[1] ); // 배경음 중지
					FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[6], 0, &g_Channel[6] ); // 미션 성공 사운드
				}else{
					g_nGameState = FAILED;
					FMOD_Channel_Stop( g_Channel[1] ); // 배경음 중지
					FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[2], 0, &g_Channel[2] ); // 미션 실패 사운드
				}
				break;
	case SUCCESS :
				if( CurTime - g_UpdateOldTime > 3000 )
				{
					g_UpdateOldTime = CurTime;		
					++g_nStage;		
					Init();
					g_nGameState = READY;
					FMOD_Channel_Stop( g_Channel[6] );  // 미션 성공 사운드 출력 중지					
					FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[3], 0, &g_Channel[3]); // READY 사운드 출력
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

				// Note: 주인공 미사일 발사
				for( i = 0 ; i < 5 ; i++ )
				{
					if( g_sPlayerBullet[i].nLife )
					{
						if( g_sPlayerBullet[i].nDirect == UP || g_sPlayerBullet[i].nDirect == DOWN )
							ScreenPrint(g_sPlayerBullet[i].nX*2, g_sPlayerBullet[i].nY, "┃" );
						else
							ScreenPrint(g_sPlayerBullet[i].nX*2, g_sPlayerBullet[i].nY, "━" );
					}
				}

				// Note: 적 캐릭터 미사일 발사
				for( i = 0 ;  i < 100 ; i++ )
				{
					if( g_sEnemyBullet[i].nLife )
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
					if( g_Enemy[i].nState != ENEMY_STOP )
					{
						ScreenPrint( g_Enemy[i].nX*2, g_Enemy[i].nY, "★" );
					}
				}	

				ScreenPrint( g_Player.nX*2, g_Player.nY, "◈" );

				sprintf( string, "주인공 생명력: %d", g_Player.nLife );
				ScreenPrint( 50, 2, string );
				sprintf( string, "적 탱크 수: %d", g_Stage.nEnemyCount );
				ScreenPrint( 50, 4, string );
				sprintf( string, "파괴된 탱크 수: %d", g_nDeadEnemy );
				ScreenPrint( 50, 6, string );
				sprintf( string, "점수: %d", g_nGrade );
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
			if( g_nGameState == RESULT )  // 모든 자원을 해제하는 상태
				break;

			nKey = _getch();

			switch( nKey )
			{
			case ' ' :
						if( g_nGameState == INIT && g_nStage == 0 )
						{							
							g_nGameState = READY;
							FMOD_Channel_Stop( g_Channel[0] ); // 배경음 중지
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[3], 0, &g_Channel[3]); // 스테이지 출력
							g_UpdateOldTime = clock();  // ready를 일정시간 지속해 주기 위해 							
						}
						break;

			case 72 :	// 위쪽
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
			case 80 :   //아래쪽
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
			case 75 :  // 왼쪽
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
			case 77 : //오른쪽
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
			case 's' : // 주인공 미사일 발사
						if( CurTime - g_Player.nOldFireTime >= g_Player.nFireTime )
						{
							g_Player.nOldFireTime = CurTime;
							for( i = 0 ; i < 5 ; i++ )
							{
								if( g_sPlayerBullet[i].nLife == 0 )
								{
									g_sPlayerBullet[i].nLife = 1 ;
									g_sPlayerBullet[i].nDirect = g_Player.nDirect;

									// 초기 위치 설정
									g_sPlayerBullet[i].nX = g_Player.nX; 
									g_sPlayerBullet[i].nY = g_Player.nY;

									// Note: 방향에 따른 초기 좌표 재조정
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
							FMOD_Channel_Stop( g_Channel[2] );  // 미션 실패 사운드 출력 중지 
							FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound[3], 0, &g_Channel[3]); // 스테이지 출력
							g_UpdateOldTime = clock();
 						}						
							break;
			case 'n' :
			case 'N' :
						if( g_nGameState == FAILED )
						{							
							g_nGameState = RESULT;							
							FMOD_Channel_Stop( g_Channel[2] );  // 미션 실패 사운드 출력 중지							
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
