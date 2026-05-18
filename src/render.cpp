#include "render.h"
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static GLuint bomb_texture = 0;
static GLuint banana_texture = 0;
static GLuint pear_texture = 0;
static GLuint apple_texture = 0;
static GLuint basket_texture = 0;
static GLuint heart_texture = 0;

static GLuint load_texture(const char* filename) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* image = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
    if (!image) {
        printf("Erro ao carregar textura: %s\n", filename);
        return 0;
    }

    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    stbi_image_free(image);
    return tex_id;
}

void render_init() {
    bomb_texture = load_texture("img/bomb.png");
    banana_texture = load_texture("img/banana.png");
    pear_texture = load_texture("img/pear.png");
    apple_texture = load_texture("img/apple.png");
    basket_texture = load_texture("img/fruit-basket.png");
    heart_texture = load_texture("img/heart.png");
}

static void draw_texture(GLuint tex_id, float x, float y, float w, float h) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x + w, y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x + w, y + h);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + h);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
}

static void draw_rect(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y); 
    glVertex2f(x + w, y); 
    glVertex2f(x + w, y + h); 
    glVertex2f(x, y + h);
    glEnd();
}

static void draw_stroke_text(const char* t, float x, float y, float scale, float r, float g, float b, float line_width = 2.0f) {
    glColor3f(r, g, b);
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glLineWidth(line_width);
    for(int i = 0; i < (int)strlen(t); i++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, t[i]);
    }
    glPopMatrix();
    glLineWidth(1.0f);
}

static void draw_stroke_centered(const char* t, float cx, float cy, float scale, float r, float g, float b, float line_width = 2.0f) {
    float text_w = glutStrokeLength(GLUT_STROKE_ROMAN, (const unsigned char*)t) * scale;
    float y_adjust = (119.05f * scale) / 2.0f; // centraliza verticalmente aprox
    draw_stroke_text(t, cx - text_w / 2.0f, cy - y_adjust, scale, r, g, b, line_width);
}

static void draw_button(const char* t, float x, float y, float w, float h, float r, float g, float b) {
    draw_rect(x, y, w, h, r, g, b);
    draw_stroke_centered(t, x + w / 2.0f, y + h / 2.0f, 0.15f, 1.0f, 1.0f, 1.0f, 2.0f);
}

static void render_menu(const game_ctx* c) {
    // titulo enorme e chamativo
    draw_stroke_centered("BAR GAME", WD / 2.0f, 480.0f, 0.45f, 0.1f, 0.1f, 0.1f, 5.0f);

    const char* diff_str = c->diff == DIFF_EASY ? "FACIL" : (c->diff == DIFF_MED ? "MEDIO" : "DIFICIL");
    
    // header do seletor
    draw_button(diff_str, 300.0f, 350.0f, 200.0f, 50.0f, 0.4f, 0.4f, 0.4f);
    
    // indicador visual de dropdown
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(480.0f, 380.0f);
    glVertex2f(490.0f, 380.0f);
    glVertex2f(485.0f, 365.0f);
    glEnd();

    // se o seletor estiver aberto, renderiza por cima
    if (c->diff_dropdown_open) {
        draw_button("FACIL", 300.0f, 300.0f, 200.0f, 50.0f, 0.5f, 0.5f, 0.5f);
        draw_button("MEDIO", 300.0f, 250.0f, 200.0f, 50.0f, 0.5f, 0.5f, 0.5f);
        draw_button("DIFICIL", 300.0f, 200.0f, 200.0f, 50.0f, 0.5f, 0.5f, 0.5f);
    }

    // botao play verde gigante
    draw_button("PLAY", 300.0f, 100.0f, 200.0f, 60.0f, 0.2f, 0.8f, 0.2f);
}

static void render_play(const game_ctx* c) {
    char s_txt[64];
    sprintf(s_txt, "SCORE: %d", c->points);
    draw_stroke_text(s_txt, 20.0f, HT - 40.0f, 0.2f, 0.1f, 0.1f, 0.1f, 2.0f);

    // Desenha as vidas como coracoes (tamanho logico 30x30) no canto direito
    float heart_w = 30.0f;
    float heart_h = 30.0f;
    float gap = 5.0f;
    float start_x = WD - (3 * (heart_w + gap)); 
    for (int i = 0; i < c->lives; i++) {
        draw_texture(heart_texture, start_x + i * (heart_w + gap), HT - 40.0f, heart_w, heart_h);
    }

    draw_texture(basket_texture, c->p.x, c->p.y, c->p.w, c->p.h);

    for(size_t i = 0; i < c->items.size(); i++) {
        const falling_item* it = &c->items[i];
        if(!it->active) continue;

        if (it->type == TYPE_BLACK) {
            draw_texture(bomb_texture, it->x, it->y, it->w, it->h);
        } else if (it->type == TYPE_YELLOW) {
            draw_texture(banana_texture, it->x, it->y, it->w, it->h);
        } else if (it->type == TYPE_WHITE) {
            draw_texture(pear_texture, it->x, it->y, it->w, it->h);
        } else if (it->type == TYPE_RED) {
            draw_texture(apple_texture, it->x, it->y, it->w, it->h);
        }
    }
}

static void render_over(const game_ctx* c) {
    draw_stroke_centered("GAME OVER", WD / 2.0f, 400.0f, 0.4f, 0.8f, 0.1f, 0.1f, 4.0f);
    
    char s_txt[64];
    sprintf(s_txt, "PONTUACAO FINAL: %d", c->points);
    draw_stroke_centered(s_txt, WD / 2.0f, 320.0f, 0.15f, 0.1f, 0.1f, 0.1f, 2.0f);

    draw_button("VOLTAR AO MENU", 300.0f, 200.0f, 200.0f, 50.0f, 0.4f, 0.4f, 0.4f);
}

void render_draw(const game_ctx* c) {
    glClearColor(0.85f, 0.85f, 0.85f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Matrizes já são setadas no reshape para focar na renderizacao limpa
    glLoadIdentity();

    if(c->state == STATE_MENU) render_menu(c);
    else if(c->state == STATE_PLAY) render_play(c);
    else if(c->state == STATE_OVER) render_over(c);

    glutSwapBuffers();
}