/* A Bison parser, made by GNU Bison 2.7.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002-2012 Free Software Foundation, Inc.
   
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


/* First part of user declarations.  */

/* Line 279 of lalr1.cc  */
#line 38 "step.tab.cxx"


#include "step.tab.hxx"

/* User implementation prologue.  */

/* Line 285 of lalr1.cc  */
#line 46 "step.tab.cxx"
/* Unqualified %code blocks.  */
/* Line 286 of lalr1.cc  */
#line 39 "StepFile/step.yacc"

#include "recfile.ph"		/* definitions des types d'arguments */
#include "recfile.pc"		/* la-dedans, tout y est */
#include "scanner.hpp"
#undef yylex
#define yylex scanner->lex
/*
#define stepparse STEPparse
#define steplex STEPlex
#define stepwrap STEPwrap
#define steprestart STEPrestart
#define steplex STEPlex
#define steplval STEPlval
#define stepval STEPval
#define stepchar STEPchar
#define stepdebug STEPdebug
#define stepnerrs STEPnerrs
#define steperror STEPerror
*/

#define stepclearin yychar = -1
#define steperrok yyerrflag = 0

/*
#define stepin STEPin
#define yyerrflag STEPerrflag
#define yyerrstatus STEPerrflag
*/

/* ABV 19.12.00: merging porting modifications by POP (for WNT, AIX) */
#if defined(WNT) && !defined(MSDOS)
#define MSDOS WNT
#endif
#if defined(_AIX)
#include <malloc.h>
#define alloca malloc
#endif


// disable MSVC warnings in bison code
#ifdef _MSC_VER
#pragma warning(disable:4065 4244 4131 4127 4702)
#define YYMALLOC malloc
#define YYFREE free
#endif
void StepFile_Interrupt (char* nomfic); /* rln 13.09.00 port on HP*/


/* Line 286 of lalr1.cc  */
#line 99 "step.tab.cxx"


# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location) YYUSE(Type)
# define YY_REDUCE_PRINT(Rule)        static_cast<void>(0)
# define YY_STACK_PRINT()             static_cast<void>(0)

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {
/* Line 353 of lalr1.cc  */
#line 194 "step.tab.cxx"

  /// Build a parser object.
  parser::parser (yy::scanner* scanner_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      scanner (scanner_yyarg)
  {
  }

  parser::~parser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  parser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    std::ostream& yyo = debug_stream ();
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  parser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  parser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
  
	default:
	  break;
      }
  }

  void
  parser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  inline bool
  parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    // State.
    int yyn;
    int yylen = 0;
    int yystate = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    static semantic_type yyval_default;
    semantic_type yylval = yyval_default;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[3];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
        YYCDEBUG << "Reading a token: ";
        yychar = yylex (&yylval, &yylloc);
      }

    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yy_table_value_is_error_ (yyn))
	  goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    // Compute the default @$.
    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }

    // Perform the reduction.
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
          case 11:
/* Line 670 of lalr1.cc  */
#line 97 "StepFile/step.yacc"
    {  rec_finfile();  return(0);  /*  fini pour celui-la  */  }
    break;

  case 16:
/* Line 670 of lalr1.cc  */
#line 106 "StepFile/step.yacc"
    {  rec_finhead();  }
    break;

  case 17:
/* Line 670 of lalr1.cc  */
#line 108 "StepFile/step.yacc"
    {  rec_typarg(rec_argIdent);     rec_newarg();  }
    break;

  case 18:
/* Line 670 of lalr1.cc  */
#line 109 "StepFile/step.yacc"
    {  /* deja fait par lex*/ 	 rec_newarg();  }
    break;

  case 19:
/* Line 670 of lalr1.cc  */
#line 110 "StepFile/step.yacc"
    {  rec_newarg();  }
    break;

  case 20:
/* Line 670 of lalr1.cc  */
#line 111 "StepFile/step.yacc"
    {  rec_newarg();  }
    break;

  case 21:
/* Line 670 of lalr1.cc  */
#line 112 "StepFile/step.yacc"
    {  rec_typarg(rec_argMisc);      rec_newarg();
			   yyerrstatus_ = 1; yyclearin;  }
    break;

  case 22:
/* Line 670 of lalr1.cc  */
#line 117 "StepFile/step.yacc"
    {  rec_listype();  }
    break;

  case 23:
/* Line 670 of lalr1.cc  */
#line 120 "StepFile/step.yacc"
    {  rec_deblist();  }
    break;

  case 24:
/* Line 670 of lalr1.cc  */
#line 123 "StepFile/step.yacc"
    {  if (modeprint > 0)
		{  printf("Record no : %d -- ",nbrec+1);  rec_print(currec);  }
	   rec_newent ();  yyerrstatus_ = 0; }
    break;

  case 41:
/* Line 670 of lalr1.cc  */
#line 150 "StepFile/step.yacc"
    {  scope_debut();  }
    break;

  case 42:
/* Line 670 of lalr1.cc  */
#line 153 "StepFile/step.yacc"
    {  rec_typarg(rec_argIdent);    rec_newarg();  }
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 159 "StepFile/step.yacc"
    {  rec_deblist();  }
    break;

  case 46:
/* Line 670 of lalr1.cc  */
#line 162 "StepFile/step.yacc"
    {  scope_fin();  }
    break;

  case 47:
/* Line 670 of lalr1.cc  */
#line 164 "StepFile/step.yacc"
    {  printf("***  Warning : Export List not yet processed\n");
	   rec_newent();  scope_fin() ; }
    break;

  case 48:
/* Line 670 of lalr1.cc  */
#line 169 "StepFile/step.yacc"
    {  rec_ident();  }
    break;

  case 49:
/* Line 670 of lalr1.cc  */
#line 172 "StepFile/step.yacc"
    {  rec_type ();  }
    break;


/* Line 670 of lalr1.cc  */
#line 573 "step.tab.cxx"
      default:
        break;
      }

    /* User semantic actions sometimes alter yychar, and that requires
       that yytoken be updated with the new translation.  We take the
       approach of translating immediately before every use of yytoken.
       One alternative is translating here after every semantic action,
       but that translation would be missed if the semantic action
       invokes YYABORT, YYACCEPT, or YYERROR immediately after altering
       yychar.  In the case of YYABORT or YYACCEPT, an incorrect
       destructor might then be invoked immediately.  In the case of
       YYERROR, subsequent parser actions might lead to an incorrect
       destructor call or verbose syntax error message before the
       lookahead is translated.  */
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = yytranslate_ (yychar);

    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	if (yychar == yyempty_)
	  yytoken = yyempty_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[1] = yylloc;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */
        if (yychar <= yyeof_)
          {
            /* Return failure if at end of input.  */
            if (yychar == yyeof_)
              YYABORT;
          }
        else
          {
            yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
            yychar = yyempty_;
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
    if (false)
      goto yyerrorlab;

    yyerror_range[1] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (!yy_pact_value_is_default_ (yyn))
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	  YYABORT;

	yyerror_range[1] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[2] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      {
        /* Make sure we have latest lookahead translation.  See comments
           at user semantic actions for why this is necessary.  */
        yytoken = yytranslate_ (yychar);
        yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval,
                     &yylloc);
      }

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystate_stack_.height ())
      {
        yydestruct_ ("Cleanup: popping",
                     yystos_[yystate_stack_[0]],
                     &yysemantic_stack_[0],
                     &yylocation_stack_[0]);
        yypop_ ();
      }

    return yyresult;
    }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (yychar != yyempty_)
          {
            /* Make sure we have latest lookahead translation.  See
               comments at user semantic actions for why this is
               necessary.  */
            yytoken = yytranslate_ (yychar);
            yydestruct_ (YY_NULL, yytoken, &yylval, &yylloc);
          }

        while (1 < yystate_stack_.height ())
          {
            yydestruct_ (YY_NULL,
                         yystos_[yystate_stack_[0]],
                         &yysemantic_stack_[0],
                         &yylocation_stack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (int, int)
  {
    return YY_("syntax error");
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char parser::yypact_ninf_ = -26;
  const signed char
  parser::yypact_[] =
  {
        27,    30,   -26,   -26,   -26,    31,    36,   -26,   -26,    55,
     -26,    49,   -26,    14,   -26,    41,    55,   -26,   -26,    10,
      42,   -26,   -26,     9,   -26,    37,    41,    -3,   -26,   -26,
     -26,   -26,   -26,    14,   -26,   -26,     4,   -26,    65,    59,
     -26,     1,    54,   -26,   -26,    24,   -26,   -26,   -26,    56,
      48,    47,    14,    61,   -26,   -26,    -7,    14,   -26,    44,
      47,    -5,   -26,    51,   -26,   -26,    14,   -26,   -26,    58,
      -5,    52,   -26,    57,   -26,   -26,   -26,   -11,    53,   -26,
     -26,    58,   -26,   -26,   -26
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  parser::yydefact_[] =
  {
         0,     0,     9,    10,    11,     0,     0,     1,    15,     0,
      49,     0,    12,     0,    16,     0,     0,    13,    23,     0,
       0,    36,    48,     0,    31,     0,     0,    21,    22,    17,
      18,    24,    28,     0,    25,    19,     0,    14,    36,     0,
      32,     0,     0,    20,    30,     0,    26,     7,    41,     0,
       0,     0,     0,     0,    21,    29,     0,     0,    33,    46,
       0,     0,    39,     4,     6,    40,     0,    37,    45,     0,
       0,     0,     2,     5,    38,    42,    43,     0,     0,    35,
       3,     0,    47,    34,    44
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  parser::yypgoto_[] =
  {
       -26,   -26,   -26,   -26,   -26,   -26,   -26,   -26,    64,    60,
      33,   -26,   -26,    43,   -13,   -26,   -25,   -20,   -26,   -12,
     -26,    -1,   -26,   -26,    21,   -26,    -4
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  parser::yydefgoto_[] =
  {
        -1,    73,    64,     2,     3,     4,     5,    11,    12,    15,
      32,    33,    19,    34,    35,    36,    23,    24,    56,    50,
      51,    76,    77,    69,    61,    25,    52
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char parser::yytable_ninf_ = -28;
  const signed char
  parser::yytable_[] =
  {
        20,    42,    13,    40,    10,    44,    10,    13,   -27,    48,
      38,    27,    10,    81,    39,    82,    65,    49,   -27,    22,
      43,    28,    40,    49,    29,    54,    60,    31,    45,    30,
       1,     7,    18,    31,     6,    28,    18,     8,    29,    62,
      40,     9,    21,    30,    67,    57,    18,    10,    21,    71,
       8,    22,    66,    74,    16,    21,    59,    22,    78,    53,
      10,    14,    41,    37,    22,    -8,    47,    10,    63,    58,
      68,    72,    75,    79,    83,    17,    26,    80,    55,    46,
      84,    70
  };

  /* YYCHECK.  */
  const unsigned char
  parser::yycheck_[] =
  {
        13,    26,     6,    23,    11,     1,    11,    11,    11,     8,
       1,     1,    11,    24,     5,    26,    23,    22,    21,    10,
      33,    11,    42,    22,    14,     1,    51,    23,    24,    19,
       3,     0,    22,    23,     4,    11,    22,     1,    14,    52,
      60,     5,     1,    19,    57,    49,    22,    11,     1,    61,
       1,    10,    56,    66,     5,     1,     9,    10,    70,     5,
      11,     6,    25,    21,    10,     0,     7,    11,     7,    21,
      26,    20,    14,    21,    21,    11,    16,    20,    45,    36,
      81,    60
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  parser::yystos_[] =
  {
         0,     3,    30,    31,    32,    33,     4,     0,     1,     5,
      11,    34,    35,    53,     6,    36,     5,    35,    22,    39,
      41,     1,    10,    43,    44,    52,    36,     1,    11,    14,
      19,    23,    37,    38,    40,    41,    42,    21,     1,     5,
      44,    25,    43,    41,     1,    24,    40,     7,     8,    22,
      46,    47,    53,     5,     1,    37,    45,    53,    21,     9,
      43,    51,    41,     7,    29,    23,    53,    41,    26,    50,
      51,    46,    20,    28,    41,    14,    48,    49,    46,    21,
      20,    24,    26,    21,    48
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      32,    59,    40,    41,    44,    61,    47
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  parser::yyr1_[] =
  {
         0,    27,    28,    28,    29,    29,    30,    31,    32,    33,
      33,    33,    34,    34,    35,    35,    36,    37,    37,    37,
      37,    37,    38,    39,    40,    41,    41,    41,    42,    42,
      42,    43,    43,    44,    44,    44,    44,    45,    45,    46,
      46,    47,    48,    49,    49,    50,    51,    51,    52,    53
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  parser::yyr2_[] =
  {
         0,     2,     1,     2,     1,     2,     8,     7,     6,     1,
       1,     1,     1,     2,     3,     1,     1,     1,     1,     1,
       2,     1,     1,     1,     1,     2,     3,     2,     1,     3,
       2,     1,     2,     4,     7,     6,     1,     2,     3,     2,
       3,     1,     1,     1,     3,     1,     1,     4,     1,     1
  };

#if YYDEBUG
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const parser::yytname_[] =
  {
    "$end", "error", "$undefined", "STEP", "HEADER", "ENDSEC", "DATA",
  "ENDSTEP", "SCOPE", "ENDSCOPE", "ENTITY", "TYPE", "INTEGER", "FLOAT",
  "IDENT", "TEXT", "NONDEF", "ENUM", "HEXA", "QUID", "' '", "';'", "'('",
  "')'", "','", "'='", "'/'", "$accept", "finvide", "finstep", "stepf1",
  "stepf2", "stepf3", "stepf", "headl", "headent", "endhead", "unarg",
  "listype", "deblist", "finlist", "listarg", "arglist", "model", "bloc",
  "plex", "unent", "debscop", "unid", "export", "debexp", "finscop",
  "entlab", "enttype", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const parser::rhs_number_type
  parser::yyrhs_[] =
  {
        33,     0,    -1,    20,    -1,    28,    20,    -1,     7,    -1,
       7,    28,    -1,     3,     4,    34,     5,    36,    43,     5,
      29,    -1,     3,     4,     5,    36,    43,     5,     7,    -1,
       3,     4,     5,    36,    43,     1,    -1,    30,    -1,    31,
      -1,    32,    -1,    35,    -1,    34,    35,    -1,    53,    41,
      21,    -1,     1,    -1,     6,    -1,    14,    -1,    19,    -1,
      41,    -1,    38,    41,    -1,     1,    -1,    11,    -1,    22,
      -1,    23,    -1,    39,    40,    -1,    39,    42,    40,    -1,
      39,     1,    -1,    37,    -1,    42,    24,    37,    -1,    42,
       1,    -1,    44,    -1,    43,    44,    -1,    52,    25,    46,
      21,    -1,    52,    25,    47,    43,    51,    46,    21,    -1,
      52,    25,    47,    51,    46,    21,    -1,     1,    -1,    53,
      41,    -1,    45,    53,    41,    -1,    53,    41,    -1,    22,
      45,    23,    -1,     8,    -1,    14,    -1,    48,    -1,    49,
      24,    48,    -1,    26,    -1,     9,    -1,     9,    50,    49,
      26,    -1,    10,    -1,    11,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  parser::yyprhs_[] =
  {
         0,     0,     3,     5,     8,    10,    13,    22,    30,    37,
      39,    41,    43,    45,    48,    52,    54,    56,    58,    60,
      62,    65,    67,    69,    71,    73,    76,    80,    83,    85,
      89,    92,    94,    97,   102,   110,   117,   119,   122,   126,
     129,   133,   135,   137,   139,   143,   145,   147,   152,   154
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned char
  parser::yyrline_[] =
  {
         0,    89,    89,    90,    91,    92,    93,    94,    95,    96,
      96,    96,    99,   100,   102,   103,   105,   108,   109,   110,
     111,   112,   116,   119,   122,   127,   128,   129,   131,   132,
     133,   135,   136,   138,   139,   140,   141,   143,   144,   146,
     147,   149,   152,   155,   156,   158,   161,   163,   168,   171
  };

  // Print the state stack on the debug stream.
  void
  parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  parser::token_number_type
  parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,    20,     2,     2,     2,     2,     2,     2,     2,
      22,    23,     2,     2,    24,     2,     2,    26,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    21,
       2,    25,     2,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int parser::yyeof_ = 0;
  const int parser::yylast_ = 81;
  const int parser::yynnts_ = 27;
  const int parser::yyempty_ = -2;
  const int parser::yyfinal_ = 7;
  const int parser::yyterror_ = 1;
  const int parser::yyerrcode_ = 256;
  const int parser::yyntokens_ = 27;

  const unsigned int parser::yyuser_token_number_max_ = 274;
  const parser::token_number_type parser::yyundef_token_ = 2;


} // yy
/* Line 1141 of lalr1.cc  */
#line 1067 "step.tab.cxx"
/* Line 1142 of lalr1.cc  */
#line 174 "StepFile/step.yacc"


void yy::parser::error(const parser::location_type& l, const std::string& m)
{
  char newmess[80];
  sprintf(newmess, "At line %d, %s : %s", scanner->lineno() + 1, l, m.c_str());
  StepFile_Interrupt(newmess);
}
