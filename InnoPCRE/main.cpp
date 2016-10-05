/*
** main.cpp
** Copyright (C) 2016 Krinkels Inc
** Contact: site: http://krinkels.org/
** Этот файл часть проекта InnoPCRE
**
** InnoPCRE - свободная программа: вы можете перераспространять ее и/или
** изменять ее на условиях Стандартной общественной лицензии GNU в том виде,
** в каком она была опубликована Фондом свободного программного обеспечения;
** либо версии 3 лицензии, либо (по вашему выбору) любой более поздней
** версии.
**
** InnoPCRE распространяется в надежде, что она будет полезной,
** но БЕЗО ВСЯКИХ ГАРАНТИЙ; даже без неявной гарантии ТОВАРНОГО ВИДА
** или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ. Подробнее см. в Стандартной
** общественной лицензии GNU.
**
** Вы должны были получить копию Стандартной общественной лицензии GNU
** вместе с этой программой. Если это не так, см.
** <http://www.gnu.org/licenses/>.)
**
*/

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
		mRegEx->operator=( mPattern );
	}
	else
		mRegEx = new Pcre( mPattern );
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
char* __stdcall pcreReplace( char *stuff, char *Then, char *Thef, char *flag )
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