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
    int nVal = 200;

    nVal = IntScanf( 3, 1, nVal );
    printf( "\n 수정된 값 : %d", nVal );

	_getch();
    return 0;
}
