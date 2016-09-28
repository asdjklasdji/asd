#include <windows.h>
#include <stdio.h>
#include <conio.h>

char *g_Menu[5] = {"파일  읽기", "파일  저장", "맵 정보", "적 캐릭터", "종    료" };

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



int main(void)
{
	int nMenu;

	while( 1 )
	{
		nMenu = VerticalMenu( 6, 3, g_Menu, 5 );
		gotoxy( 6, 15 );
		printf( "%s 선택", g_Menu[nMenu] );
		_getch();
		gotoxy( 6, 15 );
		printf( "                      " );
	}

	return 0;
}

