#include "audio.h"
#include <windows.h>
#include <mmsystem.h>
#include <cstdio>
#include <cstring>

static bool music_open = false;
static int music_volume = 25;

static int clamp_volume(int volume) {
    if(volume < 0) return 0;
    if(volume > 100) return 100;
    return volume;
}

void audio_set_music_volume(int volume) {
    music_volume = clamp_volume(volume);
    if(!music_open) return;

    char command[64];
    snprintf(command, sizeof(command), "setaudio bg_music volume to %d", music_volume * 10);
    mciSendStringA(command, NULL, 0, NULL);
}

void audio_play_music() {
    if (music_open) return;

    mciSendStringA("stop bg_music", NULL, 0, NULL);
    mciSendStringA("close bg_music", NULL, 0, NULL);

    char error_buf[128];

    DWORD err = mciSendStringA("open \"audio/musica.wav\" alias bg_music", NULL, 0, NULL);
    if (err != 0) {
        mciGetErrorStringA(err, error_buf, sizeof(error_buf));
        printf("[MCI ERRO ABRIR]: %s\n", error_buf);
        music_open = false;
        return;
    }

    music_open = true;
    audio_set_music_volume(music_volume);

    err = mciSendStringA("play bg_music repeat", NULL, 0, NULL);

    if (err != 0) {
        err = mciSendStringA("play bg_music", NULL, 0, NULL);
        if (err != 0) {
            mciGetErrorStringA(err, error_buf, sizeof(error_buf));
            printf("[MCI ERRO TOCAR]: %s\n", error_buf);
            audio_stop_music();
            music_open = false;
            return;
        }
    }
}

void update_audio_loop(){
    if(!music_open) return; 
    char status_buf[128];
    mciSendStringA("status bg_music mode", status_buf, sizeof(status_buf), NULL);
    if(!strcmp(status_buf, "stopped")) {
        mciSendStringA("play bg_music from 0", NULL, 0, NULL);
    }
}

void audio_play_sfx(const char* path) {    
    PlaySoundA(path, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}

void audio_stop_music() {
    if(!music_open) return;
    mciSendStringA("stop bg_music", NULL, 0, NULL);
    mciSendStringA("close bg_music", NULL, 0, NULL);
    music_open = false;
}
