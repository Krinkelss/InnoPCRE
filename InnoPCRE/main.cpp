#include <windows.h>
#include "pcrepp.h"
#include "Error.h"

// Инициализация
// mPattern - само выражение
// flags - флаги
Pcre *mRegEx;

void __stdcall pcreInitialize( char *mPattern, char *flags )
{
	mRegEx = new Pcre( mPattern, flags );
}

// После работы освобождаем ресурсы
void __stdcall pcredeInitialize( void )
{
	if( mRegEx )
		delete mRegEx;
}

// Перезаписываем текущее регулярное выражение
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

// Поиск с указанием смещения
int __stdcall pcreSearchOffset( const char *stuff, int OffSet )
{
	return mRegEx->search( stuff, OffSet );
}

// Поиск
int __stdcall pcreSearch( const char *stuff )
{
	return mRegEx->search( stuff );
}

// Замена. [Текст для замены] [Что] [Чем] [Флаги]
char* __stdcall pcreReplace( const char *stuff, const char *Then, const char *Thef, const char *flag )
{
	return mRegEx->Replace( stuff, Then, Thef, flag );
}

// Результат работы функции Compile. Если -1 то какая то ошибка
int __stdcall pcreResultCompile()
{
	return mRegEx->ResultCompile();
}

// Возвращает смещение в тексте
int __stdcall pcreNextPos()
{
	return mRegEx->NextPos();
}

// Возвращает смещение в тексте
char* __stdcall pcreResult()
{
//	MessageBoxA( 0, mRegEx->Result(), "", NULL );
	return ( char * )mRegEx->Result();
}

// Начало смещения для обработки
int __stdcall pcreStartMatch()
{
	return mRegEx->get_match_start();
}

// Возвращает смещение в тексте
int __stdcall pcreMatchEnd()
{
	return mRegEx->get_match_end();
}