/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 29 "giscanner/scannerparser.y" /* yacc.c:339  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glib.h>
#include <glib/gstdio.h>
#include "sourcescanner.h"
#include "scannerparser.h"

extern FILE *yyin;
extern int lineno;
extern char linebuf[2000];
extern char *yytext;

extern int yylex (GISourceScanner *scanner);
static void yyerror (GISourceScanner *scanner, const char *str);

extern void ctype_free (GISourceType * type);

static int last_enum_value = -1;
static gboolean is_bitfield;

/**
 * parse_c_string_literal:
 * @str: A string containing a C string literal
 *
 * Based on g_strcompress(), but also handles
 * hexadecimal escapes.
 */
static char *
parse_c_string_literal (const char *str)
{
  const gchar *p = str, *num;
  gchar *dest = g_malloc (strlen (str) + 1);
  gchar *q = dest;

  while (*p)
    {
      if (*p == '\\')
        {
          p++;
          switch (*p)
            {
            case '\0':
              g_warning ("parse_c_string_literal: trailing \\");
              goto out;
            case '0':  case '1':  case '2':  case '3':  case '4':
            case '5':  case '6':  case '7':
              *q = 0;
              num = p;
              while ((p < num + 3) && (*p >= '0') && (*p <= '7'))
                {
                  *q = (*q * 8) + (*p - '0');
                  p++;
                }
              q++;
              p--;
              break;
	    case 'x':
	      *q = 0;
	      p++;
	      num = p;
	      while ((p < num + 2) && (g_ascii_isxdigit(*p)))
		{
		  *q = (*q * 16) + g_ascii_xdigit_value(*p);
		  p++;
		}
              q++;
              p--;
	      break;
            case 'b':
              *q++ = '\b';
              break;
            case 'f':
              *q++ = '\f';
              break;
            case 'n':
              *q++ = '\n';
              break;
            case 'r':
              *q++ = '\r';
              break;
            case 't':
              *q++ = '\t';
              break;
            default:            /* Also handles \" and \\ */
              *q++ = *p;
              break;
            }
        }
      else
        *q++ = *p;
      p++;
    }
out:
  *q = 0;

  return dest;
}

enum {
  IRRELEVANT = 1,
  NOT_GI_SCANNER = 2,
  FOR_GI_SCANNER = 3,
};

static void
update_skipping (GISourceScanner *scanner)
{
  GList *l;
  for (l = scanner->conditionals.head; l != NULL; l = g_list_next (l))
    {
      if (GPOINTER_TO_INT (l->data) == NOT_GI_SCANNER)
        {
           scanner->skipping = TRUE;
           return;
        }
    }

  scanner->skipping = FALSE;
}

static void
push_conditional (GISourceScanner *scanner,
                  gint type)
{
  g_assert (type != 0);
  g_queue_push_head (&scanner->conditionals, GINT_TO_POINTER (type));
}

static gint
pop_conditional (GISourceScanner *scanner)
{
  gint type = GPOINTER_TO_INT (g_queue_pop_head (&scanner->conditionals));

  if (type == 0)
    {
      gchar *filename = g_file_get_path (scanner->current_file);
      fprintf (stderr, "%s:%d: mismatched %s", filename, lineno, yytext);
      g_free (filename);
    }

  return type;
}

static void
warn_if_cond_has_gi_scanner (GISourceScanner *scanner,
                             const gchar *text)
{
  /* Some other conditional that is not __GI_SCANNER__ */
  if (strstr (text, "__GI_SCANNER__"))
    {
      gchar *filename = g_file_get_path (scanner->current_file);
      fprintf (stderr, "%s:%d: the __GI_SCANNER__ constant should only be used with simple #ifdef or #endif: %s",
               filename, lineno, text);
      g_free (filename);
    }
}

static void
toggle_conditional (GISourceScanner *scanner)
{
  switch (pop_conditional (scanner))
    {
    case FOR_GI_SCANNER:
      push_conditional (scanner, NOT_GI_SCANNER);
      break;
    case NOT_GI_SCANNER:
      push_conditional (scanner, FOR_GI_SCANNER);
      break;
    case 0:
      break;
    default:
      push_conditional (scanner, IRRELEVANT);
      break;
    }
}

static void
set_or_merge_base_type (GISourceType *type,
                        GISourceType *base)
{
  if (base->type == CTYPE_INVALID)
    {
      g_assert (base->base_type == NULL);

      type->storage_class_specifier |= base->storage_class_specifier;
      type->type_qualifier |= base->type_qualifier;
      type->function_specifier |= base->function_specifier;
      type->is_bitfield |= base->is_bitfield;

      ctype_free (base);
    }
  else
    {
      g_assert (type->base_type == NULL);

      type->base_type = base;
    }
}


#line 270 "giscanner/scannerparser.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_GISCANNER_SCANNERPARSER_H_INCLUDED
# define YY_YY_GISCANNER_SCANNERPARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    BASIC_TYPE = 258,
    IDENTIFIER = 259,
    TYPEDEF_NAME = 260,
    INTEGER = 261,
    FLOATING = 262,
    BOOLEAN = 263,
    CHARACTER = 264,
    STRING = 265,
    INTL_CONST = 266,
    INTUL_CONST = 267,
    ELLIPSIS = 268,
    ADDEQ = 269,
    SUBEQ = 270,
    MULEQ = 271,
    DIVEQ = 272,
    MODEQ = 273,
    XOREQ = 274,
    ANDEQ = 275,
    OREQ = 276,
    SL = 277,
    SR = 278,
    SLEQ = 279,
    SREQ = 280,
    EQ = 281,
    NOTEQ = 282,
    LTEQ = 283,
    GTEQ = 284,
    ANDAND = 285,
    OROR = 286,
    PLUSPLUS = 287,
    MINUSMINUS = 288,
    ARROW = 289,
    AUTO = 290,
    BREAK = 291,
    CASE = 292,
    CONST = 293,
    CONTINUE = 294,
    DEFAULT = 295,
    DO = 296,
    ELSE = 297,
    ENUM = 298,
    EXTENSION = 299,
    EXTERN = 300,
    FOR = 301,
    GOTO = 302,
    IF = 303,
    INLINE = 304,
    REGISTER = 305,
    RESTRICT = 306,
    RETURN = 307,
    SHORT = 308,
    SIGNED = 309,
    SIZEOF = 310,
    STATIC = 311,
    STRUCT = 312,
    SWITCH = 313,
    THREAD_LOCAL = 314,
    TYPEDEF = 315,
    UNION = 316,
    UNSIGNED = 317,
    VOID = 318,
    VOLATILE = 319,
    WHILE = 320,
    FUNCTION_MACRO = 321,
    OBJECT_MACRO = 322,
    IFDEF_GI_SCANNER = 323,
    IFNDEF_GI_SCANNER = 324,
    IFDEF_COND = 325,
    IFNDEF_COND = 326,
    IF_COND = 327,
    ELIF_COND = 328,
    ELSE_COND = 329,
    ENDIF_COND = 330
  };
#endif
/* Tokens.  */
#define BASIC_TYPE 258
#define IDENTIFIER 259
#define TYPEDEF_NAME 260
#define INTEGER 261
#define FLOATING 262
#define BOOLEAN 263
#define CHARACTER 264
#define STRING 265
#define INTL_CONST 266
#define INTUL_CONST 267
#define ELLIPSIS 268
#define ADDEQ 269
#define SUBEQ 270
#define MULEQ 271
#define DIVEQ 272
#define MODEQ 273
#define XOREQ 274
#define ANDEQ 275
#define OREQ 276
#define SL 277
#define SR 278
#define SLEQ 279
#define SREQ 280
#define EQ 281
#define NOTEQ 282
#define LTEQ 283
#define GTEQ 284
#define ANDAND 285
#define OROR 286
#define PLUSPLUS 287
#define MINUSMINUS 288
#define ARROW 289
#define AUTO 290
#define BREAK 291
#define CASE 292
#define CONST 293
#define CONTINUE 294
#define DEFAULT 295
#define DO 296
#define ELSE 297
#define ENUM 298
#define EXTENSION 299
#define EXTERN 300
#define FOR 301
#define GOTO 302
#define IF 303
#define INLINE 304
#define REGISTER 305
#define RESTRICT 306
#define RETURN 307
#define SHORT 308
#define SIGNED 309
#define SIZEOF 310
#define STATIC 311
#define STRUCT 312
#define SWITCH 313
#define THREAD_LOCAL 314
#define TYPEDEF 315
#define UNION 316
#define UNSIGNED 317
#define VOID 318
#define VOLATILE 319
#define WHILE 320
#define FUNCTION_MACRO 321
#define OBJECT_MACRO 322
#define IFDEF_GI_SCANNER 323
#define IFNDEF_GI_SCANNER 324
#define IFDEF_COND 325
#define IFNDEF_COND 326
#define IF_COND 327
#define ELIF_COND 328
#define ELSE_COND 329
#define ENDIF_COND 330

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 234 "giscanner/scannerparser.y" /* yacc.c:355  */

  char *str;
  GList *list;
  GISourceSymbol *symbol;
  GISourceType *ctype;
  StorageClassSpecifier storage_class_specifier;
  TypeQualifier type_qualifier;
  FunctionSpecifier function_specifier;
  UnaryOperator unary_operator;

#line 471 "giscanner/scannerparser.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (GISourceScanner* scanner);

#endif /* !YY_YY_GISCANNER_SCANNERPARSER_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 488 "giscanner/scannerparser.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  72
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2370

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  100
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  78
/* YYNRULES -- Number of rules.  */
#define YYNRULES  252
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  422

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   330

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    89,     2,     2,     2,    91,    84,     2,
      76,    77,    85,    86,    83,    87,    82,    90,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    97,    99,
      92,    98,    93,    96,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    80,     2,    81,    94,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    78,    95,    79,    88,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   328,   328,   337,   353,   359,   365,   372,   373,   377,
     385,   400,   414,   421,   422,   426,   427,   431,   435,   439,
     443,   447,   451,   458,   459,   463,   464,   468,   472,   495,
     502,   509,   513,   521,   525,   529,   533,   537,   541,   548,
     549,   561,   562,   568,   576,   587,   588,   594,   603,   604,
     616,   625,   626,   632,   638,   644,   653,   654,   660,   669,
     670,   679,   680,   689,   690,   699,   700,   711,   712,   723,
     724,   731,   732,   739,   740,   741,   742,   743,   744,   745,
     746,   747,   748,   749,   753,   754,   755,   762,   768,   786,
     793,   798,   803,   816,   817,   822,   827,   832,   840,   844,
     851,   852,   856,   860,   864,   868,   872,   876,   883,   890,
     894,   898,   902,   907,   908,   909,   917,   937,   942,   950,
     955,   963,   964,   971,   991,   996,   997,  1002,  1010,  1014,
    1022,  1025,  1026,  1030,  1041,  1048,  1055,  1062,  1069,  1076,
    1085,  1085,  1094,  1102,  1110,  1122,  1126,  1130,  1134,  1141,
    1148,  1153,  1157,  1162,  1166,  1171,  1176,  1186,  1193,  1202,
    1207,  1211,  1222,  1235,  1236,  1243,  1247,  1254,  1259,  1264,
    1269,  1276,  1282,  1291,  1292,  1296,  1301,  1302,  1310,  1314,
    1319,  1324,  1329,  1334,  1340,  1350,  1356,  1369,  1376,  1377,
    1378,  1382,  1383,  1389,  1390,  1391,  1392,  1393,  1394,  1398,
    1399,  1400,  1404,  1405,  1409,  1410,  1414,  1415,  1419,  1420,
    1424,  1425,  1426,  1430,  1431,  1432,  1433,  1434,  1435,  1436,
    1437,  1438,  1439,  1443,  1444,  1445,  1446,  1447,  1453,  1454,
    1458,  1459,  1460,  1464,  1465,  1469,  1470,  1476,  1483,  1490,
    1494,  1511,  1516,  1521,  1526,  1531,  1536,  1543,  1548,  1556,
    1557,  1558,  1559
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "BASIC_TYPE", "\"identifier\"",
  "\"typedef-name\"", "INTEGER", "FLOATING", "BOOLEAN", "CHARACTER",
  "STRING", "INTL_CONST", "INTUL_CONST", "ELLIPSIS", "ADDEQ", "SUBEQ",
  "MULEQ", "DIVEQ", "MODEQ", "XOREQ", "ANDEQ", "OREQ", "SL", "SR", "SLEQ",
  "SREQ", "EQ", "NOTEQ", "LTEQ", "GTEQ", "ANDAND", "OROR", "PLUSPLUS",
  "MINUSMINUS", "ARROW", "AUTO", "BREAK", "CASE", "CONST", "CONTINUE",
  "DEFAULT", "DO", "ELSE", "ENUM", "EXTENSION", "EXTERN", "FOR", "GOTO",
  "IF", "INLINE", "REGISTER", "RESTRICT", "RETURN", "SHORT", "SIGNED",
  "SIZEOF", "STATIC", "STRUCT", "SWITCH", "THREAD_LOCAL", "TYPEDEF",
  "UNION", "UNSIGNED", "VOID", "VOLATILE", "WHILE", "FUNCTION_MACRO",
  "OBJECT_MACRO", "IFDEF_GI_SCANNER", "IFNDEF_GI_SCANNER", "IFDEF_COND",
  "IFNDEF_COND", "IF_COND", "ELIF_COND", "ELSE_COND", "ENDIF_COND", "'('",
  "')'", "'{'", "'}'", "'['", "']'", "'.'", "','", "'&'", "'*'", "'+'",
  "'-'", "'~'", "'!'", "'/'", "'%'", "'<'", "'>'", "'^'", "'|'", "'?'",
  "':'", "'='", "';'", "$accept", "primary_expression", "strings",
  "identifier", "identifier_or_typedef_name", "postfix_expression",
  "argument_expression_list", "unary_expression", "unary_operator",
  "cast_expression", "multiplicative_expression", "additive_expression",
  "shift_expression", "relational_expression", "equality_expression",
  "and_expression", "exclusive_or_expression", "inclusive_or_expression",
  "logical_and_expression", "logical_or_expression",
  "conditional_expression", "assignment_expression", "assignment_operator",
  "expression", "constant_expression", "declaration",
  "declaration_specifiers", "init_declarator_list", "init_declarator",
  "storage_class_specifier", "basic_type", "type_specifier",
  "struct_or_union_specifier", "struct_or_union",
  "struct_declaration_list", "struct_declaration",
  "specifier_qualifier_list", "struct_declarator_list",
  "struct_declarator", "enum_specifier", "enum_keyword", "enumerator_list",
  "$@1", "enumerator", "type_qualifier", "function_specifier",
  "declarator", "direct_declarator", "pointer", "type_qualifier_list",
  "parameter_list", "parameter_declaration", "identifier_list",
  "type_name", "abstract_declarator", "direct_abstract_declarator",
  "typedef_name", "initializer", "initializer_list", "statement",
  "labeled_statement", "compound_statement", "block_item_list",
  "block_item", "expression_statement", "selection_statement",
  "iteration_statement", "jump_statement", "translation_unit",
  "external_declaration", "function_definition", "declaration_list",
  "function_macro", "object_macro", "function_macro_define",
  "object_macro_define", "preproc_conditional", "macro", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,    40,    41,   123,   125,
      91,    93,    46,    44,    38,    42,    43,    45,   126,    33,
      47,    37,    60,    62,    94,   124,    63,    58,    61,    59
};
# endif

#define YYPACT_NINF -237

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-237)))

#define YYTABLE_NINF -15

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    2078,  -237,  -237,  -237,  -237,  -237,  -237,  -237,  -237,  -237,
    -237,  -237,  -237,  -237,  -237,  -237,  -237,  -237,  -237,  -237,
    -237,  -237,  -237,  -237,  -237,  -237,  -237,  -237,  -237,  -237,
    -237,  -237,    11,  2306,  -237,  2306,  -237,    44,  -237,    47,
    2306,  2306,  -237,  1997,  -237,  -237,   -41,  1635,  -237,  -237,
    -237,  -237,  -237,   138,   207,  -237,  -237,   -61,  -237,   884,
      87,    19,  -237,  -237,  2179,  -237,   -23,  -237,  -237,   -10,
    -237,  -237,  -237,  -237,    74,  -237,  -237,  -237,  -237,  -237,
      29,    50,  1674,  1674,    65,  1724,  1222,  -237,  -237,  -237,
    -237,  -237,  -237,  -237,   134,  -237,   145,  -237,  1635,  -237,
      58,    94,   209,   176,   267,    98,    96,   104,   211,    25,
    -237,  -237,   136,  -237,  -237,   207,   138,  -237,   441,  1322,
    -237,    11,  -237,  2047,  2151,  1341,    87,  2179,  1929,  -237,
      21,  2179,  2179,   137,    74,  -237,  -237,   -18,  1674,  1674,
    1763,  -237,  -237,   129,  1222,  -237,  1782,   177,  -237,  -237,
     -11,   130,   166,  -237,  -237,  -237,   324,  1380,  1763,   324,
    -237,  1635,  1635,  1635,  1635,  1635,  1635,  1635,  1635,  1635,
    1635,  1635,  1635,  1635,  1635,  1635,  1635,  1635,  1635,  1763,
    -237,  -237,  -237,  -237,   154,   178,  1635,   185,   189,   827,
     196,   324,   237,   945,   240,   243,  -237,  -237,   210,   233,
     -30,  -237,   252,  -237,  -237,  -237,   547,  -237,  -237,  -237,
    -237,  -237,  1322,  -237,  -237,  -237,  -237,  -237,  -237,    90,
      69,  -237,   152,  -237,   224,  -237,  -237,  -237,  1635,   -20,
    -237,   257,  -237,  1966,  -237,    32,   241,  -237,   231,  -237,
      74,   278,   279,  1782,   741,   280,  1160,   275,  -237,  -237,
    -237,  -237,  -237,  -237,  -237,  -237,  -237,  -237,  -237,  1635,
    -237,  1635,  1902,  1430,   242,  -237,   245,  1635,  -237,  -237,
     173,  -237,   -49,  -237,  -237,  -237,  -237,    58,    58,    94,
      94,   209,   209,   209,   209,   176,   176,   267,    98,    96,
     104,   211,   -22,  -237,   263,  -237,   827,   296,   961,   264,
    1763,  -237,    18,  1763,  1763,   827,  -237,  -237,  -237,  -237,
     248,  1840,  -237,    39,  -237,  -237,  2275,  -237,  -237,  -237,
      21,  -237,  1635,  -237,  -237,  -237,  1635,  -237,    37,  -237,
    -237,  -237,   644,  -237,  -237,  -237,  -237,   180,   285,  -237,
     283,   245,  2213,  1469,  -237,  -237,  1635,  -237,  1763,   827,
    -237,   289,  1047,    46,  -237,   182,  -237,   184,   193,  -237,
    -237,  1283,  -237,  -237,  -237,  -237,  -237,   290,  -237,  -237,
    -237,  -237,   197,  -237,   287,  -237,   275,  -237,  1763,  1488,
      72,  1063,   827,   827,   827,  -237,  -237,  -237,  -237,  -237,
     208,   827,   218,  1527,  1577,    73,   329,  -237,  -237,   274,
    -237,   827,   827,   220,   827,   221,  1616,   827,  -237,  -237,
    -237,   827,  -237,   827,   827,   229,  -237,  -237,  -237,  -237,
     827,  -237
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,   252,   108,   187,   105,   145,   139,   147,   103,   149,
     106,   146,   110,   104,   119,   107,   102,   120,   111,   109,
     148,   237,   238,   241,   242,   243,   244,   245,   246,   247,
     248,   231,     0,    91,   112,    93,   113,     0,   114,     0,
      95,    97,   115,     0,   228,   230,     0,     0,   249,   250,
     251,   232,    12,     0,   160,    89,   152,     0,    98,   100,
     151,     0,    90,    92,     0,    13,   118,    14,   140,   138,
      94,    96,     1,   229,     0,     3,     6,     4,     5,    10,
       0,     0,     0,     0,     0,     0,     0,    33,    34,    35,
      36,    37,    38,    15,     7,     2,    25,    39,     0,    41,
      45,    48,    51,    56,    59,    61,    63,    65,    67,    69,
      87,   240,     0,   163,   162,   159,     0,    88,     0,     0,
     235,     0,   234,     0,     0,     0,   150,   125,     0,   121,
     130,   127,     0,     0,     0,   140,   171,     0,     0,     0,
       0,    26,    27,     0,     0,    31,   147,    39,    71,    84,
       0,   173,     0,    11,    21,    22,     0,     0,     0,     0,
      28,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     153,   164,   161,    99,   100,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   202,   208,     2,     0,
       0,   206,   115,   207,   193,   194,     0,   204,   195,   196,
     197,   198,     0,   188,   101,   236,   233,   170,   158,   169,
       0,   165,     0,   155,     0,   124,   117,   122,     0,     0,
     128,   131,   126,     0,   135,     0,   143,   141,     0,   239,
       0,     0,     0,     0,     0,     0,     0,    86,    77,    78,
      74,    75,    76,    82,    81,    83,    79,    80,    73,     0,
       8,     0,     0,     0,   175,   174,   176,     0,    20,    18,
       0,    23,     0,    19,    42,    43,    44,    46,    47,    49,
      50,    54,    55,    52,    53,    57,    58,    60,    62,    64,
      66,    68,     0,   225,     0,   224,     0,     0,     0,     0,
       0,   226,     0,     0,     0,     0,   209,   203,   205,   191,
       0,     0,   167,   175,   168,   156,     0,   157,   154,   132,
     130,   123,     0,   116,   137,   142,     0,   134,     0,   172,
      29,    30,     0,    32,    72,    85,   183,     0,     0,   179,
       0,   177,     0,     0,    40,    17,     0,    16,     0,     0,
     201,     0,     0,     0,   223,     0,   227,     0,     0,   199,
     189,     0,   166,   129,   133,   144,   136,     0,   184,   178,
     180,   185,     0,   181,     0,    24,    70,   200,     0,     0,
       0,     0,     0,     0,     0,   190,   192,     9,   186,   182,
       0,     0,     0,     0,     0,     0,   210,   212,   213,     0,
     215,     0,     0,     0,     0,     0,     0,     0,   214,   219,
     217,     0,   216,     0,     0,     0,   211,   221,   220,   218,
       0,   222
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -237,  -237,  -237,   -32,   -26,  -237,  -237,   255,  -237,   -81,
     183,   175,   164,   172,   192,   199,   204,   206,   212,  -237,
     -39,   -99,  -237,   -70,  -168,    14,     4,  -237,   268,  -237,
    -237,     7,  -237,  -237,   254,  -101,   -77,  -237,    67,  -237,
    -237,   256,  -237,   258,   -24,  -237,   -31,   -59,   -51,  -237,
    -118,    81,   265,   259,  -105,  -236,    -6,  -200,  -237,  -165,
    -237,   -40,   155,  -196,  -237,  -237,  -237,  -237,  -237,   357,
    -237,  -237,  -237,  -237,  -237,  -237,  -237,  -237
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    93,    94,    95,   199,    96,   270,   147,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     148,   149,   259,   200,   111,   201,   121,    57,    58,    33,
      34,    35,    36,    37,   128,   129,   130,   229,   230,    38,
      39,   133,   134,   325,    40,    41,   112,    60,    61,   115,
     337,   221,   137,   152,   338,   266,    42,   214,   310,   203,
     204,   205,   206,   207,   208,   209,   210,   211,    43,    44,
      45,   123,    46,    47,    48,    49,    50,    51
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      56,    59,   126,   114,    32,    65,   220,    65,   110,   151,
     308,    66,   309,    69,    31,    52,   150,   160,   294,   122,
     213,    56,   116,    52,   297,    52,   224,   227,   341,    56,
     113,    67,   347,    67,   261,    74,    52,    62,   117,    63,
     131,    52,   136,    52,    70,    71,   265,    32,    52,     3,
     225,    52,     3,   261,   232,   132,   178,    31,   271,   239,
     319,   261,   131,   320,   182,   240,   260,   151,   135,   306,
     150,   127,   261,   120,   150,   348,   247,   341,    52,   321,
     274,   275,   276,   216,    56,   184,   198,    53,   272,    56,
     184,   181,   136,   127,    52,    53,    54,    53,    56,   231,
     264,   261,   236,   131,   131,   138,    54,   131,   131,   292,
      55,   324,   202,   213,   314,   311,   366,   356,   228,   263,
     131,   179,    64,   302,    65,    68,   139,    65,   219,   261,
     268,   350,   227,   273,   127,   127,   308,   215,   127,   127,
     359,   143,    52,   161,   153,   381,   315,   110,   162,   163,
      67,   127,   316,    67,   364,   261,   261,   198,   365,    65,
     334,   386,   335,   124,   340,   299,   311,   125,   313,   151,
     263,   393,   406,   247,   198,    54,   150,   154,   155,   156,
     164,   165,   174,    67,   377,    67,   344,    56,   312,   110,
     175,   248,   249,   250,   251,   252,   253,   254,   255,   176,
     202,   256,   257,   236,   168,   169,   262,   244,   329,   131,
     263,   264,   198,   180,    53,    54,   234,   396,   397,   398,
     235,   157,   131,    54,   372,   158,   400,   159,   353,   317,
     355,   166,   167,   357,   358,   240,   409,   410,   202,   412,
     127,   177,   416,   267,   374,     5,   417,   375,   418,   419,
     345,     7,   119,   127,   126,   421,   346,   368,    11,   382,
     313,   383,   213,   316,   198,   261,   219,   261,   170,   171,
     384,    20,   298,   198,   388,   258,   261,   293,   376,    56,
     316,    56,   380,   110,   295,   399,   296,   110,    56,   231,
      67,   261,    54,   172,   173,   401,   236,   411,   413,    67,
     198,   261,    97,   261,   261,   318,   420,   -13,   390,   392,
     327,   395,   261,   300,   328,   219,   303,   198,   262,   304,
     219,   342,   263,   403,   405,   343,   202,   360,    52,     3,
     305,   361,   281,   282,   283,   284,   415,   141,   142,   326,
     145,   279,   280,    67,   285,   286,   219,   277,   278,   -14,
     198,   198,   198,    97,   322,   330,   331,   333,   261,   198,
     349,   351,   369,   354,   370,   378,   287,   387,   389,   198,
     198,   407,   198,   408,   288,   198,    67,    67,    67,   198,
     289,   198,   198,   290,   183,    67,   233,   363,   198,   222,
     291,   238,   237,   241,   242,    67,    67,   362,    67,   332,
      73,    67,     0,   245,     0,    67,     0,    67,    67,     0,
       0,     0,     0,     0,    67,     0,    97,    97,    97,    97,
      97,    97,    97,    97,    97,    97,    97,    97,    97,    97,
      97,    97,    97,    97,     0,     0,     0,     0,     0,     0,
       0,    97,     0,     0,     2,    52,     3,    75,    76,    77,
      78,    79,    80,    81,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    82,    83,     0,     4,   185,   186,     5,
     187,   188,   189,    97,     6,   146,     8,   190,   191,   192,
       9,    10,    11,   193,     0,    12,    85,    13,    14,   194,
      15,    16,    17,    18,    19,    20,   195,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    86,     0,   118,
     196,     0,    97,     0,     0,    87,    88,    89,    90,    91,
      92,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     197,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       2,    52,     3,    75,    76,    77,    78,    79,    80,    81,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    97,     0,    82,
      83,    97,     4,   185,   186,     5,   187,   188,   189,     0,
       6,   146,     8,   190,   191,   192,     9,    10,    11,   193,
       0,    12,    85,    13,    14,   194,    15,    16,    17,    18,
      19,    20,   195,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    86,     0,   118,   307,     0,     0,     0,
       0,    87,    88,    89,    90,    91,    92,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   197,     2,    52,     3,
      75,    76,    77,    78,    79,    80,    81,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    82,    83,     0,     4,
     185,   186,     5,   187,   188,   189,     0,     6,   146,     8,
     190,   191,   192,     9,    10,    11,   193,     0,    12,    85,
      13,    14,   194,    15,    16,    17,    18,    19,    20,   195,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      86,     0,   118,   367,     0,     0,     0,     0,    87,    88,
      89,    90,    91,    92,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   197,     2,    52,     3,    75,    76,    77,
      78,    79,    80,    81,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    82,    83,     0,     4,   185,   186,     5,
     187,   188,   189,     0,     6,   146,     8,   190,   191,   192,
       9,    10,    11,   193,     0,    12,    85,    13,    14,   194,
      15,    16,    17,    18,    19,    20,   195,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    86,     0,   118,
       0,     0,     0,     0,     0,    87,    88,    89,    90,    91,
      92,    52,     3,    75,    76,    77,    78,    79,    80,    81,
     197,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    82,
      83,     0,     0,   185,   186,     0,   187,   188,   189,     0,
       0,   243,     0,   190,   191,   192,     0,     0,     0,   193,
       0,     0,    85,     0,     0,   194,     0,     2,     0,     3,
       0,     0,   195,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    86,     0,   118,     0,     0,     0,     0,
       0,    87,    88,    89,    90,    91,    92,     0,     0,     4,
       0,     0,     5,     0,     0,     0,   197,     6,     7,     8,
       0,     0,     0,     9,    10,    11,     0,     0,    12,     0,
      13,    14,     0,    15,    16,    17,    18,    19,    20,    52,
       0,    75,    76,    77,    78,    79,    80,    81,     0,     0,
       0,     0,   118,     0,     0,    52,     0,    75,    76,    77,
      78,    79,    80,    81,     0,     0,     0,    82,    83,     0,
       0,     0,   119,     0,     0,     0,     0,     0,     0,   243,
       0,     0,     0,    82,    83,     0,     0,     0,     0,     0,
      85,     0,     0,     0,     0,   243,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    85,     0,     0,     0,
       0,    86,     0,     0,     0,     0,     0,     0,     0,    87,
      88,    89,    90,    91,    92,     0,     0,    86,     0,     0,
       0,     0,     0,     0,   301,    87,    88,    89,    90,    91,
      92,    52,     0,    75,    76,    77,    78,    79,    80,    81,
     352,     0,     0,     0,     0,     0,     0,    52,     0,    75,
      76,    77,    78,    79,    80,    81,     0,     0,     0,    82,
      83,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   243,     0,     0,     0,    82,    83,     0,     0,     0,
       0,     0,    85,     0,     0,     0,     0,   243,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    85,     0,
       0,     0,     0,    86,     0,     0,     0,     0,     0,     0,
       0,    87,    88,    89,    90,    91,    92,     0,     0,    86,
       0,     0,     0,     0,     0,     0,   379,    87,    88,    89,
      90,    91,    92,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   394,     2,    52,     3,    75,    76,    77,    78,
      79,    80,    81,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    82,    83,     0,     0,     0,     0,     5,     0,
       0,     0,     0,     6,   146,     0,     0,     0,     0,     0,
       0,    11,     0,     0,    12,    85,     0,    14,     0,     0,
       0,    17,    18,    19,    20,     2,    52,     3,    75,    76,
      77,    78,    79,    80,    81,     0,    86,     0,   244,     0,
       0,     0,     0,     0,    87,    88,    89,    90,    91,    92,
       0,     0,     0,     0,    82,    83,     0,     0,     0,     0,
       5,     0,     0,     0,     0,     6,   146,     0,     0,     0,
       0,     0,     0,    11,     0,     0,    12,    85,     0,    14,
       0,     0,     0,    17,    18,    19,    20,    52,     0,    75,
      76,    77,    78,    79,    80,    81,     0,     0,    86,     0,
       0,     0,     0,     0,     0,     0,    87,    88,    89,    90,
      91,    92,     0,     0,     0,    82,    83,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    52,    84,    75,    76,
      77,    78,    79,    80,    81,     0,     0,     0,    85,     0,
       0,     0,     0,     0,     0,    52,     0,    75,    76,    77,
      78,    79,    80,    81,    82,    83,     0,     0,     0,    86,
       0,   212,   385,     0,     0,     0,    84,    87,    88,    89,
      90,    91,    92,    82,    83,     0,     0,    85,     0,     0,
       0,     0,     0,     0,    52,    84,    75,    76,    77,    78,
      79,    80,    81,     0,     0,     0,    85,     0,    86,     0,
     212,     0,     0,     0,     0,     0,    87,    88,    89,    90,
      91,    92,    82,    83,     0,     0,     0,    86,     0,     0,
       0,     0,   223,     0,    84,    87,    88,    89,    90,    91,
      92,     0,     0,     0,    52,    85,    75,    76,    77,    78,
      79,    80,    81,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    86,   269,     0,     0,
       0,     0,    82,    83,    87,    88,    89,    90,    91,    92,
       0,     0,     0,    52,    84,    75,    76,    77,    78,    79,
      80,    81,     0,     0,     0,    85,     0,     0,     0,     0,
       0,     0,    52,     0,    75,    76,    77,    78,    79,    80,
      81,    82,    83,     0,     0,     0,    86,     0,     0,     0,
       0,   339,     0,    84,    87,    88,    89,    90,    91,    92,
      82,    83,     0,     0,    85,     0,     0,     0,     0,     0,
       0,    52,   243,    75,    76,    77,    78,    79,    80,    81,
       0,     0,     0,    85,     0,    86,     0,     0,     0,     0,
     373,     0,     0,    87,    88,    89,    90,    91,    92,    82,
      83,     0,     0,     0,    86,   391,     0,     0,     0,     0,
       0,   243,    87,    88,    89,    90,    91,    92,     0,     0,
       0,    52,    85,    75,    76,    77,    78,    79,    80,    81,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    86,   402,     0,     0,     0,     0,    82,
      83,    87,    88,    89,    90,    91,    92,     0,     0,     0,
      52,   243,    75,    76,    77,    78,    79,    80,    81,     0,
       0,     0,    85,     0,     0,     0,     0,     0,     0,    52,
       0,    75,    76,    77,    78,    79,    80,    81,    82,    83,
       0,     0,     0,    86,   404,     0,     0,     0,     0,     0,
     243,    87,    88,    89,    90,    91,    92,    82,    83,     0,
       0,    85,     0,     0,     0,     0,     0,     0,    52,    84,
      75,    76,    77,    78,    79,    80,    81,     0,     0,     0,
      85,     0,    86,   414,     0,     0,     0,     0,     0,     0,
      87,    88,    89,    90,    91,    92,    82,    83,     0,     0,
       0,    86,     0,     0,     0,     0,     0,     0,    84,    87,
      88,    89,    90,    91,    92,     0,     0,     0,    52,    85,
      75,    76,    77,    78,    79,    80,    81,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     140,     0,     0,     0,     0,     0,    82,    83,    87,    88,
      89,    90,    91,    92,     0,     0,     0,    52,    84,    75,
      76,    77,    78,    79,    80,    81,     0,     0,     0,    85,
       0,     0,     0,     0,     0,     0,    52,     0,    75,    76,
      77,    78,    79,    80,    81,    82,    83,     0,     0,     0,
     144,     0,     0,     0,     0,     0,     0,   243,    87,    88,
      89,    90,    91,    92,    82,    83,     0,     0,    85,     0,
       0,     0,     0,     0,     0,     0,   243,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    85,     0,    86,
       0,     0,     0,     2,    52,     3,     0,    87,    88,    89,
      90,    91,    92,   217,     0,     0,     0,     0,   246,     0,
       0,     0,     0,     0,     0,     0,    87,    88,    89,    90,
      91,    92,     0,     0,     0,     4,     0,     0,     5,     0,
       0,     0,     0,     6,     7,     8,     0,     0,     0,     9,
      10,    11,     0,     0,    12,     0,    13,    14,     0,    15,
      16,    17,    18,    19,    20,     2,     0,     3,     0,     0,
       0,     0,     0,     0,     0,   217,   311,   336,     0,     0,
     263,     0,     0,     0,     0,    54,     0,     0,     0,     0,
       0,     0,     2,     0,     3,     0,     0,     4,     0,     0,
       5,     0,     0,     0,     0,     6,     7,     8,     0,     0,
       0,     9,    10,    11,     0,     0,    12,     0,    13,    14,
       0,    15,    16,    17,    18,    19,    20,     5,     0,     2,
       0,     3,     6,     7,     0,     0,     0,     0,   262,   336,
      11,     0,   263,    12,     0,     0,    14,    54,     0,     0,
      17,    18,    19,    20,     0,     0,     0,    72,     1,     0,
       2,     0,     3,     0,     5,     0,     0,     0,   226,     6,
       7,     0,     0,     0,     0,     0,     0,    11,     0,     0,
      12,     0,     0,    14,     0,     0,     0,    17,    18,    19,
      20,     0,     4,     0,     0,     5,     0,     0,     0,     0,
       6,     7,     8,     0,     0,   323,     9,    10,    11,     0,
       2,    12,     3,    13,    14,     0,    15,    16,    17,    18,
      19,    20,     0,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,     0,     0,     0,     0,     0,     0,     1,
       0,     2,     4,     3,     0,     5,     0,     0,     0,     0,
       6,     7,     8,     0,     0,     0,     9,    10,    11,     0,
       0,    12,     0,    13,    14,     0,    15,    16,    17,    18,
      19,    20,     0,     4,     0,     0,     5,     0,     0,     0,
       0,     6,     7,     8,     0,   118,     0,     9,    10,    11,
       0,     0,    12,     0,    13,    14,     0,    15,    16,    17,
      18,    19,    20,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,     2,    52,     3,     0,     0,     0,
       0,     0,     0,     0,   217,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     2,     0,     3,     0,     4,     0,     0,     5,
       0,     0,     0,     0,     6,     7,     8,     0,     0,     0,
       9,    10,    11,     0,     0,    12,     0,    13,    14,     0,
      15,    16,    17,    18,    19,    20,     2,     5,     3,     0,
       0,     0,     6,     7,     0,     0,   217,     0,   218,     0,
      11,     0,     0,    12,     0,     0,    14,     0,     0,     0,
      17,    18,    19,    20,     0,     0,     0,     0,     4,     0,
       0,     5,     0,     0,     0,     0,     6,     7,     8,     0,
       0,     0,     9,    10,    11,     0,     0,    12,     0,    13,
      14,     0,    15,    16,    17,    18,    19,    20,     2,     0,
       3,     0,     0,     0,     0,     0,     0,     0,   217,     0,
     371,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     2,
       4,     3,     0,     5,     0,     0,     0,     0,     6,     7,
       8,     0,     0,     0,     9,    10,    11,     0,     0,    12,
       0,    13,    14,     0,    15,    16,    17,    18,    19,    20,
       0,     4,     0,     0,     5,     0,     0,     0,     0,     6,
       7,     8,     0,     0,     0,     9,    10,    11,     0,     0,
      12,     0,    13,    14,     0,    15,    16,    17,    18,    19,
      20
};

static const yytype_int16 yycheck[] =
{
      32,    32,    61,    54,     0,    37,   124,    39,    47,    86,
     206,    37,   212,    39,     0,     4,    86,    98,   186,    59,
     119,    53,    83,     4,   189,     4,   125,   128,   264,    61,
      54,    37,    81,    39,    83,    76,     4,    33,    99,    35,
      64,     4,    74,     4,    40,    41,   151,    43,     4,     5,
     127,     4,     5,    83,   131,    78,    31,    43,   157,    77,
     228,    83,    86,    83,   115,    83,    77,   144,    78,    99,
     140,    64,    83,    59,   144,    97,   146,   313,     4,    99,
     161,   162,   163,   123,   116,   116,   118,    76,   158,   121,
     121,   115,   124,    86,     4,    76,    85,    76,   130,   130,
     151,    83,   134,   127,   128,    76,    85,   131,   132,   179,
      99,    79,   118,   212,   219,    76,    79,    99,    97,    80,
     144,    96,    78,   193,   156,    78,    76,   159,   124,    83,
     156,   296,   233,   159,   127,   128,   332,   123,   131,   132,
     305,    76,     4,    85,    10,    99,    77,   186,    90,    91,
     156,   144,    83,   159,   322,    83,    83,   189,   326,   191,
     259,   361,   261,    76,   263,   191,    76,    80,   219,   246,
      80,    99,    99,   243,   206,    85,   246,    32,    33,    34,
      86,    87,    84,   189,   349,   191,   267,   219,   219,   228,
      94,    14,    15,    16,    17,    18,    19,    20,    21,    95,
     206,    24,    25,   235,    28,    29,    76,    78,   240,   233,
      80,   262,   244,    77,    76,    85,    79,   382,   383,   384,
      83,    76,   246,    85,   342,    80,   391,    82,   298,    77,
     300,    22,    23,   303,   304,    83,   401,   402,   244,   404,
     233,    30,   407,    77,   343,    38,   411,   346,   413,   414,
      77,    44,    98,   246,   313,   420,    83,    77,    51,    77,
     311,    77,   361,    83,   296,    83,   262,    83,    92,    93,
      77,    64,    76,   305,    77,    98,    83,    99,   348,   311,
      83,   313,   352,   322,    99,    77,    97,   326,   320,   320,
     296,    83,    85,    26,    27,    77,   328,    77,    77,   305,
     332,    83,    47,    83,    83,    81,    77,    97,   378,   379,
      79,   381,    83,    76,    83,   311,    76,   349,    76,    76,
     316,    76,    80,   393,   394,    80,   332,    79,     4,     5,
      97,    83,   168,   169,   170,   171,   406,    82,    83,    98,
      85,   166,   167,   349,   172,   173,   342,   164,   165,    97,
     382,   383,   384,    98,    97,    77,    77,    77,    83,   391,
      97,    65,    77,    99,    81,    76,   174,    77,    81,   401,
     402,    42,   404,    99,   175,   407,   382,   383,   384,   411,
     176,   413,   414,   177,   116,   391,   132,   320,   420,   124,
     178,   135,   134,   138,   139,   401,   402,   316,   404,   244,
      43,   407,    -1,   144,    -1,   411,    -1,   413,   414,    -1,
      -1,    -1,    -1,    -1,   420,    -1,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   186,    -1,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    32,    33,    -1,    35,    36,    37,    38,
      39,    40,    41,   228,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    -1,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    -1,    78,
      79,    -1,   267,    -1,    -1,    84,    85,    86,    87,    88,
      89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   322,    -1,    32,
      33,   326,    35,    36,    37,    38,    39,    40,    41,    -1,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      -1,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    76,    -1,    78,    79,    -1,    -1,    -1,
      -1,    84,    85,    86,    87,    88,    89,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    99,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    32,    33,    -1,    35,
      36,    37,    38,    39,    40,    41,    -1,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    -1,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    -1,    78,    79,    -1,    -1,    -1,    -1,    84,    85,
      86,    87,    88,    89,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    99,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    32,    33,    -1,    35,    36,    37,    38,
      39,    40,    41,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    -1,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    -1,    78,
      -1,    -1,    -1,    -1,    -1,    84,    85,    86,    87,    88,
      89,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,
      33,    -1,    -1,    36,    37,    -1,    39,    40,    41,    -1,
      -1,    44,    -1,    46,    47,    48,    -1,    -1,    -1,    52,
      -1,    -1,    55,    -1,    -1,    58,    -1,     3,    -1,     5,
      -1,    -1,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    76,    -1,    78,    -1,    -1,    -1,    -1,
      -1,    84,    85,    86,    87,    88,    89,    -1,    -1,    35,
      -1,    -1,    38,    -1,    -1,    -1,    99,    43,    44,    45,
      -1,    -1,    -1,    49,    50,    51,    -1,    -1,    54,    -1,
      56,    57,    -1,    59,    60,    61,    62,    63,    64,     4,
      -1,     6,     7,     8,     9,    10,    11,    12,    -1,    -1,
      -1,    -1,    78,    -1,    -1,     4,    -1,     6,     7,     8,
       9,    10,    11,    12,    -1,    -1,    -1,    32,    33,    -1,
      -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,    44,
      -1,    -1,    -1,    32,    33,    -1,    -1,    -1,    -1,    -1,
      55,    -1,    -1,    -1,    -1,    44,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,    -1,    -1,
      -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      85,    86,    87,    88,    89,    -1,    -1,    76,    -1,    -1,
      -1,    -1,    -1,    -1,    99,    84,    85,    86,    87,    88,
      89,     4,    -1,     6,     7,     8,     9,    10,    11,    12,
      99,    -1,    -1,    -1,    -1,    -1,    -1,     4,    -1,     6,
       7,     8,     9,    10,    11,    12,    -1,    -1,    -1,    32,
      33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    44,    -1,    -1,    -1,    32,    33,    -1,    -1,    -1,
      -1,    -1,    55,    -1,    -1,    -1,    -1,    44,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,
      -1,    -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    85,    86,    87,    88,    89,    -1,    -1,    76,
      -1,    -1,    -1,    -1,    -1,    -1,    99,    84,    85,    86,
      87,    88,    89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    99,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    33,    -1,    -1,    -1,    -1,    38,    -1,
      -1,    -1,    -1,    43,    44,    -1,    -1,    -1,    -1,    -1,
      -1,    51,    -1,    -1,    54,    55,    -1,    57,    -1,    -1,
      -1,    61,    62,    63,    64,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    -1,    76,    -1,    78,    -1,
      -1,    -1,    -1,    -1,    84,    85,    86,    87,    88,    89,
      -1,    -1,    -1,    -1,    32,    33,    -1,    -1,    -1,    -1,
      38,    -1,    -1,    -1,    -1,    43,    44,    -1,    -1,    -1,
      -1,    -1,    -1,    51,    -1,    -1,    54,    55,    -1,    57,
      -1,    -1,    -1,    61,    62,    63,    64,     4,    -1,     6,
       7,     8,     9,    10,    11,    12,    -1,    -1,    76,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    86,    87,
      88,    89,    -1,    -1,    -1,    32,    33,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     4,    44,     6,     7,
       8,     9,    10,    11,    12,    -1,    -1,    -1,    55,    -1,
      -1,    -1,    -1,    -1,    -1,     4,    -1,     6,     7,     8,
       9,    10,    11,    12,    32,    33,    -1,    -1,    -1,    76,
      -1,    78,    79,    -1,    -1,    -1,    44,    84,    85,    86,
      87,    88,    89,    32,    33,    -1,    -1,    55,    -1,    -1,
      -1,    -1,    -1,    -1,     4,    44,     6,     7,     8,     9,
      10,    11,    12,    -1,    -1,    -1,    55,    -1,    76,    -1,
      78,    -1,    -1,    -1,    -1,    -1,    84,    85,    86,    87,
      88,    89,    32,    33,    -1,    -1,    -1,    76,    -1,    -1,
      -1,    -1,    81,    -1,    44,    84,    85,    86,    87,    88,
      89,    -1,    -1,    -1,     4,    55,     6,     7,     8,     9,
      10,    11,    12,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    76,    77,    -1,    -1,
      -1,    -1,    32,    33,    84,    85,    86,    87,    88,    89,
      -1,    -1,    -1,     4,    44,     6,     7,     8,     9,    10,
      11,    12,    -1,    -1,    -1,    55,    -1,    -1,    -1,    -1,
      -1,    -1,     4,    -1,     6,     7,     8,     9,    10,    11,
      12,    32,    33,    -1,    -1,    -1,    76,    -1,    -1,    -1,
      -1,    81,    -1,    44,    84,    85,    86,    87,    88,    89,
      32,    33,    -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,
      -1,     4,    44,     6,     7,     8,     9,    10,    11,    12,
      -1,    -1,    -1,    55,    -1,    76,    -1,    -1,    -1,    -1,
      81,    -1,    -1,    84,    85,    86,    87,    88,    89,    32,
      33,    -1,    -1,    -1,    76,    77,    -1,    -1,    -1,    -1,
      -1,    44,    84,    85,    86,    87,    88,    89,    -1,    -1,
      -1,     4,    55,     6,     7,     8,     9,    10,    11,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    76,    77,    -1,    -1,    -1,    -1,    32,
      33,    84,    85,    86,    87,    88,    89,    -1,    -1,    -1,
       4,    44,     6,     7,     8,     9,    10,    11,    12,    -1,
      -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,     4,
      -1,     6,     7,     8,     9,    10,    11,    12,    32,    33,
      -1,    -1,    -1,    76,    77,    -1,    -1,    -1,    -1,    -1,
      44,    84,    85,    86,    87,    88,    89,    32,    33,    -1,
      -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,     4,    44,
       6,     7,     8,     9,    10,    11,    12,    -1,    -1,    -1,
      55,    -1,    76,    77,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    85,    86,    87,    88,    89,    32,    33,    -1,    -1,
      -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,    44,    84,
      85,    86,    87,    88,    89,    -1,    -1,    -1,     4,    55,
       6,     7,     8,     9,    10,    11,    12,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    -1,    -1,    -1,    -1,    -1,    32,    33,    84,    85,
      86,    87,    88,    89,    -1,    -1,    -1,     4,    44,     6,
       7,     8,     9,    10,    11,    12,    -1,    -1,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    -1,     4,    -1,     6,     7,
       8,     9,    10,    11,    12,    32,    33,    -1,    -1,    -1,
      76,    -1,    -1,    -1,    -1,    -1,    -1,    44,    84,    85,
      86,    87,    88,    89,    32,    33,    -1,    -1,    55,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    44,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,    76,
      -1,    -1,    -1,     3,     4,     5,    -1,    84,    85,    86,
      87,    88,    89,    13,    -1,    -1,    -1,    -1,    76,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    86,    87,
      88,    89,    -1,    -1,    -1,    35,    -1,    -1,    38,    -1,
      -1,    -1,    -1,    43,    44,    45,    -1,    -1,    -1,    49,
      50,    51,    -1,    -1,    54,    -1,    56,    57,    -1,    59,
      60,    61,    62,    63,    64,     3,    -1,     5,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    13,    76,    77,    -1,    -1,
      80,    -1,    -1,    -1,    -1,    85,    -1,    -1,    -1,    -1,
      -1,    -1,     3,    -1,     5,    -1,    -1,    35,    -1,    -1,
      38,    -1,    -1,    -1,    -1,    43,    44,    45,    -1,    -1,
      -1,    49,    50,    51,    -1,    -1,    54,    -1,    56,    57,
      -1,    59,    60,    61,    62,    63,    64,    38,    -1,     3,
      -1,     5,    43,    44,    -1,    -1,    -1,    -1,    76,    77,
      51,    -1,    80,    54,    -1,    -1,    57,    85,    -1,    -1,
      61,    62,    63,    64,    -1,    -1,    -1,     0,     1,    -1,
       3,    -1,     5,    -1,    38,    -1,    -1,    -1,    79,    43,
      44,    -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,
      54,    -1,    -1,    57,    -1,    -1,    -1,    61,    62,    63,
      64,    -1,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
      43,    44,    45,    -1,    -1,    79,    49,    50,    51,    -1,
       3,    54,     5,    56,    57,    -1,    59,    60,    61,    62,
      63,    64,    -1,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    -1,    -1,    -1,    -1,    -1,    -1,     1,
      -1,     3,    35,     5,    -1,    38,    -1,    -1,    -1,    -1,
      43,    44,    45,    -1,    -1,    -1,    49,    50,    51,    -1,
      -1,    54,    -1,    56,    57,    -1,    59,    60,    61,    62,
      63,    64,    -1,    35,    -1,    -1,    38,    -1,    -1,    -1,
      -1,    43,    44,    45,    -1,    78,    -1,    49,    50,    51,
      -1,    -1,    54,    -1,    56,    57,    -1,    59,    60,    61,
      62,    63,    64,    -1,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,     3,     4,     5,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    13,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,    -1,     5,    -1,    35,    -1,    -1,    38,
      -1,    -1,    -1,    -1,    43,    44,    45,    -1,    -1,    -1,
      49,    50,    51,    -1,    -1,    54,    -1,    56,    57,    -1,
      59,    60,    61,    62,    63,    64,     3,    38,     5,    -1,
      -1,    -1,    43,    44,    -1,    -1,    13,    -1,    77,    -1,
      51,    -1,    -1,    54,    -1,    -1,    57,    -1,    -1,    -1,
      61,    62,    63,    64,    -1,    -1,    -1,    -1,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    43,    44,    45,    -1,
      -1,    -1,    49,    50,    51,    -1,    -1,    54,    -1,    56,
      57,    -1,    59,    60,    61,    62,    63,    64,     3,    -1,
       5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,    -1,
      77,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
      35,     5,    -1,    38,    -1,    -1,    -1,    -1,    43,    44,
      45,    -1,    -1,    -1,    49,    50,    51,    -1,    -1,    54,
      -1,    56,    57,    -1,    59,    60,    61,    62,    63,    64,
      -1,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    43,
      44,    45,    -1,    -1,    -1,    49,    50,    51,    -1,    -1,
      54,    -1,    56,    57,    -1,    59,    60,    61,    62,    63,
      64
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     5,    35,    38,    43,    44,    45,    49,
      50,    51,    54,    56,    57,    59,    60,    61,    62,    63,
      64,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,   125,   126,   129,   130,   131,   132,   133,   139,   140,
     144,   145,   156,   168,   169,   170,   172,   173,   174,   175,
     176,   177,     4,    76,    85,    99,   103,   127,   128,   146,
     147,   148,   126,   126,    78,   103,   104,   156,    78,   104,
     126,   126,     0,   169,    76,     6,     7,     8,     9,    10,
      11,    12,    32,    33,    44,    55,    76,    84,    85,    86,
      87,    88,    89,   101,   102,   103,   105,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   124,   146,   144,   148,   149,    83,    99,    78,    98,
     125,   126,   161,   171,    76,    80,   147,   131,   134,   135,
     136,   144,    78,   141,   142,    78,   103,   152,    76,    76,
      76,   107,   107,    76,    76,   107,    44,   107,   120,   121,
     123,   136,   153,    10,    32,    33,    34,    76,    80,    82,
     109,    85,    90,    91,    86,    87,    22,    23,    28,    29,
      92,    93,    26,    27,    84,    94,    95,    30,    31,    96,
      77,   144,   148,   128,   146,    36,    37,    39,    40,    41,
      46,    47,    48,    52,    58,    65,    79,    99,   103,   104,
     123,   125,   156,   159,   160,   161,   162,   163,   164,   165,
     166,   167,    78,   121,   157,   125,   161,    13,    77,   126,
     150,   151,   152,    81,   121,   136,    79,   135,    97,   137,
     138,   146,   136,   134,    79,    83,   103,   143,   141,    77,
      83,   107,   107,    44,    78,   153,    76,   123,    14,    15,
      16,    17,    18,    19,    20,    21,    24,    25,    98,   122,
      77,    83,    76,    80,   148,   154,   155,    77,   104,    77,
     106,   121,   123,   104,   109,   109,   109,   110,   110,   111,
     111,   112,   112,   112,   112,   113,   113,   114,   115,   116,
     117,   118,   123,    99,   124,    99,    97,   159,    76,   104,
      76,    99,   123,    76,    76,    97,    99,    79,   163,   157,
     158,    76,   146,   148,   154,    77,    83,    77,    81,   124,
      83,    99,    97,    79,    79,   143,    98,    79,    83,   103,
      77,    77,   162,    77,   121,   121,    77,   150,   154,    81,
     121,   155,    76,    80,   109,    77,    83,    81,    97,    97,
     159,    65,    99,   123,    99,   123,    99,   123,   123,   159,
      79,    83,   151,   138,   124,   124,    79,    79,    77,    77,
      81,    77,   150,    81,   121,   121,   123,   159,    76,    99,
     123,    99,    77,    77,    77,    79,   157,    77,    77,    81,
     123,    77,   123,    99,    99,   123,   159,   159,   159,    77,
     159,    77,    77,   123,    77,   123,    99,    42,    99,   159,
     159,    77,   159,    77,    77,   123,   159,   159,   159,   159,
      77,   159
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   100,   101,   101,   101,   101,   101,   101,   101,   101,
     102,   102,   103,   104,   104,   105,   105,   105,   105,   105,
     105,   105,   105,   106,   106,   107,   107,   107,   107,   107,
     107,   107,   107,   108,   108,   108,   108,   108,   108,   109,
     109,   110,   110,   110,   110,   111,   111,   111,   112,   112,
     112,   113,   113,   113,   113,   113,   114,   114,   114,   115,
     115,   116,   116,   117,   117,   118,   118,   119,   119,   120,
     120,   121,   121,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   122,   123,   123,   123,   124,   125,   125,
     126,   126,   126,   126,   126,   126,   126,   126,   127,   127,
     128,   128,   129,   129,   129,   129,   129,   129,   130,   131,
     131,   131,   131,   131,   131,   131,   132,   132,   132,   133,
     133,   134,   134,   135,   136,   136,   136,   136,   137,   137,
     138,   138,   138,   138,   139,   139,   139,   139,   139,   140,
     142,   141,   141,   143,   143,   144,   144,   144,   144,   145,
     146,   146,   147,   147,   147,   147,   147,   147,   147,   148,
     148,   148,   148,   149,   149,   150,   150,   151,   151,   151,
     151,   152,   152,   153,   153,   154,   154,   154,   155,   155,
     155,   155,   155,   155,   155,   155,   155,   156,   157,   157,
     157,   158,   158,   159,   159,   159,   159,   159,   159,   160,
     160,   160,   161,   161,   162,   162,   163,   163,   164,   164,
     165,   165,   165,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   167,   167,   167,   167,   167,   168,   168,
     169,   169,   169,   170,   170,   171,   171,   172,   173,   174,
     175,   176,   176,   176,   176,   176,   176,   176,   176,   177,
     177,   177,   177
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     3,     6,
       1,     2,     1,     1,     1,     1,     4,     4,     3,     3,
       3,     2,     2,     1,     3,     1,     2,     2,     2,     4,
       4,     2,     4,     1,     1,     1,     1,     1,     1,     1,
       4,     1,     3,     3,     3,     1,     3,     3,     1,     3,
       3,     1,     3,     3,     3,     3,     1,     3,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       5,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     2,     1,     3,     2,
       2,     1,     2,     1,     2,     1,     2,     1,     1,     3,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     5,     4,     2,     1,
       1,     1,     2,     3,     2,     1,     2,     1,     1,     3,
       0,     1,     2,     3,     5,     4,     6,     5,     2,     1,
       0,     2,     3,     1,     3,     1,     1,     1,     1,     1,
       2,     1,     1,     3,     4,     3,     4,     4,     3,     2,
       1,     3,     2,     1,     2,     1,     3,     2,     2,     1,
       1,     1,     3,     1,     2,     1,     1,     2,     3,     2,
       3,     3,     4,     2,     3,     3,     4,     1,     1,     3,
       4,     1,     3,     1,     1,     1,     1,     1,     1,     3,
       4,     3,     2,     3,     1,     2,     1,     1,     1,     2,
       5,     7,     5,     5,     7,     6,     7,     7,     8,     7,
       8,     8,     9,     3,     2,     2,     2,     3,     1,     2,
       1,     1,     1,     4,     3,     1,     2,     1,     1,     4,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (scanner, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, GISourceScanner* scanner)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, GISourceScanner* scanner)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, scanner);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, GISourceScanner* scanner)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, GISourceScanner* scanner)
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (GISourceScanner* scanner)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 329 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = g_hash_table_lookup (scanner->const_table, (yyvsp[0].str));
		if ((yyval.symbol) == NULL) {
			(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		} else {
			(yyval.symbol) = gi_source_symbol_ref ((yyval.symbol));
		}
	  }
#line 2306 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 3:
#line 338 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		char *rest;
		guint64 value;
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		if (g_str_has_prefix (yytext, "0x") && strlen (yytext) > 2) {
			value = g_ascii_strtoull (yytext + 2, &rest, 16);
		} else if (g_str_has_prefix (yytext, "0") && strlen (yytext) > 1) {
			value = g_ascii_strtoull (yytext + 1, &rest, 8);
		} else {
			value = g_ascii_strtoull (yytext, &rest, 10);
		}
		(yyval.symbol)->const_int = value;
		(yyval.symbol)->const_int_is_unsigned = (rest && (rest[0] == 'U'));
	  }
#line 2326 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 4:
#line 354 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_boolean_set = TRUE;
		(yyval.symbol)->const_boolean = g_ascii_strcasecmp (yytext, "true") == 0 ? TRUE : FALSE;
	  }
#line 2336 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 5:
#line 360 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = g_utf8_get_char(yytext + 1);
	  }
#line 2346 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 6:
#line 366 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_double_set = TRUE;
		(yyval.symbol)->const_double = 0.0;
        sscanf (yytext, "%lf", &((yyval.symbol)->const_double));
	  }
#line 2357 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 8:
#line 374 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = (yyvsp[-1].symbol);
	  }
#line 2365 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 9:
#line 378 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2373 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 10:
#line 386 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		yytext[strlen (yytext) - 1] = '\0';
		(yyval.symbol)->const_string = parse_c_string_literal (yytext + 1);
                if (!g_utf8_validate ((yyval.symbol)->const_string, -1, NULL))
                  {
#if 0
                    g_warning ("Ignoring non-UTF-8 constant string \"%s\"", yytext + 1);
#endif
                    g_free((yyval.symbol)->const_string);
                    (yyval.symbol)->const_string = NULL;
                  }

	  }
#line 2392 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 11:
#line 401 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		char *strings, *string2;
		(yyval.symbol) = (yyvsp[-1].symbol);
		yytext[strlen (yytext) - 1] = '\0';
		string2 = parse_c_string_literal (yytext + 1);
		strings = g_strconcat ((yyval.symbol)->const_string, string2, NULL);
		g_free ((yyval.symbol)->const_string);
		g_free (string2);
		(yyval.symbol)->const_string = strings;
	  }
#line 2407 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 12:
#line 415 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.str) = g_strdup (yytext);
	  }
#line 2415 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 16:
#line 428 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2423 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 17:
#line 432 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2431 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 18:
#line 436 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2439 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 19:
#line 440 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2447 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 20:
#line 444 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2455 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 21:
#line 448 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2463 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 22:
#line 452 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2471 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 26:
#line 465 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2479 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 27:
#line 469 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2487 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 28:
#line 473 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		switch ((yyvsp[-1].unary_operator)) {
		case UNARY_PLUS:
			(yyval.symbol) = (yyvsp[0].symbol);
			break;
		case UNARY_MINUS:
			(yyval.symbol) = gi_source_symbol_copy ((yyvsp[0].symbol));
			(yyval.symbol)->const_int = -(yyvsp[0].symbol)->const_int;
			break;
		case UNARY_BITWISE_COMPLEMENT:
			(yyval.symbol) = gi_source_symbol_copy ((yyvsp[0].symbol));
			(yyval.symbol)->const_int = ~(yyvsp[0].symbol)->const_int;
			break;
		case UNARY_LOGICAL_NEGATION:
			(yyval.symbol) = gi_source_symbol_copy ((yyvsp[0].symbol));
			(yyval.symbol)->const_int = !gi_source_symbol_get_const_boolean ((yyvsp[0].symbol));
			break;
		default:
			(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
			break;
		}
	  }
#line 2514 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 29:
#line 496 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = (yyvsp[-1].symbol);
		if ((yyval.symbol)->const_int_set) {
			(yyval.symbol)->base_type = gi_source_basic_type_new ((yyval.symbol)->const_int_is_unsigned ? "guint64" : "gint64");
		}
	  }
#line 2525 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 30:
#line 503 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = (yyvsp[-1].symbol);
		if ((yyval.symbol)->const_int_set) {
			(yyval.symbol)->base_type = gi_source_basic_type_new ("guint64");
		}
	  }
#line 2536 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 31:
#line 510 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2544 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 32:
#line 514 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		ctype_free ((yyvsp[-1].ctype));
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2553 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 33:
#line 522 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.unary_operator) = UNARY_ADDRESS_OF;
	  }
#line 2561 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 34:
#line 526 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.unary_operator) = UNARY_POINTER_INDIRECTION;
	  }
#line 2569 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 35:
#line 530 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.unary_operator) = UNARY_PLUS;
	  }
#line 2577 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 36:
#line 534 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.unary_operator) = UNARY_MINUS;
	  }
#line 2585 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 37:
#line 538 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.unary_operator) = UNARY_BITWISE_COMPLEMENT;
	  }
#line 2593 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 38:
#line 542 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.unary_operator) = UNARY_LOGICAL_NEGATION;
	  }
#line 2601 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 40:
#line 550 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = (yyvsp[0].symbol);
		if ((yyval.symbol)->const_int_set || (yyval.symbol)->const_double_set || (yyval.symbol)->const_string != NULL) {
			(yyval.symbol)->base_type = (yyvsp[-2].ctype);
		} else {
			ctype_free ((yyvsp[-2].ctype));
		}
	  }
#line 2614 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 42:
#line 563 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int * (yyvsp[0].symbol)->const_int;
	  }
#line 2624 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 43:
#line 569 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		if ((yyvsp[0].symbol)->const_int != 0) {
			(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int / (yyvsp[0].symbol)->const_int;
		}
	  }
#line 2636 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 44:
#line 577 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		if ((yyvsp[0].symbol)->const_int != 0) {
			(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int % (yyvsp[0].symbol)->const_int;
		}
	  }
#line 2648 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 46:
#line 589 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int + (yyvsp[0].symbol)->const_int;
	  }
#line 2658 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 47:
#line 595 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int - (yyvsp[0].symbol)->const_int;
	  }
#line 2668 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 49:
#line 605 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int << (yyvsp[0].symbol)->const_int;

		/* assume this is a bitfield/flags declaration
		 * if a left shift operator is sued in an enum value
                 * This mimics the glib-mkenum behavior.
		 */
		is_bitfield = TRUE;
	  }
#line 2684 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 50:
#line 617 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int >> (yyvsp[0].symbol)->const_int;
	  }
#line 2694 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 52:
#line 627 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int < (yyvsp[0].symbol)->const_int;
	  }
#line 2704 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 53:
#line 633 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int > (yyvsp[0].symbol)->const_int;
	  }
#line 2714 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 54:
#line 639 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int <= (yyvsp[0].symbol)->const_int;
	  }
#line 2724 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 55:
#line 645 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int >= (yyvsp[0].symbol)->const_int;
	  }
#line 2734 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 57:
#line 655 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int == (yyvsp[0].symbol)->const_int;
	  }
#line 2744 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 58:
#line 661 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int != (yyvsp[0].symbol)->const_int;
	  }
#line 2754 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 60:
#line 671 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int & (yyvsp[0].symbol)->const_int;
	  }
#line 2764 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 62:
#line 681 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int ^ (yyvsp[0].symbol)->const_int;
	  }
#line 2774 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 64:
#line 691 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int | (yyvsp[0].symbol)->const_int;
	  }
#line 2784 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 66:
#line 701 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int =
		  gi_source_symbol_get_const_boolean ((yyvsp[-2].symbol)) &&
		  gi_source_symbol_get_const_boolean ((yyvsp[0].symbol));
	  }
#line 2796 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 68:
#line 713 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int =
		  gi_source_symbol_get_const_boolean ((yyvsp[-2].symbol)) ||
		  gi_source_symbol_get_const_boolean ((yyvsp[0].symbol));
	  }
#line 2808 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 70:
#line 725 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_get_const_boolean ((yyvsp[-4].symbol)) ? (yyvsp[-2].symbol) : (yyvsp[0].symbol);
	  }
#line 2816 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 72:
#line 733 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2824 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 86:
#line 756 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2832 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 88:
#line 769 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		GList *l;
		for (l = (yyvsp[-1].list); l != NULL; l = l->next) {
			GISourceSymbol *sym = l->data;
			gi_source_symbol_merge_type (sym, gi_source_type_copy ((yyvsp[-2].ctype)));
			if ((yyvsp[-2].ctype)->storage_class_specifier & STORAGE_CLASS_TYPEDEF) {
				sym->type = CSYMBOL_TYPE_TYPEDEF;
			} else if (sym->base_type->type == CTYPE_FUNCTION) {
				sym->type = CSYMBOL_TYPE_FUNCTION;
			} else {
				sym->type = CSYMBOL_TYPE_OBJECT;
			}
			gi_source_scanner_add_symbol (scanner, sym);
			gi_source_symbol_unref (sym);
		}
		ctype_free ((yyvsp[-2].ctype));
	  }
#line 2854 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 89:
#line 787 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		ctype_free ((yyvsp[-1].ctype));
	  }
#line 2862 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 90:
#line 794 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = (yyvsp[0].ctype);
		(yyval.ctype)->storage_class_specifier |= (yyvsp[-1].storage_class_specifier);
	  }
#line 2871 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 91:
#line 799 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_type_new (CTYPE_INVALID);
		(yyval.ctype)->storage_class_specifier |= (yyvsp[0].storage_class_specifier);
	  }
#line 2880 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 92:
#line 804 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = (yyvsp[-1].ctype);
		/* combine basic types like unsigned int and long long */
		if ((yyval.ctype)->type == CTYPE_BASIC_TYPE && (yyvsp[0].ctype)->type == CTYPE_BASIC_TYPE) {
			char *name = g_strdup_printf ("%s %s", (yyval.ctype)->name, (yyvsp[0].ctype)->name);
			g_free ((yyval.ctype)->name);
			(yyval.ctype)->name = name;
			ctype_free ((yyvsp[0].ctype));
		} else {
			set_or_merge_base_type ((yyvsp[-1].ctype), (yyvsp[0].ctype));
		}
	  }
#line 2897 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 94:
#line 818 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = (yyvsp[0].ctype);
		(yyval.ctype)->type_qualifier |= (yyvsp[-1].type_qualifier);
	  }
#line 2906 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 95:
#line 823 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_type_new (CTYPE_INVALID);
		(yyval.ctype)->type_qualifier |= (yyvsp[0].type_qualifier);
	  }
#line 2915 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 96:
#line 828 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = (yyvsp[0].ctype);
		(yyval.ctype)->function_specifier |= (yyvsp[-1].function_specifier);
	  }
#line 2924 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 97:
#line 833 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_type_new (CTYPE_INVALID);
		(yyval.ctype)->function_specifier |= (yyvsp[0].function_specifier);
	  }
#line 2933 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 98:
#line 841 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.list) = g_list_append (NULL, (yyvsp[0].symbol));
	  }
#line 2941 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 99:
#line 845 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.list) = g_list_append ((yyvsp[-2].list), (yyvsp[0].symbol));
	  }
#line 2949 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 102:
#line 857 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.storage_class_specifier) = STORAGE_CLASS_TYPEDEF;
	  }
#line 2957 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 103:
#line 861 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.storage_class_specifier) = STORAGE_CLASS_EXTERN;
	  }
#line 2965 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 104:
#line 865 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.storage_class_specifier) = STORAGE_CLASS_STATIC;
	  }
#line 2973 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 105:
#line 869 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.storage_class_specifier) = STORAGE_CLASS_AUTO;
	  }
#line 2981 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 106:
#line 873 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.storage_class_specifier) = STORAGE_CLASS_REGISTER;
	  }
#line 2989 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 107:
#line 877 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.storage_class_specifier) = STORAGE_CLASS_THREAD_LOCAL;
	  }
#line 2997 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 108:
#line 884 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.str) = g_strdup (yytext);
	  }
#line 3005 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 109:
#line 891 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_type_new (CTYPE_VOID);
	  }
#line 3013 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 110:
#line 895 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_basic_type_new ("signed");
	  }
#line 3021 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 111:
#line 899 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_basic_type_new ("unsigned");
	  }
#line 3029 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 112:
#line 903 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_type_new (CTYPE_BASIC_TYPE);
		(yyval.ctype)->name = (yyvsp[0].str);
	  }
#line 3038 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 115:
#line 910 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_typedef_new ((yyvsp[0].str));
		g_free ((yyvsp[0].str));
	  }
#line 3047 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 116:
#line 918 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		GISourceSymbol *sym;
		(yyval.ctype) = (yyvsp[-4].ctype);
		(yyval.ctype)->name = (yyvsp[-3].str);
		(yyval.ctype)->child_list = (yyvsp[-1].list);

		sym = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		if ((yyval.ctype)->type == CTYPE_STRUCT) {
			sym->type = CSYMBOL_TYPE_STRUCT;
		} else if ((yyval.ctype)->type == CTYPE_UNION) {
			sym->type = CSYMBOL_TYPE_UNION;
		} else {
			g_assert_not_reached ();
		}
		sym->ident = g_strdup ((yyval.ctype)->name);
		sym->base_type = gi_source_type_copy ((yyval.ctype));
		gi_source_scanner_add_symbol (scanner, sym);
		gi_source_symbol_unref (sym);
	  }
#line 3071 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 117:
#line 938 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = (yyvsp[-3].ctype);
		(yyval.ctype)->child_list = (yyvsp[-1].list);
	  }
#line 3080 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 118:
#line 943 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = (yyvsp[-1].ctype);
		(yyval.ctype)->name = (yyvsp[0].str);
	  }
#line 3089 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 119:
#line 951 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
                scanner->private = FALSE;
		(yyval.ctype) = gi_source_struct_new (NULL);
	  }
#line 3098 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 120:
#line 956 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
                scanner->private = FALSE;
		(yyval.ctype) = gi_source_union_new (NULL);
	  }
#line 3107 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 122:
#line 965 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.list) = g_list_concat ((yyvsp[-1].list), (yyvsp[0].list));
	  }
#line 3115 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 123:
#line 972 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
	    GList *l;
	    (yyval.list) = NULL;
	    for (l = (yyvsp[-1].list); l != NULL; l = l->next)
	      {
		GISourceSymbol *sym = l->data;
		if ((yyvsp[-2].ctype)->storage_class_specifier & STORAGE_CLASS_TYPEDEF)
		    sym->type = CSYMBOL_TYPE_TYPEDEF;
		else
		    sym->type = CSYMBOL_TYPE_MEMBER;
		gi_source_symbol_merge_type (sym, gi_source_type_copy ((yyvsp[-2].ctype)));
                sym->private = scanner->private;
                (yyval.list) = g_list_append ((yyval.list), sym);
	      }
	    ctype_free ((yyvsp[-2].ctype));
	  }
#line 3136 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 124:
#line 992 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = (yyvsp[-1].ctype);
		set_or_merge_base_type ((yyvsp[-1].ctype), (yyvsp[0].ctype));
	  }
#line 3145 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 126:
#line 998 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = (yyvsp[0].ctype);
		(yyval.ctype)->type_qualifier |= (yyvsp[-1].type_qualifier);
	  }
#line 3154 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 127:
#line 1003 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_type_new (CTYPE_INVALID);
		(yyval.ctype)->type_qualifier |= (yyvsp[0].type_qualifier);
	  }
#line 3163 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 128:
#line 1011 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.list) = g_list_append (NULL, (yyvsp[0].symbol));
	  }
#line 3171 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 129:
#line 1015 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.list) = g_list_append ((yyvsp[-2].list), (yyvsp[0].symbol));
	  }
#line 3179 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 130:
#line 1022 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 3187 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 132:
#line 1027 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 3195 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 133:
#line 1031 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = (yyvsp[-2].symbol);
		if ((yyvsp[0].symbol)->const_int_set) {
		  (yyval.symbol)->const_int_set = TRUE;
		  (yyval.symbol)->const_int = (yyvsp[0].symbol)->const_int;
		}
	  }
#line 3207 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 134:
#line 1042 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_enum_new ((yyvsp[-3].str));
		(yyval.ctype)->child_list = (yyvsp[-1].list);
		(yyval.ctype)->is_bitfield = is_bitfield || scanner->flags;
		last_enum_value = -1;
	  }
#line 3218 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 135:
#line 1049 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_enum_new (NULL);
		(yyval.ctype)->child_list = (yyvsp[-1].list);
		(yyval.ctype)->is_bitfield = is_bitfield || scanner->flags;
		last_enum_value = -1;
	  }
#line 3229 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 136:
#line 1056 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_enum_new ((yyvsp[-4].str));
		(yyval.ctype)->child_list = (yyvsp[-2].list);
		(yyval.ctype)->is_bitfield = is_bitfield || scanner->flags;
		last_enum_value = -1;
	  }
#line 3240 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 137:
#line 1063 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_enum_new (NULL);
		(yyval.ctype)->child_list = (yyvsp[-2].list);
		(yyval.ctype)->is_bitfield = is_bitfield || scanner->flags;
		last_enum_value = -1;
	  }
#line 3251 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 138:
#line 1070 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_enum_new ((yyvsp[0].str));
	  }
#line 3259 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 139:
#line 1077 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
                scanner->flags = FALSE;
                scanner->private = FALSE;
          }
#line 3268 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 140:
#line 1085 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		/* reset flag before the first enum value */
		is_bitfield = FALSE;
	  }
#line 3277 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 141:
#line 1090 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
            (yyvsp[0].symbol)->private = scanner->private;
            (yyval.list) = g_list_append (NULL, (yyvsp[0].symbol));
	  }
#line 3286 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 142:
#line 1095 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
            (yyvsp[0].symbol)->private = scanner->private;
            (yyval.list) = g_list_append ((yyvsp[-2].list), (yyvsp[0].symbol));
	  }
#line 3295 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 143:
#line 1103 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_OBJECT, scanner->current_file, lineno);
		(yyval.symbol)->ident = (yyvsp[0].str);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = ++last_enum_value;
		g_hash_table_insert (scanner->const_table, g_strdup ((yyval.symbol)->ident), gi_source_symbol_ref ((yyval.symbol)));
	  }
#line 3307 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 144:
#line 1111 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_OBJECT, scanner->current_file, lineno);
		(yyval.symbol)->ident = (yyvsp[-2].str);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[0].symbol)->const_int;
		last_enum_value = (yyval.symbol)->const_int;
		g_hash_table_insert (scanner->const_table, g_strdup ((yyval.symbol)->ident), gi_source_symbol_ref ((yyval.symbol)));
	  }
#line 3320 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 145:
#line 1123 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.type_qualifier) = TYPE_QUALIFIER_CONST;
	  }
#line 3328 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 146:
#line 1127 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.type_qualifier) = TYPE_QUALIFIER_RESTRICT;
	  }
#line 3336 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 147:
#line 1131 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.type_qualifier) = TYPE_QUALIFIER_EXTENSION;
	  }
#line 3344 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 148:
#line 1135 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.type_qualifier) = TYPE_QUALIFIER_VOLATILE;
	  }
#line 3352 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 149:
#line 1142 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.function_specifier) = FUNCTION_INLINE;
	  }
#line 3360 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 150:
#line 1149 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = (yyvsp[0].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), (yyvsp[-1].ctype));
	  }
#line 3369 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 152:
#line 1158 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		(yyval.symbol)->ident = (yyvsp[0].str);
	  }
#line 3378 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 153:
#line 1163 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = (yyvsp[-1].symbol);
	  }
#line 3386 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 154:
#line 1167 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = (yyvsp[-3].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), gi_source_array_new ((yyvsp[-1].symbol)));
	  }
#line 3395 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 155:
#line 1172 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = (yyvsp[-2].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), gi_source_array_new (NULL));
	  }
#line 3404 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 156:
#line 1177 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		GISourceType *func = gi_source_function_new ();
		// ignore (void) parameter list
		if ((yyvsp[-1].list) != NULL && ((yyvsp[-1].list)->next != NULL || ((GISourceSymbol *) (yyvsp[-1].list)->data)->base_type->type != CTYPE_VOID)) {
			func->child_list = (yyvsp[-1].list);
		}
		(yyval.symbol) = (yyvsp[-3].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), func);
	  }
#line 3418 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 157:
#line 1187 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		GISourceType *func = gi_source_function_new ();
		func->child_list = (yyvsp[-1].list);
		(yyval.symbol) = (yyvsp[-3].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), func);
	  }
#line 3429 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 158:
#line 1194 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		GISourceType *func = gi_source_function_new ();
		(yyval.symbol) = (yyvsp[-2].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), func);
	  }
#line 3439 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 159:
#line 1203 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_pointer_new (NULL);
		(yyval.ctype)->type_qualifier = (yyvsp[0].type_qualifier);
	  }
#line 3448 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 160:
#line 1208 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.ctype) = gi_source_pointer_new (NULL);
	  }
#line 3456 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 161:
#line 1212 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		GISourceType **base = &((yyvsp[0].ctype)->base_type);

		while (*base != NULL) {
			base = &((*base)->base_type);
		}
		*base = gi_source_pointer_new (NULL);
		(*base)->type_qualifier = (yyvsp[-1].type_qualifier);
		(yyval.ctype) = (yyvsp[0].ctype);
	  }
#line 3471 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 162:
#line 1223 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		GISourceType **base = &((yyvsp[0].ctype)->base_type);

		while (*base != NULL) {
			base = &((*base)->base_type);
		}
		*base = gi_source_pointer_new (NULL);
		(yyval.ctype) = (yyvsp[0].ctype);
	  }
#line 3485 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 164:
#line 1237 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.type_qualifier) = (yyvsp[-1].type_qualifier) | (yyvsp[0].type_qualifier);
	  }
#line 3493 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 165:
#line 1244 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.list) = g_list_append (NULL, (yyvsp[0].symbol));
	  }
#line 3501 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 166:
#line 1248 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.list) = g_list_append ((yyvsp[-2].list), (yyvsp[0].symbol));
	  }
#line 3509 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 167:
#line 1255 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = (yyvsp[0].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), (yyvsp[-1].ctype));
	  }
#line 3518 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 168:
#line 1260 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = (yyvsp[0].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), (yyvsp[-1].ctype));
	  }
#line 3527 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 169:
#line 1265 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		(yyval.symbol)->base_type = (yyvsp[0].ctype);
	  }
#line 3536 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 170:
#line 1270 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_ELLIPSIS, scanner->current_file, lineno);
	  }
#line 3544 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 171:
#line 1277 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		GISourceSymbol *sym = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		sym->ident = (yyvsp[0].str);
		(yyval.list) = g_list_append (NULL, sym);
	  }
#line 3554 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 172:
#line 1283 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		GISourceSymbol *sym = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		sym->ident = (yyvsp[0].str);
		(yyval.list) = g_list_append ((yyvsp[-2].list), sym);
	  }
#line 3564 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 175:
#line 1297 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		gi_source_symbol_merge_type ((yyval.symbol), (yyvsp[0].ctype));
	  }
#line 3573 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 177:
#line 1303 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = (yyvsp[0].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), (yyvsp[-1].ctype));
	  }
#line 3582 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 178:
#line 1311 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = (yyvsp[-1].symbol);
	  }
#line 3590 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 179:
#line 1315 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		gi_source_symbol_merge_type ((yyval.symbol), gi_source_array_new (NULL));
	  }
#line 3599 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 180:
#line 1320 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		gi_source_symbol_merge_type ((yyval.symbol), gi_source_array_new ((yyvsp[-1].symbol)));
	  }
#line 3608 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 181:
#line 1325 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = (yyvsp[-2].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), gi_source_array_new (NULL));
	  }
#line 3617 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 182:
#line 1330 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.symbol) = (yyvsp[-3].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), gi_source_array_new ((yyvsp[-1].symbol)));
	  }
#line 3626 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 183:
#line 1335 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		GISourceType *func = gi_source_function_new ();
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		gi_source_symbol_merge_type ((yyval.symbol), func);
	  }
#line 3636 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 184:
#line 1341 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		GISourceType *func = gi_source_function_new ();
		// ignore (void) parameter list
		if ((yyvsp[-1].list) != NULL && ((yyvsp[-1].list)->next != NULL || ((GISourceSymbol *) (yyvsp[-1].list)->data)->base_type->type != CTYPE_VOID)) {
			func->child_list = (yyvsp[-1].list);
		}
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		gi_source_symbol_merge_type ((yyval.symbol), func);
	  }
#line 3650 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 185:
#line 1351 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		GISourceType *func = gi_source_function_new ();
		(yyval.symbol) = (yyvsp[-2].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), func);
	  }
#line 3660 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 186:
#line 1357 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		GISourceType *func = gi_source_function_new ();
		// ignore (void) parameter list
		if ((yyvsp[-1].list) != NULL && ((yyvsp[-1].list)->next != NULL || ((GISourceSymbol *) (yyvsp[-1].list)->data)->base_type->type != CTYPE_VOID)) {
			func->child_list = (yyvsp[-1].list);
		}
		(yyval.symbol) = (yyvsp[-3].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), func);
	  }
#line 3674 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 187:
#line 1370 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.str) = g_strdup (yytext);
	  }
#line 3682 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 237:
#line 1477 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.str) = g_strdup (yytext + strlen ("#define "));
	  }
#line 3690 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 238:
#line 1484 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		(yyval.str) = g_strdup (yytext + strlen ("#define "));
	  }
#line 3698 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 240:
#line 1495 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		if ((yyvsp[0].symbol)->const_int_set || (yyvsp[0].symbol)->const_boolean_set || (yyvsp[0].symbol)->const_double_set || (yyvsp[0].symbol)->const_string != NULL) {
			GISourceSymbol *macro = gi_source_symbol_copy ((yyvsp[0].symbol));
			g_free (macro->ident);
			macro->ident = (yyvsp[-1].str);
			gi_source_scanner_add_symbol (scanner, macro);
			gi_source_symbol_unref (macro);
			gi_source_symbol_unref ((yyvsp[0].symbol));
		} else {
			g_free ((yyvsp[-1].str));
			gi_source_symbol_unref ((yyvsp[0].symbol));
		}
	  }
#line 3716 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 241:
#line 1512 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		push_conditional (scanner, FOR_GI_SCANNER);
		update_skipping (scanner);
	  }
#line 3725 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 242:
#line 1517 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		push_conditional (scanner, NOT_GI_SCANNER);
		update_skipping (scanner);
	  }
#line 3734 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 243:
#line 1522 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
	 	warn_if_cond_has_gi_scanner (scanner, yytext);
		push_conditional (scanner, IRRELEVANT);
	  }
#line 3743 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 244:
#line 1527 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		warn_if_cond_has_gi_scanner (scanner, yytext);
		push_conditional (scanner, IRRELEVANT);
	  }
#line 3752 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 245:
#line 1532 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		warn_if_cond_has_gi_scanner (scanner, yytext);
		push_conditional (scanner, IRRELEVANT);
	  }
#line 3761 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 246:
#line 1537 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		warn_if_cond_has_gi_scanner (scanner, yytext);
		pop_conditional (scanner);
		push_conditional (scanner, IRRELEVANT);
		update_skipping (scanner);
	  }
#line 3772 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 247:
#line 1544 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		toggle_conditional (scanner);
		update_skipping (scanner);
	  }
#line 3781 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;

  case 248:
#line 1549 "giscanner/scannerparser.y" /* yacc.c:1646  */
    {
		pop_conditional (scanner);
		update_skipping (scanner);
	  }
#line 3790 "giscanner/scannerparser.c" /* yacc.c:1646  */
    break;


#line 3794 "giscanner/scannerparser.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (scanner, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (scanner, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 1562 "giscanner/scannerparser.y" /* yacc.c:1906  */

static void
yyerror (GISourceScanner *scanner, const char *s)
{
  /* ignore errors while doing a macro scan as not all object macros
   * have valid expressions */
  if (!scanner->macro_scan)
    {
      fprintf(stderr, "%s:%d: %s in '%s' at '%s'\n",
	      g_file_get_parse_name (scanner->current_file), lineno, s, linebuf, yytext);
    }
}

static int
eat_hspace (FILE * f)
{
  int c;
  do
    {
      c = fgetc (f);
    }
  while (c == ' ' || c == '\t');
  return c;
}

static int
pass_line (FILE * f, int c,
           FILE *out)
{
  while (c != EOF && c != '\n')
    {
      if (out)
        fputc (c, out);
      c = fgetc (f);
    }
  if (c == '\n')
    {
      if (out)
        fputc (c, out);
      c = fgetc (f);
      if (c == ' ' || c == '\t')
        {
          c = eat_hspace (f);
        }
    }
  return c;
}

static int
eat_line (FILE * f, int c)
{
  return pass_line (f, c, NULL);
}

static int
read_identifier (FILE * f, int c, char **identifier)
{
  GString *id = g_string_new ("");
  while (g_ascii_isalnum (c) || c == '_')
    {
      g_string_append_c (id, c);
      c = fgetc (f);
    }
  *identifier = g_string_free (id, FALSE);
  return c;
}

void
gi_source_scanner_parse_macros (GISourceScanner *scanner, GList *filenames)
{
  GError *error = NULL;
  char *tmp_name = NULL;
  FILE *fmacros =
    fdopen (g_file_open_tmp ("gen-introspect-XXXXXX.h", &tmp_name, &error),
            "w+");
  GList *l;

  for (l = filenames; l != NULL; l = l->next)
    {
      FILE *f = fopen (l->data, "r");
      int line = 1;

      GString *define_line;
      char *str;
      gboolean error_line = FALSE;
      gboolean end_of_word;
      int c = eat_hspace (f);
      while (c != EOF)
        {
          if (c != '#')
            {
              /* ignore line */
              c = eat_line (f, c);
              line++;
              continue;
            }

          /* print current location */
          str = g_strescape (l->data, "");
          fprintf (fmacros, "# %d \"%s\"\n", line, str);
          g_free (str);

          c = eat_hspace (f);
          c = read_identifier (f, c, &str);
          end_of_word = (c == ' ' || c == '\t' || c == '\n' || c == EOF);
          if (end_of_word &&
              (g_str_equal (str, "if") ||
               g_str_equal (str, "endif") ||
               g_str_equal (str, "ifndef") ||
               g_str_equal (str, "ifdef") ||
               g_str_equal (str, "else") ||
               g_str_equal (str, "elif")))
            {
              fprintf (fmacros, "#%s ", str);
              g_free (str);
              c = pass_line (f, c, fmacros);
              line++;
              continue;
            }
          else if (strcmp (str, "define") != 0 || !end_of_word)
            {
              g_free (str);
              /* ignore line */
              c = eat_line (f, c);
              line++;
              continue;
            }
          g_free (str);
          c = eat_hspace (f);
          c = read_identifier (f, c, &str);
          if (strlen (str) == 0 || (c != ' ' && c != '\t' && c != '('))
            {
              g_free (str);
              /* ignore line */
              c = eat_line (f, c);
              line++;
              continue;
            }
          define_line = g_string_new ("#define ");
          g_string_append (define_line, str);
          g_free (str);
          if (c == '(')
            {
              while (c != ')')
                {
                  g_string_append_c (define_line, c);
                  c = fgetc (f);
                  if (c == EOF || c == '\n')
                    {
                      error_line = TRUE;
                      break;
                    }
                }
              if (error_line)
                {
                  g_string_free (define_line, TRUE);
                  /* ignore line */
                  c = eat_line (f, c);
                  line++;
                  continue;
                }

              g_assert (c == ')');
              g_string_append_c (define_line, c);
              c = fgetc (f);

              /* found function-like macro */
              fprintf (fmacros, "%s\n", define_line->str);

              g_string_free (define_line, TRUE);
              /* ignore rest of line */
              c = eat_line (f, c);
              line++;
              continue;
            }
          if (c != ' ' && c != '\t')
            {
              g_string_free (define_line, TRUE);
              /* ignore line */
              c = eat_line (f, c);
              line++;
              continue;
            }
          while (c != EOF && c != '\n')
            {
              g_string_append_c (define_line, c);
              c = fgetc (f);
              if (c == '\\')
                {
                  c = fgetc (f);
                  if (c == '\n')
                    {
                      /* fold lines when seeing backslash new-line sequence */
                      c = fgetc (f);
                    }
                  else
                    {
                      g_string_append_c (define_line, '\\');
                    }
                }
            }

          /* found object-like macro */
          fprintf (fmacros, "%s\n", define_line->str);

          c = eat_line (f, c);
          line++;
        }

      fclose (f);
    }

  rewind (fmacros);
  gi_source_scanner_parse_file (scanner, fmacros);
  fclose (fmacros);
  g_unlink (tmp_name);
}

gboolean
gi_source_scanner_parse_file (GISourceScanner *scanner, FILE *file)
{
  g_return_val_if_fail (file != NULL, FALSE);

  lineno = 1;
  yyin = file;
  yyparse (scanner);
  yyin = NULL;

  return TRUE;
}

gboolean
gi_source_scanner_lex_filename (GISourceScanner *scanner, const gchar *filename)
{
  lineno = 1;
  yyin = fopen (filename, "r");

  while (yylex (scanner) != YYEOF)
    ;

  fclose (yyin);

  return TRUE;
}
