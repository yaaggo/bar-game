#ifndef AUDIO_H
#define AUDIO_H

void audio_play_music();
void audio_stop_music();
void audio_set_music_volume(int volume);
void audio_play_sfx(const char* path);
void update_audio_loop();

#endif // AUDIO_H
