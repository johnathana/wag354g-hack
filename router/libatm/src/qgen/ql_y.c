/* A Bison parser, made from ql_y.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	TOK_BREAK	257
# define	TOK_CASE	258
# define	TOK_DEF	259
# define	TOK_DEFAULT	260
# define	TOK_LENGTH	261
# define	TOK_MULTI	262
# define	TOK_RECOVER	263
# define	TOK_ABORT	264
# define	TOK_ID	265
# define	TOK_INCLUDE	266
# define	TOK_STRING	267

#line 1 "ql_y.y"

/* ql.y - Q.2931 data structures description language */

/* Written 1995-1997 by Werner Almesberger, EPFL-LRC */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "common.h"
#include "qgen.h"
#include "file.h"


#define MAX_TOKEN 256
#define DEFAULT_NAMELIST_FILE "default.nl"


FIELD *def = NULL;
static STRUCTURE *structures = NULL;
static const char *abort_id; /* indicates abort flag */


static NAME_LIST *get_name_list(const char *name)
{
    static NAME_LIST *name_lists = NULL;
    FILE *file;
    NAME_LIST *list;
    NAME *last,*this;
    char line[MAX_TOKEN+1];
    char path[PATH_MAX+1];
    char *start,*here,*walk;
    int searching,found;

    for (list = name_lists; list; list = list->next)
	if (list->list_name == name) return list;
    sprintf(path,"%s.nl",name);
    if (!(file = fopen(path,"r")) && !(file = fopen(strcpy(path,
      DEFAULT_NAMELIST_FILE),"r"))) yyerror("can't open list file");
    list = alloc_t(NAME_LIST);
    list->list_name = name;
    list->list = last = NULL;
    list->id = -1;
    list->next = name_lists;
    name_lists = list;
    searching = 1;
    found = 0;
    while (fgets(line,MAX_TOKEN,file)) {
	for (start = line; *start && isspace(*start); start++);
	if (!*start || *start == '#') continue;
	if ((here = strchr(start,'\n'))) *here = 0;
	for (walk = strchr(start,0)-1; walk > start && isspace(*walk); walk--)
	    *walk = 0;
	if (*start == ':') {
	    if (!(searching = strcmp(start+1,name)))
		if (found) yyerror("multiple entries");
		else found = 1;
	    continue;
	}
	if (searching) continue;
	if (!(here = strchr(start,'='))) yyerror("invalid name list");
	*here++ = 0;
	for (walk = here-2; walk > start && isspace(*walk); walk--)
	    *walk = 0;
	while (*here && isspace(*here)) here++;
	this = alloc_t(NAME);
	this->value = stralloc(start);
	this->name = stralloc(here);
	this->next = NULL;
	if (last) last->next = this;
	else list->list = this;
	last = this;
    }
    (void) fclose(file);
    if (!found) yyerror("no symbol list entry found");
    return list;
}


static FIELD *copy_block(FIELD *orig_field)
{
    FIELD *copy,**new_field;

    copy = NULL;
    new_field = &copy;
    while (orig_field) {
	*new_field = alloc_t(FIELD);
	**new_field = *orig_field;
	if (orig_field->value) {
	    (*new_field)->value = alloc_t(VALUE);
	    *(*new_field)->value = *orig_field->value;
	    switch (orig_field->value->type) {
		case vt_length:
		    (*new_field)->value->block =
		      copy_block(orig_field->value->block);
		    break;
		case vt_case:
		case vt_multi:
		    {
			TAG *orig_tag,**new_tag;

			new_tag = &(*new_field)->value->tags;
			for (orig_tag = orig_field->value->tags; orig_tag;
			  orig_tag = orig_tag->next) {
			    VALUE_LIST *orig_value,**new_value;

			    *new_tag = alloc_t(TAG);
			    **new_tag = *orig_tag;
			    new_value = &(*new_tag)->more;
			    for (orig_value = orig_tag->more; orig_value;
			      orig_value = orig_value->next) {
				*new_value = alloc_t(VALUE_LIST);
				**new_value = *orig_value;
				new_value = &(*new_value)->next;
			    }
			    (*new_tag)->block = copy_block(orig_tag->block);
			    new_tag = &(*new_tag)->next;
			}
		    }
	    }
	}
	if (orig_field->structure)
	    yyerror("sorry, can't handle nested structures");
	new_field = &(*new_field)->next;
	orig_field = orig_field->next;
    }
    return copy;
}



#line 139 "ql_y.y"
#ifndef YYSTYPE
typedef union {
    const char *str;
    int num;
    FIELD *field;
    VALUE *value;
    VALUE_LIST *list;
    TAG *tag;
    NAME_LIST *nlist;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		86
#define	YYFLAG		-32768
#define	YYNTBASE	23

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 267 ? yytranslate[x] : 47)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    21,    18,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    22,     2,
      17,    14,    19,     2,    20,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    15,     2,    16,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     4,     5,     8,     9,    12,    17,    18,    21,
      23,    27,    30,    31,    34,    40,    41,    43,    47,    53,
      54,    57,    59,    60,    63,    64,    67,    69,    74,    79,
      83,    84,    87,    88,    90,    91,    97,    98,   105,   106,
     112,   113,   120,   121,   124,   125
};
static const short yyrhs[] =
{
      24,    25,    29,     0,     0,    12,    24,     0,     0,    25,
      26,     0,     5,    11,    14,    29,     0,     0,    28,    29,
       0,    11,     0,    15,    30,    16,     0,    10,    11,     0,
       0,    31,    30,     0,    32,    11,    40,    17,    33,     0,
       0,     3,     0,    18,    35,    19,     0,    35,    34,    36,
      19,    37,     0,     0,    20,    35,     0,    11,     0,     0,
      21,    11,     0,     0,    14,    38,     0,    11,     0,     4,
      15,    41,    16,     0,     8,    15,    43,    16,     0,    39,
       7,    29,     0,     0,     9,    11,     0,     0,    13,     0,
       0,     6,    11,    45,    46,    29,     0,     0,    11,    45,
      46,    29,    42,    41,     0,     0,     6,    11,    45,    46,
      27,     0,     0,    11,    45,    46,    27,    44,    43,     0,
       0,    22,    11,     0,     0,    21,    11,    46,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   163,   175,   176,   184,   185,   188,   202,   202,   212,
     231,   236,   243,   247,   254,   278,   282,   288,   301,   318,
     322,   329,   339,   343,   350,   354,   360,   367,   374,   380,
     390,   394,   400,   404,   410,   414,   431,   431,   454,   458,
     474,   474,   497,   501,   507,   511
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "TOK_BREAK", "TOK_CASE", "TOK_DEF", 
  "TOK_DEFAULT", "TOK_LENGTH", "TOK_MULTI", "TOK_RECOVER", "TOK_ABORT", 
  "TOK_ID", "TOK_INCLUDE", "TOK_STRING", "'='", "'{'", "'}'", "'<'", 
  "'-'", "'>'", "'@'", "','", "':'", "all", "includes", "structures", 
  "structure", "rep_block", "@1", "block", "fields", "field", "opt_break", 
  "field_cont", "opt_pos", "decimal", "opt_more", "opt_val", "value", 
  "opt_recover", "opt_name_list", "tags", "@2", "rep_tags", "@3", 
  "opt_id", "list", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    23,    24,    24,    25,    25,    26,    28,    27,    29,
      29,    29,    30,    30,    31,    32,    32,    33,    33,    34,
      34,    35,    36,    36,    37,    37,    38,    38,    38,    38,
      39,    39,    40,    40,    41,    41,    42,    41,    43,    43,
      44,    43,    45,    45,    46,    46
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     3,     0,     2,     0,     2,     4,     0,     2,     1,
       3,     2,     0,     2,     5,     0,     1,     3,     5,     0,
       2,     1,     0,     2,     0,     2,     1,     4,     4,     3,
       0,     2,     0,     1,     0,     5,     0,     6,     0,     5,
       0,     6,     0,     2,     0,     3
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       2,     2,     4,     3,     0,     0,     0,     9,    12,     5,
       1,     0,    11,    16,     0,    12,     0,     0,    10,    13,
      32,     6,    33,     0,     0,    21,     0,    14,    19,     0,
       0,    22,    17,    20,     0,     0,    23,    24,    30,    18,
       0,     0,     0,    26,    25,     0,    34,    38,    31,     0,
       0,    42,     0,     0,    42,     0,    29,    42,     0,    44,
      27,    42,    44,    28,    44,    43,     0,     0,    44,     7,
       0,    44,    36,     7,    40,     0,    35,    45,    34,    39,
      38,     8,    37,    41,     0,     0,     0
};

static const short yydefgoto[] =
{
      84,     2,     4,     9,    74,    75,    10,    14,    15,    16,
      27,    31,    28,    35,    39,    44,    45,    23,    52,    78,
      55,    80,    59,    67
};

static const short yypact[] =
{
      -8,    -8,-32768,-32768,    -4,     3,    17,-32768,    -1,-32768,
  -32768,    20,-32768,-32768,    21,    -1,    22,    11,-32768,-32768,
      23,-32768,-32768,    24,    -3,-32768,    27,-32768,    15,    25,
      27,    26,-32768,-32768,    28,    29,-32768,    31,     9,-32768,
      34,    36,    32,-32768,-32768,    33,    18,    19,-32768,    11,
      35,    30,    38,    44,    30,    40,-32768,    30,    46,    39,
  -32768,    30,    39,-32768,    39,-32768,    48,    11,    39,-32768,
      11,    39,-32768,-32768,-32768,    11,-32768,-32768,    18,-32768,
      19,-32768,-32768,-32768,    42,    61,-32768
};

static const short yypgoto[] =
{
  -32768,    62,-32768,-32768,   -11,-32768,   -17,    49,-32768,-32768,
  -32768,-32768,     1,-32768,-32768,-32768,-32768,-32768,   -13,-32768,
     -14,-32768,   -38,   -59
};


#define	YYLAST		66


static const short yytable[] =
{
      21,     5,    13,    69,     1,    70,     6,     7,    25,    73,
     -15,     8,    77,    40,    11,    26,    62,    41,    42,    64,
      43,     6,     7,    68,    50,    53,     8,    29,    12,    51,
      54,    33,    56,    20,    17,    30,    22,    18,    25,    36,
      49,    24,    85,    48,    32,    38,    57,    34,    37,    46,
      72,    47,    58,    76,    60,    61,    63,    65,    81,    71,
      66,    86,    79,     3,    19,    82,    83
};

static const short yycheck[] =
{
      17,     5,     3,    62,    12,    64,    10,    11,    11,    68,
      11,    15,    71,     4,    11,    18,    54,     8,     9,    57,
      11,    10,    11,    61,     6,     6,    15,    26,    11,    11,
      11,    30,    49,    11,    14,    20,    13,    16,    11,    11,
       7,    17,     0,    11,    19,    14,    11,    21,    19,    15,
      67,    15,    22,    70,    16,    11,    16,    11,    75,    11,
      21,     0,    73,     1,    15,    78,    80
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 1:
#line 165 "ql_y.y"
{
	    STRUCTURE *walk;

	    def = yyvsp[0].field;
	    for (walk = structures; walk; walk = walk->next)
		if (!walk->instances)
		    fprintf(stderr,"unused structure: %s\n",walk->id);
	}
    break;
case 3:
#line 177 "ql_y.y"
{
	    to_c("#%s\n",yyvsp[-1].str);
	    to_test("#%s\n",yyvsp[-1].str);
	    if (dump) to_dump("#%s\n",yyvsp[-1].str);
	}
    break;
case 6:
#line 190 "ql_y.y"
{
	    STRUCTURE *n;

	    n = alloc_t(STRUCTURE);
	    n->id = yyvsp[-2].str;
	    n->block = yyvsp[0].field;
	    n->instances = 0;
	    n->next = structures;
	    structures = n;
	}
    break;
case 7:
#line 203 "ql_y.y"
{
	    abort_id = NULL;
	}
    break;
case 8:
#line 207 "ql_y.y"
{
	    yyval.field = yyvsp[0].field;
	}
    break;
case 9:
#line 214 "ql_y.y"
{
	    STRUCTURE *walk;

	    for (walk = structures; walk; walk = walk->next)
		if (walk->id == yyvsp[0].str) break;
	    if (!walk) yyerror("no such structure");
	    walk->instances++;
	    yyval.field = alloc_t(FIELD);
	    yyval.field->id = NULL;
	    yyval.field->name_list = NULL;
	    yyval.field->value = NULL;
	    yyval.field->brk = 0;
	    yyval.field->structure = walk;
	    yyval.field->my_block = copy_block(walk->block);
	    yyval.field->next = NULL;
	    abort_id = NULL;
	}
    break;
case 10:
#line 232 "ql_y.y"
{
	    yyval.field = yyvsp[-1].field;
	    abort_id = NULL;
	}
    break;
case 11:
#line 237 "ql_y.y"
{
	    yyval.field = NULL;
	    abort_id = yyvsp[0].str;
	}
    break;
case 12:
#line 244 "ql_y.y"
{
	    yyval.field = NULL;
	}
    break;
case 13:
#line 248 "ql_y.y"
{
	    yyval.field = yyvsp[-1].field;
	    yyvsp[-1].field->next = yyvsp[0].field;
	}
    break;
case 14:
#line 256 "ql_y.y"
{
	    TAG *walk;

	    yyval.field = yyvsp[0].field;
	    yyval.field->name_list = yyvsp[-2].nlist;
	    yyval.field->brk = yyvsp[-4].num;
	    yyval.field->id = yyvsp[-3].str;
	    if (yyval.field->var_len == -2) {
		if (*yyval.field->id == '_') yyerror("var-len field must be named");
	    }
	    else if (*yyval.field->id == '_' && !yyval.field->value)
		    yyerror("unnamed fields must have value");
	    if (*yyval.field->id == '_' && yyval.field->value && yyval.field->value->type == vt_case)
		for (walk = yyval.field->value->tags; walk; walk = walk->next)
		    if (walk->more)
			yyerror("value list only allowed in named case "
			  "selections");
	    if (*yyval.field->id != '_' && yyval.field->value && yyval.field->value->type == vt_multi)
		yyerror("multi selectors must be unnamed");
	}
    break;
case 15:
#line 279 "ql_y.y"
{
	    yyval.num = 0;
	}
    break;
case 16:
#line 283 "ql_y.y"
{
	    yyval.num = 1;
	}
    break;
case 17:
#line 290 "ql_y.y"
{
	    yyval.field = alloc_t(FIELD);
	    yyval.field->size = yyvsp[-1].num;
	    yyval.field->var_len = -2; /* hack */
	    if (yyvsp[-1].num & 7) yyerror("var-len field must have integral size");
	    yyval.field->pos = 0;
	    yyval.field->flush = 1;
	    yyval.field->value = NULL;
	    yyval.field->structure = NULL;
	    yyval.field->next = NULL;
	}
    break;
case 18:
#line 302 "ql_y.y"
{
	    yyval.field = alloc_t(FIELD);
	    yyval.field->size = yyvsp[-4].num;
	    yyval.field->var_len = -1;
	    yyval.field->pos = yyvsp[-3].num;
	    yyval.field->flush = !yyvsp[-2].num;
	    if (yyval.field->pos == -1)
		if (yyval.field->size & 7)
		    yyerror("position required for small fields");
		else yyval.field->pos = 0;
	    yyval.field->value = yyvsp[0].value;
	    yyval.field->structure = NULL;
	    yyval.field->next = NULL;
	}
    break;
case 19:
#line 319 "ql_y.y"
{
	    yyval.num = -1;
	}
    break;
case 20:
#line 323 "ql_y.y"
{
	    yyval.num = yyvsp[0].num-1;
	    if (yyval.num < 0 || yyval.num > 7) yyerror("invalid position");
	}
    break;
case 21:
#line 331 "ql_y.y"
{
	    char *end;

	    yyval.num = strtoul(yyvsp[0].str,&end,10);
	    if (*end) yyerror("no a decimal number");
	}
    break;
case 22:
#line 340 "ql_y.y"
{
	    yyval.num = 0;
	}
    break;
case 23:
#line 344 "ql_y.y"
{
	    if (strcmp(yyvsp[0].str,"more")) yyerror("\"more\" expected");
	    yyval.num = 1;
	}
    break;
case 24:
#line 351 "ql_y.y"
{
	    yyval.value = NULL;
	}
    break;
case 25:
#line 355 "ql_y.y"
{
	    yyval.value = yyvsp[0].value;
	}
    break;
case 26:
#line 362 "ql_y.y"
{
	    yyval.value = alloc_t(VALUE);
	    yyval.value->type = vt_id;
	    yyval.value->id = yyvsp[0].str;
	}
    break;
case 27:
#line 368 "ql_y.y"
{
	    yyval.value = alloc_t(VALUE);
	    yyval.value->type = vt_case;
	    yyval.value->id = NULL;
	    yyval.value->tags = yyvsp[-1].tag;
	}
    break;
case 28:
#line 375 "ql_y.y"
{
	    yyval.value = alloc_t(VALUE);
	    yyval.value->type = vt_multi;
	    yyval.value->tags = yyvsp[-1].tag;
	}
    break;
case 29:
#line 381 "ql_y.y"
{
	    yyval.value = alloc_t(VALUE);
	    yyval.value->type = vt_length;
	    yyval.value->recovery = yyvsp[-2].str;
	    yyval.value->block = yyvsp[0].field;
	    yyval.value->abort_id = abort_id;
	}
    break;
case 30:
#line 391 "ql_y.y"
{
	    yyval.str = NULL;
	}
    break;
case 31:
#line 395 "ql_y.y"
{
	    yyval.str = yyvsp[0].str;
	}
    break;
case 32:
#line 401 "ql_y.y"
{
	    yyval.nlist = NULL;
	}
    break;
case 33:
#line 405 "ql_y.y"
{
	    yyval.nlist = get_name_list(yyvsp[0].str);
	}
    break;
case 34:
#line 411 "ql_y.y"
{
	    yyval.tag = NULL;
	}
    break;
case 35:
#line 415 "ql_y.y"
{
	    yyval.tag = alloc_t(TAG);
	    yyval.tag->deflt = 1;
	    if (yyvsp[-2].str) {
		yyval.tag->id = yyvsp[-3].str;
		yyval.tag->value = yyvsp[-2].str;
	    }
	    else {
		yyval.tag->id = NULL;
		yyval.tag->value = yyvsp[-3].str;
	    }
	    yyval.tag->more = yyvsp[-1].list;
	    yyval.tag->block = yyvsp[0].field;
	    yyval.tag->next = NULL;
	    yyval.tag->abort_id = abort_id;
	}
    break;
case 36:
#line 432 "ql_y.y"
{
	    yyval.tag = alloc_t(TAG);
	    yyval.tag->abort_id = abort_id;
	}
    break;
case 37:
#line 437 "ql_y.y"
{
	    yyval.tag = yyvsp[-1].tag;
	    yyval.tag->deflt = 0;
	    if (yyvsp[-4].str) {
		yyval.tag->id = yyvsp[-5].str;
		yyval.tag->value = yyvsp[-4].str;
	    }
	    else {
		yyval.tag->id = NULL;
		yyval.tag->value = yyvsp[-5].str;
	    }
	    yyval.tag->more = yyvsp[-3].list;
	    yyval.tag->block = yyvsp[-2].field;
	    yyval.tag->next = yyvsp[0].tag;
	}
    break;
case 38:
#line 455 "ql_y.y"
{
	    yyval.tag = NULL;
	}
    break;
case 39:
#line 459 "ql_y.y"
{
	    yyval.tag = alloc_t(TAG);
	    yyval.tag->deflt = 1;
	    if (yyvsp[-2].str) {
		yyval.tag->id = yyvsp[-3].str;
		yyval.tag->value = yyvsp[-2].str;
	    }
	    else {
		yyval.tag->id = NULL;
		yyval.tag->value = yyvsp[-3].str;
	    }
	    yyval.tag->more = yyvsp[-1].list;
	    yyval.tag->block = yyvsp[0].field;
	    yyval.tag->next = NULL;
	}
    break;
case 40:
#line 475 "ql_y.y"
{
	    yyval.tag = alloc_t(TAG);
	    yyval.tag->abort_id = abort_id;
	}
    break;
case 41:
#line 480 "ql_y.y"
{
	    yyval.tag = yyvsp[-1].tag;
	    yyval.tag->deflt = 0;
	    if (yyvsp[-4].str) {
		yyval.tag->id = yyvsp[-5].str;
		yyval.tag->value = yyvsp[-4].str;
	    }
	    else {
		yyval.tag->id = NULL;
		yyval.tag->value = yyvsp[-5].str;
	    }
	    yyval.tag->more = yyvsp[-3].list;
	    yyval.tag->block = yyvsp[-2].field;
	    yyval.tag->next = yyvsp[0].tag;
	}
    break;
case 42:
#line 498 "ql_y.y"
{
	    yyval.str = NULL;
	}
    break;
case 43:
#line 502 "ql_y.y"
{
	    yyval.str = yyvsp[0].str;
	}
    break;
case 44:
#line 508 "ql_y.y"
{
	    yyval.list = NULL;
	}
    break;
case 45:
#line 512 "ql_y.y"
{
	    yyval.list = alloc_t(VALUE_LIST);
	    yyval.list->value = yyvsp[-1].str;
	    yyval.list->next = yyvsp[0].list;
	}
    break;
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 518 "ql_y.y"
