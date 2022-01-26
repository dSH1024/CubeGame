#ifndef MAIN_H
#define MAIN_H

#define LENGHT_BORDER 16
#define WIDTH_BORDER  16
#define HEIGHT_BORDER 256


typedef struct
{
    GLfloat r, g, b;
} TColor;

typedef struct
{
    GLuint x, y, z;
} TPos;

typedef struct
{
    TPos pos;
    TColor color;
    unsigned int id;
    BOOL active;
} TBlock;

TBlock block[16][16][256];

GLfloat vertex[] = { 0, 0, 0,  0, 1, 0,  1, 1, 0,  1, 0, 0 };
GLfloat crossVert[] = { 0, -0.1,  0, 0.1,  -0.1, 0,  0.1, 0 };
GLfloat texVertex[] = { 0, 0,  0, 1,  1, 1,  1, 0 };
unsigned char textures[2];

BOOL showMask;

HWND hwnd;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);
void worldShow();
void gameShow();

#endif
