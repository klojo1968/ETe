/*
===========================================================================

Wolfenstein: Enemy Territory GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

This file is part of the Wolfenstein: Enemy Territory GPL Source Code (Wolf ET Source Code).  

Wolf ET Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolf ET Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolf ET Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Wolf: ET Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Wolf ET Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
// cl_discord.c

#ifdef USE_DISCORD

//#include "../qcommon/q_shared.h"
//#include "../qcommon/qcommon.h"
#include "client.h"
#include "discord_rpc.h"
#include "discord_register.h"

// This is the discord appid for Enemy Territory (ETe)
static const char* discord_appid = "556373888875888640";
//static const char* discord_appid_etf = "556373888875888640";

typedef struct statusIcon_s {
	const char *string;
	const char *icon;
} statusIcon_t;

static const char* unknownMapIcon = "unknownmap";
static const char* unknownMapIcon_etf = "unknownmap_etf";

static const statusIcon_t mapIcons[] = {
	{"battery", "battery"},
	{"fueldump", "fueldump"},
	{"goldrush", "goldrush"},
	{"oasis", "oasis"},
	{"radar", "radar"},
	{"railgun", "railgun"},
	{"etf_allduel", "etf_allduel"},
	{"etf_bases", "etf_bases"},
	{"etf_canalzone", "etf_canalzone"},
	{"etf_chaos", "etf_chaos"},
	{"etf_forts", "etf_forts"},
	{"etf_gotduck", "etf_gotduck"},
	{"etf_hardcore", "etf_hardcore"},
	{"etf_japanc", "etf_japanc"},
	{"etf_lastresort", "etf_lastresort"},
	{"etf_mach", "etf_mach"},
	{"etf_muon", "etf_muon"},
	{"etf_nocturne", "etf_nocturne"},
	{"etf_openfire", "etf_openfire"},
	{"etf_rock", "etf_rock"},
	{"etf_silverfort", "etf_silverfort"},
	{"etf_smooth", "etf_smooth"},
	{"etf_stag", "etf_stag"},
	{"etf_well", "etf_well"},
	//{"etf_egypt", "etf_egypt"},
	{"etf_entropy", "etf_entropy"},
	{"etf_spring", "etf_spring"},
	{"etf_xpress", "etf_xpress"}
};
static const size_t numMapIcons = ARRAY_LEN(mapIcons);

static const statusIcon_t gameType_etf = { "CTF", "etfctf" };

static const statusIcon_t gameTypes[] = {
	{"Solo", "solo"},
	{"Cooperative", "coop"},
	{"Objective", "objective"},
	{"Stopwatch", "stopwatch"},
	{"Campaign", "campaign"},
	{"Last Man Standing", "lms"}
};
static const size_t numGameTypes = ARRAY_LEN(gameTypes);

static const char* GetMapName(void) {
	if (cls.state == CA_DISCONNECTED || cls.state == CA_CONNECTING)
	{
		return "menu";
	}
	return cl.discord.mapName;
}

static const char* GetServerName(void) {
	return cl.discord.hostName;
}

static const char* GetMapIcon(void) {
	const char* mapname = GetMapName();
	int i;

	for (i = 0; i < numMapIcons; i++)
	{
		if (!Q_stricmp(mapname, mapIcons[i].string))
		{
			return mapIcons[i].icon;
		}
	}

	return cl.discord.isETF ? unknownMapIcon_etf : unknownMapIcon;
}

static qboolean IsSpectating(void) {
	if (!cl.snap.valid)
		return qfalse;
	if ((cl.snap.ps.pm_flags & PMF_FOLLOW))
		return qtrue;
	if (cl.discord.isETF && (cl.snap.ps.pm_flags & 0x8000)) // CHASE
		return qtrue;
	if (cl.snap.ps.pm_type == PM_SPECTATOR)
		return qtrue;
	if (cl.discord.isETF && (cl.snap.ps.pm_type == 3 || cl.snap.ps.pm_type == 4)) // multiple spectator states
		return qtrue;
	return qfalse;
}

static const char* GetState(void) {
	if (cls.state == CA_ACTIVE && cl.snap.valid) {
		//if (cl_discordRichPresence->integer > 1 && (cmd.buttons & BUTTON_TALK))
		//	return "chatting";
		//else if (cl_afkName || (cls.realtime - cls.afkTime) >= (5 * 60000)) //5 minutes?
		//	return "idle";
		if (IsSpectating())
			return "spectating";
		else
			return "playing";
	}
	else if (cls.state > CA_DISCONNECTED && cls.state < CA_PRIMED) {
		return "connecting";
	}
	else if (cls.state <= CA_DISCONNECTED) {
		return "menu";
	}

	return "";
}

static const char* GetGameType(qboolean imageKey)
{
	if (cl.discord.isETF) {
		return imageKey ? gameType_etf.icon : gameType_etf.string;
	}
	else {
		return imageKey ? gameTypes[cl.discord.gametype].icon : gameTypes[cl.discord.gametype].string;
	}


	//if (cl.discord.gametype > GT_FFA)
	//	return imageKey ? gameTypes[cl.discord.gametype].icon : gameTypes[cl.discord.gametype].string;

	//return imageKey ? GetState() : gameTypes[cl.discord.gametype].string;
}

cvar_t* cl_discordRichPresenceSharePassword;
static const char* joinSecret(void)
{
	if (clc.demoplaying)
		return NULL;

	if (cls.state >= CA_LOADING && cls.state <= CA_ACTIVE)
	{
		const char* password = Cvar_VariableString("password");

		if (cl_discordRichPresenceSharePassword->integer && cl.discord.needPassword && strlen(password)) {
			return va("%s %s %s", cls.servername, cl.discord.gameDir, password);
		}
		else {
			return va("%s %s \"\"", cls.servername, cl.discord.gameDir);
		}
	}

	return NULL;
}

static const char* PartyID(void)
{
	if (clc.demoplaying)
		return NULL;

	if (cls.state >= CA_LOADING && cls.state <= CA_ACTIVE)
	{
		return va("%sx", cls.servername);
	}

	return NULL;
}



static const char* BuildServerState(void) {
	if (cls.state == CA_ACTIVE) {
		if (clc.demoplaying) {
			return GetGameType(qfalse);
		}

		//return va("%d / %d players", cl.discord.playerCount, cl.discord.maxPlayers);
		return va("%s | %dv%d", GetGameType(qfalse), cl.discord.redTeam, cl.discord.blueTeam);
	}

	if (cls.state <= CA_DISCONNECTED || cls.state == CA_CINEMATIC)
		return "";

	return GetState();
}

static const char* BuildServerDetails(void) {
	if (cls.state == CA_ACTIVE) {
		if (cl_discordRichPresence->integer > 1) {
			return GetServerName();
		}

		if (clc.demoplaying) {
			return va("Playing demo - %s", GetServerName());
		}

		if (com_sv_running->integer) {
			if (Q_stricmp(Cvar_VariableString("sv_hostname"), "ETHost"))
				return va("Playing offline - %s\n", GetServerName());

			return "Playing offline";
		}

		if (cl.snap.valid && ((cl.snap.ps.pm_flags & PMF_FOLLOW) || cl.snap.ps.pm_type == PM_SPECTATOR))
			return va("Spectating on %s", GetServerName());

		return va("Playing on %s", GetServerName());

		//return GetServerName();
	}

	if (cls.state > CA_DISCONNECTED && cls.state < CA_ACTIVE)
		return "";

	if (cls.state <= CA_DISCONNECTED || cls.state == CA_CINEMATIC)
		return "In Menu";

	return NULL;
	//return "";
}


static void Discord_OnReady(const DiscordUser* request) {
	Com_Printf(S_COLOR_CYAN "Discord::OnReady(%s,%s#%s)\n", request->userId, request->username, request->discriminator);
}

static void Discord_OnDisconnected(int errorCode, const char* message) {
	Com_Printf(S_COLOR_CYAN "Discord::OnDisconnected(%d,%s)\n", errorCode, message);
}

static void Discord_OnErrored(int errorCode, const char* message) {
	Com_Printf(S_COLOR_RED "Discord::OnErrored(%d,%s)\n", errorCode, message);
}

static void Discord_JoinGame(const char* secret)
{
	char ip[60] = { 0 };
	char fsgame[60] = { 0 };
	char password[MAX_CVAR_VALUE_STRING];
	int parsed = 0;

	Com_Printf(S_COLOR_CYAN "Discord::JoinGame(%s)\n", secret);

	parsed = sscanf(secret, "%s %s %s", ip, fsgame, password);

	switch (parsed)
	{
	case 3: //ip, password, and fsgame
		Cbuf_AddText(va("connect %s ; set password %s\n", ip, password));
		break;
	case 2://ip and fsgame
	case 1://ip only
		Cbuf_AddText(va("connect %s\n", ip));
		break;
	default:
		//Com_Printf("^5Discord: %1Failed to parse server information from join secret\n");
		Com_Printf( S_COLOR_CYAN "Discord: Failed to parse server information from join secret\n");
		break;
	}
}

static void Discord_SpectateGame(const char* secret)
{
	Com_Printf(S_COLOR_CYAN "Discord::SpectateGame(%s)\n", secret);
}

static void Discord_JoinRequest(const DiscordUser* request)
{
	int response = -1;

	Com_Printf(S_COLOR_CYAN "Discord::JoinRequest(%s,%s#%s)\n",
		request->userId,
		request->username,
		request->discriminator
	);

	if (response != -1) {
		Discord_Respond(request->userId, response);
	}
}

static DiscordRichPresence discordPresence;
static DiscordEventHandlers handlers;
void CL_DiscordInitialize(void) {
	memset(&handlers, 0, sizeof(handlers));
	handlers.ready = Discord_OnReady;
	handlers.errored = Discord_OnErrored;
	handlers.disconnected = Discord_OnDisconnected;
	handlers.joinGame = Discord_JoinGame;
	handlers.spectateGame = Discord_SpectateGame;
	handlers.joinRequest = Discord_JoinRequest;

	Discord_Initialize(discord_appid, &handlers, 1, "");
	Discord_Register(discord_appid, NULL);

	Discord_UpdateHandlers(&handlers);

	cl_discordRichPresenceSharePassword = Cvar_Get("cl_discordRichPresenceSharePassword", "1", CVAR_ARCHIVE_ND);
	Cvar_SetDescription(cl_discordRichPresenceSharePassword, "If set, sends password to Discord friends who request to join your game");

	Q_strncpyz(cl.discord.hostName, "ETHost", sizeof(cl.discord.hostName));
	Q_strncpyz(cl.discord.mapName, "nomap", sizeof(cl.discord.mapName));
	Q_strncpyz(cl.discord.gameDir, FS_GetCurrentGameDir(), sizeof(cl.discord.gameDir));
	if (!Q_stricmp(cl.discord.gameDir, "etf"))
		cl.discord.isETF = qtrue;
	else
		cl.discord.isETF = qfalse;
}

void CL_DiscordShutdown(void)
{
	Discord_Shutdown();
}


void CL_DiscordUpdatePresence(void)
{
	const char* partyID = PartyID();
	const char* joinID = joinSecret();

	if (!cls.discordInit)
		return;

	if (cl.discord.gametype < 0 || cl.discord.gametype >= numGameTypes)
		cl.discord.gametype = 0;

	Com_Memset(&discordPresence, 0, sizeof(discordPresence));

	discordPresence.state = BuildServerState();
	discordPresence.details = BuildServerDetails();
	discordPresence.largeImageKey = GetMapIcon();
	discordPresence.largeImageText = GetMapName();
	/*if (cl_discordRichPresence->integer > 1 || cls.state < CA_ACTIVE || cl.discord.gametype == GT_FFA) {
		discordPresence.smallImageKey = GetState();
		discordPresence.smallImageText = GetState();
	}
	else {*/
		discordPresence.smallImageKey = GetGameType(qtrue);
		discordPresence.smallImageText = GetGameType(qfalse);
	//}
	if (!clc.demoplaying && !com_sv_running->integer)
	{ //send join information blank since it won't do anything in this case
		discordPresence.partyId = partyID; // Server-IP zum abgleichen discordchat - send join request in discord chat
		/*if (cl_discordRichPresence->integer > 1) {
			discordPresence.partySize = cls.state == CA_ACTIVE ? 1 : NULL;
			discordPresence.partyMax = cls.state == CA_ACTIVE ? ((cl.discord.maxPlayers - cl.discord.playerCount) + discordPresence.partySize) : NULL;
		}
		else {*/
			discordPresence.partySize = cls.state >= CA_LOADING ? cl.discord.playerCount : 0;
			discordPresence.partyMax = cls.state >= CA_LOADING ? cl.discord.maxPlayers : 0;
		//}
		discordPresence.joinSecret = joinID; // Server-IP zum discordJoin ausf�hren - serverip for discordjoin to execute
	}
	Discord_UpdatePresence(&discordPresence);

	Discord_RunCallbacks();
}

#endif
