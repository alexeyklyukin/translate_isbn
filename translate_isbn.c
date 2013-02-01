#include "postgres.h"

#include <stdlib.h>
#include <string.h>

#include "utils/builtins.h"


#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

#define char_digit_to_int(var) ((var) - '0')
#define int_to_char_digit(var) ((var) + '0')

Datum		translate_isbn1013(PG_FUNCTION_ARGS);

static int add_isbn(char *result, char *token, int append_pos);
static char *convert_10_to_13(char *isbn10);
static char *convert_13_to_10(char *isbn13);
static char ch13(char *isbn13);
static char ch10(char *isbn10);


PG_FUNCTION_INFO_V1(translate_isbn1013);

Datum 
translate_isbn1013(PG_FUNCTION_ARGS)
{
	int    pos = 0;
	char  *input,
		  *output,
		  *token,
		  *store;
	text  *intext;
	const char *delims = " \f\n\r\t\v";
	
	intext = PG_GETARG_TEXT_P(0);
	input = text_to_cstring(intext);
	
	/* 
	 * Worst case memory-wise: original string + fixed string + converted string,
	 * 3 extra characters. We also allocate extra strlen(argstr) for spaces
	 */
	output = (char *) palloc(strlen(input) * 4);
	for (token  = strtok_r(input, delims, &store); token; token = strtok_r(NULL, delims, &store))
 		pos = add_isbn(output, token, pos);
	pfree(input);
	/* XXX: an attempt to call pfree(input) leads to a crash here */
	
	if (pos > 0)
	{
		output[pos] = 0;
		PG_RETURN_TEXT_P(cstring_to_text(output));
	}
	pfree(output);	
	PG_RETURN_TEXT_P(intext);
}

/* Add new code to the result ISBN string. Convert it in the process */
static int
add_isbn(char *result, char *token, int append_pos)
{
	char	cchar,
			chdigit;
	bool	heading,
			valid,
			checksum_valid;
	int		processed_len = 0;
	int 	i,
			init_len;
	char   *candidate;
	
	/* Fastpath exit if token is invalid */
	if (!token)
		return append_pos;

	init_len = strlen(token);
	/* No need to store the ending '0' below. */
	candidate = (char  *) palloc(strlen(token));
	heading = true;
	valid = checksum_valid = false;
	for (i = 0; i < init_len; i++)
	{
		cchar = toupper(token[i]);
		/* Skip non-ISBN symbols (non-digits or not 'x') */
		if (!isdigit(cchar) && (cchar != 'X'))
			continue;
		valid = false;
		if (cchar == 'X' && !(processed_len == 9 || processed_len == 12))
			break;
		candidate[processed_len++] = cchar;
		if (processed_len == 10 || processed_len == 13)
			valid = true;
	}
	/* Validate the code and append validated and coverted versions if necessary */
	if (valid)
	{
		char   *converted = NULL;
		/* 
		 * now we have either 10 or 13-digit sequence that looks like ISBN.
		 * time to validate the checksum.
		 */
		chdigit = (processed_len == 10) ? ch10(candidate) : ch13(candidate);
		if (chdigit == candidate[processed_len - 1])
			checksum_valid = true;
		else
			checksum_valid = false;
		/* 
		 * append the candidate code first. If this is not the first ISBN
		 * insert a heading space.
		 */
		if (append_pos > 0)
			result[append_pos++] = ' '; 
		memcpy(result + append_pos, candidate, processed_len);
		append_pos += processed_len;
		/* append the candidate once again with the corrected checksum digit */
		if (!checksum_valid)
		{
			result[append_pos++] = ' ';
			/* copy the value one more time */
			memcpy(result + append_pos, candidate, processed_len);
			/* but change the control digit */
			append_pos += processed_len;
			result[append_pos - 1] = chdigit;
		}
		converted = (processed_len == 10) ? convert_10_to_13(candidate) : 
											convert_13_to_10(candidate);
		/* Not every 13-digit ISBN is valid for conversion to 10-digit version */
		if (converted) 
		{
			/* Append the conversion result */
			processed_len = (processed_len == 10) ? 13 : 10;
			result[append_pos++] = ' ';
			memcpy(result + append_pos, converted, processed_len);
			append_pos += processed_len;
			pfree(converted);
		}
	}
	pfree(candidate);
	return append_pos;
}

static char ch13(char *isbn13)
{
	int 	i;
	int		chnumber = 0;
	char	cdigit;
	int 	weights[12] = {1,3,1,3,1,3,1,3,1,3,1,3};

	for (i = 0; i < 12; i++)
		chnumber += char_digit_to_int(isbn13[i]) * weights[i];
	chnumber = (10 - (chnumber % 10)) % 10;
	cdigit = int_to_char_digit(chnumber);
	return cdigit;
}

static char ch10(char *isbn10)
{
	int    i;
	int    chnumber = 0;
	char   chdigit;
	
	for (i = 0; i < 9; i++)
		chnumber += (10 - i) * char_digit_to_int(isbn10[i]);
	chnumber = (11 - (chnumber % 11)) % 11;
	chdigit = (chnumber == 10) ? 'X' : int_to_char_digit(chnumber);
	return chdigit;
}

static char *convert_10_to_13(char *isbn10)
{
	char   *isbn13 = (char *) palloc(13);
	memmove(isbn13, "978", 3);
	memmove(isbn13 + 3, isbn10, 10);
	/* fix the checksum */
	isbn13[12] = ch13(isbn13);
	return isbn13;
}

static char *convert_13_to_10(char *isbn13)
{
	char   *isbn10 = NULL;
	/* First 3 digits should be 978 */
	if (isbn13[0] == '9' && isbn13[1] == '7' && isbn13[2] == '8')
	{
		isbn10 = (char *)palloc(10);
		memmove(isbn10, isbn13 + 3, 10);
		/* fix the checksum */
		isbn10[9] = ch10(isbn10);
	}
	return isbn10;
}
