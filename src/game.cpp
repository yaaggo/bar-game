#include "game.h"
#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <mmsystem.h>

void game_init(game_ctx* c) {
    srand((unsigned)time(0));
    c->state = STATE_MENU;
    c->diff = DIFF_MED;
    c->diff_dropdown_open = false;
    c->points = 0;
    c->lives = 3;
    
    c->p.w = 100.0f; 
    c->p.h = 20.0f;
    c->p.x = WD / 2.0f - c->p.w / 2.0f; 
    c->p.y = 30.0f;
    c->p.speed = 400.0f; 
    c->p.dash_speed = 1500.0f;
    c->p.dir = 0;
    
    c->spawn_timer = 1.0f; 
    c->time_since_spawn = 0.0f;
    c->items.clear();
}

void game_reset(game_ctx* c) {
    c->points = 0; 
    c->lives = 3;
    c->p.x = WD / 2.0f - c->p.w / 2.0f;
    c->items.clear(); 
    c->time_since_spawn = 0.0f; 
    c->p.dir = 0;
    
    if(c->diff == DIFF_EASY) c->spawn_timer = 1.5f;
    else if(c->diff == DIFF_MED) c->spawn_timer = 1.0f;
    else c->spawn_timer = 0.5f;
}

bool game_check_col(const player_bar* b, const falling_item* i) {
    bool cx = b->x + b->w >= i->x && i->x + i->w >= b->x;
    bool cy = b->y + b->h >= i->y && i->y + i->h >= b->y;
    return cx && cy;
}

void game_spawn(game_ctx* c) {
    falling_item i;
    i.w = 80.0f; i.h = 80.0f;
    i.y = (float)HT; 
    i.active = true;

    float base = 200.0f;
    if(c->diff == DIFF_EASY) base = 150.0f;
    else if(c->diff == DIFF_HARD) base = 300.0f;
    
    float var = (rand() % 40 - 20) / 100.0f;
    i.speed = base * (1.0f + var);

    int r = rand() % 100;
    if(r < 50) i.type = TYPE_WHITE;
    else if(r < 75) i.type = TYPE_YELLOW;
    else if(r < 85) i.type = TYPE_RED;
    else i.type = TYPE_BLACK;

    bool valid_x = false;
    float safe_dist = c->p.w + 50.0f; 
    
    for (int attempts = 0; attempts < 15; attempts++) {
        i.x = (float)(rand() % (int)(WD - i.w));
        valid_x = true;
        
        for (size_t j = 0; j < c->items.size(); j++) {
            falling_item* other = &c->items[j];
            if (!other->active) continue;
            
            bool is_new_bomb = (i.type == TYPE_BLACK);
            bool is_other_bomb = (other->type == TYPE_BLACK);
            
            if (is_new_bomb != is_other_bomb) {
                if (abs(i.y - other->y) < 300.0f) {
                    if (abs(i.x - other->x) < safe_dist) {
                        valid_x = false;
                        break;
                    }
                }
            }
        }
        if (valid_x) break;
    }

    c->items.push_back(i);
}

void game_update(game_ctx* c, float dt, bool shift_pressed) {
    if(c->state != STATE_PLAY) return;

    float spd = shift_pressed ? c->p.dash_speed : c->p.speed;
    c->p.x += c->p.dir * spd * dt;

    if(c->p.x < 0) c->p.x = 0;
    if(c->p.x + c->p.w > WD) c->p.x = WD - c->p.w;

    c->time_since_spawn += dt;
    if(c->time_since_spawn >= c->spawn_timer) {
        game_spawn(c);
        c->time_since_spawn = 0.0f;
    }

    int active_cnt = 0;
    for(size_t i = 0; i < c->items.size(); i++) {
        falling_item* it = &c->items[i];
        if(!it->active) continue;

        it->y -= it->speed * dt;

        if(game_check_col(&c->p, it)) {
            it->active = false;
            if(it->type == TYPE_BLACK) {
                PlaySound(TEXT("audio/explosion.wav"), NULL, SND_FILENAME | SND_ASYNC);
                c->state = STATE_OVER;
            } else {
                PlaySound(TEXT("audio/pickupCoin.wav"), NULL, SND_FILENAME | SND_ASYNC);
                if(it->type == TYPE_WHITE) c->points += 10;
                else if(it->type == TYPE_YELLOW) c->points += 30;
                else if(it->type == TYPE_RED) c->points += 100;
            }
        }
        else if(it->y + it->h < 0) {
            it->active = false;
            if(it->type != TYPE_BLACK) {
                PlaySound(TEXT("audio/vida.wav"), NULL, SND_FILENAME | SND_ASYNC);
                c->lives--;
                if(c->lives <= 0) c->state = STATE_OVER;
            }
        }

        if(it->active) c->items[active_cnt++] = *it;
    }
    c->items.resize(active_cnt);
}

void game_click(game_ctx* c, int btn, int st, int x, int y) {
    if(btn != 0 || st != 0) return; 

    // converter resolucao da janela para logica (WDxHT)
    int win_w = glutGet(GLUT_WINDOW_WIDTH);
    int win_h = glutGet(GLUT_WINDOW_HEIGHT);
    float lx = (float)x / win_w * WD;
    float ly = (float)(win_h - y) / win_h * HT;

    if(c->state == STATE_MENU) {
        // Dropdown Header (X: 300-500, Y: 350-400)
        if(lx >= 300 && lx <= 500 && ly >= 350 && ly <= 400) {
            c->diff_dropdown_open = !c->diff_dropdown_open;
        } 
        else if (c->diff_dropdown_open) {
            // Facil (X: 300-500, Y: 300-350)
            if(lx >= 300 && lx <= 500 && ly >= 300 && ly <= 350) {
                c->diff = DIFF_EASY; c->diff_dropdown_open = false;
            }
            // Medio (X: 300-500, Y: 250-300)
            else if(lx >= 300 && lx <= 500 && ly >= 250 && ly <= 300) {
                c->diff = DIFF_MED; c->diff_dropdown_open = false;
            }
            // Dificil (X: 300-500, Y: 200-250)
            else if(lx >= 300 && lx <= 500 && ly >= 200 && ly <= 250) {
                c->diff = DIFF_HARD; c->diff_dropdown_open = false;
            } else {
                c->diff_dropdown_open = false; // clica fora, fecha dropdown
            }
        }
        else {
            // PLAY (X: 300-500, Y: 100-160)
            if(lx >= 300 && lx <= 500 && ly >= 100 && ly <= 160) {
                game_reset(c);
                c->state = STATE_PLAY;
            }
        }
    }
    else if(c->state == STATE_OVER) {
        // voltar ao menu
        if(lx >= 300 && lx <= 500 && ly >= 200 && ly <= 250) {
            c->state = STATE_MENU;
        }
    }
}