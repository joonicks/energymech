/*

    EnergyMech, IRC bot software
    Copyright (c) 1997-2025 proton

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

/*

	These are defined in config.h

	DCC	0x000100		requires DCC
	CC	0x000200		requires commandchar
	PASS	0x000400		requires password / authentication
	CARGS	0x000800		requires args
	NOPUB	0x001000		ignore in channel (for password commands)
	NOCMD	0x002000		not allowed to be executed thru CMD
	GAXS	0x004000		check global access
	CAXS	0x008000		check channel access
	REDIR	0x010000		may be redirected
	LBUF	0x020000		can be linebuffered to server
	CBANG	0x040000		command may be prefixed with a bang (!)
	ACCHAN  0x080000		needs an active channel
	SUPRES  0x100000		command is not suitable to run on many bots at once, try to suppress it
	NOARGF  0x200000                run a special function if no arguments are supplied

	CLEVEL	0x000ff

*/

#define CCPW	CC|PASS

struct CommandList
{
	int		pass;		/* multiple passes to sort & such */
	const char	*name;
	const char	*func;
	uint32_t	flags;
	char		*cmdarg;

} pre_mcmd[] =
{
	/*
	 *  Level 0: public access commands, no CCPW
	 */
	{ 0, "AUTH",		"do_auth",		 0	| NOPUB	| CBANG	| NOARGF		}, // double up on AUTH/VERIFY to better
	{ 0, "VERIFY",		"do_auth",		 0	| NOPUB	| CBANG | NOARGF		}, // catch login attempts
#ifdef TOYBOX
	{ 0, "8BALL",		"do_8ball",		 0	| CBANG	| SUPRES			},
	{ 0, "RAND",		"do_rand",		 0	| CBANG	| SUPRES			},
#endif /* TOYBOX */
	{ 0, "CV",		"do_convert",		 0	| CBANG | SUPRES			},
	{ 0, "CALC",		"do_calc",		 0	| CBANG | SUPRES			},

	/*
	 *  Level 5: Semi-public commands, No PW required. CC included by default.
	 */
	{ 0, "BYE",		"do_bye",		5						},
	{ 0, "DOWN",		"do_opdeopme",		5	| CAXS					},
#ifdef DCC_FILE
	{ 0, "SEND",		"do_send",		5	| NOCMD | CBANG | CARGS	| LBUF		},
#endif /* DCC_FILE */

	/*
	 *  Level 10: All commands level 10+ PW by default (plus CC from Level 5+).
	 */
	{ 0, "ACCESS",		"do_access",		10						},
	{ 0, "CHAT",		"do_chat",		10	| NOCMD					},
#ifdef RAWDNS
	{ 0, "DNS",		"do_dns",		10	| GAXS | CARGS | SUPRES			},
#endif /* RAWDNS */
	{ 0, "ECHO",		"do_echo",		10	| CARGS					},
	{ 0, "HELP",		"do_help",		10	| REDIR | LBUF | SUPRES			},
	{ 0, "PASSWD",		"do_passwd",		10	| NOPUB | CARGS				},
	{ 0, "USAGE",		"do_usage",		10	| REDIR	| CARGS				},

	/*
	 *  Level 20
	 */
	{ 0, "ONTIME",		"do_upontime",		20						, "Ontime: %s" },
	{ 0, "UPTIME",		"do_upontime",		20						, "Uptime: %s" },
	{ 0, "VER",		"do_version",		20						},
	{ 0, "WHOM",		"do_whom",		20	| REDIR | LBUF				},
#ifdef SEEN
	{ 0, "SEEN",		"do_seen",		20	| CBANG					},
#endif /* SEEN */
#ifdef URLCAPTURE
	{ 0, "URLHIST",		"do_urlhist",		20	| REDIR | LBUF				},
#endif /* ifdef URLCAPTURE */

	/*
	 *  Level 40
	 */
	{ 0, "BAN",		"do_kickban",		40	| CCPW	| CAXS | CARGS | ACCHAN		, "\\x00ban\\0bann" },
	{ 0, "BANLIST",		"do_banlist",		40	| CAXS | DCC | REDIR | LBUF | ACCHAN	},
	{ 0, "CCHAN",		"do_cchan",		40	| CCPW					},	/* global_access ? */
	{ 0, "CSERV",		"do_cserv",		40	| CCPW					},
	{ 0, "CHANNELS",	"do_channels",		40	| CCPW	| DCC				},
	{ 0, "DEOP",		"do_opvoice",		40	| CAXS | CARGS				, "o-" },
	{ 0, "ESAY",		"do_esay",		40	| CAXS | CARGS				},
	{ 0, "IDLE",		"do_idle",		40	| CCPW	| CARGS				},
	{ 0, "INVITE",		"do_invite",		40	| CAXS | ACCHAN				},
	{ 0, "KB",		"do_kickban",		40	| CAXS | CARGS | ACCHAN			, "\\x04kickban\\0kickbann" },
	{ 0, "KICK",		"do_kickban",		40	| CAXS | CARGS | ACCHAN			, "\\x07kick\\0kick" },
	{ 0, "LUSERS",		"do_irclusers",		40	| CCPW	| DCC | REDIR | LBUF		},
	{ 0, "ME",		"do_sayme",		40	| CCPW	| CARGS				},
	{ 0, "MODE",		"do_mode",		40	| CCPW	| CARGS				},
	{ 0, "NAMES",		"do_names",		40	| CCPW					},
	{ 0, "OP",		"do_opvoice",		40	| CAXS					, "o+" },
	{ 0, "SAY",		"do_sayme",		40	| CCPW	| CARGS				},
	{ 0, "SCREW",		"do_kickban",		40	| CAXS | CARGS | ACCHAN			, "\\x02screwban\\0screwbann" },
	{ 0, "SET",		"do_set",		40	| CCPW					},
	{ 0, "SITEBAN",		"do_kickban",		40	| CAXS | CARGS | ACCHAN			, "\\x01siteban\\0sitebann" },
	{ 0, "SITEKB",		"do_kickban",		40	| CAXS | CARGS | ACCHAN			, "\\x05sitekickban\\0sitekickbann" },
	{ 0, "TIME",		"do_time",		40	| CCPW					},
	{ 0, "TOPIC",		"do_topic",		40	| CAXS | CARGS | ACCHAN | SUPRES	},
	{ 0, "UNBAN",		"do_unban",		40	| CAXS					},
	{ 0, "UNVOICE",		"do_opvoice",		40	| CAXS | CARGS				, "v-" },
	{ 0, "UP",		"do_opdeopme",		40	| CAXS					},
	{ 0, "USER",		"do_user",		40	| CCPW	| CARGS				},
	{ 0, "USERHOST",	"do_ircwhois",		40	| CCPW	| CARGS				},
	{ 0, "VOICE",		"do_opvoice",		40	| CAXS					, "v+" },
	{ 0, "WALL",		"do_wall",		40	| CCPW	| CAXS | CARGS | ACCHAN		},
	{ 0, "WHO",		"do_who",		40	| CCPW	| CAXS | DCC			},
	{ 0, "WHOIS",		"do_ircwhois",		40	| CCPW	| CARGS | DCC | REDIR | LBUF	},
#ifdef NOTE
	{ 0, "NOTE",		"do_note",		40	| CCPW	| CARGS				},
	{ 0, "READ",		"do_read",		40	| CCPW					},
#endif /* NOTE */
#ifdef STATS
	{ 0, "INFO",		"do_info",		40	| CCPW	| REDIR | CAXS | DCC		},
#endif /* STATS */

	/*
	 *  Level 50
	 */
	{ 0, "QSHIT",		"do_shit",		50	| CCPW	| CARGS				},
	{ 0, "RSHIT",		"do_rshit",		50	| CCPW	| CARGS				},
	{ 0, "SHIT",		"do_shit",		50	| CCPW	| CARGS				},
	{ 0, "SHITLIST",	"do_shitlist",		50	| CCPW	| DCC | REDIR | LBUF		},
#ifdef GREET
	{ 0, "GREET",		"do_greet",		50	| CCPW	| CARGS				},
#endif /* GREET */
#ifdef TOYBOX
	{ 0, "INSULT",		"do_randmsg",		50	| CCPW					, RANDINSULTFILE },
	{ 0, "PICKUP",		"do_randmsg",		50	| CCPW					, RANDPICKUPFILE },
	{ 0, "RSAY",		"do_randmsg",		50	| CCPW					, RANDSAYFILE },
	{ 0, "RT",		"do_randtopic",		50	| CCPW	| CAXS | ACCHAN			},
	{ 0, "ASCII",		"do_ascii",		50	| CCPW	| CAXS | CARGS | SUPRES		},
#endif /* TOYBOX */
#ifdef TRIVIA
	{ 0, "TRIVIA",		"do_trivia",		50	| CCPW	| CAXS | CARGS | CBANG		},
#endif /* TRIVIA */

	/*
	 *  Level 60
	 */
	{ 0, "SHOWIDLE",	"do_showidle",		60	| CCPW	| CAXS | DCC | ACCHAN		},
	{ 0, "USERLIST",	"do_userlist",		60	| CCPW	| DCC				},
#ifdef CTCP
	{ 0, "CTCP",		"do_ping_ctcp",		60	| CCPW	| CARGS				},
	{ 0, "PING",		"do_ping_ctcp",		60	| CCPW	| CARGS				},
#endif /* CTCP */

	/*
	 *  Level 70 == JOINLEVEL
	 */
	{ 0, "CYCLE",		"do_cycle",		70	| CCPW	| CAXS | ACCHAN			},
	{ 0, "FORGET",		"do_forget",		70	| CCPW	| CAXS				},
	{ 0, "JOIN",		"do_join",		70	| CCPW	| CARGS				},
	{ 0, "KS",		"do_kicksay",		70	| CCPW	| REDIR | LBUF			},
	{ 0, "PART",		"do_part",		70	| CCPW	| CAXS | ACCHAN			},
	{ 0, "RKS",		"do_rkicksay",		70	| CCPW	| CARGS				},
	{ 0, "SETPASS",		"do_setpass",		70	| CCPW	| NOPUB	| CARGS			},
#ifdef NOTIFY
	{ 0, "NOTIFY",		"do_notify",		70	| CCPW	| DCC | GAXS | REDIR | LBUF	},
#endif /* NOTIFY */

	/*
	 *  Level 80 == ASSTLEVEL
	 */
	{ 0, "AWAY",		"do_away",		80	| CCPW	| GAXS				},
	{ 0, "BOOT",		"do_boot",		80	| CCPW	| GAXS | CARGS			},
#if defined(BOTNET) && defined(REDIRECT)
	{ 0, "CMD",		"do_cmd",		80	| CCPW	| CARGS				},
#endif /* BOTNET && REDIRECT */
	{ 0, "CQ",		"do_clearqueue",	80	| CCPW	| GAXS				},
	{ 0, "LAST",		"do_last",		80	| CCPW	| DCC				},
	{ 0, "LOAD",		"do_load",		80	| CCPW	| GAXS				},
	{ 0, "MSG",		"do_msg",		80	| CCPW	| CARGS				},
	{ 0, "NEXTSERVER",	"do_server",		80	| CCPW	| GAXS				},
	{ 0, "SAVE",		"do_save",		80	| CCPW	| GAXS				},
	{ 0, "SERVER",		"do_server",		80	| CCPW	| GAXS | REDIR | NOPUB | NOARGF },
	{ 0, "STATS",		"do_ircstats",		80	| CCPW	| DCC | CARGS			},
#ifdef ALIAS
	{ 0, "ALIAS",		"do_alias",		80	| CCPW	| GAXS				},
	{ 0, "UNALIAS",		"do_unalias",		80	| CCPW	| GAXS | CARGS			},
#endif /* ALIAS */
#ifdef TOYBOX
	{ 0, "BIGSAY",		"do_bigsay",		80	| CCPW	| CAXS | CARGS | SUPRES		},
#endif /* TOYBOX */

	/*
	 *  Level 90
	 */
	{ 0, "CLEARSHIT",	"do_clearshit",		90	| CCPW	| GAXS				},
	{ 0, "DO",		"do_do",		90	| CCPW	| GAXS | CARGS			},
	{ 0, "NICK",		"do_nick",		90	| CCPW	| GAXS | CARGS			},
	{ 0, "RSPY",		"do_rspy",		90	| CCPW	| CARGS				},
	{ 0, "SPY",		"do_spy",		90	| CCPW					},
#ifdef BOTNET
	{ 0, "LINK",		"do_link",		90	| CCPW	| GAXS | NOPUB | NOARGF		},
#endif /* BOTNET */
#ifdef DYNCMD
	{ 0, "CHACCESS",	"do_chaccess",		90	| CCPW	| GAXS | CARGS			},
#endif /* DYNCMD */
#ifdef UPTIME
	{ 0, "UPSEND",		"do_upsend",		90	| CCPW	| GAXS				},
#endif /* UPTIME */

	/*
	 *  Level 100
	 */
#ifdef HOSTINFO
	{ 0, "SYSINFO",		"do_sysinfo",		100	| CCPW	| GAXS				},
	{ 0, "MEMINFO",		"do_meminfo",		100	| CCPW	| GAXS				},
	{ 0, "CPUINFO",		"do_cpuinfo",		100	| CCPW	| GAXS				},
	{ 0, "FILEMON",		"do_filemon",		100	| CCPW	| GAXS | CARGS			},
#endif /* HOSTINFO */
#ifdef RAWDNS
	{ 0, "DNSSERVER",	"do_dnsserver",		100	| CCPW  | GAXS				},
	{ 0, "DNSROOT",		"do_dnsroot",		100	| CCPW	| GAXS | CARGS			},
#endif /* RAWDNS */
	{ 0, "CORE",		"do_core",		100	| CCPW	| REDIR | DCC			},
	{ 0, "DIE",		"do_die",		100	| CCPW	| GAXS				},
	{ 0, "RESET",		"do_reset",		100	| CCPW	| GAXS | NOCMD			},
	{ 0, "SHUTDOWN",	"do_shutdown",		100	| CCPW	| GAXS | NOPUB | NOCMD		},
#ifdef DEBUG
	{ 0, "DEBUG",		"do_debug",		100	| CCPW	| GAXS				},
	{ 0, "CRASH",		"do_crash",		100	| CCPW	| GAXS				},
#endif /* DEBUG */
#ifdef PERL
#ifdef PLEASE_HACK_MY_SHELL
	{ 0, "PERL",		"do_perl",		100	| CCPW	| GAXS | CARGS			},
#endif /* PLEASE_HACK_MY_SHELL */
	{ 0, "PERLSCRIPT",	"do_perlscript",	100	| CCPW	| GAXS | CARGS			},
#endif /* PERL */
#ifdef PYTHON
#ifdef PLEASE_HACK_MY_SHELL
	{ 0, "PYTHON",		"do_python",		100	| CCPW	| GAXS | CARGS			},
#endif /* PLEASE_HACK_MY_SHELL */
	{ 0, "PYTHONSCRIPT",	"do_pythonscript",	100	| CCPW	| GAXS | CARGS			},
#endif /* PYTHON */
#ifdef TCL
#ifdef PLEASE_HACK_MY_SHELL
	{ 0, "TCL",		"do_tcl",		100	| CCPW	| GAXS | CARGS			},
#endif /* PLEASE_HACK_MY_SHELL */
	{ 0, "TCLSCRIPT",	"do_tcl",		100	| CCPW	| GAXS | CARGS			},
#endif /* TCL */
	/*---*/
	{ 0, NULL,		NULL,			0						},
};
