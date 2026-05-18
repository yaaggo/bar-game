#ifndef GAME_H
#define GAME_H

#include "types.h"

void game_init(game_ctx* c);
void game_reset(game_ctx* c);
void game_update(game_ctx* c, float dt, bool shift_pressed);
void game_spawn(game_ctx* c);
bool game_check_col(const player_bar* b, const falling_item* i);
void game_click(game_ctx* c, int btn, int st, int x, int y);

#endif // GAME_H