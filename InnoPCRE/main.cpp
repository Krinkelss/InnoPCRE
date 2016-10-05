#include <windows.h>
#include "pcrepp.h"
#include "Error.h"

// �������������
// mPattern - ���� ���������
// flags - �����
Pcre *mRegEx;

void __stdcall pcreInitialize( char *mPattern, char *flags )
{
	mRegEx = new Pcre( mPattern, flags );
}

// ����� ������ ����������� �������
void __stdcall pcredeInitialize( void )
{
	if( mRegEx )
		delete mRegEx;
}

// �������������� ������� ���������� ���������
void __stdcall pcreReInitialize( char *mPattern, char *flags )
{
	if( mRegEx )
	{
		delete mRegEx;
		mRegEx = new Pcre( mPattern, ( char * )flags );
	}
	else
		mRegEx = new Pcre( mPattern, ( char * )flags );
}

// ����� � ��������� ��������
int __stdcall pcreSearchOffset( const char *stuff, int OffSet )
{
	return mRegEx->search( stuff, OffSet );
}

// �����
int __stdcall pcreSearch( const char *stuff )
{
	return mRegEx->search( stuff );
}

// ������. [����� ��� ������] [���] [���] [�����]
char* __stdcall pcreReplace( const char *stuff, const char *Then, const char *Thef, const char *flag )
{
	return mRegEx->Replace( stuff, Then, Thef, flag );
}

// ��������� ������ ������� Compile. ���� -1 �� ����� �� ������
int __stdcall pcreResultCompile()
{
	return mRegEx->ResultCompile();
}

// ���������� �������� � ������
int __stdcall pcreNextPos()
{
	return mRegEx->NextPos();
}

// ���������� �������� � ������
char* __stdcall pcreResult()
{
//	MessageBoxA( 0, mRegEx->Result(), "", NULL );
	return ( char * )mRegEx->Result();
}

// ������ �������� ��� ���������
int __stdcall pcreStartMatch()
{
	return mRegEx->get_match_start();
}

// ���������� �������� � ������
int __stdcall pcreMatchEnd()
{
	return mRegEx->get_match_end();
}