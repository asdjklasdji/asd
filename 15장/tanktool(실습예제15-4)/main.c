#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

void gotoxy( int x, int y )
{
     COORD CursorPosition = { x, y };
     SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), CursorPosition );
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
    int nVal = 200;

    nVal = IntScanf( 3, 1, nVal );
    printf( "\n ������ �� : %d", nVal );

	_getch();
    return 0;
}
