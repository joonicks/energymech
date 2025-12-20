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
#ifndef H_H
#define H_H 1

#if !defined(__STRICT_ANSI__)
#define __INLINE__		inline
#else
#define __INLINE__
#endif

#if defined(__GNUC__)
#define __notused__ __attribute__((unused))
#else
#define __notused__
#endif

#define ischannel(x)		(*x == '#')

#define nullstr(x)		((x) ? x : NULLSTR)
#define nullbuf(x)		(x && *x) ? x : NULLSTR

#define chkhigh(x)		if (x > cx.hisock) { cx.hisock = x; }

#define COMMAND_ARGS		char *from, const char *to, char *rest, const int cmdaccess

#define STREND(x)		stringchr(x,0)

/*
 *  some default code for socket flags
 */
#ifdef ASSUME_SOCKOPTS

#define unset_closeonexec(x)	fcntl(x,F_SETFD,0);

#else /* ASSUME_SOCKOPTS */

#define	unset_closeonexec(x)	fcntl(x,F_SETFD,(~FD_CLOEXEC) & fcntl(x,F_GETFD));

#endif /* ASSUME_SOCKOPTS */

/*
 *  Dont try this at home kids...
 */
#ifdef __ELF__
#define __sect(x)		__section__ (x)
#else
#define __sect(x)		/* nothing */
#endif

#ifdef __GNUC__
#define __page(x)		__attribute__ (( __sect(x) ))
#define __attr(x,y)		__attribute__ (( __sect(x), y ))
#else
#define __page(x)		/* nothing */
#define __attr(x,y)		/* nothing */
#endif

#define RARE_SEG	".text.a"
#define INIT_SEG	".text.b"
#define CFG1_SEG	".text.c"
#define CORE_SEG	".text.d"
#define DBUG_SEG	".text.e"
#define CMD1_SEG	".text.f"

#ifdef DEBUG

#define set_mallocdoer(x)	mallocdoer = x;

#else /* not DEBUG */

#define set_mallocdoer(x)

#endif /* DEBUG */

/* alias.c */

void alias_reformat(char *, const char *, const char *)			__page(CORE_SEG);
void do_alias(COMMAND_ARGS)						__page(CMD1_SEG);
void do_unalias(COMMAND_ARGS)						__page(CMD1_SEG);

/* auth.c */

char *cipher(char *)							__page(CMD1_SEG);
char *makepass(char *)							__page(CMD1_SEG);
int passmatch(char *, char *)						__page(CMD1_SEG);
void delete_auth(char *)						__page(CMD1_SEG);
void remove_auth(Auth *)						__page(CMD1_SEG);
void change_authnick(char *, char *)					__page(CORE_SEG);
void aucheck(User *)							__page(CORE_SEG);
User *get_authuser(const char *, const char *)				__page(CORE_SEG);
int get_authaccess(const char *, const char *)				__page(CORE_SEG);
int make_auth(const char *, const User *)				__page(CMD1_SEG);
void do_auth_noargs(const char *)					__page(CMD1_SEG);
void do_auth(COMMAND_ARGS)						__page(CMD1_SEG);

/* bounce.c */

void bounce_parse(ircLink *, char *);
void bounce_cleanup(void);
void new_port_bounce(const struct Setting *);
void select_bounce(void)						__page(CORE_SEG);
void process_bounce(void)						__page(CORE_SEG);

/* calc.c */

void do_calc(COMMAND_ARGS)						__page(CMD1_SEG);
void do_convert(COMMAND_ARGS)						__page(CMD1_SEG);

/* channel.c */

void check_idlekick(void);
Chan *find_channel(const char *, int)					__page(CORE_SEG);
Chan *find_channel_ac(const char *)					__page(CORE_SEG);
Chan *find_channel_ny(const char *)					__page(CORE_SEG);
void remove_chan(Chan *)						__page(CMD1_SEG);
void join_channel(char *, char *)					__page(CFG1_SEG);
void reverse_topic(Chan *, char *, char *)				__page(CORE_SEG);
void cycle_channel(Chan *)						__page(CMD1_SEG);
int reverse_mode(char *, Chan *, int, int)				__page(CORE_SEG);
void chan_modestr(Chan *, char *)					__page(CORE_SEG);
char *get_nuh(const ChanUser *)						__page(CORE_SEG);
char *find_nuh(char *)							__page(CORE_SEG);
char *nick2uh(char *, char *)						__page(CORE_SEG);
Ban *make_ban(Ban **, char *, char *, time_t)				__page(CMD1_SEG);
void delete_ban(Chan *, char *)						__page(CMD1_SEG);
void delete_modemask(Chan *, char *, int)				__page(CMD1_SEG);
void channel_massmode(const Chan *, char *, int, char, char)		__page(CORE_SEG);
void channel_massunban(Chan *, char *, time_t)				__page(CORE_SEG);
ChanUser *find_chanuser(Chan *, const char *)				__page(CORE_SEG);
ChanUser *find_chanbot(Chan *, const char *)				__page(CORE_SEG);
void remove_chanuser(Chan *, const char *)				__page(CORE_SEG);
void make_chanuser(char *, char *)					__page(CORE_SEG);
void purge_chanusers(Chan *)						__page(CMD1_SEG);
void do_join(COMMAND_ARGS)						__page(CMD1_SEG);
void do_part(COMMAND_ARGS)						__page(CMD1_SEG);
void do_cycle(COMMAND_ARGS)						__page(CMD1_SEG);
void do_forget(COMMAND_ARGS)						__page(CMD1_SEG);
void do_channels(COMMAND_ARGS)						__page(CMD1_SEG);
void do_wall(COMMAND_ARGS)						__page(CMD1_SEG);
void do_mode(COMMAND_ARGS)						__page(CMD1_SEG);
void do_names(COMMAND_ARGS)						__page(CMD1_SEG);
void do_cchan(COMMAND_ARGS)						__page(CMD1_SEG);
void do_invite(COMMAND_ARGS)						__page(CMD1_SEG);
void do_sayme(COMMAND_ARGS)						__page(CMD1_SEG);
void do_who(COMMAND_ARGS)						__page(CMD1_SEG);
void do_topic(COMMAND_ARGS)						__page(CMD1_SEG);
void do_showidle(COMMAND_ARGS)						__page(CMD1_SEG);
void do_idle(COMMAND_ARGS)						__page(CMD1_SEG);

/* core.c */

#define getbotnicklen(z)	(z->nick.opt)
#define getbotnick(z)		((z->nick.opt < 16) ? z->nick.x.string : z->nick.x.ptr)
#define getbotwantnick(z)	((z->wantnick.opt < 16) ? z->wantnick.x.string : z->wantnick.x.ptr)
#define getbotuserhost(z)	((z->userhost.opt == 0) ? UNKNOWNATUNKNOWN : ((z->userhost.opt < 64) ? z->userhost.x.string : z->userhost.x.ptr))

#define setbotnick(z,y)		set_mix16(&z->nick,y)
#define setbotwantnick(z,y)	set_mix16(&z->wantnick,y)
#define setbotuserhost(z,y)	set_mix64(&z->userhost,y)

void set_mix16(Mix16 *, const char *)					__page(CORE_SEG);
void set_mix64(Mix64 *, const char *)					__page(CORE_SEG);

int conf_callback(char *line)						__page(CFG1_SEG);
void readcfgfile(void)							__page(INIT_SEG);
int write_session(void)							__page(CORE_SEG);
Mech *add_bot(int guid, char *nick)					__page(CFG1_SEG);
void signoff(char *from, char *reason)					__page(RARE_SEG);
void kill_all_bots(char *reason)					__attr(RARE_SEG, __noreturn__);;
Server *add_server(const char *host, const int port, const char *pass, const char *group) __page(CFG1_SEG);
Server *find_server(int id)						__page(CORE_SEG);
int try_server(Server *sp, char *hostname)				__page(CORE_SEG);
void connect_to_server(void)						__page(CORE_SEG);
void register_with_server(void)						__page(CORE_SEG);
int sub_compile_timer(int, uint32_t *, uint32_t *, char *)		__page(CFG1_SEG);
int compile_timer(HookTimer *, char *)					__page(CFG1_SEG);
void update(SequenceTime *this)						__page(CORE_SEG);
void process_server_input(void)						__page(CORE_SEG);
void do_version(COMMAND_ARGS)						__page(CMD1_SEG);
void do_core(COMMAND_ARGS)						__page(CMD1_SEG);
void do_die(COMMAND_ARGS)						__page(RARE_SEG);
void do_shutdown(COMMAND_ARGS)						__page(RARE_SEG);
void do_server_noargs(const char *from)					__page(CMD1_SEG);
void do_server(COMMAND_ARGS)						__page(CMD1_SEG);
void do_cserv(COMMAND_ARGS)						__page(CMD1_SEG);
void do_away(COMMAND_ARGS)						__page(CMD1_SEG);
void do_do(COMMAND_ARGS)						__page(CMD1_SEG);
void do_nick(COMMAND_ARGS)						__page(CMD1_SEG);
void do_time(COMMAND_ARGS)						__page(CMD1_SEG);
void do_upontime(COMMAND_ARGS)						__page(CMD1_SEG);
void do_msg(COMMAND_ARGS)						__page(CMD1_SEG);

/* ctcp.c */

Client *find_client(const char *nick);
void delete_client(Client *client);
int dcc_sendfile(char *target, char *filename);
void dcc_pushfile(Client *client, off_t where);
int dcc_freeslots(int uaccess);
void parse_dcc(Client *client);
void process_dcc(void)							__page(CORE_SEG);
void ctcp_dcc(char *from, char *to, char *rest)				__page(CORE_SEG);
void ctcp_finger(char *from, char *to, char *rest)			__page(CORE_SEG);
void ctcp_ping(char *from, char *to, char *rest)			__page(CORE_SEG);
void ctcp_version(char *from, char *to, char *rest)			__page(CORE_SEG);
void on_ctcp(char *from, char *to, char *rest)				__page(CORE_SEG);
void do_ping_ctcp(COMMAND_ARGS)						__page(CMD1_SEG);
void do_send(COMMAND_ARGS)						__page(CMD1_SEG);

/* debug.c */

void strflags(char *dst, const DEFstruct *flagsstruct, int flags)	__page(DBUG_SEG);
const char *strdef(const DEFstruct *dtab, int num)			__page(CMD1_SEG);
const char *funcdef(const DEFstruct *dtab, void *func)			__page(CMD1_SEG);
void memreset(void)							__page(CMD1_SEG);
void memtouch(const void *addr)						__page(CMD1_SEG);
const char *proc_lookup(void *addr, int size)				__page(CMD1_SEG);
char *atime(time_t when)						__page(CMD1_SEG);
void debug_server(Server *sp, char *pad)				__page(CMD1_SEG);
void debug_settings(UniVar *setting, int type)				__page(CMD1_SEG);
void debug_memory(void)							__page(CMD1_SEG);
void debug_botinfo(BotInfo *binfo)					__page(CMD1_SEG);
void debug_botnet(void)							__page(CMD1_SEG);
void debug_core(void)							__page(CMD1_SEG);
void debug_rawdns(void)							__page(CMD1_SEG);
char *uint32tobin(int limit, uint32_t x)				__page(CMD1_SEG);
void debug_scripthook(void)						__page(CMD1_SEG);
void run_debug(void)							__page(CMD1_SEG);
int wrap_debug(void)							__page(CMD1_SEG);
void do_crash(COMMAND_ARGS)						__page(RARE_SEG);
void do_debug(COMMAND_ARGS)						__page(CMD1_SEG);
void do_inject(COMMAND_ARGS)						__page(CMD1_SEG);
void debug(char *format, ...)						__page(DBUG_SEG);

/* dns.c */

void init_rawdns(void)							__page(CFG1_SEG);
struct in_addr dnsroot_lookup(const char *hostname)			__page(CORE_SEG);
const char *get_dns_token(const char *src, const char *packet, char *dst, int sz) __page(CORE_SEG);
int make_query(char *packet, const char *hostname)			__page(CORE_SEG);
struct in_addr *get_stored_ip(const char *ipdata)			__page(CORE_SEG);
void dns_hook(char *host, char * resolved)				__page(CORE_SEG);
void parse_query(int psz, dnsQuery *query)				__page(CORE_SEG);
void rawdns(const char *hostname)					__page(CORE_SEG);
void select_rawdns(void)						__page(CORE_SEG);
void process_rawdns(void)						__page(CORE_SEG);
char *poll_rawdns(char *hostname)					__page(CORE_SEG);
int read_dnsroot(char *line)						__page(CFG1_SEG);
uint32_t rawdns_get_ip(const char *host)				__page(CORE_SEG);
void do_dnsroot(COMMAND_ARGS)						__page(CMD1_SEG);
void do_dnsserver(COMMAND_ARGS)						__page(CMD1_SEG);
void do_dns(COMMAND_ARGS)						__page(CMD1_SEG);

/* dynamode.c */

/* function.c */

void *Calloc(int)							__page(CORE_SEG);
void Free(char **)							__page(CORE_SEG);
Strp *make_strp(Strp **, const char *)					__page(CORE_SEG);
Strp *append_strp(Strp **, const char *)				__page(CORE_SEG);
Strp *prepend_strp(Strp **, const char *)				__page(CORE_SEG);
void purge_linklist(void **)						__page(CORE_SEG);
void dupe_strp(Strp *, Strp **)						__page(CORE_SEG);
const int StrlenX(const char *, ...)					__page(CORE_SEG);
const int Strlen2(const char *, const char *)				__page(CORE_SEG);
char *getuh(char *)							__page(CORE_SEG);
char *get_token(char **, const char *)					__page(CORE_SEG);
char *maketimestr(time_t, int)						__page(CORE_SEG);
char *idle2str(time_t, int)						__page(CORE_SEG);
const char *get_channel(const char *, char **)				__page(CORE_SEG);
const char *get_channel2(const char *, char **)				__page(CORE_SEG);
char *cluster(char *)							__page(CORE_SEG);
char *format_uh(char *, int)						__page(CORE_SEG);
int is_nick(const char *)						__page(CORE_SEG);
int asc2int(const char *)						__page(CORE_SEG);
int get_number(const char *)						__page(CORE_SEG);
void parse_range(char **, int *)					__page(CORE_SEG);
void fix_config_line(char *)						__page(CFG1_SEG);
int matches(const char *, const char *)					__page(CORE_SEG);
int num_matches(const char *, const char *)				__page(CORE_SEG);
int is_safepath(const char *, int)					__page(CORE_SEG);

/* greet.c */

void greet(void)							__page(CORE_SEG);
void do_greet(COMMAND_ARGS)						__page(CMD1_SEG);

/* help.c */

void print_help(char *from, char *line, int len)			__page(CMD1_SEG);
int do_help_callback(char *line)					__page(CMD1_SEG);
void usage_command(char *to, const char *arg)				__page(CMD1_SEG);
void usage(char *to)							__page(CMD1_SEG);
void do_help(COMMAND_ARGS)						__page(CMD1_SEG);
void do_usage(COMMAND_ARGS)						__page(CMD1_SEG);

/* hostinfo.c */

int monitor_fs(const char *);
void select_monitor()							__page(CORE_SEG);
void process_monitor()							__page(CORE_SEG);
int parse_proc_status(char *line)					__page(CMD1_SEG);
int parse_proc_cpuinfo(char *line)					__page(CMD1_SEG);
void do_sysinfo(COMMAND_ARGS)						__page(CMD1_SEG);
void do_meminfo(COMMAND_ARGS)						__page(CMD1_SEG);
void do_cpuinfo(COMMAND_ARGS)						__page(CMD1_SEG);
void do_filemon(COMMAND_ARGS)						__page(CMD1_SEG);

/* io.c */

uint32_t get_ip(const char *)						__page(CORE_SEG);
void SockFlags(int)							__page(CORE_SEG);
int SockOpts(void)							__page(CORE_SEG);
int SockListener(int)							__page(CORE_SEG);
int SockConnect(char *, int, int)					__page(CORE_SEG);
int SockAccept(int)							__page(CORE_SEG);
int to_file(const int sock, const char *format, ...)			__page(CORE_SEG);
void to_server(const char *format, ...)					__page(CORE_SEG);
void to_user_q(const char *, const char *, ...)				__page(CMD1_SEG);
void to_user(const char *, const char *, ...)				__page(CORE_SEG);
void table_buffer(const char *, ...)					__page(CMD1_SEG);
void table_send(const char *, const int)				__page(CMD1_SEG);
char *sockread(int, char *, char *)					__page(CORE_SEG);
void readline(int, int (*)(char *))					__page(CMD1_SEG);
void remove_ks(KillSock *)						__page(CORE_SEG);
int killsock(int)							__page(CORE_SEG);
void do_clearqueue(COMMAND_ARGS)					__page(CMD1_SEG);

/* irc.c */

void make_ireq(int, const char *, const char *)				__page(CMD1_SEG);
void send_pa(int type, const char *nick, const char *format, ...);
void do_irclusers(COMMAND_ARGS)						__page(CMD1_SEG);
void do_ircstats(COMMAND_ARGS)						__page(CMD1_SEG);
void do_ircwhois(COMMAND_ARGS)						__page(CMD1_SEG);

/* kicksay.c */

KickSay *find_kicksay(char *text, char *channel);
void check_kicksay(Chan *chan, ChanUser *doer, char *text);
void remove_kicksay(KickSay *kick);
void purge_kicklist(void);
void do_kicksay(COMMAND_ARGS)						__page(CMD1_SEG);
void do_rkicksay(COMMAND_ARGS)						__page(CMD1_SEG);

/* string.c */

char *chop(char **)							__page(CORE_SEG);
void unchop(char *, const char *)					__page(CORE_SEG);
int stringcasecmp(const char *, const char *)				__page(CORE_SEG);
int stringcmp(const char *, const char *)				__page(CORE_SEG);
int nickcmp(const char *, const char *)					__page(CORE_SEG);
char *nickcpy(char *, const char *)					__page(CORE_SEG);
void stringcpy_n(char *, const char *, int)				__page(CORE_SEG);
char *stringcpy(char *, const char *)					__page(CORE_SEG);
char *stringchr(const char *, int)					__page(CORE_SEG);
char *stringdup(const char *)						__page(CORE_SEG);
char *stringcat(char *, const char *)					__page(CORE_SEG);
char *tolowercat(char *, const char *)					__page(CORE_SEG);

/* main.c */

void mech_exec(void)							__attr(RARE_SEG, __noreturn__);
int randstring_count(char *line)					__page(CMD1_SEG);
int randstring_getline(char *line)					__page(CMD1_SEG);
char *randstring(const char *)						__page(CORE_SEG);
int sig_hup_callback(char *)						__page(RARE_SEG);		/* rare */
void do_sighup(void)							__page(CMD1_SEG);
void sig_hup(int)							__page(RARE_SEG);		/* rare */
void sig_child(int)							__page(RARE_SEG);		/* rare */
void sig_alrm(int)							__page(RARE_SEG);		/* rare */
void sig_pipe(int)							__page(CORE_SEG);
void do_sigusr1(void)							__page(CMD1_SEG);
void sig_usr1(int)							__page(CMD1_SEG);
void sig_usr2(int)							__page(DBUG_SEG);		/* DEBUG */
void sig_suicide()							__attr(RARE_SEG, __noreturn__);	/* rare */
void do_sigint(void)							__attr(RARE_SEG, __noreturn__);	/* rare */
void sig_int(int)							__page(RARE_SEG);		/* rare */
void sig_ill(int)							__page(RARE_SEG);
void sig_abrt(int)							__page(RARE_SEG);
void sig_bus(int)							__attr(RARE_SEG, __noreturn__);
#if defined(__linux__) && defined(__x86_64__) && defined(DEBUG) && !defined(__STRICT_ANSI__)
void sig_segv(int, siginfo_t *, void *)					__attr(RARE_SEG, __noreturn__);
#else
void sig_segv(int)							__attr(RARE_SEG, __noreturn__);
#endif
void sig_term(int)							__attr(RARE_SEG, __noreturn__);	/* rare */
int main(int argc, char **argv, char **envp)				__page(INIT_SEG);
void parse_commandline(int argc, char **argv, char **envp)		__page(INIT_SEG);
void mainloop(void)							__attr(CORE_SEG, __noreturn__);

/* net.c */

Mech *get_netbot(void)							__page(CORE_SEG);
void reset_linkable(int guid)						__page(CORE_SEG);
void botnet_deaduplink(BotNet *bn)					__page(CORE_SEG);
NetCfg *find_netcfg(int guid)						__page(CORE_SEG);
BotInfo *make_botinfo(int guid, int hops, char *nuh, char *server, char *version) __page(CORE_SEG);
void botnet_relay(BotNet *source, char *format, ...)			__page(CORE_SEG);
void botnet_refreshbotinfo(void)					__page(CORE_SEG);
void botnet_binfo_relay(BotNet *source, BotInfo *binfo);
void botnet_binfo_tofile(int sock, BotInfo *binfo);
void botnet_dumplinklist(BotNet *bn);
int connect_to_bot(NetCfg *cfg)						__page(CORE_SEG);
void check_botjoin(Chan *chan, ChanUser *cu);
void check_botinfo(BotInfo *binfo, const char *channel);
void basicAuth(BotNet *bn, char *rest);
void basicAuthOK(BotNet *bn, char *rest);
void basicBanner(BotNet *bn, char *rest);
void basicLink(BotNet *bn, char *version);
void basicQuit(BotNet *bn, char *rest);
void netchanNeedop(BotNet *source, char *rest);
void netchanSuppress(BotNet *, char *)					__page(CORE_SEG);
void partyAuth(BotNet *bn, char *rest);
int commandlocal(int dg, int sg, char *from, char *command);
void partyCommand(BotNet *bn, char *rest);
void partyMessage(BotNet *bn, char *rest);
void ushareUser(BotNet *bn, char *rest);
void ushareTick(BotNet *bn, char *rest);
void ushareDelete(BotNet *bn, char *rest);
void parse_botnet(BotNet *bn, char *rest);
void botnet_newsock(void)						__page(CORE_SEG);
void select_botnet(void)						__page(CORE_SEG);
void process_botnet(void)						__page(CORE_SEG);
void do_link_noargs(const char *)					__page(CMD1_SEG);
void do_link(COMMAND_ARGS)						__page(CMD1_SEG);
void do_cmd(COMMAND_ARGS)						__page(CMD1_SEG);

/* note.c */

int catch_note(char *from, char *to, char *rest)			__page(CMD1_SEG);
void do_note(COMMAND_ARGS)						__page(CMD1_SEG);
void do_read(COMMAND_ARGS)						__page(CMD1_SEG);

/* notify.c */

void purge_notify(void);
int mask_check(Notify *nf, char *userhost);
void send_ison(void);
void catch_ison(char *rest);
void catch_whois(char *nick, char *userhost, char *realname);
int notifylog_callback(char *rest);
void read_notifylog(void);
void write_notifylog(void);
int notify_callback(char *rest);
int read_notify(char *filename);
void nfshow_brief(Notify *nf);
void nfshow_full(Notify *nf);
void sub_notifynick(char *from, char *rest);
void do_notify(COMMAND_ARGS)						__page(CMD1_SEG);

/* ons.c */

uint32_t makecrc(const char *)						__page(CORE_SEG);
void send_suppress(const char *, const char *)				__page(CORE_SEG);
void on_kick(char *from, char *rest)					__page(CORE_SEG);
void on_join(Chan *chan, char *from)					__page(CORE_SEG);
void on_nick(char *from, char *newnick)					__page(CORE_SEG);
int mkhash(const char *)						__page(CORE_SEG);
void on_msg(char *from, char *to, char *rest)				__page(CORE_SEG);
void on_mode(char *from, char *channel, char *rest)			__page(CORE_SEG);
void common_public(Chan *chan, char *from, char *spyformat, char *rest) __page(CORE_SEG);
void on_action(char *from, char *to, char *rest)			__page(CORE_SEG);
int access_needed(char *name)						__page(CORE_SEG);
void do_chaccess(COMMAND_ARGS)						__page(CMD1_SEG);
void do_last(COMMAND_ARGS)						__page(CMD1_SEG);

/* parse.c */

void parse_error(char *from, char *rest)				__page(CORE_SEG);
void parse_invite(char *from, char *rest)				__page(CMD1_SEG);
void parse_join(char *from, char *rest)					__page(CORE_SEG);
void parse_mode(char *from, char *rest)					__page(CORE_SEG);
void parse_notice(char *from, char *rest)				__page(CORE_SEG);
void parse_part(char *from, char *rest)					__page(CORE_SEG);
void parse_ping(char *from, char *rest)					__page(CORE_SEG);
void parse_pong(char *from, char *rest)					__page(CORE_SEG);
void parse_privmsg(char *from, char *rest)				__page(CORE_SEG);
void parse_quit(char *from, char *rest)					__page(CORE_SEG);
void parse_topic(char *from, char *rest)				__page(CORE_SEG);
void parse_wallops(char *from, char *rest)				__page(CORE_SEG);
void parse_213(char *from, char *rest)					__page(CORE_SEG);
void parse_219(char *from, char *rest)					__page(CORE_SEG);
void parse_251(char *from, char *rest)					__page(CORE_SEG);
void parse_252(char *from, char *rest)					__page(CORE_SEG);
void parse_253(char *from, char *rest)					__page(CORE_SEG);
void parse_254(char *from, char *rest)					__page(CORE_SEG);
void parse_255(char *from, char *rest)					__page(CORE_SEG);
void parse_301(char *from, char *rest)					__page(CORE_SEG);
void parse_303(char *from, char *rest)					__page(CORE_SEG);
void parse_311(char *from, char *rest)					__page(CORE_SEG);
void parse_312(char *from, char *rest)					__page(CORE_SEG);
void parse_313(char *from, char *rest)					__page(CORE_SEG);
void parse_315(char *from, char *rest)					__page(CORE_SEG);
void parse_317(char *from, char *rest)					__page(CORE_SEG);
void parse_318(char *from, char *rest)					__page(CORE_SEG);
void parse_319(char *from, char *rest)					__page(CORE_SEG);
void parse_324(char *from, char *rest)					__page(CORE_SEG);
void parse_352(char *from, char *rest)					__page(CORE_SEG);
void parse_367(char *from, char *rest)					__page(CORE_SEG);
void parse_376(char *from, char *rest)					__page(CORE_SEG);
void parse_401(char *from, char *rest)					__page(CORE_SEG);
void parse_433(char *from, char *rest)					__page(CORE_SEG);
void parse_451(char *from, char *rest)					__page(CORE_SEG);
void parse_471(char *from, char *rest)					__page(CORE_SEG);
void parse_473(char *from, char *rest)					__page(CORE_SEG);
void parse_346(char *from, char *rest)					__page(CORE_SEG);
void parse_348(char *from, char *rest)					__page(CORE_SEG);
void parse_368(char *from, char *rest)					__page(CORE_SEG);
void parse_005(char *from, char *rest)					__page(CORE_SEG);
void parse_server_input(char *)						__page(CORE_SEG);

/* partyline.c */

int check_telnet(int, char *)						__page(CMD1_SEG);
void check_telnet_pass(Client *, char *)				__page(CMD1_SEG);
int partyline_only_command(const char *)				__page(CMD1_SEG);
void partyline_broadcast(const Client *, const char *, const char *)	__page(CMD1_SEG);
void partyline_banner(Client *)						__page(CMD1_SEG);
void dcc_chat(char *)							__page(CMD1_SEG);
void whom_printbot(char *, BotInfo *, char *)				__page(CMD1_SEG);
void do_whom(COMMAND_ARGS)						__page(CMD1_SEG);
void do_chat(COMMAND_ARGS)						__page(CMD1_SEG);
void do_bye(COMMAND_ARGS)						__page(CMD1_SEG);
void do_boot(COMMAND_ARGS)						__page(CMD1_SEG);

/* perl.c */

void do_perl(COMMAND_ARGS)						__page(CMD1_SEG);
void do_perlscript(COMMAND_ARGS)					__page(CMD1_SEG);

/* prot.c */

void screwban_format(char *)						__page(CORE_SEG);
void deop_ban(Chan *, ChanUser *, char *)				__page(CORE_SEG);
void deop_siteban(Chan *, ChanUser *)					__page(CORE_SEG);
void deop_screwban(Chan *, ChanUser *)					__page(CORE_SEG);
void send_kick(Chan *chan, const char *nick, const char *format, ...)	__page(CORE_SEG);
void push_kicks(Chan *chan)						__page(CORE_SEG);
void unmode_chanuser(Chan *chan, ChanUser *cu)				__page(CORE_SEG);
void send_mode(Chan *chan, int pri, int type, char plusminus, char modeflag, void *data) __page(CORE_SEG);
int mode_effect(Chan *chan, qMode *mode)				__page(CORE_SEG);
void push_modes(Chan *chan, int lowpri)					__page(CORE_SEG);
void update_modes(Chan *chan)						__page(CORE_SEG);
int check_mass(Chan *chan, ChanUser *doer, int type)			__page(CORE_SEG);
void mass_action(Chan *chan, ChanUser *doer)				__page(CORE_SEG);
void prot_action(Chan *chan, char *from, ChanUser *doer, char *target, ChanUser *victim) __page(CORE_SEG);
void process_chanbans(void)						__page(CORE_SEG);
void chanban_action(char *, char *, Shit *)				__page(CORE_SEG);
void check_dynamode(Chan *)						__page(CORE_SEG);
void do_opdeopme(COMMAND_ARGS)						__page(CMD1_SEG);
void do_opvoice(COMMAND_ARGS)						__page(CMD1_SEG);
void do_kickban(COMMAND_ARGS)						__page(CMD1_SEG);
void do_unban(COMMAND_ARGS)						__page(CMD1_SEG);
void do_banlist(COMMAND_ARGS)						__page(CMD1_SEG);

/* python.c */

#ifdef PYTHON

#if defined(DEBUG_C) || defined(MEGA_C)
PyObject *python_hook(PyObject *self, PyObject *args, PyObject *keywds);
PyObject *python_unhook(PyObject *self, PyObject *args, PyObject *keywds);
#endif
/*
char *python_unicode2char(PyUnicodeObject *obj);
PyObject *python_userlevel(PyObject *self, PyObject *args, PyObject *keywds);
PyObject *python_to_server(PyObject *self, PyObject *args, PyObject *keywds);
PyObject *python_to_file(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *python_dcc_sendfile(PyObject *self, PyObject *args, PyObject *keywds);
PyObject *python_debug(PyObject *self, PyObject *args);
PyMODINIT_FUNC pythonInit(void);
*/
int python_parse_jump(char *, char *, Hook *);
int python_timer_jump(Hook *hook);
void python_dcc_complete(Client *client, int cps);
int python_dns_jump(char *host, char *resolved, Hook *hook);
void init_python(void);
void free_python(void);
void do_python(COMMAND_ARGS)						__page(CMD1_SEG);
void do_pythonscript(COMMAND_ARGS)					__page(CMD1_SEG);

#endif /* PYTHON */

/* reset.c */

char *recover_client(char *env)						__page(INIT_SEG);
char *recover_debug(char *env)						__page(INIT_SEG);
char *recover_server(char *env)						__page(INIT_SEG);
void recover_reset(void)						__page(INIT_SEG);
void do_reset(COMMAND_ARGS)						__page(RARE_SEG);

/* seen.c */

int write_seenlist(void);
int read_seenlist_callback(char *rest);
int read_seenlist(void);
void make_seen(char *nick, char *userhost, char *pa, char *pb, time_t when, int t);
void do_seen(COMMAND_ARGS);

/* shit.c */

void shit_action(Chan *chan, ChanUser *cu);
void check_shit(void);
void remove_shit(Shit *shit);
void purge_shitlist(void);
Shit *add_shit(char *from, char *chan, char *mask, char *reason, int axs, int expire) __page(CFG1_SEG);
Shit *find_shit(const char *userhost, const char *channel);
Shit *get_shituser(char *userhost, char *channel);
int get_shitaction(const char *userhost, const char *chan);
void do_shit(COMMAND_ARGS)						__page(CMD1_SEG);
void do_rshit(COMMAND_ARGS)						__page(CMD1_SEG);
void do_shitlist(COMMAND_ARGS)						__page(CMD1_SEG);
void do_clearshit(COMMAND_ARGS)						__page(CMD1_SEG);

/* spy.c */

void send_spy(const char *src, const char *format, ...)			__page(CORE_SEG);
void send_global(const char *src, const char *format, ...)		__page(CORE_SEG);
void spy_typecount(Mech *bot)						__page(CORE_SEG);
int spy_source(char *from, int *t_src, const char **src)		__page(CORE_SEG);
void urlhost(const char *)						__page(CORE_SEG);
void urlcapture(const char *)						__page(CORE_SEG);
int begin_redirect(char *, char *)					__page(CORE_SEG);
void send_redirect(char *)						__page(CORE_SEG);
void end_redirect(void)							__page(CORE_SEG);
void stats_loghour(Chan *chan, char *filename, int hour)		__page(CORE_SEG);
void stats_plusminususer(Chan *chan, int plusminus)			__page(CORE_SEG);
void do_spy(COMMAND_ARGS)						__page(CMD1_SEG);
void do_rspy(COMMAND_ARGS)						__page(CMD1_SEG);
void do_info(COMMAND_ARGS)						__page(CMD1_SEG);
void do_urlhist(COMMAND_ARGS)						__page(CMD1_SEG);

/* tcl.c */
#ifdef TCL

/*
char *tcl_var_read(Tcl_TVInfo *vinfo, Tcl_Interp *I, char *n1, char *n2, int flags);
char *tcl_var_write(Tcl_TVInfo *vinfo, Tcl_Interp *I, char *n1, char *n2, int flags);
*/
int tcl_timer_jump(Hook *hook);
int tcl_parse_jump(char *from, char *rest, Hook *hook);
void tcl_dcc_complete(Client *client, int cps);
#if defined(DEBUG_C) || defined(MEGA_C)
int tcl_hook(void *foo, Tcl_Interp *I, int objc, Tcl_Obj *CONST objv[]);
#endif
/*
int tcl_unhook(void *foo, Tcl_Interp *I, int objc, Tcl_Obj *CONST objv[]);
int tcl_userlevel(void *foo, Tcl_Interp *I, int objc, Tcl_Obj *CONST objv[]);
int tcl_debug(void *foo, Tcl_Interp *I, int objc, Tcl_Obj *CONST objv[]);
int tcl_to_server(void *foo, Tcl_Interp *I, int objc, Tcl_Obj *CONST objv[]);
int tcl_to_file(void *foo, Tcl_Interp *I, int objc, Tcl_Obj *CONST objv[]);
int tcl_dcc_sendfile(void *foo, Tcl_Interp *I, int objc, Tcl_Obj *CONST objv[]);
int tcl_dns_jump(char *host, char *resolved, Hook *hook);
int tcl_dns(void *foo, Tcl_Interp *I, int objc, Tcl_Obj *CONST objv[]);
*/
void init_tcl(void);
void do_tcl(COMMAND_ARGS)						__page(CMD1_SEG);

#endif /* TCL */

/* toybox.c */

int read_charset_callback(char *)					__page(CMD1_SEG);
int read_bigcharset(char *)						__page(CMD1_SEG);
int read_ascii(char *)							__page(CMD1_SEG);
void trivia_week_toppers(void)						__page(CMD1_SEG);
void hint_one(void)							__page(CMD1_SEG);
void hint_two(void)							__page(CMD1_SEG);
void hint_three(void)							__page(CMD1_SEG);
void trivia_cleanup(void)						__page(CMD1_SEG);
void trivia_check(Chan *, char *)					__page(CMD1_SEG);
void trivia_no_answer(void)						__page(CMD1_SEG);
char *random_question(char *)						__page(CMD1_SEG);
void trivia_question(void)						__page(CMD1_SEG);
void trivia_tick(void)							__page(CMD1_SEG);
void write_triviascore(void)						__page(CMD1_SEG);
int trivia_score_callback(char *)					__page(CMD1_SEG);
void read_triviascore(void)						__page(CMD1_SEG);
void do_bigsay(COMMAND_ARGS)						__page(CMD1_SEG);
void do_randmsg(COMMAND_ARGS)						__page(CMD1_SEG);
void do_randtopic(COMMAND_ARGS)						__page(CMD1_SEG);
void do_8ball(COMMAND_ARGS)						__page(CMD1_SEG);
void do_ascii(COMMAND_ARGS)						__page(CMD1_SEG);
void do_rand(COMMAND_ARGS)						__page(CMD1_SEG);
void do_trivia(COMMAND_ARGS)						__page(CMD1_SEG);


/* uptime.c */

void init_uptime(void)							__page(CFG1_SEG);
void send_uptime(int type)						__page(CORE_SEG);
void uptime_death(int type)						__page(RARE_SEG);
void process_uptime(void)						__page(CORE_SEG);
void do_upsend(COMMAND_ARGS)						__page(CMD1_SEG);

/* user.c */

void cfg_user(char *)							__page(CFG1_SEG);
void cfg_modcount(char *)						__page(CFG1_SEG);
void cfg_pass(char *)							__page(CFG1_SEG);
void cfg_mask(char *)							__page(CFG1_SEG);
void cfg_chan(char *)							__page(CFG1_SEG);
void cfg_opt(char *)							__page(CFG1_SEG);
void cfg_shit(char *)							__page(CFG1_SEG);
void cfg_kicksay(char *)						__page(CFG1_SEG);
void cfg_greet(char *)							__page(CFG1_SEG);
void cfg_note(char *)							__page(CFG1_SEG);
void user_sync(void)							__page(CFG1_SEG);
int read_userlist_callback(char *)					__page(CFG1_SEG);
int read_userlist(char *)						__page(CFG1_SEG);
int write_userlist(char *)						__page(CMD1_SEG);
void rehash_chanusers(void)						__page(CORE_SEG);
void addtouser(Strp **, const char *, int)				__page(CORE_SEG);
int remfromuser(Strp **, const char *)					__page(CORE_SEG);
void mirror_user(User *)						__page(CORE_SEG);
void mirror_userlist(void)						__page(CORE_SEG);
void reset_userlink(User *, User *)					__page(CORE_SEG);
void remove_user(User *)						__page(CFG1_SEG);
User *add_user(char *, char *, int)					__page(CFG1_SEG);
User *find_handle(const char *)						__page(CORE_SEG);
int userhaschannel(const User *, const char *)				__page(CORE_SEG);
User *get_user(const char *, const char *)				__page(CORE_SEG);
int get_useraccess(const char *, const char *)				__page(CORE_SEG);
int get_maxaccess(const char *)						__page(CORE_SEG);
int is_bot(const char *)						__page(CORE_SEG);
int get_protaction(Chan *, char *)					__page(CORE_SEG);
int usercanmodify(const char *, const User *)				__page(CMD1_SEG);
void change_pass(User *, char *)					__page(CMD1_SEG);
void do_access(COMMAND_ARGS)						__page(CMD1_SEG);
void do_userlist(COMMAND_ARGS)						__page(CMD1_SEG);
void do_user(COMMAND_ARGS)						__page(CMD1_SEG);
void do_echo(COMMAND_ARGS)						__page(CMD1_SEG);
void do_passwd(COMMAND_ARGS)						__page(CMD1_SEG);
void do_setpass(COMMAND_ARGS)						__page(CMD1_SEG);
void do_save(COMMAND_ARGS)						__page(CMD1_SEG);
void do_load(COMMAND_ARGS)						__page(CMD1_SEG);

/* vars.c */

void set_str_varc(Chan *, int, char *)					__page(CFG1_SEG);
int find_setting(const char *)						__page(CORE_SEG);
void copy_vars(UniVar *, UniVar *)					__page(CFG1_SEG);
void set_binarydefault(UniVar *)					__page(CFG1_SEG);
void delete_vars(UniVar *, int)						__page(CMD1_SEG);
void var_resolve_host(const struct Setting *)				__page(CMD1_SEG);
void nobo_strcpy(const char *)						__page(CMD1_SEG);
void ec_access(char *, const char *)					__page(CMD1_SEG);
void ec_capabilities(char *, const char *)				__page(CMD1_SEG);
void ec_cc(char *, const char *)					__page(CMD1_SEG);
void ec_channels(char *, const char *)					__page(CMD1_SEG);
void ec_time(char *, const char *)					__page(CMD1_SEG);
void ec_set(char *, const char *)					__page(CMD1_SEG);
void ec_on(char *, const char *)					__page(CMD1_SEG);
void ec_server(char *, const char *)					__page(CMD1_SEG);
void ec_up(char *, const char *)					__page(CMD1_SEG);
void ec_ver(char *, const char *)					__page(CMD1_SEG);
void do_esay(COMMAND_ARGS)						__page(CMD1_SEG);
void do_set(COMMAND_ARGS)						__page(CMD1_SEG);

/* web.c */

char *webread(int, char *, char *);
void eml_fmt(WebSock *, char *);
void web_raw(WebSock *, char *);
void web_botstatus(WebSock *, char *);
void web_debug(WebSock *, char *);
void web_404(WebSock *, char *);
void parse_web(WebSock *, char *)					__page(CORE_SEG);
void select_web(void)							__page(CORE_SEG);
void process_web(void)							__page(CORE_SEG);

#endif /* H_H */

