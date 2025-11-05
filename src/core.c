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
#define CORE_C
#include "config.h"

#include "defines.h"
#include "structs.h"
#include "global.h"
#include "h.h"
#include "text.h"
#include "mcmd.h"
#include "settings.h"

#ifdef HOSTINFO
#include <sys/utsname.h>
#endif /* HOSTINFO */

int conf_callback(char *line)
{

	if (line && *line == COMMENT_CHAR)
		return(FALSE);

	fix_config_line(line);

	on_msg((char*)CoreUser.name,getbotnick(current),line);
	return(FALSE);
}

void readcfgfile(void)
{
	Mech	*bot;
	int	oc,in;

	oc = TRUE;
	in = -1;

#ifdef SESSION
	if (!stringcmp(CFGFILE,configfile))
	{
		if ((in = open(SESSIONFILE,O_RDONLY)) >= 0)
		{
			to_file(1,"init: Restoring previously saved session...\n");
			oc = FALSE;
		}
	}
#endif /* SESSION */

	if (oc)
	{
		if ((in = open(configfile,O_RDONLY)) < 0)
		{
			to_file(1,"init: Couldn't open the file %s\n",configfile);
			mechexit(1,exit);
		}
	}

	current = add_bot(0,"(conf)");

	*CurrentNick = 0;
	CurrentShit = NULL;
	CurrentChan = NULL;
	CurrentDCC  = (Client*)&CoreClient;
	CurrentUser = (User*)&CoreUser;

	readline(in,&conf_callback);			/* readline closes in */

	CurrentDCC  = NULL;

	if (!current->guid)
	{
		to_file(1,"init: Error: No bots in the configfile\n");
		mechexit(1,exit);
	}

	if ((current) && (current->chanlist == NULL))
		to_file(1,"init: Warning: %s will not join any channels\n",getbotnick(current));

	oc = 0;
#ifdef DEBUG
	in = dodebug;
	dodebug = 0;
#endif /* DEBUG */
	to_file(1,"init: Mech(s) added [ ");
	for(bot=botlist;bot;bot=bot->next)
	{
		if (oc > 30)
		{
			to_file(1,", ...");
			break;
		}
		to_file(1,"%s%s",(oc > 0) ? ", " : EMPTYSTR,getbotnick(bot));
		oc += getbotnicklen(bot);
	}
	to_file(1," ]\n");
#ifdef DEBUG
	dodebug = in;
#endif /* DEBUG */
}

#ifdef SESSION

int write_session(void)
{
#ifdef BOTNET
	NetCfg	*cfg;
#endif /* BOTNET */
#ifdef RAWDNS
	Strp	*p;
#endif /* RAWDNS */
#ifdef ALIAS
	Alias	*alias;
#endif /* ALIAS */
	Server	*sp;
	Spy	*spy;
	Chan	*chan;
	Mech	*bot;
	UniVar	*varval;
	int	j,sf;

	/* save to filename.sessiontemp at first, in case SIGSEGV happens */
	if ((sf = open(SESSIONFILE /* + */ TEMP,O_WRONLY|O_CREAT|O_TRUNC,NEWFILEMODE)) < 0)
		return(FALSE);

#ifdef RAWDNS
	for(p=dnsrootfiles;p;p=p->next)
		to_file(sf,"dnsroot %s\n",p->p);
	for(j=0;j<MAX_NAMESERVERS;j++)
		if (ia_ns[j].s_addr > 0)
			to_file(sf,"dnsserver %s\n",inet_ntoa(ia_ns[j]));
#endif /* RAWDNS */

	to_file(sf,"set ctimeout %i\n",ctimeout);
	for(sp=serverlist;sp;sp=sp->next)
	{
		to_file(sf,"server %s %i %s\n",sp->name,(sp->port) ? sp->port : 6667,
			(sp->pass[0]) ? sp->pass : EMPTYSTR);
	}

#ifdef BOTNET
	if (linkpass)
		to_file(sf,"set linkpass %s\n",linkpass);
	if (linkport)
		to_file(sf,"set linkport %i\n",linkport);
	if (autolink)
		to_file(sf,"set autolink 1\n");
	for(cfg=netcfglist;cfg;cfg=cfg->next)
	{
		to_file(sf,"link %i %s",cfg->guid,(cfg->pass) ? cfg->pass : MATCH_ALL);
		if (cfg->host)
			to_file(sf," %s %i",cfg->host,cfg->port);
		to_file(sf,"\n");
	}
#endif /* BOTNET */

#ifdef BOUNCE
	if (bounce_port)
		to_file(sf,"set bncport %i\n",bounce_port);
#endif /* BOUNCE */

#ifdef WEB
	if (webport)
		to_file(sf,"set webport %i\n",webport);
#endif /* WEB */

#ifdef UPTIME
	if (uptimehost && stringcasecmp(uptimehost,defaultuptimehost))
		to_file(sf,"set uphost %s\n",uptimehost);
	if (uptimeport)
		to_file(sf,"set upport %i\n",uptimeport);
	if (uptimenick)
		to_file(sf,"set upnick %s\n",uptimenick);
#endif /* UPTIME */

#ifdef TRIVIA
	if (triv_qfile)
		to_file(sf,"set qfile %s\n",triv_qfile);
	to_file(sf,"set qdelay %i\n",triv_qdelay);
	to_file(sf,"set qchar %c\n",triv_qchar);
#endif /* TRIVIA */

#ifdef SEEN
	if (seenfile)
		to_file(sf,"set seenfile %s\n",seenfile);
#endif /* SEEN */

#ifdef DYNCMD
	/*
	 *  because of "chaccess ___ disable" its best to save chaccess last
	 */
	for(j=0;mcmd[j].name;j++)
	{
		if (acmd[j] != mcmd[j].defaultaccess)
		{
			if (acmd[j] == 250)
				to_file(sf,"chaccess %s disable\n",mcmd[j].name);
			else
				to_file(sf,"chaccess %s %i\n",mcmd[j].name,(int)acmd[j]);
		}
	}
#endif /* DYNCMD */

#ifdef ALIAS
	for(alias=aliaslist;alias;alias=alias->next)
	{
		to_file(sf,"alias %s %s\n",alias->alias,alias->format);
	}
#endif /* ALIAS */

	for(bot=botlist;bot;bot=bot->next)
	{
		to_file(sf,"nick %i %s\n",bot->guid,getbotwantnick(bot));
		/*
		 *  current->setting contains channel defaults and global vars
		 */
		for(j=0;VarName[j].name;j++)
		{
			if (IsProc(j))
				continue;
			varval = &bot->setting[j];
			if (IsChar(j))
			{
				if (VarName[j].v.num != varval->char_var)
					to_file(sf,"set %s %c\n",VarName[j].name,varval->char_var);
			}
			else
			if (IsNum(j))
			{
				if (VarName[j].v.num != varval->int_var)
					to_file(sf,"set %s %i\n",VarName[j].name,varval->int_var);
			}
			else
			if (IsStr(j))
			{
				/*
				 *  There are no default string settings
				 */
				if (varval->str_var)
					to_file(sf,"set %s %s\n",VarName[j].name,varval->str_var);
			}
		}
		for(chan=bot->chanlist;chan;chan=chan->next)
		{
			if (!chan->active && !chan->rejoin)
				continue;
			to_file(sf,"join %s %s\n",chan->name,(chan->key) ? chan->key : EMPTYSTR);
			/*
			 *  using CHANSET_SIZE: only the first settings contain stuff
			 */
			for(j=0;j<CHANSET_SIZE;j++)
			{
				varval = &chan->setting[j];
				if (IsNum(j))
				{
					if (VarName[j].v.num != varval->int_var)
						to_file(sf,"set %s %i\n",VarName[j].name,varval->int_var);
				}
				else
				if (IsStr(j))
				{
					/*
					 *  There are no default string settings
					 */
					if (varval->str_var)
						to_file(sf,"set %s %s\n",VarName[j].name,varval->str_var);
				}
			}
		}
		// SPY files
		for(spy=bot->spylist;spy;spy=spy->next)
		{
			if (spy->t_dest == SPY_FILE)
			{
				if (spy->src && spy->dest)
					to_file(sf,"spy %s > %s\n",spy->src,spy->dest);
#ifdef DEBUG
				else
					debug("spy to session fail\n");
#endif /* DEBUG */
			}
		}
	}

	close(sf);
	unlink(SESSIONFILE);
	rename(SESSIONFILE /* + */ TEMP,SESSIONFILE);
	return(TRUE);
}

#endif /* SESSION */

/*
 *  Bot nicking, adding and killing
 */

Mech *add_bot(int guid, char *nick)
{
	Mech	*bot;

	set_mallocdoer(add_bot);
	bot = (Mech*)Calloc(sizeof(Mech));
	bot->connect = CN_NOSOCK;
	bot->sock = -1;
	bot->guid = guid;
	setbotnick(bot,nick);
	setbotwantnick(bot,nick);
	set_binarydefault(bot->setting);
	bot->next = botlist;
	botlist = bot;

#ifndef I_HAVE_A_LEGITIMATE_NEED_FOR_MORE_THAN_4_BOTS
	spawning_lamer++;
#endif /* I_HAVE_A_LEGITIMATE_NEED_FOR_MORE_THAN_4_BOTS */

	return(bot);
}

void signoff(char *from, char *reason)
{
	Mech	**pp;
	char	*fname;
	int	i;

	if (from)
	{
		to_user(from,"Killing mech: %s",getbotnick(current));
		to_user(from,"Saving the lists...");
	}
	fname = current->setting[STR_USERFILE].str_var;
#ifdef DEBUG
	debug("(signoff) Saving lists...\n");
#endif /* DEBUG */
	if (!write_userlist(fname) && from)
	{
		to_user(from,(fname) ? ERR_NOSAVE : ERR_NOUSERFILENAME,fname);
	}
#ifdef SEEN
	if (seenfile && !write_seenlist() && from)
	{
		to_user(from,TEXT_SEENNOSAVE,seenfile);
	}
#endif /* SEEN */
#ifdef NOTIFY
	if (current->notifylist)
		write_notifylog();
#endif /* NOTIFY */
	if (from)
	{
		to_user(from,TEXT_SHUTDOWNCOMPLETE);
	}

	while(current->chanlist)
		remove_chan(current->chanlist);

	while(current->clientlist)
		delete_client(current->clientlist);

	while(current->authlist)
		remove_auth(current->authlist);

	purge_shitlist();
	purge_kicklist();

#ifdef NOTIFY
	purge_notify();
#endif /* NOTIFY */

	if (current->sock != -1)
	{
		if (!reason)
			reason = randstring(SIGNOFFSFILE);
		to_server("QUIT :%s\n",(reason) ? reason : EMPTYSTR);
		killsock(current->sock);
		current->sock = -1;
	}

	/*
	 *  release string var memory
	 */
	delete_vars(current->setting,(SIZE_VARS-1));

#ifdef DEBUG
	debug("(signoff) Removing userlist...\n");
#endif /* DEBUG */
	while(current->userlist)
		remove_user(current->userlist);

#ifdef DEBUG
	debug("(signoff) Removing lastcmd list...\n");
#endif /* DEBUG */
	for(i=0;i<LASTCMDSIZE;i++)
	{
		Free(&current->lastcmds[i]);
	}

	/*
	 *  little of this n that
	 */

	/*
	 *  These 2 are used by do_die() to pass reason and doer.
	 */
	Free((char**)&current->signoff);
	Free((char**)&current->from);

#ifdef DEBUG
	debug("(signoff) Unlinking bot record from linked list...\n");
#endif /* DEBUG */

	pp = &botlist;
	while(*pp)
	{
		if (*pp == current)
		{
			*pp = current->next;
			break;
		}
	}
	Free((char**)&current);

	/*
	 *  get a new current bot, or exit
	 */
	if ((current = botlist) == NULL)
	{
#ifdef TRIVIA
		write_triviascore();
#endif /* TRIVIA */

#ifdef UPTIME
		uptime_death(UPTIME_GENERICDEATH);
#endif /* UPTIME */

		while(killsock(-2))
			/* killsock() sleeps 1 second in select() */
			;

		mechexit(0,exit);
	}

#ifdef DEBUG
	debug("(signoff) All done.\n");
#endif /* DEBUG */
}

void kill_all_bots(char *reason)
{
	while(TRUE)
	{
		current = botlist;
		signoff(NULL,reason);
	}
}

/*
 *  Server lists, connects, etc...
 */
Server *add_server(const char *host, const int port, const char *pass, const char *group)
{
	Server	*sp,**pp;

#ifdef DEBUG
	debug("(add_server) host = %s, port = %i, group = %s, pass = '%s'.\n",host,port,group,pass);
#endif /* DEBUG */
	pp = &serverlist;
	while(*pp)
	{
		sp = *pp;
		if ((!port || sp->port == port) && (!stringcasecmp(host,sp->name) || !stringcasecmp(host,sp->realname)))
			return(sp);
		pp = &sp->next;
	}
#ifdef DEBUG
	debug("(add_server) creating new entry: id %i.\n",serverident+1);
#endif /* DEBUG */
	set_mallocdoer(add_server);
	*pp = sp = (Server*)Calloc(sizeof(Server));
	sp->ident = serverident++;
	sp->maxontime = -1;
	/* caller must make sure host, pass, port and group is all valid */
	stringcpy_n(sp->name,host,NAMELEN);
	stringcpy_n(sp->pass,pass,PASSLEN);
	stringcpy_n(sp->group,group,SERVERGROUPLEN);
	sp->port = port;
	return(sp);
}

Server *find_server(int id)
{
	Server	*sp;

	for(sp=serverlist;sp;sp=sp->next)
		if (sp->ident == id)
			return(sp);
	return(NULL);
}

int try_server(Server *sp, char *hostname)
{
#ifdef RAWDNS
	char	temphost[NAMEBUF];
	char	*host;
	uint32_t ip;
#endif /* RAWDNS */

	if (!hostname)
	{
		send_spy(SPYSTR_STATUS,"Trying new server: %s:%i",(*sp->realname) ? sp->realname : sp->name,sp->port);
		hostname = sp->name;
	}

	sp->lastattempt = now;
	sp->usenum++;

#ifdef RAWDNS
	if ((host = poll_rawdns(hostname)))
	{
#ifdef DEBUG
		debug("(try_server) rawdns: %s ==> %s\n",sp->name,host);
#endif /* DEBUG */
		stringcpy(temphost,host);
		hostname = temphost;
	}
	else
	if ((ip = inet_addr(hostname)) == -1)
	{
		current->server = sp->ident;
		current->connect = CN_DNSLOOKUP;
		current->conntry = now;
		rawdns(hostname);
		return(0);
	}
#endif /* RAWDNS */
	current->server = sp->ident;
	if ((current->sock = SockConnect(hostname,sp->port,TRUE)) < 0)
	{
		sp->err = SP_ERRCONN;
		return(-1);
	}
	current->away = FALSE;
	current->connect = CN_TRYING;
	current->activity = current->conntry = now;
	*current->modes = 0;
	return(current->sock);
}

void connect_to_server(void)
{
	Server	*sp,*sptry;
	Chan	*chan;
	char	*sgroup;

	/*
	 *  This should prevent the bot from chewing up too
	 *  much CPU when it fails to connect to ANYWHERE
	 */
	current->conntry = now;

	/*
	 *  Is this the proper action if there is no serverlist?
	 */
	if (!serverlist)
		return;

	if (current->chanlist)
	{
#ifdef DEBUG
		if (current->connect != CN_SPINNING) debug("[CtS] Setting rejoin- and synced-status for all channels\n");
#endif /* DEBUG */
		current->rejoin = TRUE;
		for(chan=current->chanlist;chan;chan=chan->next)
		{
			if (chan->active)
			{
				chan->active = FALSE;
				chan->rejoin = TRUE;
			}
			chan->sync = TRUE;
			chan->bot_is_op = FALSE;
#ifdef STATS
			if (chan->stats)
				chan->stats->flags |= CSTAT_PARTIAL;
#endif /* STATS */
		}
	}

	if (current->nextserver)
	{
		sp = find_server(current->nextserver);
		current->nextserver = 0;
		if (sp && (try_server(sp,NULL) >= 0))
			return;
	}

	/*
	 *  The purpose of this kludge is to find the least used server
	 *  July 7th: added logic for servergroup
	 */
	if ((sgroup = current->setting[STR_SERVERGROUP].str_var))
	{
#ifdef DEBUG
		debug("[CtS] servergroup set to \"%s\"\n",sgroup);
#endif /* DEBUG */
	}
	sptry = NULL;
	for(sp=serverlist;sp;sp=sp->next)
	{
		if (sp->lastattempt == now)
			continue;
		if (sgroup && (stringcasecmp(sgroup,sp->group) != 0))
			continue;
		if (sptry && (sp->usenum > sptry->usenum))
			continue;

		if (sp->err == 0 || sp->err == SP_ERRCONN)
			sptry = sp;
		else
		if ((sp->err == SP_THROTTLED && (sp->lastattempt + 45) < now) || /* retry throttled after 45 seconds */
		    (sp->err == SP_KLINED && (sp->lastattempt + 86400) < now)) /* retry Klined after a day */
			sptry = sp;
	}
	/*
	 *  Connect...
	 */
	if (sptry)
	{
		try_server(sptry,NULL);
		return;
	}
#ifdef DEBUG
	if (current->connect != CN_SPINNING)
	{
		const char *errtxt;
		debug("[CtS] Serverlist Exhausted:\n");
		for(sp=serverlist;sp;sp=sp->next)
		{
			errtxt = (const char *[]){"No error","SP_NOAUTH","SP_KLINED","SP_FULLCLASS",
				"SP_TIMEOUT","SP_ERRCONN","SP_DIFFPORT","SP_NO_DNS","SP_THROTTLED"}[sp->err];
			debug("[CtS] (%i) %s[%i]: %s(%i) / servergroup %i\n",sp->ident,
				(*sp->realname) ? sp->realname : sp->name,
				sp->port,errtxt,sp->err,sp->group);
		}
		debug("[CtS] Server connection is spinning...\n");
	}
	current->connect = CN_SPINNING;
#endif /* DEBUG */
}

/*
 *  === according to rfc1459 ===
 *     Command: USER
 *  Parameters: <username> <hostname> <servername> <realname>
 */
void register_with_server(void)
{
	Server	*sp;
	char	*ident,*ircname;
	int	sendpass;

#ifdef IRCD_EXTENSIONS
	current->ircx_flags = 0;
#endif /* IRCD_EXTENSIONS */
	sp = find_server(current->server);
	ident = current->setting[STR_IDENT].str_var;
	ircname = current->setting[STR_IRCNAME].str_var;
	sendpass = (sp && *sp->pass);
	to_server((sendpass) ? "PASS :%s\nNICK %s\nUSER %s " MECHUSERLOGIN " 0 :%s\n" :
		"%sNICK %s\nUSER %s " MECHUSERLOGIN " 0 :%s\n",
		(sendpass) ? sp->pass : EMPTYSTR,
		getbotwantnick(current),
		(ident) ? ident : BOTLOGIN,
		(ircname) ? ircname : VERSION);
	current->connect = CN_CONNECTED;
	current->conntry = now;
}

/*
 *  scripting stuff
 */

#ifdef SCRIPTING

int sub_compile_timer(int limit, uint32_t *flags1, uint32_t *flags2, char *args)
{
	char	*s,*dash;
	uint32_t f;
	int	n,hi,lo;

	*flags1 = 0;
	if (flags2) *flags2 = 0;

	if (!args || !*args)
		return -1;

	if (args[0] == '*' && args[1] == 0)
	{
		*flags1 = -1; /*  0-29 = 1 */
		if (flags2) *flags2 = -1; /* 30-59 = 1 */
		return 0;
	}

	/* "n,n,n-m,n-m" */
	for(s=args;*s;s++)
		if (*s == ',')
			*s = ' ';

	/* "n n n-m n-m" */
	do
	{
		s = chop(&args);
		if (s && *s)
		{
			if ((dash = STRCHR(s,'-')))
			{
				*(dash++) = 0;
				if (!*dash)
					return -1;

				lo = asc2int(s);
				hi = asc2int(dash);

				if (lo < 0 || lo > limit || hi < 0 || hi > limit)
					return -1;
				for(n=lo;n<=hi;n++)
				{
					if (n >= 30)
					{
						f = (1 << (n - 30));
						if (!flags2)
							return -1;
						*flags2 |= f;
					}
					else
					{
						f = (1 << n);
						*flags1 |= f;
					}
				}
			}
			else
			{
				n = asc2int(s);
				if (n < 0 || n > limit)
					return -1;
				if (n >= 30)
				{
					f = (1 << (n - 30));
					if (!flags2)
						return -1;
					*flags2 |= f;
				}
				else
				{
					f = (1 << n);
					*flags1 |= f;
				}
			}
		}
	}
	while(s);
	return 0;
}

#if 0

 turn this:
"* * * *"
"0-59 0-59 0-23 0-6"
0,1,2,3,...,58,59 = all seconds
0,1,2,3,...,58,59 = all minutes
0,1,2,3,...,22,23 = all hours
0,1,2,3,4,5,6 = all weekdays

 into this:

typedef struct
{
        time_t  last;
        time_t  next;
        uint32_t second1:30;
        uint32_t second2:30;
        uint32_t minute1:30;
        uint32_t minute2:30;
        uint32_t hour:24;
        uint32_t weekday:7;

} HookTimer;

	//using that struct, calculate when the next time will be
	//start by determining what the time is now

	thistime = now;

	//which second is it
	thissecond = thistime % 60;

	if (ht->second1 == 0x3FFFFFFF && ht->second2 == 0x3FFFFFFF)
	{
		// dont add waiting period to get to the proper second
	}

	//which minute is it
	thistime = (thistime - thissecond) / 60;
	thisminute = thistime % 60;
	if (ht->minute1 == 0x3FFFFFFF && ht->minute2 == 0x3FFFFFFF)
	{
		// dont add waiting period to get to the proper minute
	}

	//which hour is it
	thistime = (thistime - thisminute) / 60;
	thishour = thistime % 24;
	if (ht->hour == 0xFFFFFF)
	{
		// dont add waiting period to get to the proper hour
	}

	//which weekday is it
	thistime = (thistime - thishour) / 60;	//thistime is now = day since epoch
	if (ht->weekday == 0x7F) // every day
	{
		// dont add waiting period to get to the correct day
	}

#endif /* 0 */

/*
 *  return -1 on failure
 */
int compile_timer(HookTimer *timer, char *rest)
{
	char	backup[strlen(rest)+1];
	char	*sec,*min,*hour,*day;

	stringcpy(backup,rest);
	rest = backup;
#ifdef DEBUG
	debug("(compile_timer) rest = %s\n",nullstr(rest));
#endif /* DEBUG */

	sec  = chop(&rest);
	min  = chop(&rest);
	hour = chop(&rest);
	day  = chop(&rest);

	if (sub_compile_timer(59,&timer->second1,&timer->second2,sec) < 0)
		return -1;
	if (sub_compile_timer(59,&timer->minute1,&timer->minute2,min) < 0)
		return -1;
	if (sub_compile_timer(23,&timer->hour,NULL,hour) < 0)
		return -1;
	if (sub_compile_timer(6,&timer->weekday,NULL,day) < 0)
		return -1;
	return 0;
}

#endif /* SCRIPTING */

/*
 *  Periodic updates
 */

void update(SequenceTime *this)
{
	Server	*sp;
	Chan	*chan;
	char	*temp,*chantemp;
	int	tt,th;
	int	x,n;

	tt = now / 600;		/* current 10-minute period */
	th =  tt / 6;		/* current hour */

#ifdef DEBUG
	x = 0;
	if (tt != this->tenminute)
	{
		debug("(update) running: ten minute updates [%i]",tt);
		x++;
	}
	if (th != this->hour)
	{
		debug("%shour updates [%i]",(x) ? ", " : "(update) running: ",th);
		x++;
	}
	if (x)
		debug("\n");
#endif /* DEBUG */

	cx.short_tv &= ~TV_REJOIN;
	for(current=botlist;current;current=current->next)
	{
		if (current->reset || current->connect != CN_ONLINE)
			continue;

		if (current->rejoin)
		{
			if ((now - current->lastrejoin) > REJOIN_DELAY)
			{
				current->rejoin = FALSE;
				current->lastrejoin = now;
			}
			cx.short_tv |= TV_REJOIN;
		}

#ifdef NOTIFY
		send_ison();
#endif /* NOTIFY */

		for(chan=current->chanlist;chan;chan=chan->next)
		{
			if (!chan->active)
			{
				if (!current->rejoin && chan->rejoin)
				{
					/*
					 *  join_channel() will set current->rejoin to TRUE
					 */
					join_channel(chan->name,chan->key);
				}
			}

			if (tt != this->tenminute)
			{
				chan->last10 = chan->this10;
				chan->this10 = 0;
			}
			if (th != this->hour)
			{
				chan->last60 = chan->this60;
				chan->this60 = 0;
#ifdef STATS
				if ((temp = chan->setting[STR_STATS].str_var))
					stats_loghour(chan,temp,this->hour);
#endif /* STATS */
			}
#ifdef DYNAMODE
			if (chan->bot_is_op && chan->sync && chan->setting[STR_DYNLIMIT].str_var)
				check_dynamode(chan);
#endif /* DYNAMODE */
		}
		if ((now - current->lastreset) > RESETINTERVAL)
		{
			current->lastreset = now;
			if (stringcmp(getbotnick(current),getbotwantnick(current)))
				to_server("NICK %s\n",getbotwantnick(current));
			check_idlekick();
			if ((x = current->setting[INT_AAWAY].int_var) && current->away == FALSE)
			{
				if ((now - current->activity) > (x * 60))
				{
					temp = randstring(AWAYFILE);
					to_server(AWAYFORM,(temp && *temp) ? temp : "auto-away",time2away(now));
					current->away = TRUE;
				}
			}
		}
		/*
		 *  10 minute update.
		 */
		if (tt != this->tenminute)
		{
			/*
			 *  use `x' to count channels for the status line
			 */
			x = 0;
			for(chan=current->chanlist;chan;chan=chan->next)
			{
#ifdef BOTNET
				if (chan->bot_is_op == FALSE)
				{
					botnet_relay(NULL,"CO%i %s\n",current->guid,chan->name);
				}
#endif /* BOTNET */
				if ((n = chan->setting[INT_AUB].int_var))
				{
					channel_massunban(chan,MATCH_ALL,60*n);
				}
				x++;
			}
			temp = TEXT_NOTINSERVLIST;
			if ((sp = find_server(current->server)))
			{
				if (sp->maxontime > current->ontime)
					sp->maxontime = current->ontime;
				sprintf(globaldata,"%s:%i",(*sp->realname) ? sp->realname : sp->name,sp->port);
				temp = globaldata;
			}
			chantemp = (current->activechan) ? current->activechan->name : TEXT_NONE;
			send_spy(SPYSTR_STATUS,"C:%s AC:%i CS:%s",chantemp,x,temp);
		}
		/*
		 *  Hourly update.
		 */
		if (th != this->hour)
		{
			if (current->userlist && current->ul_save)
			{
				temp = current->setting[STR_USERFILE].str_var;
				if (!write_userlist(temp))
					send_spy(SPYSTR_STATUS,(temp) ? ERR_NOSAVE : ERR_NOUSERFILENAME,temp);
				else
					send_spy(SPYSTR_STATUS,TEXT_LISTSAVED,temp);
			}
#ifdef SEEN
			if (seenfile && !write_seenlist())
			{
				send_spy(SPYSTR_STATUS,TEXT_SEENNOSAVE,seenfile);
			}
#endif /* SEEN */
#ifdef NOTIFY
			if (current->notifylist)
				write_notifylog();
#endif /* NOTIFY */
		}
	}

#ifdef SESSION
	if (th != this->hour)
	{
		if (!write_session())
			send_global(SPYSTR_STATUS,"Session could not be saved to file %s",SESSIONFILE);
	}
#endif /* SESSION */

	this->tenminute = tt;
	this->hour = th;
}

/*
 *  Read data from server socket
 */
void process_server_input(void)
{
#ifdef WINGATE
	Server	*sp;
#endif /* WINGATE */
	char	linebuf[MSGLEN];
	char	*res;

	if (FD_ISSET(current->sock,&write_fds))
	{
		setbotnick(current,getbotwantnick(current));
#ifdef DEBUG
		debug("[PSI] {%i} connection established (%s) [ASYNC]\n",
			current->sock,getbotwantnick(current));
#endif /* DEBUG */
#ifdef WINGATE
		if ((current->vhost_type & VH_WINGATE_BOTH) == VH_WINGATE)
		{
			sp = find_server(current->server);
			if (!sp)
			{
				goto breaksock2;
			}
#ifdef DEBUG
			debug("[PSI] Connecting via WinGate proxy...\n");
#endif /* DEBUG */
			to_server("%s %i\n",sp->name,sp->port);
			/*
			 *  If we fail here, we'd better drop the WinGate
			 *  and retry the SAME server again
			 */
			if (current->sock == -1)
			{
				current->nextserver = sp->ident;
				current->vhost_type |= VH_WINGATE_FAIL;
			}
			current->connect = CN_WINGATEWAIT;
			current->conntry = now;
			current->heartbeat = 0;
			return;
		}
#endif /* WINGATE */
		/*
		 *  send NICK, USER and maybe PASS
		 */
		register_with_server();
		return;
	}
	if (FD_ISSET(current->sock,&read_fds))
	{
get_line:
		res = sockread(current->sock,current->sockdata,linebuf);
		if (res)
		{
#ifdef WINGATE
			if (current->connect == CN_WINGATEWAIT)
			{
				if (!matches("Connecting to host *Connected",linebuf))
				{
#ifdef DEBUG
					debug("[PSI] WinGate proxy active\n");
#endif /* DEBUG */
					register_with_server();
					return;
				}
				else
				{
					goto breaksock2;
				}
			}
#endif /* WINGATE */
			current->conntry = now;
			current->heartbeat = 0;
			parse_server_input(linebuf);
			goto get_line;
		}
		switch(errno)
		{
		case EINTR:
		case EAGAIN:
			break;
		default:
			goto breaksock;
		}
	}

	/* server has been quiet for too long */
	if (current->conntry + SERVERSILENCETIMEOUT <= now && current->heartbeat == 0)
	{
#ifdef DEBUG
		debug("[PSI] {%i} server has been quiet for too long (%is)...\n",current->sock,SERVERSILENCETIMEOUT);
#endif /* DEBUG */
		/* push something to the server to test the socket, this should break a zombie socket */
		to_server("PING :LT%lu\n",current->conntry);
		current->heartbeat = 1;
	}

	/* server has been quiet for WAY too long */
	if (current->conntry + (SERVERSILENCETIMEOUT*2) <= now)
	{
#ifdef DEBUG
		debug("[PSI] {%i} server has been quiet for WAY too long (%is), forcing reconnect...\n",current->sock,SERVERSILENCETIMEOUT*2);
#endif /* DEBUG */
		errno = 110; /* connection timed out */
		goto breaksock;
	}

	/* situation normal */
	return;

breaksock:
#ifdef DEBUG
	debug("[PSI] {%i} errno = %i; closing server socket\n",current->sock,errno);
#endif /* DEBUG */
breaksock2:
	*current->sockdata = 0;
	close(current->sock);
	current->sock = -1;
	current->connect = CN_NOSOCK;
	return;
}

/*
 *
 *  commands associated with core.c
 *
 */

void do_version(COMMAND_ARGS)
{
	to_user_q(from,"%s %s",BOTCLASS,VERSION);
}

void do_core(COMMAND_ARGS)
{
#ifdef HOSTINFO
        char    *h,hostname[256];
        struct utsname un;
#endif /* HOSTINFO */
	const char *extra;
	char	tmp[MSGLEN];	/* big buffers at the top */
	Server	*sp;
	Chan	*chan;
	User	*user;
	char *pt;
	int	i,u,su,bu;

	u = su = bu = 0;
	for(user=current->userlist;user;user=user->next)
	{
		u++;
		if (user->x.x.access == OWNERLEVEL)
			su++;
		if (user->x.x.access == BOTLEVEL)
			bu++;
	}

	i = stringcmp(getbotnick(current),getbotwantnick(current));
	if (i)
		table_buffer(TEXT_CURRNICKWANT,getbotnick(current),getbotwantnick(current),current->guid);
	else
		table_buffer(TEXT_CURRNICKHAS,getbotnick(current),current->guid);
	table_buffer(TEXT_USERLISTSTATS,u,su,EXTRA_CHAR(su),bu,EXTRA_CHAR(bu));

	pt = tmp;
	*pt = i = 0;
	for(chan=current->chanlist;chan;chan=chan->next)
	{
		if (*tmp && ((pt - tmp) + strlen(chan->name) > 57))
		{
			table_buffer((i) ? TEXT_MOREACTIVECHANS : TEXT_ACTIVECHANS,tmp);
			pt = tmp;
			i++;
		}
		if (chan->bot_is_op)
			*(pt++) = '@';
		if (chan == current->activechan)
		{
			*(pt++) = '\037';
			pt = stringcpy(pt,chan->name);
			*(pt++) = '\037';
		}
		else
		{
			pt = stringcpy(pt,chan->name);
		}
		if (chan->next)
			*(pt++) = ' ';
		*pt = 0;
	}
	table_buffer((i) ? TEXT_MOREACTIVECHANS : TEXT_ACTIVECHANS,(*tmp) ? tmp : TEXT_NONE);

	if (current->setting[STR_VIRTUAL].str_var)
	{
		if ((current->vhost_type & VH_IPALIAS_FAIL) == 0)
			extra = EMPTYSTR;
		else
			extra = TEXT_VHINACTIVE;
		table_buffer(TEXT_VIRTHOST,current->setting[STR_VIRTUAL].str_var,extra);
	}
#ifdef WINGATE
	if (current->setting[STR_WINGATE].str_var && current->setting[INT_WINGPORT].int_var)
	{
		if ((current->vhost_type & VH_WINGATE_FAIL) == 0)
			extra = EMPTYSTR;
		else
			extra = TEXT_VHINACTIVE;
		table_buffer(TEXT_VIRTHOSTWINGATE,current->setting[STR_WINGATE].str_var,
			current->setting[INT_WINGPORT].int_var,extra);
	}
#endif /* WINGATE */
	sp = find_server(current->server);
	if (sp)
		table_buffer(TEXT_CURRSERVER,
			(sp->realname[0]) ? sp->realname : sp->name,sp->port);
	else
		table_buffer(TEXT_CURRSERVERNOT);
	table_buffer(TEXT_SERVERONTIME,idle2str(current->ontime,FALSE));
	table_buffer(TEXT_BOTMODES,(*current->modes) ? current->modes : TEXT_NONE);
#ifdef HOSTINFO
	hostname[255] = 0;
	if (gethostname(hostname,250) < 0)
		h = "(hostname error)";
	else
		h = hostname;

	if (uname(&un) == 0)
		table_buffer(TEXT_HOSTINFO,h,un.sysname,un.release,un.machine);
#endif /* HOSTINFO */
	table_buffer(TEXT_CURRENTTIME,time2str(now));
	table_buffer(TEXT_BOTSTARTED,time2str(uptime));
	table_buffer(TEXT_BOTUPTIME,idle2str(uptime,FALSE));
	table_buffer(TEXT_BOTVERSION,VERSION,SRCDATE);
	table_buffer(TEXT_BOTFEATURES,__mx_opts);
#ifdef DEBUG
#ifdef __profiling__
	table_buffer("Debug\t%s%s%s, Compiled with Profiling",
#else
	table_buffer("Debug\t%s%s%s",
#endif
		(const char *[]){"Off","On, Output = "}[dodebug],
		(debugfile==NULL) ? ((dodebug==TRUE) ? "Stdout" : EMPTYSTR) : debugfile);
#endif /* DEBUG */
	table_send(from,2);
}

void do_die(COMMAND_ARGS)
{
	/*
	 *  on_msg checks GAXS
	 */
	char	*reason;

#ifdef SESSION
	write_session();
#endif /* SESSION */

	if (!rest || !*rest)
	{
		if ((reason = randstring(SIGNOFFSFILE)) == NULL)
			reason = "I'll get you for this!!!";
	}
	else
	{
		reason = rest;
		if (!uptime)
		{
			to_file(1,"init: %s\n",rest);
			_exit(0);
		}
	}

	set_mallocdoer(do_die);
	current->signoff = stringdup(reason);
	set_mallocdoer(do_die);
	current->from = stringdup(from);

	current->connect = CN_BOTDIE;
}

void do_shutdown(COMMAND_ARGS)
{
	send_global(SPYSTR_STATUS,TEXT_SHUTDOWNBY,nickcpy(NULL,from));

#ifdef SESSION
	write_session();
#endif /* SESSION */

	kill_all_bots(NULL);
	/* NOT REACHED */
}

void do_server_noargs(const char *from)
{
	Server	*sp;
	const char *str_currentserver,*str_ago;
	char	maxontimebuf[36],*maxontime,*lastconnect;

	if (partyline_only_command(from))
		return;
	table_buffer(str_underline("server") "\t" str_underline("last connect") "\t"
		str_underline("maxontime") "\t" str_underline("group"));

	for(sp=serverlist;sp;sp=sp->next)
	{
		str_ago = str_currentserver = EMPTYSTR;
		if (sp->ident == current->server)
		{
			if (sp->maxontime == -1 || sp->maxontime > current->ontime)
				sp->maxontime = current->ontime;
			str_currentserver = TEXT_CURRENT;
		}

		if (sp->maxontime == -1)
			maxontime = TEXT_NEVER;
		else
		{
			maxontime = maxontimebuf;
			stringcpy(maxontimebuf,idle2str(sp->maxontime,FALSE));
		}

		if (sp->lastconnect)
		{
			str_ago = TEXT_AGO;
			lastconnect = idle2str(sp->lastconnect,FALSE);
		}
		else
		switch(sp->err)
		{
		case SP_NOAUTH:
			lastconnect = TEXT_SP_NOAUTH;
			break;
		case SP_KLINED:
			lastconnect = TEXT_SP_KLINED;
			break;
		case SP_FULLCLASS:
			lastconnect = TEXT_SP_FULLCLASS;
			break;
		case SP_TIMEOUT:
			lastconnect = TEXT_SP_TIMEOUT;
			break;
		case SP_ERRCONN:
			lastconnect = TEXT_SP_ERRCONN;
			break;
		case SP_DIFFPORT:
			lastconnect = TEXT_SP_DIFFPORT;
			break;
		case SP_NO_DNS:
			lastconnect = TEXT_SP_NO_DNS;
			break;
		default:
			lastconnect = TEXT_NEVER;
		}
		table_buffer("%s:%i%s\t%s%s\t%s\t%s",(*sp->realname) ? sp->realname : sp->name,sp->port,
			str_currentserver,lastconnect,str_ago,maxontime,sp->group);
	}
	table_send(from,2);
}

void do_server(COMMAND_ARGS)
{
	Server	*sp,*dp,**spp;
	char	*server,*aport,*temp;
	const char *pass,*group;
	char	add_or_sub,*quitmsg = TEXT_TRYNEWSERVER;
	int	n,iport;

	if (CurrentCmd->name == C_NEXTSERVER)
	{
		to_user(from,FMT_PLAIN,TEXT_SWITCHSERVER);
		goto do_server_jump;
	}


	add_or_sub = *rest;
	if (*rest == '-' || *rest == '+')
		rest++;

	if (*rest == 0)
		goto do_server_einval;

	server = chop(&rest);
	if ((cx.chop_end - server) >= MAXHOSTLEN)
	{
		to_user(from,TEXT_NAMETOOLONG);
		return;
	}

	aport = "6667";
	group = DEFAULTSTR;
	pass  = EMPTYSTR;
	do
	{
		if (*rest == COMMENT_CHAR)
			break;
		temp = chop(&rest);
		if (temp == NULL)
			break;

		if (*temp >= '1' && *temp <= '9')
			aport = temp;
		else
		if (*temp == '@' && ((cx.chop_end - temp) <= SERVERGROUPLEN))
			group = temp + 1;
		else
		if (*temp == '"')
		{
			char *c;

			temp++;
			c = stringchr(temp,'"');
			if (c && ((c - temp) <= PASSLEN))
				pass = temp, *c = 0;
		}
		else
			goto do_server_einval;
	}
	while(*rest);

	iport = asc2int(aport);

	if (errno || iport < 1 || iport > 65534)
	{
do_server_einval:
		usage(from);
		return;
	}

	if (add_or_sub == '-')
	{
		if (!serverlist)
		{
			to_user(from,TEXT_EMPTYSERVLIST);
			return;
		}
		n = 0;
		dp = NULL;
		for(sp=serverlist;sp;sp=sp->next)
		{
			if ((!stringcasecmp(server,sp->name)) || (!stringcasecmp(server,sp->realname)))
			{
				if (!iport || (iport && sp->port == iport))
				{
					dp = sp;
					n++;
				}
			}
		}
		switch(n)
		{
		case 0:
			to_user(from,(iport) ? TEXT_NOSERVMATCHP : TEXT_NOSERVMATCH,server,iport);
			break;
		case 1:
			to_user(from,TEXT_SERVERDELETED,server,dp->port);
			for(spp=&serverlist;*spp;spp=&(*spp)->next)
			{
				if (*spp == dp)
				{
					*spp = dp->next;
					Free((void*)&dp);
					break;
				}
			}
			break;
		default:
			to_user(from,TEXT_MANYSERVMATCH,server);
		}
		return;
	}
	sp = add_server(server,iport,pass,group); /* add_server has no failure mode */
	if (add_or_sub == '+' || from == CoreUser.name)
		return;

	current->nextserver = sp->ident;
	to_user(from,"Trying new server: %s on port %i",server,iport);
do_server_jump:
	switch(current->connect)
	{
	case CN_CONNECTED:
	case CN_ONLINE:
		to_server("QUIT :%s\n",quitmsg);
		killsock(current->sock);
		break;
	default:
		if (current->sock != -1)
			close(current->sock);
	}
	current->sock = -1;
}

void do_cserv(COMMAND_ARGS)
{
	Server	*sp;

	if ((sp = find_server(current->server)))
		to_user(from,TEXT_CSERV,(*sp->realname) ? sp->realname : sp->name,sp->port);
	else
		to_user(from,TEXT_CSERVNOT);
}

void do_away(COMMAND_ARGS)
{
	if (!rest || !*rest)
	{
		to_server("AWAY\n");
		to_user(from,TEXT_NOLONGERAWAY);
		current->away = FALSE;
		current->activity = now;
		return;
	}
	to_server(AWAYFORM,rest,time2away(now));
	to_user(from,TEXT_NOWSETAWAY);
	current->away = TRUE;
}

void do_do(COMMAND_ARGS)
{
	/*
	 *  on_msg checks: CARGS + GAXS
	 */
	to_server(FMT_PLAINLINE,rest);
}

void do_nick(COMMAND_ARGS)
{
	/*
	 *  on_msg checks: GAXS + CARGS
	 */
	Mech	*backup;
	char	*nick;
	int	guid;

	if (!rest || !*rest)
	{
		usage(from);	/* usage for CurrentCmd->name */
		return;
	}
	nick = chop(&rest);
	guid = asc2int(nick);
	backup = current;
	if (errno == 0) /* we got a number */
	{
		nick = chop(&rest);
		for(current=botlist;current;current=current->next)
		{
			if (current->guid == guid)
				break;
			if (current->guid == 0)
				break;
		}
	}
	if (is_nick(nick) == FALSE)
	{
		current = backup;
		to_user(from,ERR_NICK,nick);
		return;
	}
	if (!current)
	{
#ifdef DEBUG
		debug("(do_nick) Adding new bot: guid = %i; nick = %s\n",guid,nick);
#endif /* DEBUG */
		current = add_bot(guid,nick);
		if (!sigmaster)
			sigmaster = guid;
		if (from == CoreUser.name)
			return;
	}
	else
	{
		if (current->guid == 0)
		{
			setbotnick(current,nick);
			current->guid = guid; /* guid might be undefined? */
		}
		setbotwantnick(current,nick);
		to_server("NICK %s\n",getbotwantnick(current));
	}
	current = backup;
}

void do_time(COMMAND_ARGS)
{
	to_user_q(from,"Current time: %s",time2away(now));
}

void do_upontime(COMMAND_ARGS)
{
	time_t	temp;

	temp = ((CurrentCmd->name == C_UPTIME) ? uptime : current->ontime);
	to_user_q(from,CurrentCmd->cmdarg,idle2str(temp,FALSE));
}

void do_msg(COMMAND_ARGS)
{
	/*
	 *  on_msg checks: CARGS
	 */
	Client	*client;

	to = chop(&rest);
	if (ischannel(to))
	{
		/*
		 *  need to check channel access
		 */
		if (get_authaccess(from,to) < cmdaccess)
			return;
	}

	if (*to == '=')
	{
		to++;
		if ((client = find_client(to)) == NULL)
		{
			to_user(from,"I have no DCC connection to %s",to);
			return;
		}
	}
	CoreChan.name = to;
	CurrentChan = (Chan*)&CoreChan;
	to_user_q(to,FMT_PLAIN,rest);
	to_user(from,"Message sent to %s",to);
}
