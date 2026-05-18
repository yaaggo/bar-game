#ifndef TYPES_H
#define TYPES_H

#include <vector>

using namespace std;

enum game_state { STATE_MENU, STATE_PLAY, STATE_OVER };
enum difficulty { DIFF_EASY, DIFF_MED, DIFF_HARD };
enum item_type { TYPE_WHITE, TYPE_YELLOW, TYPE_RED, TYPE_BLACK };

struct player_bar {
    float x, y, w, h, speed, dash_speed;
    int dir; // -1 esquerda, 0 parado, 1 direita
};

struct falling_item {
    float x, y, w, h, speed;
    item_type type;
    bool active;
};

struct game_ctx {
    game_state state;
    difficulty diff;
    bool diff_dropdown_open;
    int points, lives;
    player_bar p;
    vector<falling_item> items;
    float spawn_timer, time_since_spawn;
};

#define WD 800
#define HT 600

#endif // TYPES_H