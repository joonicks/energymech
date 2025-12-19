/*

    EnergyMech, IRC bot software
    Parts Copyright (c) 1997-2025 proton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
#ifdef MEGA_C
#define MAIN_C
#include "config.h"
#include "defines.h"
#include "structs.h"
#include "global.h"
#ifdef TCL
#include <tcl.h>
#endif
#include "h.h"
#include "text.h"
#include "mcmd.h"
#undef MAIN_C
#define VARS_C
#include "settings.h"
#undef VARS_C

#include "alias.c"
#include "auth.c"
#include "bounce.c"
#include "calc.c"
#include "channel.c"
#include "core.c"
#include "ctcp.c"
#include "debug.c"
#undef DEBUG_C
#include "dns.c"
#include "function.c"
#include "greet.c"
#include "help.c"
#include "hostinfo.c"
#include "io.c"
#include "irc.c"
#include "net.c"
#include "note.c"
#include "ons.c"
#include "parse.c"
#include "partyline.c"
#if defined(TCL) && defined(PERL)
#undef STRINGIFY
#endif
#include "perl.c"
#include "prot.c"
#include "python.c"
#include "reset.c"
#include "seen.c"
#include "shit.c"
#include "spy.c"
#include "string.c"
#include "tcl.c"
#include "toybox.c"
#include "uptime.c"
#include "user.c"
#include "vars.c"
#include "web.c"
#else /* ifdef MEGA_C */

#define MAIN_C
#include "config.h"

#include "defines.h"
#include "structs.h"
#include "global.h"
#include "h.h"
#include "text.h"
#include "mcmd.h"

#endif /* ifdef-else MEGA_C */

/*
 *  we generally try to mess around as little as possible here
 */
void mech_exec(void)
{
	char	newprog[4096];		/* we're bloating the stack, so what? */
	char	*argv[5],*envp[2];
	int	i;

	respawn++;
	if (respawn > 10)
		exit(99);		/* 99 no endless crash loops */

	argv[1] = argv[2] = argv[3] = argv[4] = NULL;

	if (respawn)
	{
		sprintf(newprog,"%s %i",executable,respawn);
		argv[0] = newprog;
	}
	else
	{
		argv[0] = executable;
	}

	i = 1;

	if (makecore)
		argv[i++] = "-c";

#ifdef DEBUG
	if (debug_on_exit)
	{
		time(&cx.now);
		wrap_debug();
		argv[i++] = "-X";
	}
#endif /* DEBUG */

	envp[0] = mechresetenv;
	envp[1] = NULL;

#ifdef DEBUG
	debug("execve( %s, argv = { %s %s %s %s %s }, envp = { %s } )\n",
		nullstr(executable),
		nullstr(argv[0]),nullstr(argv[1]),nullstr(argv[2]),nullstr(argv[3]),nullstr(argv[4]),
		nullstr(envp[0]));
#endif /* DEBUG */

#ifdef __profiling__
#ifdef SIGPROF
	signal(SIGPROF,SIG_IGN);
#endif /* SIGPROF */
#endif /* __profiling__ */

	i = execve(executable,argv,envp);

#ifdef DEBUG
	debug("(mech_exec) execve() FAILED! returned %i, errno = %i\n",i,errno);
#endif /* DEBUG */
	exit(23); /* 23 mech_exec execve failed */
}

int r_ct;
char r_str[MSGLEN];

int randstring_count(char *line)
{
	r_ct++;
	return(FALSE);
}

int randstring_getline(char *line)
{
	if (--r_ct == 0)
	{
		stringcpy(r_str,line);
		return(TRUE);
	}
	return(FALSE);
}

char *randstring(const char *file)
{
	int	in;

	if ((in = open(file,O_RDONLY)) < 0)
		return(NULL);

	r_ct = 0;
	readline(in,&randstring_count);				/* readline closes in */

	if ((in = open(file,O_RDONLY)) < 0)
		return(NULL);

	r_ct = RANDOM(1,r_ct);
	readline(in,&randstring_getline);			/* readline closes in */

	return(r_str);
}

/*
 *  Signal handlers
 *  ~~~~~~~~~~~~~~~
 *  SIGHUP	Read and execute all commands in `mech.msg' file.
 *  SIGCHLD	Take care of zombies
 *  SIGALRM	Ignore ALRM signals
 *  SIGPIPE	Ignore PIPE signals
 *  SIGINT	Exit gracefully on ^C
 *  SIGILL	Illegal instruction (debug: report) quit
 *  SIGABRT	abort(3) (debug: report) quit
 *  SIGBUS	(Try to) Exit/restart gracefully on bus faults
 *  SIGSEGV	(Try to) Exit/restart gracefully on segmentation violations
 *  SIGTERM	Exit gracefully when killed
 *  SIGUSR1	Jump (a) bot to a new server
 *  SIGUSR2	Call run_debug() (dump `everything' to a debug file)
 */

struct
{
	uint32_t sighup:1,
		sigint:1,
		sigusr1:1;

} sched_sigs;

int sig_hup_callback(char *line)
{
	on_msg((char*)cx.CoreUser.name,getbotnick(current),line);
	return(FALSE);
}

void do_sighup(void)
{
	int	in;

	sched_sigs.sighup = FALSE;

	CurrentShit = NULL;
	CurrentChan = NULL;
	CurrentUser = (User*)&cx.CoreUser;
	CurrentDCC  = (Client*)&CoreClient;
	*CurrentNick = 0;

	if ((in = open(MSGFILE,O_RDONLY)) >= 0)
	{
		readline(in,&sig_hup_callback);			/* readline closes in */
		unlink(MSGFILE);
	}

	CurrentDCC  = NULL;
}

void sig_hup(int crap)
{
#ifdef DEBUG
	debug("(sighup)\n");
#endif /* DEBUG */
	signal(SIGHUP,sig_hup);
	/*
	 *  Schedule the doing of greater things
	 */
	sched_sigs.sighup = TRUE;
}

#ifndef __linux__

/*
 *  Figured it out the hard way:
 *  DONT PUT THE signal() CALL BEFORE THE ACTUAL wait()!!
 *  Fucked up SunOS will hang/sigsegv!
 */
void sig_child(int crap)
{
#ifdef DEBUG
	debug("(sig_child)\n");
#endif /* DEBUG */

	while(1)
	{
		if (waitpid(-1,NULL,WNOHANG) <= 0)
			break;
	}
	signal(SIGCHLD,sig_child);
}

void sig_alrm(int signum)
{
#ifdef DEBUG
	debug("(sigalrm)\n");
#endif /* DEBUG */
	signal(SIGALRM,sig_alrm);
}

void sig_pipe(int dummy)
{
#ifdef DEBUG
	debug("(sigpipe)\n");
#endif /* DEBUG */
	signal(SIGPIPE,sig_pipe);
}

#endif /* not __linux__ */

void do_sigusr1(void)
{
	sched_sigs.sigusr1 = FALSE;

	if (current->connect == CN_ONLINE)
		to_server(TEXT_SIGUSR1);
	else
	{
		if (current->sock != -1)
		{
			close(current->sock);
		}
		current->sock = -1;
	}
}

/*
 *  SIGUSR1 -- reconnect first bot to a new server
 */
void sig_usr1(int crap)
{
#ifdef DEBUG
	debug("(sigusr1)\n");
#endif /* DEBUG */

	sched_sigs.sigusr1 = TRUE;
	signal(SIGUSR1,sig_usr1);
}

#ifdef DEBUG

/*
 *  SIGUSR2 -- dump debug info
 */
void sig_usr2(int crap)
{
	time(&cx.now);

	debug("(sigusr2)\n");
	signal(SIGUSR2,sig_usr2);

	run_debug();
}

#endif /* DEBUG */

/*
 *  signals that cause suicide...
 */

#ifdef UPTIME
#define UP_ARGS		, int uptype
#define UP_CALL(x)	, x
#else
#define UP_ARGS		/* nothing */
#define UP_CALL(x)	/* nothing */
#endif

void sig_suicide(char *text UP_ARGS)
{
#ifdef TRIVIA
	/*
	 *  trivia is global and is saved in the same way (and places) as session
	 */
	write_triviascore();
#endif /* TRIVIA */

#ifdef SESSION
	write_session();
#endif /* SESSION */

#ifdef UPTIME
	uptime_death(uptype);
#endif /* UPTIME */

	kill_all_bots(text);
	/* NOT REACHED */
}

void do_sigint(void)
{
	/*
	 *  We dont care about resetting sched_sigs.sigint,
	 *  we're committing suicide here!
	 */

	sig_suicide(TEXT_SIGINT /* comma */ UP_CALL(UPTIME_SIGINT));
	/* NOT REACHED */
}

void sig_int(int signum)
{
	if (sched_sigs.sigint)
	{
		/*
		 *  something locked up, SIGINT twice
		 */
#ifdef DEBUG
		debug("(sigint) (twice)\n");
#endif /* DEBUG */
		do_sigint();
		/* NOT REACHED */
	}
#ifdef DEBUG
	debug("(sigint)\n");
#endif /* DEBUG */

	sched_sigs.sigint = TRUE;
	signal(SIGINT,sig_int);
}

/*
 *  SIGILL, Illegal instruction
 */
#ifdef DEBUG

void sig_ill(int crap)
{
	debug("(sigill)\n");
}

/*
 *  SIGABRT, abort(3)
 */
void sig_abrt(int crap)
{
	debug("(sigabrt)\n");
}

#endif /* DEBUG */

/*
 *  SIGBUS is a real killer and cant be scheduled.
 */
void sig_bus(int crap)
{
	if (do_exec)
		mech_exec();

#ifdef DEBUG
	debug("(sigbus)\n");
#endif /* DEBUG */

	do_exec = TRUE;
	sig_suicide(TEXT_SIGBUS /* comma */ UP_CALL(UPTIME_SIGBUS));
	/* NOT REACHED */
}

/*
 *  SIGSEGV shows no mercy, cant schedule it.
 */
#if defined(__linux__) && defined(DEBUG) && !defined(__STRICT_ANSI__)
#include <sys/ucontext.h>
void sig_segv(int sig, siginfo_t *si, void *context)
{
	ucontext_t *crashcontext = (ucontext_t*)context;
	char	*sp,*ip; /* general registers */

	startup = STARTUP_SIGSEGV;

#if defined(__x86_64__)
	ip = (char*)crashcontext->uc_mcontext.gregs[16];
	sp = (char*)crashcontext->uc_mcontext.gregs[15];
#elif defined(__i386__)
	void *ip = (void*)crashcontext->uc_mcontext.gregs[REG_EIP];
	void *sp = (void*)crashcontext->uc_mcontext.gregs[REG_ESP];
#elif defined(__aarch64__)
	void *ip = (void*)uc->uc_mcontext.pc;
	void *sp = (void*)uc->uc_mcontext.sp;
#elif defined(__arm__)
	void *pc = (void*)uc->uc_mcontext.arm_pc;
	void *sp = (void*)uc->uc_mcontext.arm_sp;
#elif defined(__mips__)
	void *pc = (void*)uc->uc_mcontext.pc;
	void *sp = (void*)uc->uc_mcontext.gregs[29];
#else
#error "sig_segv(): Unsupported architecture"
#endif

	debug("(sigsegv) trying to access "mx_pfmt"\n",(mx_ptr)si->si_addr);
	debug("(sigsegv) Stack pointer: "mx_pfmt", Instruction pointer: "mx_pfmt"\n",(mx_ptr)sp,(mx_ptr)ip);
	debug("(sigsegv) sig_segv() = "mx_pfmt"\n",(mx_ptr)sig_segv);
	debug("(sigsegv) do_crash() = "mx_pfmt"\n",(mx_ptr)do_crash);

	if (do_exec)
		mech_exec();

	do_exec = TRUE;
	sig_suicide(TEXT_SIGSEGV /* comma */ UP_CALL(UPTIME_SIGSEGV));
	/* NOT REACHED */
}

#else /* defined(__linux__) && defined(DEBUG) && !defined(__STRICT_ANSI__) */

void sig_segv(int signum)
{
	startup = STARTUP_SIGSEGV;

	if (do_exec)
		mech_exec();

	sig_suicide(TEXT_SIGSEGV /* comma */ UP_CALL(UPTIME_SIGSEGV));
	/* NOT REACHED */
}

#endif /* else defined(__linux__) && defined(DEBUG) && !defined(__STRICT_ANSI__) */

/*
 *  SIGTERM
 */
void sig_term(int signum)
{
#ifdef __profiling__
	exit(0); /* 0 sigterm profiling exit */
#endif /* __profiling__ */

#ifdef DEBUG
	debug("(sigterm)\n");
#endif /* DEBUG */

	sig_suicide(TEXT_SIGTERM /* comma */ UP_CALL(UPTIME_SIGTERM));
	/* NOT REACHED */
}

/*
 *
 *  The main loop
 *
 */

void mainloop(void)
{
	SequenceTime this;
	Chan	*chan;
	Client	*client;
	Strp	*qm;
	struct	timeval tv;
	time_t	last_update;


	last_update = cx.now;

	/*
	 *  init update times so that they dont all run right away
	 */
	this.tenminute = cx.now / 600;
	this.hour = cx.now / 3600;

	/*
	 *  The Main Loop
	 */
mainloop:
	/*
	 *  signal processing
	 */
	for(current=botlist;current;current=current->next)
	{
		if (current->guid == sigmaster)
			break;
	}
	if (!current)
		current = botlist;
	if (sched_sigs.sighup)
		do_sighup();
	if (sched_sigs.sigint)
		do_sigint();
	if (sched_sigs.sigusr1)
		do_sigusr1();

	/*
	 *  check for regular updates
	 */
	if (last_update != cx.now)
	{
		last_update = cx.now;
		update(&this);
	}

	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	cx.hisock = -1;

#ifdef BOTNET
	select_botnet();
#endif /* BOTNET */

#ifdef WEB
	select_web();
#endif /* WEB */

#ifdef RAWDNS
	select_rawdns();
#endif /* RAWDNS */

#ifdef BOUNCE
	select_bounce();
#endif /* BOUNCE */

#ifdef HOSTINFO
	select_monitor();
#endif

	/*
	 *  unset here, reset if needed in bot loop
	 */
	cx.short_tv &= ~(TV_SERVCONNECT|TV_LINEBUF);
	for(current=botlist;current;current=current->next)
	{
		if (current->sock == -1)
		{
#ifdef RAWDNS
			if (current->connect == CN_DNSLOOKUP)
			{
				Server	*sp;
				char	*host = NULL;

				if ((sp = find_server(current->server)))
				{
					if ((cx.now - current->conntry) > ctimeout)
					{
#ifdef DEBUG
						debug("(doit) RAWDNS timed out (%s)\n",sp->name);
#endif /* DEBUG */
						current->connect = CN_NOSOCK;
						sp->err = SP_NO_DNS;
						goto doit_jumptonext;
					}
					if ((host = poll_rawdns(sp->name)))
					{
						char	hosttemp[strlen(host)+1];

#ifdef DEBUG
						debug("(doit) rawdns: %s ==> %s\n",sp->name,host);
#endif /* DEBUG */
						stringcpy(hosttemp,host);
						host = hosttemp;
						try_server(sp,host);
					}
				}
			}
			else
			{
				if (current->connect == CN_SPINNING)
				{
					if ((cx.now - current->conntry) >= 60)
						connect_to_server();
				}
				else
				{
doit_jumptonext:
					cx.short_tv |= TV_SERVCONNECT;
					if ((cx.now - current->conntry) >= 2)
						connect_to_server();
				}
			}
#else /* ! RAWDNS */
			if (current->connect == CN_SPINNING)
			{
				if ((cx.now - current->conntry) >= 60)
					connect_to_server();
			}
			else
			{
				cx.short_tv |= TV_SERVCONNECT;
				if ((cx.now - current->conntry) >= 2)
					connect_to_server();
			}
#endif /* RAWDNS */
		}
		/* not an else block since previous block might set sock to != -1 */
		if (current->sock != -1)
		{
			if (current->ip.s_addr == 0)
			{
				struct	sockaddr_in sai;
				unsigned int sz;

				sz = sizeof(sai);
				if (getsockname(current->sock,(struct sockaddr *)&sai,&sz) == 0)
					current->ip.s_addr = sai.sin_addr.s_addr;
			}
			if ((current->connect == CN_TRYING) || (current->connect == CN_CONNECTED))
			{
				cx.short_tv |= TV_SERVCONNECT;
				if ((cx.now - current->conntry) > ctimeout)
				{
#ifdef DEBUG
					debug("(doit) {%i} Connection timed out\n",current->sock);
#endif /* DEBUG */
					close(current->sock);
					current->sock = -1;
					goto restart_dcc;
				}
				if (current->connect == CN_TRYING)
					FD_SET(current->sock,&write_fds);
			}
			if (current->sendq)
			{
				cx.short_tv |= TV_LINEBUF;
			}
			else
			{
				for(chan=current->chanlist;chan;chan=chan->next)
				{
					if (chan->kicklist || chan->modelist)
					{
						cx.short_tv |= TV_LINEBUF;
						break;
					}
				}
			}
			chkhigh(current->sock);
			FD_SET(current->sock,&read_fds);
		}

		/*
		 *  Clean out DCC_DELETE clients
		 */
restart_dcc:
		for(client=current->clientlist;client;client=client->next)
		{
			if (client->flags == DCC_DELETE)
			{
				delete_client(client);
				goto restart_dcc;
			}
		}

		for(client=current->clientlist;client;client=client->next)
		{
			if (client->flags & DCC_ASYNC)
			{
				chkhigh(client->sock);
				FD_SET(client->sock,&write_fds);
			}
			if (client->sock != -1)
			{
				chkhigh(client->sock);
				FD_SET(client->sock,&read_fds);
			}
		}
	}

#ifdef UPTIME
	if (uptimesock >= 0)
		chkhigh(uptimesock);
#endif /* UPTIME */

	/*
	 *  Longer delay saves CPU but some features require shorter delays
	 */
#ifdef NOTIFY
	tv.tv_sec = (cx.short_tv) ? 1 : 5;
#else /* NOTIFY */
	tv.tv_sec = (cx.short_tv) ? 1 : 30;
#endif /* NOTIFY */
	tv.tv_usec = 0;

	if ((select(cx.hisock+1,&read_fds,&write_fds,0,&tv) == -1) && (errno == EINTR))
		goto mainloop;

	/*
	 *  Update current time
	 */
	time(&cx.now);

	for(current=botlist;current;current=current->next)
	{
		/*
		 *  sendq_time can never be smaller than the current time
		 *  it is important that the check is done before anything
		 *  else that could potentially send output to the server!
		 */
		if (current->sendq_time < cx.now)
			current->sendq_time = cx.now;
	}

	for(current=botlist;current;current=current->next)
	{
		if (current->clientlist)
			process_dcc();

		if (current->sock != -1)
			process_server_input();

#ifdef DEBUG
		if (current->inject)
		{
			char	injection[MSGLEN];

			stringcpy(injection,current->inject);
			Free((char**)&current->inject);
			debug("(*inject) %s\n");
			parse_server_input(injection);
		}
#endif /* DEBUG */

		if (current->connect == CN_ONLINE)
		{
			/*
			 *  Keep server idle-timer low to seem like you are chatting
			 */
			if (current->setting[TOG_NOIDLE].int_var)
			{
				if ((cx.now - current->lastantiidle) > PINGSENDINTERVAL)
				{
					to_server("PRIVMSG * :0\n");
					current->lastantiidle = cx.now;
				}
			}
			/*
			 *  check for HIGH priority modes
			 */
			for(chan=current->chanlist;chan;chan=chan->next)
			{
				if (chan->modelist && chan->bot_is_op)
					push_modes(chan,8);
			}
			/*
			 *  check for waiting kicks
			 */
			for(chan=current->chanlist;chan;chan=chan->next)
			{
				if (chan->kicklist && chan->bot_is_op)
					push_kicks(chan);
			}
			/*
			 *  check for LOW priority modes
			 */
			for(chan=current->chanlist;chan;chan=chan->next)
			{
				if (chan->modelist && chan->bot_is_op)
					push_modes(chan,0);
			}
			/*
			 *  the un-important sendq only sends when sendq_time <= now
			 */
			if ((current->sendq) && (current->sendq_time <= cx.now))
			{
				qm = current->sendq;
				to_server(FMT_PLAINLINE,qm->p);
				current->sendq = qm->next;
				Free((char**)&qm);
			}
		}
	}

	/*
	 *  Check for do_die()'d bots...
	 */
restart_die:
	for(current=botlist;current;current=current->next)
	{
		if (current->connect == CN_BOTDIE)
		{
			signoff(current->from,current->signoff);
			/*
			 *  signoff touches the botlist, so we need to restart
			 */
			goto restart_die;
		}
	}

#ifdef BOTNET
	if (botnetlist)
		process_botnet();
#endif /* BOTNET */

#ifdef BOUNCE
	if (bounce_sock != -1 || bnclist)
		process_bounce();
#endif /* BOUNCE */

#ifdef CHANBAN
	process_chanbans();
#endif /* CHANBAN */

#ifdef RAWDNS
	/*
	 *  Only a single socket to check.
	 */
	if (dnssock != -1 && FD_ISSET(dnssock,&read_fds))
		process_rawdns();
#endif /* RAWDNS */

#ifdef UPTIME
	process_uptime();
#endif /* UPTIME */

#ifdef WEB
	process_web();
#endif /* WEB */

#ifdef HOSTINFO
	process_monitor();
#endif

#ifdef TRIVIA
	if (triv_next_time && (cx.now >= triv_next_time))
		trivia_tick();
#endif /* TRIVIA */

	/*
	 *  Check killsocks
	 */
	if (killsocks)
		killsock(-1);

	goto mainloop;
}

/*
 *  parse commandline
 */

const char *bad_exe = "init: Error: Improper executable name\n";

void parse_commandline(int argc, char **argv, char **envp)
{
	struct stat st;
	char	*opt;
	int	do_fork = TRUE;
	int	versiononly = FALSE;
#ifdef NEWBIE
	int	n = 0;
#endif
	memset(&cx,0,sizeof(cx));

	uptime = time(&cx.now);
	startup = STARTUP_NORMALSTART;

	if ((getuid() == 0) || (geteuid() == 0))
	{
		to_file(1,"init: Do NOT run EnergyMech as root!\n");
		exit(11); /* 11 dont run mech as root */
	}

	stat("..",&st);
	parent_inode = st.st_ino; /* used for is_safepath() */

	if (stat("/proc",&st) >= 0)
	{
		cx.system_uptime = st.st_ctime;
	}

	srand(cx.now + getpid());

	/*
	 *   Code to detect and recover after a RESET
	 */
	/*
	execve( ./energymech, argv = { ./energymech <NULL> <NULL> <NULL> <NULL> },
		envp = { MECHRESET=d3 f1881:2:X12 } )
	*/
	while(*envp)
	{
		char	*p1;
		const char *p2;

		p1 = *envp;
		p2 = STR_MECHRESET;
		while(*p2)
		{
			if (*p1 != *p2)
				break;
			p1++;
			p2++;
		}
		if (*p2 == 0)
		{
			mechresetenv = p1;
			do_fork = FALSE;
#ifdef DEBUG
			if (*p1 == 'd')
				mechresetenv = recover_debug(p1+1);
#endif /* DEBUG */
			break;
		}
		envp++;
	}

#ifdef DEBUG
	/* memory tracking */
	mrrec = calloc(sizeof(aMEA),1);
#endif /* DEBUG */

	if (!*argv || !**argv)
	{
		to_file(1,bad_exe);
		exit(13); /* 13 bad executable name or bad arguments */
	}
	if ((opt = stringchr(*argv,' ')) != NULL)
	{
		*(opt++) = 0;
		respawn = asc2int(opt);
		if (errno)
		{
			to_file(1,bad_exe);
			exit(13); /* 13 bad executable name or bad arguments */
		}
	}

	executable = *argv;

	while((argc > 1) && (argv[1][0] == '-'))
	{
		argc--;
		argv++;
		opt = *argv;
		switch(opt[1])
		{
		case 'c':
			makecore = TRUE;
			break;
#ifdef DEBUG
		case 'd':
			dodebug = TRUE;
			do_fork = TRUE;
			if (opt[2] != 0) /* -d[file] */
			{
				debugfile = &opt[2];
			}
			else
			if (argv[1] && argv[1][0] != '-') /* -d [file] */
			{
				++argv;
				debugfile = *argv;
				argc--;
			}
			else
				do_fork = FALSE;
			break;
#endif /* DEBUG */
		case 'e': /* run a single command before exiting */
			startup = 3;
			++argv;
			if (*argv)
			{
				/*void on_msg(char *from, char *to, char *rest)
				on_msg(); */
			}
			else
				to_file(1,"error: Missing argument for -e <command string>\n");
			exit(0); /* 0 normal exit after -e command execution */
		case 'f':
			if (opt[2] != 0)
			{
				configfile = &opt[2];
			}
			else
			{
				++argv;
				if(!*argv)
				{
					to_file(1,ERR_MISSINGCONF);
					exit(16); /* 16 missing filename after -f */
				}
				configfile = *argv;
				argc--;
			}
			to_file(1,INFO_USINGCONF,configfile);
			break;
		case 'h':
			to_file(1,TEXT_USAGE,executable);
			to_file(1,
				TEXT_CSWITCH
#ifdef DEBUG
				TEXT_DSWITCH
#endif /* DEBUG */
				TEXT_ESWITCH
				TEXT_FSWITCH
				TEXT_HSWITCH
				TEXT_PSWITCH1
				TEXT_PSWITCH2
				TEXT_TSWITCH
				TEXT_VSWITCH
#ifdef DEBUG
				TEXT_XSWITCH
#endif /* DEBUG */
				  );
			exit(0); /* 0 normal exit after -h */
		case 'p':
			++argv;
			if (*argv)
				to_file(1,"%s\n",makepass(*argv));
			else
				to_file(1,"error: Missing argument for -p <string>\n");
			exit(0); /* 0 normal exit after -p */
		case 't':
			startup = STARTUP_TESTRUN;
			break;
		case 'v':
			versiononly = TRUE;
			break;
#ifdef DEBUG
		case 'x':
			debug_on_exit = TRUE;
			break;
#endif /* DEBUG */
		default:
			to_file(1,ERR_UNKNOWNOPT,opt);
			exit(8); /* 8 unknown option */
		}
	}

	if (!mechresetenv)
	{
		to_file(1,TEXT_HDR_VERS,VERSION,SRCDATE);
		to_file(1,"Compiled on " GENDATE "\n");
		to_file(1,TEXT_HDR_FEAT,__mx_opts);
	}

	if (versiononly)
		exit(0);	/* 0 normal exit after -v */

#ifdef NEWBIE
#ifdef SESSION
	if (stringcmp(CFGFILE,configfile) && stringcmp(SESSIONFILE,configfile))
	{
		to_file(1,"warning: current configuration file overrides session file\n");
	}
#endif /* SESSION */
	if (stat(configfile,&st))
	{
		if ((st.st_mode & (S_IWGRP|S_IWOTH)) != 0)
		{
			to_file(1,"error: configfile is writeable by others, exiting...\n");
			exit(24); /* 24 config file is world writeable */
		}
		if ((st.st_mode & (S_IRGRP|S_IROTH)) != 0)
			to_file(1,"warning: configfile is readable by others\n");
	}
	if (stat(".",&st))
	{
		if ((st.st_mode & (S_IWGRP|S_IWOTH)) != 0)
		{
			to_file(1,"error: energymech home directory is writeable by others, exiting...\n");
			exit(25); /* 25 energymech home directory is world writeable */
		}
	}
#endif /* NEWBIE */

#ifdef __linux__
	signal(SIGCHLD,SIG_IGN);
	signal(SIGALRM,SIG_IGN);
	signal(SIGPIPE,SIG_IGN);
#else
	signal(SIGCHLD,sig_child);
	signal(SIGALRM,sig_alrm);
	signal(SIGPIPE,sig_pipe);
#endif
	signal(SIGHUP,sig_hup);
	signal(SIGINT,sig_int);
	signal(SIGBUS,sig_bus);
	signal(SIGTERM,sig_term);
	signal(SIGUSR1,sig_usr1);
#ifdef DEBUG
	signal(SIGUSR2,sig_usr2);
#else /* DEBUG */
	signal(SIGUSR2,SIG_IGN);
#endif /* DEBUG */

#ifdef RAWDNS
	memset(&ia_ns,0,sizeof(ia_ns));
	ia_default.s_addr = LOCALHOST_ULONG;
#endif /* RAWDNS */

	cx.CoreUser.x.x.access = 100;
	cx.LocalBot.x.x.access = 200;
	cx.LocalBot.x.x.aop = 1;
	cx.LocalBot.chan = cx.CoreUser.chan = (Strp*)&CMA;

	readcfgfile();

#ifdef UPTIME
	init_uptime();
#endif /* UPTIME */

#ifndef I_HAVE_A_LEGITIMATE_NEED_FOR_MORE_THAN_4_BOTS
	if (spawning_lamer > 4)
	{
		to_file(1,"init: I refuse to run more than 4 bots!\n");
		exit(18); /* 18 luser trying to spawn too many bots */
	}
#endif /* I_HAVE_A_LEGITIMATE_NEED_FOR_MORE_THAN_4_BOTS */

	for(current=botlist;current;current=current->next)
	{
		if ((opt = current->setting[STR_USERFILE].str_var))
#ifndef NEWBIE
			read_userlist(opt);
	}
#else
		{
			if (stat(opt,&st))
			{
				if ((st.st_mode & (S_IWGRP|S_IWOTH)) != 0)
				{
					to_file(1,"error: userfile(%s) is writeable by others, exiting...\n",opt);
					exit(26); /* 26 userfile is world writeable */
				}
				if ((st.st_mode & (S_IRGRP|S_IROTH)) != 0)
					to_file(1,"warning: userfile(%s) is readable by others\n",opt);
			}
			read_userlist(opt);
		}
		if (current->userlist == NULL)
		{
			char *nick;

			nick = getbotnick(current);
			to_file(1,"init: No userlist loaded for %s\n",nullstr(nick));
			n++;
		}
	}
	if (n)
	{
		exit(10); /* 10 bots are missing userlist */
	}
#endif /* NEWBIE */

	for(current=botlist;current;current=current->next)
	{
		mirror_userlist();
	}

#ifdef SEEN
	read_seenlist();
#endif /* SEEN */

	if (!mechresetenv)
		to_file(1,INFO_RUNNING);

	if (do_fork && startup != 666)
	{
		close(0);
		close(1);
		close(2);

		switch(fork())
		{
		case 0:
			break;
		default:
			exit(0); /* 0 normal fork parent exit */
		case -1:
			exit(3); /* 3 problem with fork */
		}
		setsid();
	}

	/*
	 *  save pid to `mech.pid'
	 */
	if ((do_fork = open(PIDFILE,O_WRONLY|O_CREAT|O_TRUNC,NEWFILEMODE)) >= 0)
	{
		to_file(do_fork,"%i\n",(int)(getpid()));
		close(do_fork);
	}

#ifdef CTCP
	memset(&ctcp_slot,0,sizeof(ctcp_slot));
#endif /* CTCP */

#ifdef BOTNET
	last_autolink = cx.now + 30 + (rand() >> 27);	/* + 0-31 seconds */
#endif /* BOTNET */

	if (mechresetenv)
		recover_reset();

	/*
	 *  wait until after recover_reset() cuz it might change makecore
	 */
	if (!makecore)
	{
#if defined(__linux__) && defined(__x86_64__) && defined(DEBUG) && !defined(__STRICT_ANSI__)
		struct sigaction s;
		s.sa_flags = SA_SIGINFO;
		sigemptyset(&s.sa_mask);
		s.sa_sigaction = sig_segv;
		if (sigaction(SIGSEGV, &s, NULL) < 0)
		{
			debug("(main) binding SIGSEGV handler failed: %s\n",strerror(errno));
		}
#else
		signal(SIGSEGV,sig_segv);
#endif
#ifdef DEBUG
		signal(SIGILL,sig_ill);
		signal(SIGABRT,sig_abrt);
#endif /* DEBUG */
	}

#ifdef HOSTINFO
	monitor_fs(executable);
#endif

	if (startup == STARTUP_TESTRUN)
	{
#ifdef DEBUG
		if (debug_on_exit == TRUE)
			run_debug();
#endif /* DEBUG */
		to_file(1,"init: test run completed, exiting...\n");
		exit(0); /* 0 normal exit after -t */
	}
	startup = STARTUP_RUNNING;
#ifdef DEBUG
	debug("(main) entering main loop...\n");
#endif
}

#include <sys/prctl.h>

/*
 *  Make main short and sweet, reduce stack data
 *  Main(), we love it and cant live without it
 */
int main(int argc, char **argv, char **envp)
{
#ifdef USE_PRCTL
	prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
#endif /* USE_PRCTL */
	parse_commandline(argc, argv, envp);
	mainloop();
}
