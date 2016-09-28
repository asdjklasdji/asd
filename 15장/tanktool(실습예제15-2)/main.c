#include <windows.h>
#include <stdio.h>
#include <conio.h>

char *g_Menu[5] = {"파일  읽기", "파일  저장", "맵 정보", "적 캐릭터", "종    료" };
char *g_Sub[4] = { "입력", "수정", "삭제", "상위" };
char *g_Sub2[6] = { "입력", "수정", "삭제", "이전", "다음", "상위" };
char *g_Sub3[5] = { "입력", "수정", "이전", "다음", "상위" };
char *g_StageSubMenu[4] = { "적 캐릭터 종류", "적 캐릭터 출현 시간", "적 캐릭터 출현 위치", "상위 메뉴로" };
char *g_MapSubMenu[4] = {"편집", "맵 전체 삭제", "데이터 생성", "상위 메뉴로" };

void gotoxy( int x, int y )
{
	COORD CursorPosition = { x, y };
	SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), CursorPosition );
}

int VerticalMenu( int nX, int nY, char** pMenu, int nCount )
{
	int nKey, i, nMenu = 0, nLength, nMaxLength = 0;
	char cBlank[50];

	// 최대 메뉴 항목의 최대 길이와 지우기 위한 공백 
	for( i = 0 ; i < nCount ; i++ )
	{
		nLength = strlen( pMenu[i] );
		if( nLength > nMaxLength )
			nMaxLength = nLength;
	}	
	
	memset( cBlank, ' ', sizeof( cBlank ) );
	cBlank[nMaxLength + 6 ] = 0; // 널 문자 처리
		 
	while( 1 )
	{	
		for( i = 0 ; i < nCount ; i++ )
		{	
			gotoxy( nX - 2, nY + i*2 ); 
			printf( "%s", cBlank );
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
}

int LineMenu( int nX, int nY, char** pMenu, int nCount )
{
	int nKey, i, nMenu = 0;	
	int nMaxLength = 0, nLength;
	char cBlank[80];
	
	// 최대 메뉴 항목의 최대 길이와 지우기 위한 공백 
	for( i = 0 ; i < nCount ; i++ )
	{
		nLength = strlen( pMenu[i] );
		if( nLength > nMaxLength )
			nMaxLength = nLength;
	}	
	
	memset( cBlank, ' ', sizeof( cBlank ) );
	cBlank[ (nMaxLength+2)*nCount+1] = 0; // 널 문자 처리

	while( 1 )
	{		
		gotoxy( nX - 2, nY ) ; 
		printf( "%s", cBlank );
		for( i = 0 ; i < nCount ; i++ )
		{
			gotoxy( nX + i*(nMaxLength + 2), nY); 
			printf( "%s", pMenu[i] );
		}
		
		gotoxy( nMenu*(nMaxLength + 2) + nX -1, nY ); printf( "[" );
		gotoxy( nMenu*(nMaxLength + 2) + nX + nMaxLength, nY); printf( "]" );
		
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
}

int main(void)
{
	int nMenu;	

	while( 1 )
	{
		system( "cls" );
		nMenu = VerticalMenu( 17, 6, g_Menu, 5 );
		
		if( nMenu == 4 )
			break;

		switch( nMenu )
		{
		case 0:
				system( "cls" );
				gotoxy( 17, 10 );
				printf( "파일 읽기 실행" );
				_getch();
				break;
		case 1:
				system( "cls" );
				gotoxy( 17, 10 );
				printf( "파일 저장 실행" );
				_getch();
				break;
		case 2:
				while( 1 )
				{
					system( "cls" );
					nMenu = VerticalMenu( 17, 6, g_MapSubMenu, 4 );
					if( nMenu == 3 )
						break;

					switch( nMenu )
					{
					case 0: 
							system( "cls" );
							gotoxy( 17, 10 );
							printf( "편집 상태" );
							_getch();
							break;
					case 1:
							system( "cls" );
							gotoxy( 17, 10 );
							printf( "맵 전체 삭제" );
							_getch();
							break;
					case 2:
							system( "cls" );
							gotoxy( 17, 10 );
							printf( "데이터 생성" );
							_getch();
							break;					
					}
				}
				break;
		case 3:				
				while( 1 )
				{
					system( "cls" );
					nMenu = VerticalMenu( 17, 6, g_StageSubMenu, 4 );
					if( nMenu == 3 )
						break;
					
					switch( nMenu )
					{	
					case 0:
							
							while( 1 )
							{
								system( "cls" );
								nMenu = LineMenu( 5, 5, g_Sub, 4 );
								if( nMenu == 3 )
									break;
								
								switch( nMenu )
								{
								case 0:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "적 캐릭터 종류 실행" );
										gotoxy( 17, 10 );
										printf( "입력 상태" );
										_getch();
										break;
								case 1:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "적 캐릭터 종류 실행" );
										gotoxy( 17, 10 );
										printf( "수정 상태" );
										_getch();
										break;
								case 2:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "적 캐릭터 종류 실행" );
										gotoxy( 17, 10 );
										printf( "삭제 상태" );
										_getch();
										break;
								}
							}
								
							break;
					case 1:
							
							while( 1 )
							{	
								system( "cls" );
								nMenu = LineMenu( 5, 5, g_Sub2, 6 );
								if( nMenu == 5 )
									break;

								switch( nMenu )
								{
								case 0:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "적 캐릭터 출현 시간");
										gotoxy( 17, 10 );
										printf( "입력 상태" );
										_getch();
										break;
								case 1:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "적 캐릭터 출현 시간");
										gotoxy( 17, 10 );
										printf( "수정 상태" );
										_getch();
										break;
								case 2:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "적 캐릭터 출현 시간");
										gotoxy( 17, 10 );
										printf( "삭제 상태" );
										_getch();
										break;
								case 3:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "적 캐릭터 출현 시간");
										gotoxy( 17, 10 );
										printf( "이전 상태" );
										_getch();
										break;
								case 4:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "적 캐릭터 출현 시간");
										gotoxy( 17, 10 );
										printf( "다음 상태" );
										_getch();
										break;
								}
							}
							break;
					case 2:
							while(1)
							{
								system( "cls" );
								nMenu = LineMenu( 5, 5, g_Sub3, 5 );
								if( nMenu == 4 )
									break;
								
								switch( nMenu )
								{
								case 0:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "적 캐릭터 출현 위치" );
										gotoxy( 17, 10 );
										printf( "입력 상태" );
										_getch();
										break;
								case 1:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "적 캐릭터 출현 위치" );
										gotoxy( 17, 10 );
										printf( "수정 상태" );
										_getch();
										break;
								case 2:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "적 캐릭터 출현 위치" );
										gotoxy( 17, 10 );
										printf( "이전 상태" );
										_getch();
										break;
								case 3:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "적 캐릭터 출현 위치" );
										gotoxy( 17, 10 );
										printf( "다음 상태" );
										_getch();
										break;
								}
							}
							break;
					}
				}
				break;		
		}
	}
	return 0;
}



