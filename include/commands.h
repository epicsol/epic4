/*
 * commands.h: header for commands.c
 *
 * Copyright 1990 Michael Sandrof
 * Copyright 1994 Matthew Green
 * Copyright 1997 EPIC Software Labs
 * See the COPYRIGHT file, or do a HELP IRCII COPYRIGHT 
 */

#ifndef __commands_h__
#define __commands_h__

/* flags used by e_away */
#define AWAY_ONE 			0
#define AWAY_ALL 			1

/* flags used by parse_line */
#define SECURITY_NO_VARIABLE_COMMAND	1
#define SECURITY_NO_NONINTERACTIVE_EXEC 2
#define SECURITY_NO_NONINTERACTIVE_SET	4

extern	char	*sent_nick;
extern	char	*sent_body;
extern	char	*recv_nick;

extern	int	will_catch_break_exceptions;
extern	int	will_catch_continue_exceptions;
extern	int	will_catch_return_exceptions;
extern	int	break_exception;
extern	int	continue_exception;
extern	int	return_exception;

extern	int	need_defered_commands;

	int	check_wait_command 	(char *);
	void	ExecuteTimers		(void);
	void	parse_line 		(const char *, const char *, const char *, int, int);
	BUILT_IN_COMMAND(load);
	void	send_text	 	(const char *, const char *, const char *, int);
	void	redirect_text		(int, const char *, const char *, char *, int);
	int	command_exist		(char *);
	BUILT_IN_COMMAND(e_channel);
	void	do_defered_commands	(void);

#endif /* __commands_h__ */
