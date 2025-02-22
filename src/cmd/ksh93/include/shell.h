/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1982-2012 AT&T Intellectual Property          *
*          Copyright (c) 2020-2021 Contributors to ksh 93u+m           *
*                      and is licensed under the                       *
*                 Eclipse Public License, Version 1.0                  *
*                    by AT&T Intellectual Property                     *
*                                                                      *
*                A copy of the License is available at                 *
*          http://www.eclipse.org/org/documents/epl-v10.html           *
*         (with md5 checksum b35adb5213ca9657e911e9befb180842)         *
*                                                                      *
*              Information and Software Systems Research               *
*                            AT&T Research                             *
*                           Florham Park NJ                            *
*                                                                      *
*                  David Korn <dgk@research.att.com>                   *
*                                                                      *
***********************************************************************/
#pragma prototyped
#ifndef SH_INTERACTIVE
/*
 * David Korn
 * AT&T Labs
 *
 * Interface definitions for shell command language
 *
 */

#define SH_VERSION	20071012

#include	<ast.h>
#include	<cdt.h>
#ifdef _SH_PRIVATE
#   include	"name.h"
#else
#   include	<nval.h>
#endif /* _SH_PRIVATE */

/* options */
typedef struct
{
	unsigned long v[4];
}
Shopt_t;

typedef struct Shell_s Shell_t;

#include	<shcmd.h>

typedef void	(*Shinit_f)(Shell_t*, int);
#ifndef SH_wait_f_defined
    typedef int	(*Shwait_f)(int, long, int);
#   define SH_wait_f_defined
#endif

union Shnode_u;
typedef union Shnode_u Shnode_t;

/*
 * Shell state flags. Used with sh_isstate(), sh_onstate(), sh_offstate().
 * See also shell options below. States 0-5 are also used as shell options.
 */
#define SH_NOFORK	0	/* set when fork not necessary */
#define	SH_FORKED	7	/* set when process has been forked */
#define	SH_PROFILE	8	/* set when processing profiles */
#define SH_NOALIAS	9	/* do not expand non-exported aliases */
#define SH_NOTRACK	10	/* set to disable sftrack() function */
#define SH_STOPOK	11	/* set for stopable builtins */
#define SH_GRACE	12	/* set for timeout grace period */
#define SH_TIMING	13	/* set while timing pipelines */
#define SH_DEFPATH	14	/* set when using default path */
#define SH_INIT		15	/* set when initializing the shell */
#define SH_TTYWAIT	16	/* waiting for keyboard input */
#define	SH_FCOMPLETE	17	/* set for filename completion */
#define	SH_PREINIT	18	/* set with SH_INIT before parsing options */
#define SH_COMPLETE	19	/* set for command completion */
#define SH_XARG		21	/* set while in xarg (command -x) mode */

/*
 * Shell options (set -o). Used with sh_isoption(), sh_onoption(), sh_offoption().
 * There can be a maximum of 256 (0..0xFF) shell options.
 * The short option letters are defined in optksh[] and flagval[] in sh/args.c.
 * The long option names are defined in shtab_options[] in data/options.c.
 */
#define SH_CFLAG	0
#define SH_HISTORY	1	/* used also as a state */
#define	SH_ERREXIT	2	/* used also as a state */
#define	SH_VERBOSE	3	/* used also as a state */
#define SH_MONITOR	4	/* used also as a state */
#define	SH_INTERACTIVE	5	/* used also as a state */
#define	SH_RESTRICTED	6
#define	SH_XTRACE	7
#define	SH_KEYWORD	8
#define SH_NOUNSET	9
#define SH_NOGLOB	10
#define SH_ALLEXPORT	11
#if SHOPT_PFSH
#define SH_PFSH		12
#endif
#define SH_IGNOREEOF	13
#define SH_NOCLOBBER	14
#define SH_MARKDIRS	15
#define SH_BGNICE	16
#if SHOPT_VSH
#define SH_VI		17
#define SH_VIRAW	18
#endif
#define	SH_TFLAG	19
#define SH_TRACKALL	20
#define	SH_SFLAG	21
#define	SH_NOEXEC	22
#if SHOPT_ESH
#define SH_GMACS	24
#define SH_EMACS	25
#endif
#define SH_PRIVILEGED	26
#define SH_NOLOG	28
#define SH_NOTIFY	29
#define SH_DICTIONARY	30
#define SH_PIPEFAIL	32
#define SH_GLOBSTARS	33
#if SHOPT_GLOBCASEDET
#define SH_GLOBCASEDET	34
#endif
#define SH_RC		35
#define SH_SHOWME	36
#define SH_LETOCTAL	37
#if SHOPT_BRACEPAT
#define SH_BRACEEXPAND	42
#endif
#define SH_POSIX	46
#define SH_MULTILINE	47
#define SH_NOBACKSLCTRL	48
#define SH_LOGIN_SHELL	67
#define SH_NOUSRPROFILE	79	/* internal use only */
#define SH_COMMANDLINE	0x100	/* bit flag for invocation-only options ('set -o' cannot change them) */

/*
 * passed as flags to builtins in Nambltin_t struct when BLT_OPTIM is on
 */
#define SH_BEGIN_OPTIM	0x1
#define SH_END_OPTIM	0x2

/* The following type is used for error messages */

/* error messages */
extern const char	e_found[];
#ifdef ENAMETOOLONG
extern const char	e_toolong[];
#endif
extern const char	e_format[];
extern const char 	e_number[];
extern const char	e_restricted[];
extern const char	e_recursive[];
extern char		e_version[];

typedef struct sh_scope
{
	struct sh_scope	*par_scope;
	int		argc;
	char		**argv;
	char		*cmdname;
	char		*filename;
	char		*funname;
	int		lineno;
	Dt_t		*var_tree;
	struct sh_scope	*self;
} Shscope_t;

/*
 * Saves the state of the shell
 */

struct Shell_s
{
	Shopt_t		options;	/* set -o options */
	Dt_t		*var_tree;	/* for shell variables */
	Dt_t		*fun_tree;	/* for shell functions */
	Dt_t		*alias_tree;	/* for alias names */
	Dt_t		*bltin_tree;    /* for builtin commands */
	Shscope_t	*topscope;	/* pointer to top-level scope */
	int		inlineno;	/* line number of current input file */
	int		exitval;	/* exit status of the command currently being run */
	int		savexit;	/* $? == exit status of the last command executed */
	unsigned char	trapnote;	/* set when trap/signal is pending */
	char		shcomp;		/* set when running shcomp */
	unsigned int	subshell;	/* set for virtual subshell */
#ifdef _SH_PRIVATE
	_SH_PRIVATE
#endif /* _SH_PRIVATE */
};

/* used for builtins */
typedef struct Libcomp_s
{
	void*		dll;
	char*		lib;
	dev_t		dev;
	ino_t		ino;
	unsigned int	attr;
} Libcomp_t;
extern Libcomp_t *liblist;

/* flags for sh_parse */
#define SH_NL		1	/* Treat new-lines as ; */
#define SH_EOF		2	/* EOF causes syntax error */

/* symbolic values for sh_iogetiop */
#define SH_IOCOPROCESS	(-2)
#define SH_IOHISTFILE	(-3)

#include	<cmd.h>

/* symbolic value for sh_fdnotify */
#define SH_FDCLOSE	(-1)

#undef getenv			/* -lshell provides its own */

#if defined(__EXPORT__) && defined(_DLL)
#	define extern __EXPORT__
#endif /* _DLL */

extern Dt_t		*sh_bltin_tree(void);
extern void		sh_subfork(void);
extern Shell_t		*sh_init(int,char*[],Shinit_f);
extern int		sh_reinit(char*[]);
extern int 		sh_eval(Sfio_t*,int);
extern void 		sh_delay(double,int);
extern void		*sh_parse(Shell_t*, Sfio_t*,int);
extern int 		sh_trap(const char*,int);
extern int 		sh_fun(Namval_t*,Namval_t*, char*[]);
extern int 		sh_funscope(int,char*[],int(*)(void*),void*,int);
extern Sfio_t		*sh_iogetiop(int,int);
extern int		sh_main(int, char*[], Shinit_f);
extern int		sh_run(int, char*[]);
extern void		sh_menu(Sfio_t*, int, char*[]);
extern Namval_t		*sh_addbuiltin(const char*, int(*)(int, char*[],Shbltin_t*), void*);
extern char		*sh_fmtq(const char*);
extern char		*sh_fmtqf(const char*, int, int);
extern Sfdouble_t	sh_strnum(const char*, char**, int);
extern int		sh_access(const char*,int);
extern int 		sh_close(int);
extern int		sh_chdir(const char*);
extern int 		sh_dup(int);
extern void 		sh_exit(int);
extern int		sh_fchdir(int);
extern int		sh_fcntl(int, int, ...);
extern Sfio_t		*sh_fd2sfio(int);
extern int		(*sh_fdnotify(int(*)(int,int)))(int,int);
extern Shell_t		*sh_getinterp(void);
extern int		sh_open(const char*, int, ...);
extern int		sh_openmax(void);
extern Sfio_t		*sh_pathopen(const char*);
extern ssize_t 		sh_read(int, void*, size_t);
extern ssize_t 		sh_write(int, const void*, size_t);
extern off_t		sh_seek(int, off_t, int);
extern int 		sh_pipe(int[]);
extern mode_t 		sh_umask(mode_t);
extern void		*sh_waitnotify(Shwait_f);
extern Shscope_t	*sh_getscope(int,int);
extern Shscope_t	*sh_setscope(Shscope_t*);
extern void		sh_sigcheck(Shell_t*);
extern unsigned long	sh_isoption(int);
extern unsigned long	sh_onoption(int);
extern unsigned long	sh_offoption(int);
extern int 		sh_waitsafe(void);
extern int		sh_exec(const Shnode_t*,int);

/*
 * direct access to sh is obsolete, use sh_getinterp() instead
 */
extern Shell_t sh;

#ifdef _DLL
#   undef extern
#endif /* _DLL */

#define chdir(a)	sh_chdir(a)
#define fchdir(a)	sh_fchdir(a)
#ifndef _SH_PRIVATE
#   define access(a,b)	sh_access(a,b)
#   define close(a)	sh_close(a)
#   define exit(a)	sh_exit(a)
#   define fcntl(a,b,c)	sh_fcntl(a,b,c)
#   define pipe(a)	sh_pipe(a)
#   define read(a,b,c)	sh_read(a,b,c)
#   define write(a,b,c)	sh_write(a,b,c)
#   define umask(a)	sh_umask(a)
#   define dup		sh_dup
#   if _lib_lseek64
#	define open64	sh_open
#	define lseek64	sh_seek
#   else
#	define open	sh_open
#	define lseek	sh_seek
#   endif
#endif /* !_SH_PRIVATE */

#define SH_SIGSET	4
#define SH_EXITSIG	0400	/* signal exit bit */
#define SH_EXITMASK	(SH_EXITSIG-1)	/* normal exit status bits */
#define SH_RUNPROG	-1022	/* needs to be negative and < 256 */

#endif /* SH_INTERACTIVE */
