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
	int nEnemyCount;				// �� ĳ���� ��
	int nEnemyPositionCount;		// �� ���� ��ġ ����
	POS_XY *pEnemyPosition;			// �� ���� ��ġ �迭
	int nBossX, nBossY;				// ���� �ʱ� ��ġ
	int nPlayerX, nPlayerY;			// �÷��̾� �ʱ� ��ġ
	int nEnemyTypeCount;			// �� ĳ���� ���� ����
} STAGE;

typedef struct _ENEMY_TYPE
{	
	int			 nLife;		// �����
	unsigned int nMoveTime;	// �ӷ�
	unsigned int nFireTime;  // �Ѿ� �߻� �ð� ����	
} ENEMY_TYPE;

typedef struct _ENEMY
{
	int nTypeIndex;		// Ÿ�� �ε���
	int nPosIndex;		// ���� �ʱ� ��ġ �ε���
	// int nX, nY;      // �̵� ��ǥ
	// int nState;		// ���� ����
	// int nLife;		// �����
	// int nDirect;		// ����
	unsigned int nAppearanceTime; // ���� �ð�
	// unsigned int nOldTime; // ���� �̵� �ð�
} ENEMY;

ENEMY_TYPE g_EnemyType[10]; // ��ũ ������ �ִ� 10������ ���� �Ѵ�.
ENEMY g_Enemy[30];
int g_nMap[MAP_ROW][MAP_COL];
STAGE g_Stage;

char *g_Menu[5] = {"����  �б�", "����  ����", "�� ����", "�� ĳ����", "��    ��" };
char *g_Sub[4] = { "�Է�", "����", "����", "����" };
char *g_Sub2[6] = { "�Է�", "����", "����", "����", "����", "����" };
char *g_Sub3[5] = { "�Է�", "����", "����", "����", "����" };
char *g_StageSubMenu[4] = { "�� ĳ���� ����", "�� ĳ���� ���� �ð�", "�� ĳ���� ���� ��ġ", "���� �޴���" };
char *g_MapSubMenu[4] = {"����", "�� ��ü ����", "������ ����", "����" };
char g_Title[6][3] = { "  ", "��", "��", "��", "��", "��" };

void gotoxy( int x, int y )
{
	COORD CursorPosition = { x, y };
	SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), CursorPosition );
}

void Box()
{
	gotoxy( 0, 0 );
	printf( "��������������������������������������������������\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��                                              ��\n" );
	printf( "��������������������������������������������������\n" );
}

void BoxXY(int nX, int nY, int nHeight )
{
	int i;

	gotoxy( nX, nY); printf( "������������������������������������������\n" );

	for( i = 1 ; i <= nHeight ; i++ )
	{
		gotoxy( nX, nY + i );
		printf( "��                                      ��\n" );
	}
	
	gotoxy( nX, nY + nHeight );
	printf( "������������������������������������������\n" );
}

int VerticalMenu( int nX, int nY, char** pMenu, int nCount )
{
	 int nKey, i, nMenu = 0, nLength, nMaxLength = 0;
	 char *pBlank;

	// �ִ� �޴� �׸��� �ִ� ���̿� ����� ���� ���� 
	for( i = 0 ; i < nCount ; i++ )
	{
		nLength = strlen( pMenu[i] );
		if( nLength > nMaxLength )
			nMaxLength = nLength;
	}	

	// �������� ���پ� ����� ���� �κ�
	pBlank = (char*)malloc( nMaxLength + 5 );
	memset( pBlank, ' ', nMaxLength + 5 );
	pBlank[nMaxLength+4] = 0; // �ι��� ���ڿ� ó��
	 
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
		 case 72 : // ȭ��ǥ ���� Ű
				if( nMenu - 1 >= 0 )
					nMenu--;												
				break;
		 case 80 : // ȭ��ǥ �Ʒ� Ű
				if( nMenu + 1 < nCount )
					nMenu++;
				break;				
		 case 13 : // ����Ű (CR)				
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

	// �ִ� �޴� �׸��� �ִ� ���̿� ����� ���� ���� 
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
		 case 75 : // ȭ��ǥ ���� Ű
				nMenu--;
				if( nMenu < 0 )
					nMenu = 0;							
				break;
		 case 77 : // ȭ��ǥ ������ Ű
				nMenu++;
				if( nMenu == nCount )
					nMenu = nCount - 1 ;				
				break;
		 case 13 : // ����Ű (CR)				
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
	 nLength = strlen( strTemp ); // ���� ������ ����

	 // ���� ���
     gotoxy( x, y ); printf( "%s", strTemp );
	 gotoxy( x, y ); 	
	 while( 1 )
	 {		
		cMunja = _getch();
		switch( cMunja )
		{
		case 13 : // ����Ű
				return atoi( strTemp );
				
		case 8 : // �齺���̽� ó��
			    if( nIndex - 1 < 0 )
					nIndex = 0;
				else
				{
					nIndex--;
					strTemp[nIndex] = 0; // �ι��ڷ� ���� ���� ==> �����ִ� ȿ��						
				}				
				break;
		default :
			    if( nIndex + 1 < 19 ) // 18���� �̻� ��� ���� �Է� ���� �ʴ´�.
				{					
					strTemp[nIndex++] = cMunja;
					strTemp[nIndex] = 0; // �ι���
				}
		}

		 // Note: ���̸�ŭ ȭ���� �����.
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
		printf( "�� ��ũ ���� ���� �� ��" );
		nMenu = VerticalMenu( 17, 6, g_Menu, 5 );
		
		switch( nMenu )
		{
		case 0 : // ���� �б�
				system( "cls" );
				BoxXY( 5, 5, 4 );
				gotoxy( 7, 7 ); printf( "���� �б� : " );
				scanf( "%s", strFileName ); fflush( stdin );
				
				fp = fopen( strFileName, "rt" );
				if( fp == NULL )
				{
					BoxXY( 5, 5, 4 );
					gotoxy( 7, 7 ); printf( "������ �����ϴ�!" );
					_getch(); 
					fflush( stdin );
				}else{
					fscanf( fp, "�� ĳ���ͼ� : %d\n", &g_Stage.nEnemyCount );
					fscanf( fp, "�� ĳ���Ͱ� ������ ��ġ ���� : %d\n", &g_Stage.nEnemyPositionCount );
					
					if( g_Stage.pEnemyPosition != NULL )
						free( g_Stage.pEnemyPosition );
					g_Stage.pEnemyPosition = (POS_XY*)malloc( sizeof( POS_XY )*g_Stage.nEnemyPositionCount );
					for( i = 0 ; i < g_Stage.nEnemyPositionCount ; i++ )
						fscanf( fp, "%d %d\n", &g_Stage.pEnemyPosition[i].nX, &g_Stage.pEnemyPosition[i].nY );
					
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

					system( "cls" );
					BoxXY( 5, 5, 4 );
					gotoxy( 7, 7 ); printf( "���� �б� �Ϸ�" );
					_getch(); 
					fflush( stdin );
				}
				break;
		case 1 : // ���� ����
				system( "cls" );
				BoxXY( 5, 5, 4 );
				gotoxy( 7, 7 ); printf( "���� ���� : " );
				scanf( "%s", strFileName ); fflush( stdin );
				
				fp = fopen( strFileName, "wt" );
				fprintf( fp, "�� ĳ���ͼ� : %d\n", g_Stage.nEnemyCount );
				fprintf( fp, "�� ĳ���Ͱ� ������ ��ġ ���� : %d\n", g_Stage.nEnemyPositionCount );
			
				for( i = 0 ; i < g_Stage.nEnemyPositionCount ; i++ )
					fprintf( fp, "%d %d\n", g_Stage.pEnemyPosition[i].nX, g_Stage.pEnemyPosition[i].nY );
				fprintf( fp, "���� �ʱ� ��ġ : %d %d\n", g_Stage.nBossX, g_Stage.nBossY );
				fprintf( fp, "�÷��̾� �ʱ� ��ġ : %d %d\n", g_Stage.nPlayerX , g_Stage.nPlayerY );
				fprintf( fp, "�� ĳ���� ���� ���� : %d\n", g_Stage.nEnemyTypeCount );

				for( i = 0 ; i < g_Stage.nEnemyTypeCount ; i++ )
					fprintf( fp, "�����: %d �̵��ð� ����: %d �̻���: %d\n", g_EnemyType[i].nLife, g_EnemyType[i].nMoveTime, g_EnemyType[i].nFireTime );

				for( i = 0 ; i < g_Stage.nEnemyCount ; i++ )
					fprintf( fp, "Ÿ���ε���: %d ��ĳ���� ���� �ε���: %d ���� �ð�: %d\n", g_Enemy[i].nTypeIndex, g_Enemy[i].nPosIndex, g_Enemy[i].nAppearanceTime );

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
				gotoxy( 7, 7 ); printf( "���� ���� �Ϸ�" );
				_getch(); 
				fflush( stdin );
				break;
		case 2 : // �� ����
				nPositionCount = nRow = nCol = 0;
				for( i = 0 ; i < MAP_ROW ; i++ )
				{
					for( j = 0 ; j < MAP_COL ; j++ )
					{
						if( g_nMap[i][j] == 3 )
							nPositionCount++;
					}
				}

				gotoxy( 52, 1 );  printf( "�� �޴� �׸� ��" );		
				gotoxy( 50, 2 );  printf( "��������������������" );
				gotoxy( 50, 3 );  printf( "��                ��" );
				gotoxy( 50, 4 );  printf( "��                ��" );
				gotoxy( 50, 5 );  printf( "��                ��" );
				gotoxy( 50, 6 );  printf( "��                ��" );
				gotoxy( 50, 7 );  printf( "��                ��" );
				gotoxy( 50, 8 );  printf( "��                ��" );
				gotoxy( 50, 9 );  printf( "��                ��" );
				gotoxy( 50, 10 ); printf( "��������������������" );
				gotoxy( 52, 12 ); printf( "1: ��� ��" );
				gotoxy( 52, 13 ); printf( "2: ��ȣ�� ��" ); 
				gotoxy( 52, 14 ); printf( "3: �� ��ũ ���� ��ġ ��" );
				gotoxy( 52, 15 ); printf( "4: �Ʊ� ������ ��ġ ���� ��" );
				gotoxy( 52, 16 ); printf( "5: ���ΰ� ��ġ ���� ��" );					
				gotoxy( 52, 17 ); printf( "6: ����" );
				gotoxy( 52, 18 ); printf( "7: �޴���" );	

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
					case 0 : // ���� ����
							while( 1 )
							{							
								gotoxy( nCol*2 + 2, nRow+1 );
								nKey = _getch();
								if( nKey == '7' )
									break;
								
								switch( nKey )
								{
								case 72 : // ����
										if( nRow - 1 >= 0 )
											nRow--;
										break;
								case 80 : // �Ʒ�
										if( nRow + 1 < MAP_ROW )
											nRow++; 																		
										break;
								case 77 : // ������
										if( nCol + 1 < MAP_COL )
											nCol++;
										break;
								case 75 : // ����
										if( nCol - 1 >= 0 )
											nCol--;
										break;
								case '1' : // ���
										g_nMap[nRow][nCol] = 1;
										break;
								case '2' : // ��ȣ ���
										g_nMap[nRow][nCol] = 2;
										break;
								case '3' : // �� ĳ���� ���� ��ġ
										g_nMap[nRow][nCol] = 3;
										nPositionCount++;
										break;
								case '4' : // ���� ����
										g_nMap[nRow][nCol] = 4;
										g_Stage.nBossX = nCol;
										g_Stage.nBossY = nRow;
										break;
								case '5' : // ���ΰ� ����
										g_nMap[nRow][nCol] = 5;
										g_Stage.nPlayerX = nCol;
										g_Stage.nPlayerY = nRow;
										break;

								case '6' : // ����
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
							// �� ��ü ���� 
							memset( g_nMap, 0, sizeof( int )*MAP_ROW*MAP_COL );
							break;
					case 2 :
							// ������ ����						
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
							gotoxy( 7, 7 ); printf( "������ ���� �Ϸ�" );
							_getch();
							break;
					}					
				}
				break;
		case 3 : // �� ĳ���� ����			   
				while( 1 )
				{ 
					Box();
					gotoxy( 19, 2 );
					printf( "%s", "�� ĳ����" );
					nMenu = VerticalMenu( 13, 6, g_StageSubMenu, 4 );
					if( nMenu == 3 ) // ���� �޴���
						break;

					switch( nMenu )
					{
					case 0 : // ��ĳ���� ���� �� �Ӽ� ����
							while( 1 )
							{
								Box();
								gotoxy( 2, 4 ); printf( "----------------------------------------------");
								gotoxy( 2, 5 ); printf( " �ε���   �����  �̵��ð�����  �̻��Ϲ߻�ð�" );
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
								case 0 : // �Է�
										BoxXY( 5, 5, 5);
										gotoxy( 7, 6 ); printf( "�ε��� : ");
										gotoxy( 7, 7 ); printf( "����� : " );
										gotoxy( 7, 8 ); printf( "�̵� �ð� ���� : " );
										gotoxy( 7, 9 ); printf( "�̻��� �߻� �ð� ���� : " );										
										gotoxy( 17, 6 ); scanf( "%d", &nIndex ); fflush( stdin );
										gotoxy( 17, 7 ); scanf( "%d", &g_EnemyType[ nIndex ].nLife ); fflush( stdin );
										gotoxy( 25, 8 ); scanf( "%d", &g_EnemyType[ nIndex ].nMoveTime ); fflush( stdin );
										gotoxy( 31, 9 ); scanf( "%d", &g_EnemyType[ nIndex ].nFireTime ); fflush( stdin );		
										g_Stage.nEnemyTypeCount++;
										break;
								case 1 : // ����
										BoxXY( 5, 5, 6 );
										gotoxy( 7, 6 ); printf( "���� �ε��� : "); scanf( "%d", &nIndex ); fflush( stdin );
										gotoxy( 7, 7 ); printf( "����� : %d" , g_EnemyType[ nIndex ].nLife );
										gotoxy( 7, 8 ); printf( "�̵� �ð� ���� : %d", g_EnemyType[ nIndex ].nMoveTime );
										gotoxy( 7, 9 ); printf( "�̻��� �߻� �ð� ���� : %d", g_EnemyType[ nIndex ].nFireTime );										
										
										g_EnemyType[ nIndex ].nLife = IntScanf( 16, 7, g_EnemyType[ nIndex ].nLife ); 
										g_EnemyType[ nIndex ].nMoveTime = IntScanf( 24, 8, g_EnemyType[ nIndex ].nMoveTime ); 
										g_EnemyType[ nIndex ].nFireTime = IntScanf( 31, 9, g_EnemyType[ nIndex ].nFireTime ); 
										break;
								case 2 : // ����
										BoxXY( 5, 5, 5);
										gotoxy( 7, 6 ); printf( " ���� �ε��� : "); scanf( "%d", &nIndex );
										memset( &g_EnemyType[nIndex], 0, sizeof( ENEMY_TYPE ) );				
										g_Stage.nEnemyTypeCount--;
										break;
								
								}
							}
							break;
					case 1 :  // �� ĳ���� ���� �ð� ����
						    nPage = 0;
							while( 1 )
							{
								Box();
								gotoxy( 2, 4 ); printf( "---------------------------------");
								gotoxy( 2, 5 ); printf( " ����      ����      ���� �ð�" );
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
								case 0 : // �Է�
										BoxXY( 5, 5, 5);
										gotoxy( 7, 6 ); printf( "���� : ");
										gotoxy( 7, 7 ); printf( "���� : " );
										gotoxy( 7, 8 ); printf( "���� �ð� : " );
										gotoxy( 14, 6 ); scanf( "%d", &nIndex ); fflush( stdin );
										gotoxy( 14, 7 ); scanf( "%d", &g_Enemy[ nIndex ].nTypeIndex ); fflush( stdin );
										gotoxy( 19, 8 ); scanf( "%d", &g_Enemy[ nIndex ].nAppearanceTime ); fflush( stdin );			
										g_Stage.nEnemyCount++;
										break;
								case 1 : // ����
										BoxXY( 5, 5, 6 );
										gotoxy( 7, 6 ); printf( "���� : "); scanf( "%d", &nIndex ); fflush( stdin );
										gotoxy( 7, 7 ); printf( "���� : %d" , g_Enemy[ nIndex ].nTypeIndex );
										gotoxy( 7, 8 ); printf( "���� �ð� : %d", g_Enemy[ nIndex ].nAppearanceTime );

										g_Enemy[ nIndex ].nTypeIndex = IntScanf( 14, 7, g_Enemy[ nIndex ].nTypeIndex ); 
										g_Enemy[ nIndex ].nAppearanceTime = IntScanf( 19, 8, g_Enemy[ nIndex ].nAppearanceTime );									
										break;
								case 2 : // ����
										BoxXY( 5, 5, 5);
										gotoxy( 7, 6 ); printf( " ���� �ε��� : "); scanf( "%d", &nIndex );
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
					case 2 : // �� ĳ���� �ʱ� ��ġ ����						
							nPage = 0;

							while( 1 )
							{
								system( "cls" );
								// Note: �� ĳ���� ���� ��ġ ��� 
								Box();								
								for( i = 0 ; i < g_Stage.nEnemyPositionCount ; i++ )
								{
									gotoxy( g_Stage.pEnemyPosition[i].nX*2 + 2, g_Stage.pEnemyPosition[i].nY+1);
									printf( "%s:%d", g_Title[3], i );	
								}		
								
								gotoxy( 50, 1 ); printf( "------------------------");
								gotoxy( 50, 2 ); printf( "���� | �����ð� | �ε���" );
								gotoxy( 50, 3 ); printf( "------------------------");
								
								for( i = 0 ; i < 10 ; i++ )
								{
									gotoxy( 50, 4 + i ); 
									printf( " %d     %6d         %d", i + nPage*10, g_Enemy[i + nPage*10].nAppearanceTime, g_Enemy[i + nPage*10].nPosIndex );									
								}								

								nMenu = LineMenu( 3, 21, 60, g_Sub3, 5 );
								if( nMenu == 4 ) // ����
								{
									system( "cls" );
									break;
								}

								switch( nMenu )
								{
								case 0 : // �Է�
										BoxXY( 5, 5, 3);
										gotoxy( 7, 6 ); printf( "���� : ");
										gotoxy( 7, 7 ); printf( "�ε��� : " );																		
										gotoxy( 14, 6 ); scanf( "%d", &nIndex ); fflush( stdin );
										gotoxy( 16, 7 ); scanf( "%d", &g_Enemy[ nIndex ].nPosIndex ); fflush( stdin );
										break;
								case 1 : // ����
										BoxXY( 5, 5, 3 );

										gotoxy( 7, 6 ); printf( "���� : "); scanf( "%d", &nIndex ); fflush( stdin );
										gotoxy( 7, 7 ); printf( "�ε��� : %d" , g_Enemy[ nIndex ].nPosIndex );
										g_Enemy[ nIndex ].nPosIndex = IntScanf( 16, 7, g_Enemy[ nIndex ].nPosIndex ); 
										break;
								case 2 : // ����
										if( nPage - 1 < 0 )
											nPage = 0;
										else
											nPage--;
										break;										
								case 3 : // ����
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