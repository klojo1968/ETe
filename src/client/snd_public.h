/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/


// background track queuing
#define QUEUED_PLAY_ONCE    -1
#define QUEUED_PLAY_LOOPED  -2
#define QUEUED_PLAY_ONCE_SILENT -3  ///< when done it goes quiet

void S_Init( void );
void S_Shutdown( void );
void S_Reload( void );

// if origin is NULL, the sound will be dynamically sourced from the entity
void S_StartSound( vec3_t origin, int entnum, int entchannel, sfxHandle_t sfx, int volume );
void S_StartSoundEx( vec3_t origin, int entnum, int entchannel, sfxHandle_t sfx, int flags, int volume );
void S_StartLocalSound( sfxHandle_t sfx, int channelNum, int volume );

void S_StartBackgroundTrack( const char *intro, const char *loop, int fadeUpTime );
void S_StopBackgroundTrack( void );

float S_StartStreamingSound( const char *intro, const char *loop, int entnum, int channel, int attenuation );
void S_StopEntStreamingSound( int entNum );
void S_FadeStreamingSound( float targetvol, int time, int stream );

// cinematics and voice-over-network will send raw samples
// 1.0 volume will be direct output of source samples
void S_RawSamples(int stream, int samples, int rate, int width, int channels,
				   const byte *data, float lvol, float rvol, int entityNum);

// stop all sounds and the background track
void S_ClearSounds(qboolean clearStreaming, qboolean clearMusic);
void S_StopAllSounds(void);
void S_FadeAllSounds(float targetVol, int time, qboolean stopSounds);

// all continuous looping sounds must be added before calling S_Update
void S_ClearLoopingSounds( void );
void S_AddLoopingSound( const vec3_t origin, const vec3_t velocity, const int range, sfxHandle_t sfx, int volume, int soundTime );
void S_AddRealLoopingSound( const vec3_t origin, const vec3_t velocity, const int range, sfxHandle_t sfx, int volume );

// recompute the relative volumes for all running sounds
// relative to the given entityNum / orientation
void S_Respatialize( int entityNum, const vec3_t origin, vec3_t axis[3], int inwater );

// let the sound system know where an entity currently is
void S_UpdateEntityPosition( int entityNum, const vec3_t origin );

void S_Update( int msec );
void S_Reload( void );

void S_DisableSounds( void );

void S_BeginRegistration( void );

// RegisterSound will allways return a valid sample, even if it
// has to create a placeholder.  This prevents continuous filesystem
// checks for missing files
sfxHandle_t	S_RegisterSound( const char *sample, qboolean compressed );

void S_DisplayFreeMemory(void);

int S_GetVoiceAmplitude( int entityNum );

void S_ClearSoundBuffer( qboolean killStreaming );

void SNDDMA_Activate( void );

// START	xkan, 9/23/2002
// returns how long the sound lasts in milliseconds
int S_GetSoundLength( sfxHandle_t sfxHandle );
// END		xkan, 9/23/2002

// ydnar: for looped sound synchronization
int S_GetCurrentSoundTime( void );


#ifdef USE_VOIP
void S_StartCapture( void );
int S_AvailableCaptureSamples( void );
void S_Capture( int samples, byte *data );
void S_StopCapture( void );
void S_MasterGain( float gain );
#endif

