/*
===========================================================================
Wolfenstein: Enemy Territory GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

ET: Legacy
Copyright (C) 2012-2017 ET:Legacy team <mail@etlegacy.com>

Copyright (C) 2005 Stuart Dalton (badcdev@gmail.com)

This file is part of the Wolfenstein: Enemy Territory GPL Source Code (Wolf ET Source Code)
This file is part of ET: Legacy - http://www.etlegacy.com

Wolf ET Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolf ET Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolf ET Source Code. If not, see <http://www.gnu.org/licenses/>.

In addition, Wolfenstein: Enemy Territory GPL Source Code is also
subject to certain additional terms. You should have received a copy
of these additional terms immediately following the terms and conditions
of the GNU General Public License which accompanied the source code.
If not, please request a copy in writing from id Software at the address below.

id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.
===========================================================================
*/

#include "client.h"
#include "snd_codec.h"
#include "snd_local.h"
#include "snd_public.h"

cvar_t *s_volume;
//cvar_t *s_muted;
cvar_t *s_musicVolume;
cvar_t *s_doppler;
cvar_t *s_backend;
cvar_t *s_muteWhenMinimized;
cvar_t *s_muteWhenUnfocused;

static soundInterface_t si;

/*
=================
S_ValidateInterface
=================
*/
static qboolean S_ValidSoundInterface( const soundInterface_t *_si )
{
	if( !_si->Shutdown ) return qfalse;
	if( !_si->Reload ) return qfalse;
	if( !_si->StartSound ) return qfalse;
	if( !_si->StartSoundEx ) return qfalse;
	if( !_si->StartLocalSound ) return qfalse;
	if( !_si->StartBackgroundTrack ) return qfalse;
	if( !_si->StopBackgroundTrack ) return qfalse;
	if( !_si->StartStreamingSound ) return qfalse;
	if( !_si->StopEntStreamingSound ) return qfalse;
	if( !_si->FadeStreamingSound ) return qfalse;
	if( !_si->RawSamples ) return qfalse;
	if( !_si->ClearSounds ) return qfalse;
	if( !_si->StopAllSounds ) return qfalse;
	if( !_si->FadeAllSounds ) return qfalse;
	if( !_si->ClearLoopingSounds ) return qfalse;
	if( !_si->AddLoopingSound ) return qfalse;
	if( !_si->AddRealLoopingSound ) return qfalse;
	if( !_si->Respatialize ) return qfalse;
	if( !_si->UpdateEntityPosition ) return qfalse;
	if( !_si->Update ) return qfalse;
	if( !_si->DisableSounds ) return qfalse;
	if( !_si->BeginRegistration ) return qfalse;
	if( !_si->RegisterSound ) return qfalse;
	if( !_si->ClearSoundBuffer ) return qfalse;
	if( !_si->SoundInfo ) return qfalse;
	if( !_si->SoundList ) return qfalse;
	if( !_si->GetVoiceAmplitude ) return qfalse;
	if( !_si->GetSoundLength ) return qfalse;
	if( !_si->GetCurrentSoundTime ) return qfalse;

#ifdef USE_VOIP
	if( !_si->StartCapture ) return qfalse;
	if( !_si->AvailableCaptureSamples ) return qfalse;
	if( !_si->Capture ) return qfalse;
	if( !_si->StopCapture ) return qfalse;
	if( !_si->MasterGain ) return qfalse;
#endif

	return qtrue;
}


/*
=================
S_StartSound
=================
*/
void S_StartSound( vec3_t origin, int entnum, int entchannel, sfxHandle_t sfx, int volume )
{
	if( si.StartSound ) {
		si.StartSound( origin, entnum, entchannel, sfx, volume );
	}
}

/*
=================
S_StartSoundEx
=================
*/
void S_StartSoundEx( vec3_t origin, int entnum, int entchannel, sfxHandle_t sfx, int flags, int volume )
{
	if( si.StartSoundEx ) {
		si.StartSoundEx( origin, entnum, entchannel, sfx, flags, volume );
	}
}

/*
=================
S_StartLocalSound
=================
*/
void S_StartLocalSound( sfxHandle_t sfx, int channelNum, int volume )
{
	if( si.StartLocalSound ) {
		si.StartLocalSound( sfx, channelNum, volume );
	}
}

/*
=================
S_StartBackgroundTrack
=================
*/
void S_StartBackgroundTrack( const char *intro, const char *loop, int fadeupTime )
{
	if ( !*intro )
		return;

	if( si.StartBackgroundTrack ) {
		si.StartBackgroundTrack( intro, loop, fadeupTime );
	}
}

/*
=================
S_StopBackgroundTrack
=================
*/
void S_StopBackgroundTrack( void )
{
	if( si.StopBackgroundTrack ) {
		si.StopBackgroundTrack( );
	}
}

/*
=================
S_StartStreamingSound
=================
*/
float S_StartStreamingSound( const char *intro, const char *loop, int entityNum, int channel, int attenuation )
{
	if( si.StartStreamingSound ) {
		return si.StartStreamingSound( intro, loop, entityNum, channel, attenuation );
	}
	return 0.0f;
}

void S_StopEntStreamingSound(int entNum)
{
	if (si.StopEntStreamingSound)
	{
		si.StopEntStreamingSound(entNum);
	}
}

void S_FadeStreamingSound(float targetvol, int time, int stream)
{
	if (si.FadeStreamingSound)
	{
		si.FadeStreamingSound(targetvol, time, stream);
	}
}

/*
=================
S_RawSamples
=================
*/
void S_RawSamples (int stream, int samples, int rate, int width, int channels,
		   const byte *data, float lvol, float rvol, int entityNum)
{
	if(si.RawSamples)
		si.RawSamples(stream, samples, rate, width, channels, data, lvol, rvol, entityNum);
}

void S_ClearSounds(qboolean clearStreaming, qboolean clearMusic)
{
	if (si.ClearSounds)
	{
		si.ClearSounds(clearStreaming, clearMusic);
	}
}

/*
=================
S_StopAllSounds
=================
*/
void S_StopAllSounds( void )
{
	if( si.StopAllSounds ) {
		si.StopAllSounds( );
	}
}

void S_FadeAllSounds(float targetVol, int time, qboolean stopSounds)
{
	if (si.FadeAllSounds)
	{
		si.FadeAllSounds(targetVol, time, stopSounds);
	}
}

/*
=================
S_ClearLoopingSounds
=================
*/
void S_ClearLoopingSounds( void )
{
	if( si.ClearLoopingSounds ) {
		si.ClearLoopingSounds( );
	}
}

/*
=================
S_AddLoopingSound
=================
*/
void S_AddLoopingSound( const vec3_t origin,
		const vec3_t velocity, const int range, sfxHandle_t sfx, int volume, int soundTime )
{
	if( si.AddLoopingSound ) {
		si.AddLoopingSound( origin, velocity, range, sfx, volume, soundTime );
	}
}

/*
=================
S_AddRealLoopingSound
=================
*/
void S_AddRealLoopingSound( const vec3_t origin,
		const vec3_t velocity, const int range, sfxHandle_t sfx, int volume )
{
	if( si.AddRealLoopingSound ) {
		si.AddRealLoopingSound( origin, velocity, range, sfx, volume );
	}
}

/*
=================
S_Respatialize
=================
*/
void S_Respatialize( int entityNum, const vec3_t origin,
		vec3_t axis[3], int inwater )
{
	if( si.Respatialize ) {
		si.Respatialize( entityNum, origin, axis, inwater );
	}
}

/*
=================
S_UpdateEntityPosition
=================
*/
void S_UpdateEntityPosition( int entityNum, const vec3_t origin )
{
	if( si.UpdateEntityPosition ) {
		si.UpdateEntityPosition( entityNum, origin );
	}
}

/*
=================
S_Update
=================
*/
void S_Update( int msec )
{
	/*if(s_muted->integer)
	{
		if(!(s_muteWhenMinimized->integer && gw_minimized) &&
		   !(s_muteWhenUnfocused->integer && !gw_active))
		{
			s_muted->integer = qfalse;
			s_muted->modified = qtrue;
		}
	}
	else
	{
		if((s_muteWhenMinimized->integer && gw_minimized) ||
		   (s_muteWhenUnfocused->integer && !gw_active))
		{
			s_muted->integer = qtrue;
			s_muted->modified = qtrue;
		}
	}*/
	
	if( si.Update ) {
		si.Update( msec );
	}
}

void S_Reload( void )
{
	if ( si.Reload ) {
		si.Reload();
	}
}

/*
=================
S_DisableSounds
=================
*/
void S_DisableSounds( void )
{
	if( si.DisableSounds ) {
		si.DisableSounds( );
	}
}

/*
=================
S_BeginRegistration
=================
*/
void S_BeginRegistration( void )
{
	if( si.BeginRegistration ) {
		si.BeginRegistration( );
	}
}

/*
=================
S_RegisterSound
=================
*/
sfxHandle_t	S_RegisterSound( const char *sample, qboolean compressed )
{
	if ( !sample || !*sample ) {
		Com_DPrintf( "NULL sound\n" );
		return 0;
	}

	if( si.RegisterSound ) {
		return si.RegisterSound( sample, compressed );
	} else {
		return 0;
	}
}

/*
=================
S_ClearSoundBuffer
=================
*/
void S_ClearSoundBuffer( qboolean killStreaming )
{
	if( si.ClearSoundBuffer ) {
		si.ClearSoundBuffer( killStreaming );
	}
}

/*
=================
S_SoundInfo
=================
*/
void S_SoundInfo( void )
{
	if( si.SoundInfo ) {
		si.SoundInfo( );
	}
}

/*
=================
S_SoundList
=================
*/
void S_SoundList( void )
{
	if( si.SoundList ) {
		si.SoundList( );
	}
}

/*
=================
S_GetVoiceAmplitude
=================
*/
int S_GetVoiceAmplitude( int entityNum )
{
	if( si.GetVoiceAmplitude ) {
		return si.GetVoiceAmplitude( entityNum );
	}
	return 0;
}

// START	xkan, 9/23/2002
// returns how long the sound lasts in milliseconds
int S_GetSoundLength( sfxHandle_t sfxHandle ) {
	if ( si.GetSoundLength ) {
		return si.GetSoundLength( sfxHandle );
	} else {
		return 0;
	}
}
// END		xkan, 9/23/2002

// ydnar: for looped sound synchronization
int S_GetCurrentSoundTime( void ) {
	if ( si.GetCurrentSoundTime ) {
		return si.GetCurrentSoundTime();
	} else {
		return 0;
	}
}


#ifdef USE_VOIP
/*
=================
S_StartCapture
=================
*/
void S_StartCapture( void )
{
	if( si.StartCapture ) {
		si.StartCapture( );
	}
}

/*
=================
S_AvailableCaptureSamples
=================
*/
int S_AvailableCaptureSamples( void )
{
	if( si.AvailableCaptureSamples ) {
		return si.AvailableCaptureSamples( );
	}
	return 0;
}

/*
=================
S_Capture
=================
*/
void S_Capture( int samples, byte *data )
{
	if( si.Capture ) {
		si.Capture( samples, data );
	}
}

/*
=================
S_StopCapture
=================
*/
void S_StopCapture( void )
{
	if( si.StopCapture ) {
		si.StopCapture( );
	}
}

/*
=================
S_MasterGain
=================
*/
void S_MasterGain( float gain )
{
	if( si.MasterGain ) {
		si.MasterGain( gain );
	}
}
#endif

//=============================================================================

/*
=================
S_Play_f
=================
*/
void S_Play_f( void ) {
	int 		i;
	int			c;
	sfxHandle_t	h;

	if( !si.RegisterSound || !si.StartLocalSound ) {
		return;
	}

	c = Cmd_Argc();

	if( c < 2 ) {
		Com_Printf ("Usage: play <sound filename> [sound filename] [sound filename] ...\n");
		return;
	}

	for( i = 1; i < c; i++ ) {
		h = si.RegisterSound( Cmd_Argv(i), qfalse );

		if( h ) {
			si.StartLocalSound( h, CHAN_LOCAL_SOUND, 127 );
		}
	}
}

/*
==============
S_QueueMusic_f
	console interface really just for testing
==============
*/
void S_QueueMusic_f( void ) {
	int type = -2;  // default to setting this as the next continual loop
	int c;

	if ( !si.StartBackgroundTrack ) {
		return;
	}

	c = Cmd_Argc();

	if ( c == 3 ) {
		type = atoi( Cmd_Argv( 2 ) );
	}

	if ( type != -1 ) { // clamp to valid values (-1, -2)
		type = -2;
	}

	// NOTE: could actually use this to touch the file now so there's not a hit when the queue'd music is played?
	si.StartBackgroundTrack( Cmd_Argv( 1 ), Cmd_Argv( 1 ), type );
}

/*
=================
S_Music_f
=================
*/
void S_Music_f( void ) {
	int		c;

	if( !si.StartBackgroundTrack ) {
		return;
	}

	c = Cmd_Argc();

	if ( c == 2 ) {
		si.StartBackgroundTrack( Cmd_Argv(1), NULL, 0 );
	} else if ( c == 3 ) {
		si.StartBackgroundTrack( Cmd_Argv(1), Cmd_Argv(2), 0 );
	} else if ( c == 4 ) {
		si.StartBackgroundTrack( Cmd_Argv(1), Cmd_Argv(2), atoi(Cmd_Argv(3)) );
	} else {
		Com_Printf ("Usage: music <musicfile> [loopfile] [fadeupTime]\n");
		return;
	}
}

void S_Stream_f(void)
{
	int c;

	if (!si.StartStreamingSound)
	{
		return;
	}

	c = Cmd_Argc();

	if (c == 2)
	{
		si.StartStreamingSound(Cmd_Argv(1), NULL, 0, 0, 0);
	}
	else if (c == 3)
	{
		si.StartStreamingSound(Cmd_Argv(1), Cmd_Argv(2), 0, 0, 0);
	}
	else if (c == 4)
	{
		si.StartStreamingSound(Cmd_Argv(1), Cmd_Argv(2),
		                       atoi(Cmd_Argv(3)), 0, 0);
	}
	else if (c == 5)
	{
		si.StartStreamingSound(Cmd_Argv(1), Cmd_Argv(2),
		                       atoi(Cmd_Argv(3)),
		                       atoi(Cmd_Argv(4)), 0);
	}
	else if (c == 6)
	{
		si.StartStreamingSound(Cmd_Argv(1), Cmd_Argv(2),
		                       atoi(Cmd_Argv(3)),
		                       atoi(Cmd_Argv(4)),
		                       atoi(Cmd_Argv(5)));
	}
	else
	{
		Com_Printf("Usage: stream <streamfile> [loopfile] [entNum] [channel] [attenuation]\n");
		return;
	}
}

/*
=================
S_StopMusic_f
=================
*/
void S_StopMusic_f( void )
{
	if(!si.StopBackgroundTrack)
		return;

	si.StopBackgroundTrack();
}


//=============================================================================

/*
=================
S_Init
=================
*/
void S_Init( void )
{
	cvar_t		*cv;
	qboolean	started = qfalse;

	Com_Printf( "------ Initializing Sound ------\n" );

	s_volume = Cvar_Get( "s_volume", "0.8", CVAR_ARCHIVE );
	s_musicVolume = Cvar_Get( "s_musicvolume", "0.25", CVAR_ARCHIVE );
	s_doppler = Cvar_Get( "s_doppler", "1", CVAR_ARCHIVE_ND );
	s_backend = Cvar_Get( "s_backend", "", CVAR_ROM );
	s_muteWhenUnfocused = Cvar_Get( "s_muteWhenUnfocused", "1", CVAR_ARCHIVE );
	s_muteWhenMinimized = Cvar_Get( "s_muteWhenMinimized", "1", CVAR_ARCHIVE );

	Cvar_CheckRange( s_volume, "0", "1", CV_FLOAT );
	Cvar_CheckRange( s_musicVolume, "0", "1", CV_FLOAT );
	Cvar_CheckRange( s_doppler, "0", "1", CV_INTEGER );
	Cvar_CheckRange( s_muteWhenUnfocused, "0", "1", CV_INTEGER );
	Cvar_CheckRange( s_muteWhenMinimized, "0", "1", CV_INTEGER );

	cv = Cvar_Get( "s_initsound", "1", 0 );
	if ( !cv->integer ) {
		Com_Printf( "Sound disabled.\n" );
	} else {

		S_CodecInit();

		Cmd_AddCommand( "play", S_Play_f );
		Cmd_AddCommand( "music", S_Music_f );
		Cmd_AddCommand( "music_queue", S_QueueMusic_f );
		Cmd_AddCommand( "stopmusic", S_StopMusic_f );
		Cmd_AddCommand( "stream", S_Stream_f );
		Cmd_AddCommand( "s_list", S_SoundList );
		Cmd_AddCommand( "s_stop", S_StopAllSounds );
		Cmd_AddCommand( "s_info", S_SoundInfo );

		/*cv = Cvar_Get( "s_useOpenAL", "1", CVAR_ARCHIVE );
		if( cv->integer ) {
			//OpenAL
			started = S_AL_Init( &si );
			Cvar_Set( "s_backend", "OpenAL" );
		}*/

		//if( !started ) {
			started = S_Base_Init( &si );
			Cvar_Set( "s_backend", "base" );
		//}

		if( started ) {
			if( !S_ValidSoundInterface( &si ) ) {
				Com_Error( ERR_FATAL, "Sound interface invalid" );
			}

			S_SoundInfo( );
			Com_Printf( "Sound initialization successful.\n" );
		} else {
			Com_Printf( "Sound initialization failed.\n" );
		}
	}

	Com_Printf( "--------------------------------\n");
}


/*
=================
S_Shutdown
=================
*/
void S_Shutdown( void )
{
	if ( si.StopAllSounds ) {
		si.StopAllSounds();
	}

	if( si.Shutdown ) {
		si.Shutdown( );
	}

	Com_Memset( &si, 0, sizeof( soundInterface_t ) );

	Cmd_RemoveCommand( "play" );
	Cmd_RemoveCommand( "music");
	Cmd_RemoveCommand( "music_queue" );
	Cmd_RemoveCommand( "stream" );
	Cmd_RemoveCommand( "stopmusic");
	Cmd_RemoveCommand( "s_list" );
	Cmd_RemoveCommand( "s_stop" );
	Cmd_RemoveCommand( "s_info" );

	S_CodecShutdown( );
}

