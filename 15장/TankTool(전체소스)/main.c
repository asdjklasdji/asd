#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>

#define MAP_COL   23
#define MAP_ROW   18

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
	int			 nLife;		// 생명력
	unsigned int nMoveTime;	// 속력
	unsigned int nFireTime;  // 총알 발사 시간 간격	
} ENEMY_TYPE;

typedef struct _ENEMY
{
	int nTypeIndex;		// 타입 인덱스
	int nPosIndex;		// 생성 초기 위치 인덱스
	// int nX, nY;      // 이동 좌표
	// int nState;		// 상태 전이
	// int nLife;		// 생명력
	// int nDirect;		// 방향
	unsigned int nAppearanceTime; // 출현 시간
	// unsigned int nOldTime; // 이전 이동 시간
} ENEMY;

ENEMY_TYPE g_EnemyType[10]; // 탱크 종류는 최대 10개까지 제한 한다.
ENEMY g_Enemy[30];
int g_nMap[MAP_ROW][MAP_COL];
STAGE g_Stage;

char *g_Menu[5] = {"파일  읽기", "파일  저장", "맵 정보", "적 캐릭터", "종    료" };
char *g_Sub[4] = { "입력", "수정", "삭제", "상위" };
char *g_Sub2[6] = { "입력", "수정", "삭제", "이전", "다음", "상위" };
char *g_Sub3[5] = { "입력", "수정", "이전", "다음", "상위" };
char *g_StageSubMenu[4] = { "적 캐릭터 종류", "적 캐릭터 출현 시간", "적 캐릭터 출현 위치", "상위 메뉴로" };
char *g_MapSubMenu[4] = {"편집", "맵 전체 삭제", "데이터 생성", "상위" };
char g_Title[6][3] = { "  ", "■", "▥", "◎", "♨", "◈" };

void gotoxy( int x, int y )
{
	COORD CursorPosition = { x, y };
	SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), CursorPosition );
}

void Box()
{
	gotoxy( 0, 0 );
	printf( "┏━━━━━━━━━━━━━━━━━━━━━━━┓\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┃                                              ┃\n" );
	printf( "┗━━━━━━━━━━━━━━━━━━━━━━━┛\n" );
}

void BoxXY(int nX, int nY, int nHeight )
{
	int i;

	gotoxy( nX, nY); printf( "┏━━━━━━━━━━━━━━━━━━━┓\n" );

	for( i = 1 ; i <= nHeight ; i++ )
	{
		gotoxy( nX, nY + i );
		printf( "┃                                      ┃\n" );
	}
	
	gotoxy( nX, nY + nHeight );
	printf( "┗━━━━━━━━━━━━━━━━━━━┛\n" );
}

int VerticalMenu( int nX, int nY, char** pMenu, int nCount )
{
	 int nKey, i, nMenu = 0, nLength, nMaxLength = 0;
	 char *pBlank;

	// 최대 메뉴 항목의 최대 길이와 지우기 위한 공백 
	for( i = 0 ; i < nCount ; i++ )
	{
		nLength = strlen( pMenu[i] );
		if( nLength > nMaxLength )
			nMaxLength = nLength;
	}	

	// 공백으로 한줄씩 지우기 위한 부분
	pBlank = (char*)malloc( nMaxLength + 5 );
	memset( pBlank, ' ', nMaxLength + 5 );
	pBlank[nMaxLength+4] = 0; // 널문자 문자열 처리
	 
	while( 1 )
	{	
		for( i = 0 ; i < nCount ; i++ )
		{	
			gotoxy( nX - 2, nY + i*2 ); 
			printf( "%s", pBlank );
			gotoxy( nX, nY + i*2 ); 
			printf( "%s", pMenu[i] );
		}

		gotoxy( nX - 2, nMenu*2 + nY ); 
		printf( "[" );
		gotoxy( nX + nMaxLength + 1, nMenu*2 + nY ); 
		printf( "]" );

		nKey = _getch();

		switch( nKey )
		{		
		 case 72 : // 화살표 위쪽 키
				if( nMenu - 1 >= 0 )
					nMenu--;												
				break;
		 case 80 : // 화살표 아래 키
				if( nMenu + 1 < nCount )
					nMenu++;
				break;				
		 case 13 : // 엔터키 (CR)				
				return nMenu;				
		}
	}
	free( pBlank );
}

int LineMenu( int nX, int nY, int nWidth, char** pMenu, int nCount )
{
	int nKey, i, nMenu = 0;	
	int nMaxLength = 0, nLength;
	char *strBlank;
	int nSpace = 3;

	strBlank = (char*)malloc( nWidth );
	memset( strBlank, ' ', nWidth );
	strBlank[nWidth-1] = 0;

	// 최대 메뉴 항목의 최대 길이와 지우기 위한 공백 
	for( i = 0 ; i < nCount ; i++ )
	{
		nLength = strlen( pMenu[i] );
		if( nLength > nMaxLength )
			nMaxLength = nLength;
	}	

	while( 1 )
	{		
		gotoxy( nX - 2, nY ) ; 
		printf( "%s", strBlank );
		for( i = 0 ; i < nCount ; i++ )
		{
			gotoxy( nX + i*nMaxLength + nSpace*i, nY); 
			printf( "%s", pMenu[i] );
		}
		
		gotoxy( nMenu*nMaxLength + nX -1 + nSpace*nMenu, nY ); printf( "[" );
		gotoxy( nMenu*nMaxLength + nX + nMaxLength + nSpace*nMenu, nY); printf( "]" );
		
		nKey = _getch();

		switch( nKey )
		{		
		 case 75 : // 화살표 왼쪽 키
				nMenu--;
				if( nMenu < 0 )
					nMenu = 0;							
				break;
		 case 77 : // 화살표 오른쪽 키
				nMenu++;
				if( nMenu == nCount )
					nMenu = nCount - 1 ;				
				break;
		 case 13 : // 엔터키 (CR)				
				return nMenu;				
		}
	}
	
	free( strBlank );
}


int IntScanf( int x, int y, int nNumber )
{
	 char strTemp[20], cMunja;
	 int nIndex = 0, i, nLength;
	 
	 itoa( nNumber, strTemp, 10 );	 
	 nLength = strlen( strTemp ); // 기존 문자의 길이

	 // 원문 출력
     gotoxy( x, y ); printf( "%s", strTemp );
	 gotoxy( x, y ); 	
	 while( 1 )
	 {		
		cMunja = _getch();
		switch( cMunja )
		{
		case 13 : // 엔터키
				return atoi( strTemp );
				
		case 8 : // 백스페이스 처리
			    if( nIndex - 1 < 0 )
					nIndex = 0;
				else
				{
					nIndex--;
					strTemp[nIndex] = 0; // 널문자로 끝을 조정 ==> 지워주는 효과						
				}				
				break;
		default :
			    if( nIndex + 1 < 19 ) // 18문자 이상 들어 오면 입력 받지 않는다.
				{					
					strTemp[nIndex++] = cMunja;
					strTemp[nIndex] = 0; // 널문자
				}
		}

		 // Note: 길이만큼 화면을 지운다.
		gotoxy( x, y );
	    for( i = 0 ; i < nLength ; i++ )
		     printf(" "); 		
				
		gotoxy( x, y ); printf( "%s", strTemp );
		nLength = strlen( strTemp );
	}
}

int main(void)
{
	int nMenu, i, j, nIndex, nKey, nRow, nCol, nPage, nPositionIndex, nPositionCount = 0;
	FILE *fp;
	char strFileName[50];

	while( 1 )
	{	
		Box();
		gotoxy( 12, 2 );
		printf( "★ 탱크 게임 월드 툴 ★" );
		nMenu = VerticalMenu( 17, 6, g_Menu, 5 );
		
		switch( nMenu )
		{
		case 0 : // 파일 읽기
				system( "cls" );
				BoxXY( 5, 5, 4 );
				gotoxy( 7, 7 ); printf( "파일 읽기 : " );
				scanf( "%s", strFileName ); fflush( stdin );
				
				fp = fopen( strFileName, "rt" );
				if( fp == NULL )
				{
					BoxXY( 5, 5, 4 );
					gotoxy( 7, 7 ); printf( "파일이 없습니다!" );
					_getch(); 
					fflush( stdin );
				}else{
					fscanf( fp, "적 캐릭터수 : %d\n", &g_Stage.nEnemyCount );
					fscanf( fp, "적 캐릭터가 출현할 위치 개수 : %d\n", &g_Stage.nEnemyPositionCount );
					
					if( g_Stage.pEnemyPosition != NULL )
						free( g_Stage.pEnemyPosition );
					g_Stage.pEnemyPosition = (POS_XY*)malloc( sizeof( POS_XY )*g_Stage.nEnemyPositionCount );
					for( i = 0 ; i < g_Stage.nEnemyPositionCount ; i++ )
						fscanf( fp, "%d %d\n", &g_Stage.pEnemyPosition[i].nX, &g_Stage.pEnemyPosition[i].nY );
					
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

					system( "cls" );
					BoxXY( 5, 5, 4 );
					gotoxy( 7, 7 ); printf( "파일 읽기 완료" );
					_getch(); 
					fflush( stdin );
				}
				break;
		case 1 : // 파일 저장
				system( "cls" );
				BoxXY( 5, 5, 4 );
				gotoxy( 7, 7 ); printf( "파일 저장 : " );
				scanf( "%s", strFileName ); fflush( stdin );
				
				fp = fopen( strFileName, "wt" );
				fprintf( fp, "적 캐릭터수 : %d\n", g_Stage.nEnemyCount );
				fprintf( fp, "적 캐릭터가 출현할 위치 개수 : %d\n", g_Stage.nEnemyPositionCount );
			
				for( i = 0 ; i < g_Stage.nEnemyPositionCount ; i++ )
					fprintf( fp, "%d %d\n", g_Stage.pEnemyPosition[i].nX, g_Stage.pEnemyPosition[i].nY );
				fprintf( fp, "보스 초기 위치 : %d %d\n", g_Stage.nBossX, g_Stage.nBossY );
				fprintf( fp, "플레이어 초기 위치 : %d %d\n", g_Stage.nPlayerX , g_Stage.nPlayerY );
				fprintf( fp, "적 캐릭터 종류 개수 : %d\n", g_Stage.nEnemyTypeCount );

				for( i = 0 ; i < g_Stage.nEnemyTypeCount ; i++ )
					fprintf( fp, "생명력: %d 이동시간 간격: %d 미사일: %d\n", g_EnemyType[i].nLife, g_EnemyType[i].nMoveTime, g_EnemyType[i].nFireTime );

				for( i = 0 ; i < g_Stage.nEnemyCount ; i++ )
					fprintf( fp, "타입인덱스: %d 적캐릭터 생성 인덱스: %d 출현 시간: %d\n", g_Enemy[i].nTypeIndex, g_Enemy[i].nPosIndex, g_Enemy[i].nAppearanceTime );

				for( i = 0 ; i < MAP_ROW ; i++ )
				{
					for( j = 0 ; j < MAP_COL ; j++ )
					{
						fprintf( fp, "%d ", g_nMap[i][j] );
					}
					fprintf( fp, "\n");
				}
				fclose( fp );

				system( "cls" );
				BoxXY( 5, 5, 4 );
				gotoxy( 7, 7 ); printf( "파일 저장 완료" );
				_getch(); 
				fflush( stdin );
				break;
		case 2 : // 맵 정보
				nPositionCount = nRow = nCol = 0;
				for( i = 0 ; i < MAP_ROW ; i++ )
				{
					for( j = 0 ; j < MAP_COL ; j++ )
					{
						if( g_nMap[i][j] == 3 )
							nPositionCount++;
					}
				}

				gotoxy( 52, 1 );  printf( "★ 메뉴 항목 ★" );		
				gotoxy( 50, 2 );  printf( "┏━━━━━━━━┓" );
				gotoxy( 50, 3 );  printf( "┃                ┃" );
				gotoxy( 50, 4 );  printf( "┃                ┃" );
				gotoxy( 50, 5 );  printf( "┃                ┃" );
				gotoxy( 50, 6 );  printf( "┃                ┃" );
				gotoxy( 50, 7 );  printf( "┃                ┃" );
				gotoxy( 50, 8 );  printf( "┃                ┃" );
				gotoxy( 50, 9 );  printf( "┃                ┃" );
				gotoxy( 50, 10 ); printf( "┗━━━━━━━━┛" );
				gotoxy( 52, 12 ); printf( "1: 블록 ■" );
				gotoxy( 52, 13 ); printf( "2: 방호벽 ▥" ); 
				gotoxy( 52, 14 ); printf( "3: 적 탱크 출현 위치 ◎" );
				gotoxy( 52, 15 ); printf( "4: 아군 보스의 위치 설정 ♨" );
				gotoxy( 52, 16 ); printf( "5: 주인공 위치 설정 ◈" );					
				gotoxy( 52, 17 ); printf( "6: 삭제" );
				gotoxy( 52, 18 ); printf( "7: 메뉴로" );	

				while( 1 )
				{
					Box();
					for( i = 0 ; i < MAP_ROW ; i++ )
					{
						for( j = 0 ; j < MAP_COL ; j++ )
						{
							gotoxy( j*2 + 2, i+1);
							printf( "%s", g_Title[ g_nMap[i][j] ] );										
						}
					}
					
					nMenu = VerticalMenu( 54, 3, g_MapSubMenu, 4 );

					if( nMenu == 3 )
					{
						system("cls" );
						break;
					}

					switch( nMenu )
					{
					case 0 : // 편집 상태
							while( 1 )
							{							
								gotoxy( nCol*2 + 2, nRow+1 );
								nKey = _getch();
								if( nKey == '7' )
									break;
								
								switch( nKey )
								{
								case 72 : // 위쪽
										if( nRow - 1 >= 0 )
											nRow--;
										break;
								case 80 : // 아래
										if( nRow + 1 < MAP_ROW )
											nRow++; 																		
										break;
								case 77 : // 오른쪽
										if( nCol + 1 < MAP_COL )
											nCol++;
										break;
								case 75 : // 왼쪽
										if( nCol - 1 >= 0 )
											nCol--;
										break;
								case '1' : // 블록
										g_nMap[nRow][nCol] = 1;
										break;
								case '2' : // 보호 블록
										g_nMap[nRow][nCol] = 2;
										break;
								case '3' : // 적 캐릭터 생성 위치
										g_nMap[nRow][nCol] = 3;
										nPositionCount++;
										break;
								case '4' : // 보스 설정
										g_nMap[nRow][nCol] = 4;
										g_Stage.nBossX = nCol;
										g_Stage.nBossY = nRow;
										break;
								case '5' : // 주인공 설정
										g_nMap[nRow][nCol] = 5;
										g_Stage.nPlayerX = nCol;
										g_Stage.nPlayerY = nRow;
										break;

								case '6' : // 삭제
										if( g_nMap[nRow][nCol] == 3 )
											nPositionCount--;
										
										g_nMap[nRow][nCol] = 0;
										break;							
								}
								gotoxy( nCol*2 + 2, nRow + 1); // x, y
								printf( "%s", g_Title[ g_nMap[nRow][nCol] ] );
							}
							break;
					case 1 :
							// 맵 전체 삭제 
							memset( g_nMap, 0, sizeof( int )*MAP_ROW*MAP_COL );
							break;
					case 2 :
							// 데이터 생성						
							g_Stage.nEnemyPositionCount = nPositionCount;
							if( g_Stage.pEnemyPosition != NULL )
								free( g_Stage.pEnemyPosition );

							g_Stage.pEnemyPosition = (POS_XY*)malloc( sizeof(POS_XY)*nPositionCount );

							nPositionIndex = 0;
							for( i = 0 ; i < MAP_ROW ; i++ )
							{
								for( j = 0 ; j < MAP_COL ; j++ )
								{
									if( g_nMap[i][j] == 3 )
									{																	
										g_Stage.pEnemyPosition[nPositionIndex].nX = j;
										g_Stage.pEnemyPosition[nPositionIndex++].nY = i;										
									}
								}
							}

							BoxXY( 5, 5, 4 );
							gotoxy( 7, 7 ); printf( "데이터 생성 완료" );
							_getch();
							break;
					}					
				}
				break;
		case 3 : // 적 캐릭터 정보			   
				while( 1 )
				{ 
					Box();
					gotoxy( 19, 2 );
					printf( "%s", "적 캐릭터" );
					nMenu = VerticalMenu( 13, 6, g_StageSubMenu, 4 );
					if( nMenu == 3 ) // 상위 메뉴로
						break;

					switch( nMenu )
					{
					case 0 : // 적캐릭터 종류 별 속성 설정
							while( 1 )
							{
								Box();
								gotoxy( 2, 4 ); printf( "----------------------------------------------");
								gotoxy( 2, 5 ); printf( " 인덱스   생명력  이동시간간격  미사일발사시간" );
								gotoxy( 2, 6 ); printf( "----------------------------------------------");
								for( i = 0 ; i < 10 ; i++ )
								{
									gotoxy( 3, 7+i); 
									printf( "  %d         %2d        %4d        %4d  ", i, g_EnemyType[i].nLife, g_EnemyType[i].nMoveTime, g_EnemyType[i].nFireTime );
								}
								
								nMenu = LineMenu( 5, 2, 43, g_Sub, 4 );
								if( nMenu == 3 )
									break;
								
								switch( nMenu )
								{
								case 0 : // 입력
										BoxXY( 5, 5, 5);
										gotoxy( 7, 6 ); printf( "인덱스 : ");
										gotoxy( 7, 7 ); printf( "생명력 : " );
										gotoxy( 7, 8 ); printf( "이동 시간 간격 : " );
										gotoxy( 7, 9 ); printf( "미사일 발사 시간 간격 : " );										
										gotoxy( 17, 6 ); scanf( "%d", &nIndex ); fflush( stdin );
										gotoxy( 17, 7 ); scanf( "%d", &g_EnemyType[ nIndex ].nLife ); fflush( stdin );
										gotoxy( 25, 8 ); scanf( "%d", &g_EnemyType[ nIndex ].nMoveTime ); fflush( stdin );
										gotoxy( 31, 9 ); scanf( "%d", &g_EnemyType[ nIndex ].nFireTime ); fflush( stdin );		
										g_Stage.nEnemyTypeCount++;
										break;
								case 1 : // 수정
										BoxXY( 5, 5, 6 );
										gotoxy( 7, 6 ); printf( "수정 인덱스 : "); scanf( "%d", &nIndex ); fflush( stdin );
										gotoxy( 7, 7 ); printf( "생명력 : %d" , g_EnemyType[ nIndex ].nLife );
										gotoxy( 7, 8 ); printf( "이동 시간 간격 : %d", g_EnemyType[ nIndex ].nMoveTime );
										gotoxy( 7, 9 ); printf( "미사일 발사 시간 간격 : %d", g_EnemyType[ nIndex ].nFireTime );										
										
										g_EnemyType[ nIndex ].nLife = IntScanf( 16, 7, g_EnemyType[ nIndex ].nLife ); 
										g_EnemyType[ nIndex ].nMoveTime = IntScanf( 24, 8, g_EnemyType[ nIndex ].nMoveTime ); 
										g_EnemyType[ nIndex ].nFireTime = IntScanf( 31, 9, g_EnemyType[ nIndex ].nFireTime ); 
										break;
								case 2 : // 삭제
										BoxXY( 5, 5, 5);
										gotoxy( 7, 6 ); printf( " 삭제 인덱스 : "); scanf( "%d", &nIndex );
										memset( &g_EnemyType[nIndex], 0, sizeof( ENEMY_TYPE ) );				
										g_Stage.nEnemyTypeCount--;
										break;
								
								}
							}
							break;
					case 1 :  // 적 캐릭터 출현 시간 설정
						    nPage = 0;
							while( 1 )
							{
								Box();
								gotoxy( 2, 4 ); printf( "---------------------------------");
								gotoxy( 2, 5 ); printf( " 순번      종류      출현 시간" );
								gotoxy( 2, 6 ); printf( "---------------------------------");
								for( i = 0 ; i < 10 ; i++ )
								{
									gotoxy( 3, 7 + i ); 
									printf( " %d         %d        %d", i + nPage*10, g_Enemy[i + nPage*10].nTypeIndex, g_Enemy[i + nPage*10].nAppearanceTime );
								}
								
								nMenu = LineMenu( 5, 2, 45, g_Sub2, 6 );
								if( nMenu == 5 )
									break;

								switch( nMenu )
								{
								case 0 : // 입력
										BoxXY( 5, 5, 5);
										gotoxy( 7, 6 ); printf( "순번 : ");
										gotoxy( 7, 7 ); printf( "종류 : " );
										gotoxy( 7, 8 ); printf( "출현 시간 : " );
										gotoxy( 14, 6 ); scanf( "%d", &nIndex ); fflush( stdin );
										gotoxy( 14, 7 ); scanf( "%d", &g_Enemy[ nIndex ].nTypeIndex ); fflush( stdin );
										gotoxy( 19, 8 ); scanf( "%d", &g_Enemy[ nIndex ].nAppearanceTime ); fflush( stdin );			
										g_Stage.nEnemyCount++;
										break;
								case 1 : // 수정
										BoxXY( 5, 5, 6 );
										gotoxy( 7, 6 ); printf( "순번 : "); scanf( "%d", &nIndex ); fflush( stdin );
										gotoxy( 7, 7 ); printf( "종류 : %d" , g_Enemy[ nIndex ].nTypeIndex );
										gotoxy( 7, 8 ); printf( "출현 시간 : %d", g_Enemy[ nIndex ].nAppearanceTime );

										g_Enemy[ nIndex ].nTypeIndex = IntScanf( 14, 7, g_Enemy[ nIndex ].nTypeIndex ); 
										g_Enemy[ nIndex ].nAppearanceTime = IntScanf( 19, 8, g_Enemy[ nIndex ].nAppearanceTime );									
										break;
								case 2 : // 삭제
										BoxXY( 5, 5, 5);
										gotoxy( 7, 6 ); printf( " 삭제 인덱스 : "); scanf( "%d", &nIndex );
										memset( &g_Enemy[nIndex], 0, sizeof( ENEMY ) );
										g_Stage.nEnemyCount--;
										break;
								case 3:
										if( nPage - 1 < 0 )
											nPage = 0;
										else
											nPage--;
										break;	
								case 4 :
										if( nPage + 1 > 2 )
											nPage = 2;
										else
											nPage++;
										break;														
								}
							}
							break;
					case 2 : // 적 캐릭터 초기 위치 설정						
							nPage = 0;

							while( 1 )
							{
								system( "cls" );
								// Note: 적 캐릭터 생성 위치 출력 
								Box();								
								for( i = 0 ; i < g_Stage.nEnemyPositionCount ; i++ )
								{
									gotoxy( g_Stage.pEnemyPosition[i].nX*2 + 2, g_Stage.pEnemyPosition[i].nY+1);
									printf( "%s:%d", g_Title[3], i );	
								}		
								
								gotoxy( 50, 1 ); printf( "------------------------");
								gotoxy( 50, 2 ); printf( "순번 | 출현시간 | 인덱스" );
								gotoxy( 50, 3 ); printf( "------------------------");
								
								for( i = 0 ; i < 10 ; i++ )
								{
									gotoxy( 50, 4 + i ); 
									printf( " %d     %6d         %d", i + nPage*10, g_Enemy[i + nPage*10].nAppearanceTime, g_Enemy[i + nPage*10].nPosIndex );									
								}								

								nMenu = LineMenu( 3, 21, 60, g_Sub3, 5 );
								if( nMenu == 4 ) // 상위
								{
									system( "cls" );
									break;
								}

								switch( nMenu )
								{
								case 0 : // 입력
										BoxXY( 5, 5, 3);
										gotoxy( 7, 6 ); printf( "순번 : ");
										gotoxy( 7, 7 ); printf( "인덱스 : " );																		
										gotoxy( 14, 6 ); scanf( "%d", &nIndex ); fflush( stdin );
										gotoxy( 16, 7 ); scanf( "%d", &g_Enemy[ nIndex ].nPosIndex ); fflush( stdin );
										break;
								case 1 : // 수정
										BoxXY( 5, 5, 3 );

										gotoxy( 7, 6 ); printf( "순번 : "); scanf( "%d", &nIndex ); fflush( stdin );
										gotoxy( 7, 7 ); printf( "인덱스 : %d" , g_Enemy[ nIndex ].nPosIndex );
										g_Enemy[ nIndex ].nPosIndex = IntScanf( 16, 7, g_Enemy[ nIndex ].nPosIndex ); 
										break;
								case 2 : // 이전
										if( nPage - 1 < 0 )
											nPage = 0;
										else
											nPage--;
										break;										
								case 3 : // 다음
										if( nPage + 1 > 2 )
											nPage = 2;
										else
											nPage++;
										break;
								}
							}							
							break;
					}					
				}
				break;

		case 4 :
				system( "cls" );
				return 0;
		}
	}

	return 0;
}