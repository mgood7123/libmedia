//
// Created by konek on 7/9/2019.
//

#ifndef MEDIA_PLAYER_PRO_REGEX_STR_H
#define MEDIA_PLAYER_PRO_REGEX_STR_H

/* Author: mgood7123 (Matthew James Good) http://github.com/mgood7123 */
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#define REGEX_STR
#pragma once

extern char * string_register_pointer;

extern void str_reg(char * str);

extern int getbitgroupcount(int bit);

#define TYPE_SETUP(TYPE) bool TYPE##_INVALIDATED;

#define TYPE_SETUP_INIT(str, TYPE) (str).TYPE##_INVALIDATED = false;

#define STR_TYPE_EMPTY 			(1 << 0)
#define STR_TYPE_BINARY 		(1 << 1)
#define STR_TYPE_ALPHABETIC 	(1 << 2)
#define STR_TYPE_ALPHANUMERIC 	(1 << 3)
#define STR_TYPE_ASCII			(1 << 4)
#define STR_TYPE_UNICODE		(1 << 5)
#define STR_TYPE_HEXADECIMAL 	(1 << 6)
#define STR_TYPE_UPPERCASE		(1 << 7)
#define STR_TYPE_LOWERCASE		(1 << 8)
#define STR_TYPE_SPACE			(1 << 9)
#define STR_TYPE_PUNCTUATION	(1 << 10)
#define STR_TYPE_PRINTABLE		(1 << 11)
#define STR_TYPE_GRAPH			(1 << 12)
#define STR_TYPE_DIGIT			(1 << 13)
#define STR_TYPE_CONTROL		(1 << 14)
#define STR_TYPE_BLANK			(1 << 15)

#define STR_SETUP_STRUCT TYPE_SETUP(EMPTY) TYPE_SETUP(BINARY) TYPE_SETUP(ALPHABETIC) TYPE_SETUP(ALPHANUMERIC) TYPE_SETUP(ASCII) TYPE_SETUP(UNICODE) TYPE_SETUP(HEXADECIMAL) TYPE_SETUP(UPPERCASE) TYPE_SETUP(LOWERCASE) TYPE_SETUP(SPACE) TYPE_SETUP(PUNCTUATION) TYPE_SETUP(PRINTABLE) TYPE_SETUP(GRAPH) TYPE_SETUP(DIGIT) TYPE_SETUP(CONTROL) TYPE_SETUP(BLANK)

#define STR_SETUP_STRUCT_INIT(str) TYPE_SETUP_INIT((str), EMPTY) TYPE_SETUP_INIT((str), BINARY) TYPE_SETUP_INIT((str), ALPHABETIC) TYPE_SETUP_INIT((str), ALPHANUMERIC) TYPE_SETUP_INIT((str), ASCII) TYPE_SETUP_INIT((str), UNICODE) TYPE_SETUP_INIT((str), HEXADECIMAL) TYPE_SETUP_INIT((str), UPPERCASE) TYPE_SETUP_INIT((str), LOWERCASE) TYPE_SETUP_INIT((str), SPACE) TYPE_SETUP_INIT((str), PUNCTUATION) TYPE_SETUP_INIT((str), PRINTABLE) TYPE_SETUP_INIT((str), GRAPH) TYPE_SETUP_INIT((str), DIGIT) TYPE_SETUP_INIT((str), CONTROL) TYPE_SETUP_INIT((str), BLANK)

struct regex_string_structure
{
    char * string;
    int16_t type;
    STR_SETUP_STRUCT;
    struct Queue * prev_type;
    int index;
    int len;
    int malloced;
};

struct regex_string
{
    struct regex_string_structure escaped;
    struct regex_string_structure json;
    struct regex_string_structure c_define;
    struct regex_string_structure c_string;
    struct regex_string_structure no_spaces;
    struct regex_string_structure reverse;
    struct regex_string_structure indented;
    char * string;
    int16_t type;
    STR_SETUP_STRUCT;
    struct Queue * prev_type;
    int index;
    int len;
    int malloced;
};

#define STR_TYPE_ADD_TYPE(STR, STR_TYPE) (STR).type |= STR_TYPE;
#define STR_TYPE_REMOVE_TYPE(STR, STR_TYPE) (STR).type &= ~STR_TYPE;

// uses ctypes.h, unknows if safe or portable
#include <ctype.h>

#define range_BINARY(ch) 			(ch=='0'||ch=='1')
#define range_UNICODE(ch)			(ch==ch) /* to be improved */

#define range_ALPHABETIC(ch) 		isalpha((unsigned char) ch)
#define range_ALPHANUMERIC(ch) 		isalnum((unsigned char) ch)
#define range_ASCII(ch) 			isascii((unsigned char) ch)
#define range_HEXADECIMAL(ch)		isxdigit((unsigned char) ch)
#define range_UPPERCASE(ch)			isupper((unsigned char) ch)
#define range_LOWERCASE(ch)			islower((unsigned char) ch)
#define range_SPACE(ch)				isspace((unsigned char) ch)
#define range_PUNCTUATION(ch)		ispunct((unsigned char) ch)
#define range_PRINTABLE(ch) 		isprint((unsigned char) ch)
#define range_GRAPH(ch) 			isgraph((unsigned char) ch)
#define range_DIGIT(ch) 			isdigit((unsigned char) ch)
#define range_CONTROL(ch) 			iscntrl((unsigned char) ch)
#define range_BLANK(ch) 			isblank((unsigned char) ch)

// this is not consistant throughout strings and is currently only the representation of the last character added

// if a flag gets disabled it should not be allowed to become re-enabled unless on the event of an undo


#define AUTOTYPE_TEST_(AUTOSTR, AUTOCH, AUTORANGE, AUTOTYPE) { \
	if (AUTOCH != 0) { \
		\
		/* AUTORANGE */ \
		if (!(AUTOSTR).AUTORANGE##_INVALIDATED && range_##AUTORANGE(AUTOCH)) { \
			if ((AUTOSTR).type == STR_TYPE_EMPTY) { \
				STR_TYPE_REMOVE_TYPE((AUTOSTR), STR_TYPE_EMPTY) \
			} \
			STR_TYPE_ADD_TYPE((AUTOSTR), STR_TYPE_##AUTOTYPE) \
		} \
		else { \
			if ((AUTOSTR).type != STR_TYPE_EMPTY) { \
				(AUTOSTR).AUTORANGE##_INVALIDATED = true; \
			} \
			STR_TYPE_REMOVE_TYPE((AUTOSTR), STR_TYPE_##AUTOTYPE) \
		} \
	} \
}

#define AUTOTYPE_TEST(AUTOSTR, AUTOCH, AUTOTYPE) AUTOTYPE_TEST_(AUTOSTR, AUTOCH, AUTOTYPE, AUTOTYPE)

#define AUTOTYPE(str, ch) { \
	AUTOTYPE_TEST((str), (ch), BINARY) \
	AUTOTYPE_TEST((str), (ch), ALPHABETIC) \
	AUTOTYPE_TEST((str), (ch), ALPHANUMERIC) \
	AUTOTYPE_TEST((str), (ch), ASCII) \
	AUTOTYPE_TEST((str), (ch), UNICODE) \
	AUTOTYPE_TEST((str), (ch), HEXADECIMAL) \
	AUTOTYPE_TEST((str), (ch), UPPERCASE) \
	AUTOTYPE_TEST((str), (ch), LOWERCASE) \
	AUTOTYPE_TEST((str), (ch), SPACE) \
	AUTOTYPE_TEST((str), (ch), PUNCTUATION) \
	AUTOTYPE_TEST((str), (ch), PRINTABLE) \
	AUTOTYPE_TEST((str), (ch), GRAPH) \
	AUTOTYPE_TEST((str), (ch), DIGIT) \
	AUTOTYPE_TEST((str), (ch), CONTROL) \
	AUTOTYPE_TEST((str), (ch), BLANK) \
}

#define TYPECHECK(str, type_) { \
	if ((str).type & type_) puts(#type_); \
}

#define TYPEINFO(str) { \
	printf("(%s).type = %d\n", #str, (str).type); \
	TYPECHECK((str), STR_TYPE_EMPTY) \
	TYPECHECK((str), STR_TYPE_BINARY) \
	TYPECHECK((str), STR_TYPE_ALPHABETIC) \
	TYPECHECK((str), STR_TYPE_ALPHANUMERIC) \
	TYPECHECK((str), STR_TYPE_ASCII) \
	TYPECHECK((str), STR_TYPE_UNICODE) \
	TYPECHECK((str), STR_TYPE_HEXADECIMAL) \
	TYPECHECK((str), STR_TYPE_UPPERCASE) \
	TYPECHECK((str), STR_TYPE_LOWERCASE) \
	TYPECHECK((str), STR_TYPE_PUNCTUATION) \
	TYPECHECK((str), STR_TYPE_PRINTABLE) \
	TYPECHECK((str), STR_TYPE_GRAPH) \
	TYPECHECK((str), STR_TYPE_DIGIT) \
	TYPECHECK((str), STR_TYPE_CONTROL) \
	TYPECHECK((str), STR_TYPE_BLANK) \
}

// type history
// A C program to demonstrate linked list based implementation of queue
// A linked list (LL) node to store a queue entry
struct QNode
{
    int16_t type;
    struct QNode *next;
};

// The queue, front stores the front node of LL and rear stores ths
// last node of LL
struct Queue
{
    struct QNode *front, *rear;
};

// A utility function to create a new linked list node.
struct QNode* newNode(int16_t type);

// A utility function to create an empty queue
struct Queue *createQueue();

extern void store_asm(struct Queue **qq, int16_t type);

struct QNode * load_asm(struct Queue **qq);

#define str_malloc_(y, z) \
	(y).string = (char*)malloc(z); \
	memset((y).string, 0, z); \
	(y).malloced = z; \
	(y).len = 0; \
	(y).index = 0; \
	(y).type = STR_TYPE_EMPTY; \
	(y).prev_type = NULL; \
	STR_SETUP_STRUCT_INIT(y);

#define str_malloc__fast(y, z) \
	(y).string = (char*)malloc(z); \
	memset((y).string, 0, z); \
	(y).malloced = z; \
	(y).len = 0; \
	(y).index = 0; \
	(y).type = STR_TYPE_EMPTY; \
	(y).prev_type = NULL; \

#define str_mallocr(y, z) \
	str_malloc_((y), z); \
	str_malloc_((y).escaped, z); \
	str_malloc_((y).json, z); \
	str_malloc_((y).c_define, z); \
	str_malloc_((y).c_string, z); \
	str_malloc_((y).no_spaces, z); \
	str_malloc_((y).reverse, z); \
	str_malloc_((y).indented, z); \

#define str_malloc(y, z) \
	struct regex_string y; \
	str_mallocr((y), z);

#define str_malloc_fast(y, z) \
	struct regex_string y; \
	str_malloc__fast((y), z);

#define str_new(str) \
	str_malloc(str, 1) \

#define str_new_fast(str) \
	str_malloc_fast(str, 1) \

#define str_free_(y) \
	memset((y).string, 0, (y).malloced); \
	free((y).string); \
	(y).string = NULL; \
	(y).malloced = 0; \
	(y).len = 0; \
	(y).index = 0; \
	(y).type = STR_TYPE_EMPTY; \
	(y).prev_type = NULL; \
	STR_SETUP_STRUCT_INIT((y));

#define str_free__fast(y) \
	memset((y).string, 0, (y).malloced); \
	free((y).string); \
	(y).string = NULL; \
	(y).malloced = 0; \
	(y).len = 0; \
	(y).index = 0; \
	(y).type = STR_TYPE_EMPTY; \
	(y).prev_type = NULL; \

#define str_free(y) \
	{ \
		str_free_((y)); \
		str_free_((y).escaped); \
		str_free_((y).json); \
		str_free_((y).c_define); \
		str_free_((y).c_string); \
		str_free_((y).no_spaces); \
		str_free_((y).reverse); \
		str_free_((y).indented); \
	}

#define str_free_fast(y) str_free__fast((y));

#define str_reset_(str) { \
	str_free_((str)) \
	str_malloc_((str), 1) \
}

#define str_reset(str) { \
	str_reset_((str)) \
	str_reset_((str).escaped) \
	str_reset_((str).json) \
	str_reset_((str).c_define) \
	str_reset_((str).c_string) \
	str_reset_((str).no_spaces) \
	str_reset_((str).reverse) \
	str_reset_((str).indented) \
}

#define str_realloc(y, z) \
	(y).string = (char*)realloc((y).string, z); \
	if ((y).malloced < z) { \
		memset((y).string+(y).malloced, 0, (z)-(y).malloced); \
	} \
	(y).malloced = z;

#define str_info_(str) \
		printf("%s.index = %d\n", #str, (str).index); \
		printf("%s.len = %d\n", #str, (str).len); \
		printf("%s.malloced = %d\n", #str, (str).malloced); \
		printf("%s.string = %s\n", #str, (str).string); \
		printf("TYPE information for %s (start)\n", #str); \
		TYPEINFO((str)) \
		printf("TYPE information for %s (end)\n", #str); \

#define str_info(str) { \
	str_info_((str)) \
	str_info_((str).escaped) \
	str_info_((str).json) \
	str_info_((str).c_define) \
	str_info_((str).c_string) \
	str_info_((str).no_spaces) \
	str_info_((str).reverse) \
	str_info_((str).indented) \
}

#define str_insert_char(str, ch) { \
	str_realloc((str), (str).malloced+2); \
	(str).string[(str).index] = ch; \
	(str).index++; \
	(str).string[(str).index] = 0; \
	(str).len = strlen((str).string); \
	store_asm(&((str).prev_type), (str).type); \
	AUTOTYPE((str), ch); \
}

#define str_insert_char_fast(str, ch) { \
	str_realloc((str), (str).malloced+2); \
	(str).string[(str).index] = ch; \
	(str).index++; \
	(str).string[(str).index] = 0; \
	(str).len = strlen((str).string); \
}

#define str_insert_char_reverse(str, ch) { \
	str_realloc((str), (str).malloced+2); \
	memmove((str).string+1, (str).string, (str).len); \
	(str).string[0] = ch; \
	(str).index++; \
	(str).string[(str).index] = 0; \
	(str).len = strlen((str).string); \
	store_asm(&((str).prev_type), (str).type); \
	AUTOTYPE((str), ch); \
}

#define str_undo_(str) { \
	(str).index--; \
	(str).string[(str).index] = 0; \
	str_realloc((str), (str).malloced-1); \
	(str).len = strlen((str).string); \
	struct QNode * node = NULL; \
	node = load_asm(&((str).prev_type)); \
	(str).type = node->type; \
	free(node); \
}

#define str_undo_reverse(str) { \
	(str).index--; \
	memmove((str).string+1, (str).string, (str).len-1); \
	(str).string[(str).index] = 0; \
	str_realloc((str), (str).malloced-1); \
	(str).len = strlen((str).string); \
	struct QNode * node = NULL; \
	node = load_asm(&((str).prev_type)); \
	(str).type = node->type; \
	free(node); \
}

#define str_undo(str) { \
	str_undo_((str)) \
	str_undo_((str).escaped) \
	str_undo_((str).json) \
	str_undo_((str).c_define) \
	str_undo_((str).c_string) \
	str_undo_((str).no_spaces) \
	str_undo_reverse((str).reverse) \
	str_undo_((str).indented) \
}


#define rng(s, e) ( uch >= s && uch <= e)

extern int STR_INDENT_LEVEL;
extern int STR_INDENT_SIZE;
#define STR_INDENT_INCREASE STR_INDENT_LEVEL+=STR_INDENT_SIZE;
#define STR_INDENT_DECREASE STR_INDENT_LEVEL-=STR_INDENT_SIZE;

#define str_insert_indent(str, INDENT) { \
	for (int i = 0; i < INDENT; i++) { \
		str_insert_char((str), ' '); \
	} \
}

#define str_remove_indent(str) { \
	for (int i = 0; i < STR_INDENT_SIZE; i++) { \
		if ((str).len > 0) str_undo_((str)); \
	} \
}

#define str_insert_string_fast(str, string_) { \
	const unsigned int str_insert_string_fast_TMP_DATA_size = strlen(string_); \
	const unsigned char * str_insert_string_fast_TMP_DATA_s = (const unsigned char *) string_; \
	unsigned int i; \
	for (i = 0; i < str_insert_string_fast_TMP_DATA_size; ++i) { \
	    str_insert_char_fast(str, str_insert_string_fast_TMP_DATA_s[i]); \
	} \
}


#define str_int2string(x,y) \
	char * x = malloc(getbitgroupcount(y)); \
	sprintf(x, "%d", y);

#define str_insert_int(str, integer) { \
	str_int2string(j, integer); \
	str_insert_string((str), j); \
	free(j); \
}

#define str_insert_int2(str, integer) { \
	str_int2string(j, integer); \
	str_insert_string_fast((str), j); \
	free(j); \
}

#define str_insert_indent_info(str) { \
	str_insert_string_fast((str), "/* indent level: "); \
	str_insert_int2((str), STR_INDENT_LEVEL); \
	str_insert_string_fast((str), " */"); \
}

#define str_insert_string(str, string) { \
	const unsigned char * s = (const unsigned char *) string; \
	const unsigned int size = strlen(string); \
	int c; \
	unsigned char uch; \
	str_insert_string_fast((str).c_define, "#define \""); \
	int i; \
	for (i = 0; i < size; ++i) { \
		c = s[i]; \
		uch = (unsigned char) s[i]; \
		str_insert_char((str), c); \
		if (c == '{') { \
			str_insert_char((str).indented, c); \
			STR_INDENT_INCREASE \
			/*str_insert_indent_info((str).indented);*/ \
			str_insert_char((str).indented, '\n'); \
			str_insert_indent((str).indented, STR_INDENT_LEVEL); \
		} \
		else if (c == '}') { \
			STR_INDENT_DECREASE \
			/*str_insert_indent_info((str).indented);*/ \
			str_insert_char((str).indented, '\n'); \
			str_insert_indent((str).indented, STR_INDENT_LEVEL); \
			str_insert_char((str).indented, c); \
		} \
		else { \
			if (c == '\n') { \
				/*str_insert_indent_info((str).indented);*/ \
			} \
			str_insert_char((str).indented, c); \
		} \
		str_insert_char_reverse((str).reverse, c); \
		if (c != ' ' || c != '\t') str_insert_char((str).no_spaces, c); \
		\
		/* escape sequences under gnu gcc are interperated by both cc1, and the terminal, assembling a string containing an invalid sequence will print as if it does not have the \ before it*/ \
		\
		if (isalnum(c) ) { \
			str_insert_char((str).c_define, c); \
		} \
		else str_insert_char((str).c_define, '_'); \
		if (c == '\a') { \
			str_insert_char((str).escaped, '\\'); \
			str_insert_char((str).escaped, 'a'); \
			str_insert_char((str).c_string, '\\'); \
			str_insert_char((str).c_string, 'a'); \
		} \
		else if (c == '\b') { \
			str_insert_char((str).escaped, '\\'); \
			str_insert_char((str).escaped, 'b'); \
			str_insert_char((str).json, '\\'); \
			str_insert_char((str).json, 'b'); \
			str_insert_char((str).c_string, '\\'); \
			str_insert_char((str).c_string, 'b'); \
		} \
		else if (c == '\f') { \
			str_insert_char((str).escaped, '\\'); \
			str_insert_char((str).escaped, 'f'); \
			str_insert_char((str).json, '\\'); \
			str_insert_char((str).json, 'f'); \
			str_insert_char((str).c_string, '\\'); \
			str_insert_char((str).c_string, 'f'); \
		} \
		else if (c == '\n') { \
			str_insert_indent((str).indented, STR_INDENT_LEVEL); \
			str_insert_char((str).escaped, '\\'); \
			str_insert_char((str).escaped, 'n'); \
			str_insert_char((str).json, '\\'); \
			str_insert_char((str).json, 'n'); \
			str_insert_char((str).c_string, '\\'); \
			str_insert_char((str).c_string, 'n'); \
			str_insert_char((str).c_string, '"'); \
			str_insert_char((str).c_string, ' '); \
			str_insert_char((str).c_string, '\\'); \
			str_insert_char((str).c_string, c); \
			str_insert_char((str).c_string, '"'); \
		} \
		else if (c == '\r') { \
			str_insert_char((str).escaped, '\\'); \
			str_insert_char((str).escaped, 'r'); \
			str_insert_char((str).json, '\\'); \
			str_insert_char((str).json, 'r'); \
			str_insert_char((str).c_string, '\\'); \
			str_insert_char((str).c_string, 'r'); \
		} \
		else if (c == '\t') { \
			str_insert_char((str).escaped, '\\'); \
			str_insert_char((str).escaped, 't'); \
			str_insert_char((str).json, '\\'); \
			str_insert_char((str).json, 't'); \
			str_insert_char((str).c_string, '\\'); \
			str_insert_char((str).c_string, 't'); \
		} \
		else if (c == '\v') { \
			str_insert_char((str).escaped, '\\'); \
			str_insert_char((str).escaped, 'v'); \
			str_insert_char((str).c_string, '\\'); \
			str_insert_char((str).c_string, 'v'); \
		} \
		else if (c == '\\') { \
			str_insert_char((str).escaped, '\\'); \
			str_insert_char((str).escaped, '\\'); \
			str_insert_char((str).json, '\\'); \
			str_insert_char((str).json, '\\'); \
			str_insert_char((str).c_string, '\\'); \
			str_insert_char((str).c_string, '\\'); \
		} \
		else if (c == '\'') { \
			str_insert_char((str).escaped, '\\'); \
			str_insert_char((str).escaped, '\''); \
			str_insert_char((str).json, '\\'); \
			str_insert_char((str).json, '\''); \
			str_insert_char((str).c_string, '\\'); \
			str_insert_char((str).c_string, '\''); \
		} \
		else if (c == '"') { \
			str_insert_char((str).escaped, '\\'); \
			str_insert_char((str).escaped, '"'); \
			str_insert_char((str).json, '\\'); \
			str_insert_char((str).json, '"'); \
			str_insert_char((str).c_string, '\\'); \
			str_insert_char((str).c_string, '"'); \
		} \
		else if (c == '\?') { \
			str_insert_char((str).escaped, '\\'); \
			str_insert_char((str).escaped, '?'); \
			str_insert_char((str).c_string, c); \
		} \
		else if (c >= ' ' && c <= 0x7e) { \
			str_insert_char((str).escaped, c); \
			str_insert_char((str).json, c); \
			str_insert_char((str).c_string, c); \
		} \
		else if (c >= '0' && c <= '9' ) { \
			/* Print \ooo */ \
			str_insert_char((str).escaped, '\\'); \
			str_insert_char((str).escaped, '0' + (c >> 6)); \
			str_insert_char((str).escaped, '0' + ((c >> 3) & 0x7)); \
			str_insert_char((str).escaped, '0' + (c & 0x7)); \
			str_insert_char((str).c_string, c); \
		} else { \
			str_insert_char((str).c_string, c); \
			/* Print \[[o]o]o */ \
			str_insert_char((str).escaped, '\\'); \
			if ((c >> 3) != 0) { \
				if ((c >> 6) != 0) str_insert_char((str).escaped, '0' + (c >> 6)); \
				str_insert_char((str).escaped, '0' + ((c >> 3) & 0x7)); \
			} \
			str_insert_char((str).escaped, '0' + (c & 0x7)); \
		} \
	} \
	str_insert_char((str).c_define, '\n'); \
	str_insert_char((str).c_string, '"'); \
}

void str_copy(struct regex_string * str, const char * string);

#define str_copy(str, cpy) \
    char cpy[str.malloced]; \
    memcpy(cpy, str.string, str.malloced);

extern int reverseBool(int val);

void str_output(struct regex_string * str, const char * file);

void str_output_append(struct regex_string * str, const char * file);

#define str_output(str, file) { \
	FILE * stream = stdout; \
	int isfile = 0; \
	if (strcmp("stdin", file) == 0) stream = stdin; \
	else if(strcmp("stdout", file) == 0) stream = stdout; \
	else if(strcmp("stderr", file) == 0) stream = stderr; \
	else isfile = 1; \
	if (isfile) stream = fopen(file, "w"); \
	fwrite((str).string,(str).len, 1, stream); \
	if (isfile) fclose(stream); \
}

#define str_output_append(str, file) { \
	FILE * stream = stdout; \
	int isfile = 0; \
	if (strcmp("stdin", file) == 0) stream = stdin; \
	else if(strcmp("stdout", file) == 0) stream = stdout; \
	else if(strcmp("stderr", file) == 0) stream = stderr; \
	else isfile = 1; \
	if (isfile) stream = fopen(file, "a"); \
	fwrite((str).string,(str).len, 1, stream); \
	if (isfile) fclose(stream); \
}

// reads a entire file
extern size_t read__(const char *file, char **p);

extern char * filetostring(const char * file);

#define str_include(str, file) { \
	char * string = filetostring(file); \
	str_insert_string((str), string); \
	free(string); \
}

#endif //MEDIA_PLAYER_PRO_REGEX_STR_H
