#include <GL/glut.h>
#include <windows.h>
#include "src/game.h"
#include "src/render.h"

game_ctx g_ctx;
int time_last = 0;

void reshape(int w, int h) {
    if(h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WD, 0, HT); // mantém proporcao logica, escalando o conteudo responsivamente
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    render_draw(&g_ctx);
}

void update() {
    int cur_t = glutGet(GLUT_ELAPSED_TIME);
    float dt = (cur_t - time_last) / 1000.0f;
    time_last = cur_t;

    if(dt > 0.1f) dt = 0.1f;

    bool shift_pressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

    game_update(&g_ctx, dt, shift_pressed);

    glutPostRedisplay();
}

void mouse_click(int btn, int st, int x, int y) {
    game_click(&g_ctx, btn, st, x, y);
}

void key_down(unsigned char key, int x, int y) {
    if(key == 27) { // tecla esc
        if (g_ctx.state == STATE_PLAY || g_ctx.state == STATE_OVER) {
            g_ctx.state = STATE_MENU;
            g_ctx.diff_dropdown_open = false;
        } else {
            exit(0);
        }
    }
}

void special_key_down(int key, int x, int y) {
    if(key == GLUT_KEY_LEFT) {
        g_ctx.p.dir = -1;
    } else if(key == GLUT_KEY_RIGHT) {
        g_ctx.p.dir = 1;
    }
}

void special_key_up(int key, int x, int y) {
    if(key == GLUT_KEY_LEFT && g_ctx.p.dir == -1) {
        g_ctx.p.dir = 0;
    } else if(key == GLUT_KEY_RIGHT && g_ctx.p.dir == 1) {
        g_ctx.p.dir = 0;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    
    //pega o tamanho do monitor para iniciar o jogo perfeitamente no centro
    int screen_w = GetSystemMetrics(SM_CXSCREEN);
    int screen_h = GetSystemMetrics(SM_CYSCREEN);
    
    glutInitWindowPosition((screen_w - WD) / 2, (screen_h - HT) / 2);
    glutInitWindowSize(WD, HT);
    glutCreateWindow("BAR GAME");

    // suavisacao para as fontes desenhadas por linha ficarem polidas
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    render_init();
    game_init(&g_ctx);
    time_last = glutGet(GLUT_ELAPSED_TIME);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(update);
    glutMouseFunc(mouse_click);
    glutKeyboardFunc(key_down);
    glutSpecialFunc(special_key_down);
    glutSpecialUpFunc(special_key_up);

    glutMainLoop();

    return 0;
}