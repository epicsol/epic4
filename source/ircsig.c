/* $EPIC: ircsig.c,v 1.5 2008/02/16 23:59:11 jnelson Exp $ */
/*
 * ircsig.c: has a `my_signal()' that uses sigaction().
 *
 * Copyright (c) 1993-1996 Matthew R. Green.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/*
 * written by matthew green, 1993.
 *
 * i stole bits of this from w. richard stevens' `advanced programming
 * in the unix environment' -mrg
 */

#include "irc.h"
#include "irc_std.h"

int	block_signal (int sig_no)
{
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, sig_no);
	return sigprocmask(SIG_BLOCK, &set, NULL);
}

int	unblock_signal (int sig_no)
{
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, sig_no);
	return sigprocmask(SIG_UNBLOCK, &set, NULL);
}

/* array of signal handlers containing mostly NULL */
volatile sigfunc *signal_handlers[NSIG];

/* grand unified signal handler, which sets flags for scriptable signals 
 * -pegasus 
 */
RETSIGTYPE signal_handler (int sig_no)
{
	signals_caught[0] = 1;
	signals_caught[sig_no]++;
	if (NULL != signal_handlers[sig_no])
		signal_handlers[sig_no](sig_no);
}

/* hook_all_signals needs to be called in main() before my_signal()
 * if any signal hooks fail, it returns SIG_ERR, otherwise it returns 
 * NULL. -pegasus
 */
sigfunc *hook_all_signals (void)
{
        struct sigaction sa, osa;
	int sig_no;
	sigfunc *error = NULL;

	/* docs say this is const. if it changes, something else is 
	 * broken. -pegasus
	 */
	sa.sa_handler = &signal_handler;
	/* end possibly risky code */
	for (sig_no = 0; sig_no < NSIG; sig_no++)
	{
		signal_handlers[sig_no] = NULL;
		/* this is ugly, but the `correct' way.  i hate c. -mrg */
		/* moved from my_signal. -pegasus */
	        sa.sa_flags = 0;
#if defined(SA_RESTART) || defined(SA_INTERRUPT)
	        if (SIGALRM == sig_no || SIGINT == sig_no)
        	{
# if defined(SA_INTERRUPT)
			sa.sa_flags |= SA_INTERRUPT;
# endif /* SA_INTERRUPT */
		}
		else
		{
# if defined(SA_RESTART)
			sa.sa_flags |= SA_RESTART;
# endif /* SA_RESTART */
		}
#endif /* SA_RESTART || SA_INTERRUPT */
		/* if it wasn't for the above code, we could move the
		 * sigemptyset() and sigaction() calls outside the loop 
		 * proper. -pegasus
		 */
		sigemptyset(&sa.sa_mask);
		sigaddset(&sa.sa_mask, sig_no);
		if (0 > sigaction(sig_no, &sa, &osa))
			error = SIG_ERR;
	}
	return error;
}

sigfunc *my_signal (int sig_no, sigfunc *sig_handler)
{
        sigfunc *old;

	if (sig_no < 0)
		return NULL;		/* Signal not implemented */

	/* Well this is certainly simpler. -pegasus */
	old = signal_handlers[sig_no];
	signal_handlers[sig_no] = sig_handler;

        return old;
}
