/*
** utils.cpp
** Copyright (C) 2016 Krinkels Inc
** Contact: site: http://krinkels.org/
** Этот файл — часть InnoPCRE.
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

#include "utils.h"

INT_PTR List_Init( void )
{
	List_t	*List;
	List = ( List_t * )malloc( sizeof( List_t ) );
	List->Num = 0;
	List->Max = 10;
	List->Str = ( char ** )calloc( List->Max, sizeof( char ** ) );
	return ( INT_PTR )List;
}

void List_Add( List_t *List, char *mStr )
{
	char 	**NewP;

	if( List->Num == List->Max )
	{
		List->Max += 5;
		NewP = ( char ** ) realloc( List->Str, List->Max * sizeof( wchar_t ** ) );
		if( !NewP )
		{
			MessageBox( NULL, L"ќшибка при выделении пам¤ти", L"¬нимание!", MB_ICONERROR );
			return;
		}
		List->Str = NewP;
	}

	//char *Tmp = mStr + Len - 1;

	List->Str[ List->Num ] = _strdup( mStr );
	List->Num++;
}

void List_Free( List_t *List )
{
	int i;
	if ( !List )
		return;

	for ( i = 0; i < List->Num; i++ )
		free( List->Str[ i ] );

	free( List->Str );
	free( List );	
}
