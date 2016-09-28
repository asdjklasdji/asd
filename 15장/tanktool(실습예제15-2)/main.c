#include <windows.h>
#include <stdio.h>
#include <conio.h>

char *g_Menu[5] = {"����  �б�", "����  ����", "�� ����", "�� ĳ����", "��    ��" };
char *g_Sub[4] = { "�Է�", "����", "����", "����" };
char *g_Sub2[6] = { "�Է�", "����", "����", "����", "����", "����" };
char *g_Sub3[5] = { "�Է�", "����", "����", "����", "����" };
char *g_StageSubMenu[4] = { "�� ĳ���� ����", "�� ĳ���� ���� �ð�", "�� ĳ���� ���� ��ġ", "���� �޴���" };
char *g_MapSubMenu[4] = {"����", "�� ��ü ����", "������ ����", "���� �޴���" };

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

int LineMenu( int nX, int nY, char** pMenu, int nCount )
{
	int nKey, i, nMenu = 0;	
	int nMaxLength = 0, nLength;
	char cBlank[80];
	
	// �ִ� �޴� �׸��� �ִ� ���̿� ����� ���� ���� 
	for( i = 0 ; i < nCount ; i++ )
	{
		nLength = strlen( pMenu[i] );
		if( nLength > nMaxLength )
			nMaxLength = nLength;
	}	
	
	memset( cBlank, ' ', sizeof( cBlank ) );
	cBlank[ (nMaxLength+2)*nCount+1] = 0; // �� ���� ó��

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
				printf( "���� �б� ����" );
				_getch();
				break;
		case 1:
				system( "cls" );
				gotoxy( 17, 10 );
				printf( "���� ���� ����" );
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
							printf( "���� ����" );
							_getch();
							break;
					case 1:
							system( "cls" );
							gotoxy( 17, 10 );
							printf( "�� ��ü ����" );
							_getch();
							break;
					case 2:
							system( "cls" );
							gotoxy( 17, 10 );
							printf( "������ ����" );
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
										printf( "�� ĳ���� ���� ����" );
										gotoxy( 17, 10 );
										printf( "�Է� ����" );
										_getch();
										break;
								case 1:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "�� ĳ���� ���� ����" );
										gotoxy( 17, 10 );
										printf( "���� ����" );
										_getch();
										break;
								case 2:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "�� ĳ���� ���� ����" );
										gotoxy( 17, 10 );
										printf( "���� ����" );
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
										printf( "�� ĳ���� ���� �ð�");
										gotoxy( 17, 10 );
										printf( "�Է� ����" );
										_getch();
										break;
								case 1:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "�� ĳ���� ���� �ð�");
										gotoxy( 17, 10 );
										printf( "���� ����" );
										_getch();
										break;
								case 2:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "�� ĳ���� ���� �ð�");
										gotoxy( 17, 10 );
										printf( "���� ����" );
										_getch();
										break;
								case 3:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "�� ĳ���� ���� �ð�");
										gotoxy( 17, 10 );
										printf( "���� ����" );
										_getch();
										break;
								case 4:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "�� ĳ���� ���� �ð�");
										gotoxy( 17, 10 );
										printf( "���� ����" );
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
										printf( "�� ĳ���� ���� ��ġ" );
										gotoxy( 17, 10 );
										printf( "�Է� ����" );
										_getch();
										break;
								case 1:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "�� ĳ���� ���� ��ġ" );
										gotoxy( 17, 10 );
										printf( "���� ����" );
										_getch();
										break;
								case 2:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "�� ĳ���� ���� ��ġ" );
										gotoxy( 17, 10 );
										printf( "���� ����" );
										_getch();
										break;
								case 3:
										system( "cls" );
										gotoxy( 17, 8 );
										printf( "�� ĳ���� ���� ��ġ" );
										gotoxy( 17, 10 );
										printf( "���� ����" );
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



