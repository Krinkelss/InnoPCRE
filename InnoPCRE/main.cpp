/*
** main.cpp
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

#include <windows.h>
#include "config.h"
#include "resource.h"

#define PCRE2_CODE_UNIT_WIDTH 0
#include "pcre2.h"

#include "Error.h"
#include "utils.h"

#define REGTEST_MAX_LENGTH32 4096

char *_expression;
BOOL global_t;

pcre2_compile_context_8 *ccontext32;
pcre2_match_context_8 *mcontext32;
pcre2_match_data_8 *match_data;
pcre2_code_8 *p_pcre2;
static pcre2_jit_stack_8 *stack32;

int errorcode;
PCRE2_SIZE erroroffset;
int length32;
int num_matches;
int nOffset;

List_t *Result_t;

PCRE2_UCHAR8 regtest_buf32[ REGTEST_MAX_LENGTH32 ] = {0};
void **stringlist;

int convert_utf8_to_utf32( PCRE2_SPTR8 input, PCRE2_UCHAR8 *output, int *offsetmap, int max_length );
int copy_char8_to_char32( PCRE2_SPTR8 input, PCRE2_UCHAR8 *output, int max_length );

BOOL Compile( int flags );
BOOL dosearch( const char *stuff, int OffSet );

void setstack32( pcre2_match_context_8 *mcontext );

// Инициализация
// mPattern - само выражение
// flags - флаги
BOOL __stdcall pcreInitialize( char *mPattern, char *flags )
{
	_expression = mPattern;
	global_t = FALSE;
	unsigned int FLAG = 0;

	nOffset = 0;

	int flaglen = strlen( flags );


	for( int flag = 0; flag < flaglen; flag++ ) 
	{
		switch( flags[ flag ] ) 
		{
			case 'i': FLAG |= PCRE2_CASELESS;					break;		// Do caseless matching
			case 'm': FLAG |= PCRE2_MULTILINE;					break;		// ^ and $ match newlines within data
			case 's': FLAG |= PCRE2_DOTALL;						break;		// . matches anything including NL
			case 'x': FLAG |= PCRE2_EXTENDED;					break;		// Ignore white space and # comments
			case 'u': FLAG |= PCRE2_UTF;						break;		// Treat pattern and subjects as UTF strings
			case 'g':                         global_t = TRUE;	break;		// Глобальный поиск
		}
	}

	if( !Compile( FLAG ) )
		return FALSE;

	return TRUE;
}

// После работы освобождаем ресурсы
void __stdcall pcredeInitialize( void )
{
	pcre2_code_free_8( p_pcre2 );
	pcre2_match_data_free_8( match_data );
	pcre2_match_context_free_8( mcontext32 );
	//pcre2_substring_list_free_8( ( PCRE2_UCHAR8 * )stringlist );
	pcre2_substring_list_free_8( ( PCRE2_SPTR8 * )stringlist );

	if( Result_t != NULL )
			List_Free( Result_t );
}

BOOL Compile( int flags )
{
	//memset( regtest_buf32, 0, sizeof( regtest_buf32 ) );
	convert_utf8_to_utf32( ( PCRE2_SPTR8 )_expression, regtest_buf32, NULL, REGTEST_MAX_LENGTH32 ); // Сам regexp

	ccontext32 = pcre2_compile_context_create_8( NULL );
	if( !ccontext32 )
	{
		Msg( "!!!", "Error pcre2_compile_context_create" );
		return FALSE;
	}

	p_pcre2 = pcre2_compile_8( regtest_buf32, PCRE2_ZERO_TERMINATED, flags, &errorcode, &erroroffset, ccontext32 );
	if( p_pcre2 == NULL )
	{
		pcre2_compile_context_free_8( ccontext32 );

		PCRE2_UCHAR8 error[ 256 ];
		pcre2_get_error_message_8( errorcode, error, sizeof( error ) );		
		Msg( "p_pcre2 == NULL", "PCRE2 compilation failed at offset %d: %s\n", ( int ) erroroffset, ( char * ) error );
		return FALSE;
	}

	pcre2_compile_context_free_8( ccontext32 );

	//setstack32( NULL );

	match_data = pcre2_match_data_create_8( ( uint32_t )p_pcre2, NULL );	
	mcontext32 = pcre2_match_context_create_8( NULL );

	if( !match_data || !mcontext32 )
		return FALSE;
	
	return TRUE;
}

// Поиск.
// stuff - текст где будет искаться значение
// OffSet - смещение для поиска, ежели будет глобальный поиск, если нет то просто 0
int __stdcall search( const char *stuff, int OffSet )
{
	return dosearch( stuff, OffSet );
}

int dosearch( const char *stuff, int OffSet )
{
	num_matches = 0;

	Msg( "NextPos", "OffSet = %d", OffSet );

	memset( regtest_buf32, 0, sizeof( regtest_buf32 ) );

	length32 = copy_char8_to_char32( ( PCRE2_SPTR8 )stuff, regtest_buf32, REGTEST_MAX_LENGTH32 ); // Текст для обработки

	if( pcre2_jit_compile_8( p_pcre2, PCRE2_JIT_COMPLETE ) )
	{
		Msg( "!!!", "32 bit: JIT compiler does not support" );
		return 0;
	}	
	setstack32( mcontext32 );
	errorcode = pcre2_jit_match_8( p_pcre2, regtest_buf32, length32, OffSet, 0, match_data, mcontext32 );	
	if( errorcode < 0 )
	{		
		return errorcode;
	}else
	if( errorcode >= 1 )
	{		
		if( Result_t != NULL )
			List_Free( Result_t );

		num_matches = errorcode;

		//Result_t = new std::vector<std::string>;
		Result_t = ( List_t * )List_Init();

		PCRE2_SIZE *ovector = pcre2_get_ovector_pointer_8( match_data );
		nOffset = ( int )ovector[ 1 ];	

		int res = pcre2_substring_list_get_8( match_data, ( PCRE2_UCHAR8 *** )&stringlist, NULL );
		if( res == 0 )
		{
			for( int i = 0; i < errorcode; ++i )
				List_Add( Result_t, ( char * )stringlist[ i ] );
				//Result_t->push_back( ( char * )stringlist[ i ] );				
		}
		else
		{
			 Msg( "!pcre2_substring_list_get", "Failure to get a substring" );
			 return 0;
		}

		if( !global_t )
			return 0;

		return 1;
	}

	return 0;
}

// Возвращает результат поиска. 0 - общий результать. Если скобок несколько, то нужно ставить число больше нуля, но не превышающее значение возвращаемое AllMatch()
char * __stdcall get_match( int pos )
{
	//Msg( "get_match", "pos = %d", pos );
	//Msg( "!!!", "Result_t->Str[ pos ] = %s", Result_t->Str[ pos ] );
	
	if( pos >= 0 && pos < num_matches ) 
	{
		//std::vector<std::string>::iterator P = Result_t->begin() + pos;
		//Msg( "!!!", "num_matches = %s", ( char * )Result_t-> );
		return Result_t->Str[ pos ];
	}
	else 
	{
		//throw exception("Pcre2::get_match( int ): out of range");
		Msg( "Error", "get_match( int ): out of range" );
		return NULL;
	}
}

// Возвращает число элементов в массив с результатом
size_t __stdcall AllMatch()
{
	return Result_t->Num;
}

// Позиция для следующего поиска
int __stdcall NextPos( void )
{	
	return nOffset;
}

static pcre2_jit_stack_8* callback32( void *arg )
{
	return ( pcre2_jit_stack_8 * )arg;
}

static pcre2_jit_stack_8 *getstack32( void )
{
	if( !stack32 )
		stack32 = pcre2_jit_stack_create_8( 1, 1024 * 1024, NULL );
	return stack32;
}

void setstack32( pcre2_match_context_8 *mcontext )
{
	if( !mcontext ) 
	{
		if( stack32 )
			pcre2_jit_stack_free_8( stack32 );
		stack32 = NULL;
		return;
	}

	pcre2_jit_stack_assign_8( mcontext, callback32, getstack32() );
}

int convert_utf8_to_utf32( PCRE2_SPTR8 input, PCRE2_UCHAR8 *output, int *offsetmap, int max_length )
{
	PCRE2_SPTR8 iptr = input;
	PCRE2_UCHAR8 *optr = output;
	unsigned int c;

	if( max_length == 0 )
		return 0;

	while( *iptr && max_length > 1 ) 
	{
		c = 0;
		if( offsetmap )
			*offsetmap++ = ( int )( iptr - input );

		if( *iptr < 0xc0 )
			c = *iptr++;
		else 
		if( !( *iptr & 0x20 ) ) 
		{
			c = ( ( iptr[ 0 ] & 0x1f ) << 6 ) | ( iptr[ 1 ] & 0x3f );
			iptr += 2;
		} else 
		if( !( *iptr & 0x10 ) ) 
		{
			c = ( ( iptr[ 0 ] & 0x0f ) << 12 ) | ( ( iptr[ 1 ] & 0x3f ) << 6 ) | ( iptr[ 2 ] & 0x3f );
			iptr += 3;
		} else 
		if( !( *iptr & 0x08 ) ) 
		{
			c = ( ( iptr[ 0 ] & 0x07 ) << 18 ) | ( ( iptr[ 1 ] & 0x3f ) << 12 ) | ( ( iptr[ 2 ] & 0x3f ) << 6 ) | ( iptr[ 3 ] & 0x3f );
			iptr += 4;
		}

		*optr++ = c;
		max_length--;
	}
	if( offsetmap )
		*offsetmap = ( int )( iptr - input );
	*optr = 0;
	return ( int )( optr - output );
}

int copy_char8_to_char32( PCRE2_SPTR8 input, PCRE2_UCHAR8 *output, int max_length )
{
	PCRE2_SPTR8 iptr = input;
	PCRE2_UCHAR8 *optr = output;

	if( max_length == 0 )
		return 0;

	while( *iptr && max_length > 1 ) 
	{
		*optr++ = *iptr++;
		max_length--;
	}
	*optr = '\0';
	return ( int )( optr - output );
}