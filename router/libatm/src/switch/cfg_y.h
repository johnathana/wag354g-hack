#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

#ifndef YYSTYPE
typedef union {
    int num;
    char *str;
    struct sockaddr_atmpvc pvc;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	TOK_COMMAND	257
# define	TOK_VPCI	258
# define	TOK_ITF	259
# define	TOK_DEFAULT	260
# define	TOK_ROUTE	261
# define	TOK_STR	262
# define	TOK_SOCKET	263
# define	TOK_OPTION	264
# define	TOK_CONTROL	265
# define	TOK_NUM	266
# define	TOK_PVC	267


extern YYSTYPE yylval;

#endif /* not BISON_Y_TAB_H */
