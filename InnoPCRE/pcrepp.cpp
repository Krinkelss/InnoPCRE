/*
** pcrepp.cpp
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

#include "pcrepp.h"
#include "../Error.h"

Pcre::Pcre( const std::string& expression, char *flags )
{
	FLAG = 0;
	Zero();
	_expression = expression;
	FLAG = GetFlags( flags );
	Compile( FLAG );
}

Pcre::Pcre( const std::string& expression )
{
	Zero();
	_expression = expression;
	Compile( 0 );
}

const Pcre& Pcre::operator = ( const std::string& expression )
{
	Zero();
	_expression = expression;
	Compile( FLAG );
	return *this;
}

Pcre::~Pcre()
{
	pcre2_code_free_8( p_pcre2 );
	pcre2_match_data_free_8( match_data );
	pcre2_match_context_free_8( mcontext32 );
	if( stack32 )
		pcre2_jit_stack_free_8( stack32 );
}

void Pcre::Compile( UINT flags )
{
	ccontext32 = pcre2_compile_context_create_8( NULL );
	if( !ccontext32 )
	{
		Msg( "<PCRE>", "Error pcre2_compile_context_create" );
		mResult = -1;
		return;
	}

	p_pcre2 = pcre2_compile_8( ( PCRE2_SPTR8 )_expression.c_str(), PCRE2_ZERO_TERMINATED, flags, &errorcode, &erroroffset, ccontext32 );
	if( p_pcre2 == NULL )
	{
		pcre2_compile_context_free_8( ccontext32 );

		PCRE2_UCHAR8 error[ 256 ];
		pcre2_get_error_message_8( errorcode, error, sizeof( error ) );

		Msg( "<PCRE>", "PCRE2 compilation failed at offset %d: %s\n", ( int )erroroffset, ( char * )error );
		mResult = -1;
		return;
	}

	pcre2_compile_context_free_8( ccontext32 );

	match_data = pcre2_match_data_create_8( 30, NULL );
	mcontext32 = pcre2_match_context_create_8( NULL );

	if( !match_data || !mcontext32 )
	{
		Msg( "<PCRE>", "!match_data and !mcontext32" );
		mResult = -1;
		return;
	}

	mResult = 1;
}

int Pcre::dosearch( const std::string& stuff, int OffSet )
{
	memset( regtest_buf32, 0, REGTEST_MAX_LENGTH8 );

	int length32 = copy_char8_to_char32( ( PCRE2_SPTR8 )stuff.c_str(), regtest_buf32, 4096 );

	if( pcre2_jit_compile_8( p_pcre2, PCRE2_JIT_COMPLETE ) )
	{
		Msg( "<PCRE>", "32 bit: JIT compiler does not support" );
		return -1;
	}

	setstack32( mcontext32 );
	errorcode = pcre2_jit_match_8( p_pcre2, regtest_buf32, length32, OffSet, 0, match_data, mcontext32 );

	if( errorcode <= 0 )
	{
		return errorcode;
	}
	else
		if( errorcode >= 1 )
		{
			PCRE2_UCHAR8 **stringlist;
			size_t **num = NULL;

			int subres = pcre2_substring_list_get( match_data, &stringlist, num );

			num_matches = errorcode;

			PCRE2_SIZE *ovector = pcre2_get_ovector_pointer_8( match_data );
			nOffset = ( int )ovector[ 1 ];
			mStartMatch = ovector[ 2 ];
			mEndMatch = ovector[ 3 ];

			int res = pcre2_substring_list_get_8( match_data, ( PCRE2_UCHAR8 *** )&stringlist, NULL );
			if( res == 0 )
			{
				Result_t = ( char * )stringlist[ 1 ];
				pcre2_substring_list_free_8( ( PCRE2_SPTR8 * )stringlist );
			}
			else
			{
				Msg( "<PCRE>", "Failure to get a substring: %d", res );
				return -1;
			}

			return 1;
		}

	return 0;
}

char *Pcre::Replace( std::string Str, std::string Then, std::string That, char *flag )
{	
	std::string::size_type p_open, p_close;
	Replaced = Str;

	p_open = Then.find_first_of( "(" );
	p_close = Then.find_first_of( ")" );
	if( p_open == std::string::npos && p_close == std::string::npos )
		Then = "(" + Then + ")";

	_expression = Then;

	Zero();

	int Num = GetFlags( flag );

	Compile( Num );

	if( mResult == -1 )
		return "";

	if( search( Str ) == 1 )
	{
		int len = get_match_end() - get_match_start();

		Replaced.replace( get_match_start(), len, That );

		return ( char * )Replaced.c_str();
	}

	return "";
}

UINT Pcre::GetFlags( char *flags )
{	
	int result = 0;

	if( !flags )
		return 0;
	
	int flaglen = strlen( flags );

	for( int flag = 0; flag < flaglen; flag++ )
	{
		switch( flags[ flag ] )
		{
			case 'i': result |= PCRE2_CASELESS;					break;		// Do caseless matching
			case 'm': result |= PCRE2_MULTILINE;				break;		// ^ and $ match newlines within data
			case 's': result |= PCRE2_DOTALL;					break;		// . matches anything including NL
			case 'x': result |= PCRE2_EXTENDED;					break;		// Ignore white space and # comments
			case 'u': result |= PCRE2_UTF | PCRE2_UCP;			break;		// Treat pattern and subjects as UTF strings
		}
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////

int Pcre::search( const std::string& stuff, int OffSet )
{
	return dosearch( stuff, OffSet );
}

int Pcre::search( const std::string& stuff )
{
	return dosearch( stuff, 0 );
}

int Pcre::ResultCompile( void )
{
	return mResult;
}

int Pcre::NextPos( void )
{
	return nOffset;
}

int Pcre::get_match_start()
{
	return mStartMatch;
}

int Pcre::get_match_end()
{
	return mEndMatch;
}

char *Pcre::Result( void )
{
	return ( char * )Result_t.c_str();
}

//////////////////////////////////////////////////////////////////////////
void Pcre::Zero( void )
{
	num_matches = -1;
	nOffset = 0;
	ccontext32 = NULL;
	p_pcre2 = NULL;
	mcontext32 = NULL;
	match_data = NULL;
	errorcode = NULL;
	erroroffset = NULL;
}

int Pcre::copy_char8_to_char32( PCRE2_SPTR8 input, PCRE2_UCHAR8 *output, int max_length )
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

void Pcre::setstack32( pcre2_match_context_8 *mcontext )
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

pcre2_jit_stack_8 *Pcre::getstack32( void )
{
	if( !stack32 )
		stack32 = pcre2_jit_stack_create_8( 1, 1024 * 1024, NULL );
	return stack32;
}

pcre2_jit_stack_8 *Pcre::callback32( void * arg )
{
	return ( pcre2_jit_stack_8 * )arg;
}