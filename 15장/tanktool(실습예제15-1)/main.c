#include <windows.h>
#include <stdio.h>
#include <conio.h>

char *g_Menu[5] = {"����  �б�", "����  ����", "�� ����", "�� ĳ����", "��    ��" };

void gotoxy( int x, int y )
{
	COORD CursorPosition = { x, y };
	SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), CursorPosition );
}

int VerticalMenu( int nX, int nY, char** pMenu, int nCount )
{
	int nKey, i, nMenu = 0, nLength, nMaxLength = 0;
	char cBlank[50];

	// �ִ� �޴� �׸��� �ִ� ���̿� ����� ���� ���� 
	for( i = 0 ; i < nCount ; i++ )
	{
		nLength = strlen( pMenu[i] );
		if( nLength > nMaxLength )
			nMaxLength = nLength;
	}	
	
	memset( cBlank, ' ', sizeof( cBlank ) );
	cBlank[nMaxLength + 6 ] = 0; // �� ���� ó��
		 
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
}



int main(void)
{
	int nMenu;

	while( 1 )
	{
		nMenu = VerticalMenu( 6, 3, g_Menu, 5 );
		gotoxy( 6, 15 );
		printf( "%s ����", g_Menu[nMenu] );
		_getch();
		gotoxy( 6, 15 );
		printf( "                      " );
	}

	return 0;
}

