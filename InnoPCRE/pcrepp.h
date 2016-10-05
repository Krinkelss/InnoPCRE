/*
** pcrepp.h
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

#pragma once

#define PCRE2_CODE_UNIT_WIDTH 8
#include "config.h"
#include "pcre2.h"
#pragma comment (lib, "pcre2-8.lib")
#define REGTEST_MAX_LENGTH8 4096

#include <windows.h>
#include <string>

static pcre2_jit_stack_8 *stack32;

class Pcre
{
public:
	Pcre( const std::string& expression, char *flags );
	Pcre( const std::string& expression );
	const Pcre& operator = ( const std::string& expression );
	~Pcre();

	int search( const std::string& stuff, int OffSet );
	int search( const std::string& stuff );

	int ResultCompile( void );
	int NextPos( void );
	char *Result( void );

	int get_match_start( void );
	int get_match_end( void );

	char *Replace( std::string Str, std::string Then, std::string That, char *flag );

private:
	void Compile( UINT flags );
	void Zero( void );
	UINT GetFlags( char *flags );
	int dosearch( const std::string& stuff, int OffSet );
	int copy_char8_to_char32( PCRE2_SPTR8 input, PCRE2_UCHAR8 *output, int max_length );
	void setstack32( pcre2_match_context_8 *mcontext );
	static pcre2_jit_stack_8 *getstack32( void );
	static pcre2_jit_stack_8* callback32( void *arg );

private:
	pcre2_compile_context_8 *ccontext32;
	pcre2_code_8 *p_pcre2;
	pcre2_match_context_8 *mcontext32;
	pcre2_match_data_8 *match_data;
	int errorcode;
	PCRE2_SIZE erroroffset;
	PCRE2_UCHAR8 regtest_buf32[ REGTEST_MAX_LENGTH8 ];

private:
	std::string _expression;
	int mResult;
	int nOffset;
	int num_matches;
	std::string Result_t;
	int mStartMatch;
	int mEndMatch;
	std::string Replaced;
	UINT FLAG;
};