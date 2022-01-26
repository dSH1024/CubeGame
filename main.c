#include <Windows.h>
#include <GL/gl.h>
#include <time.h>

#include "main.h"
#include "Camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../BeginEnd/common/stb_image.h"

#pragma comment (lib, "opengl32.lib")


BOOL isAbroad(int a, int range)
{
    return (a < 0) || (a >= range);
}

void winResize(float x, float y)
{
    glLoadIdentity();
    glViewport(0, 0, x, y);

    glMatrixMode(GL_PROJECTION);

    float ratio = x / (float)y;
    glLoadIdentity();
    glFrustum(-(ratio * 0.1), ratio * 0.1, -0.1, 0.1, 0.2, 1000);

    glMatrixMode(GL_MODELVIEW);
}

void playerMove()
{
    Camera_MoveDirection(GetKeyState('W') < 0 ? 1 : (GetKeyState('S') < 0 ? -1 : 0),
        GetKeyState('D') < 0 ? 1 : (GetKeyState('A') < 0 ? -1 : 0),
        0.2);
    Camera_AutoMoveByMouse(400, 400, 0.1);

    if (GetKeyState(VK_SPACE) < 0)
        camera.z += 0.2;

    else if (GetKeyState(VK_SHIFT) < 0)
        camera.z -= 0.2;
}

void loadTexture(char* path, int number)
{
    int width, height, cnt;
    unsigned char* data = stbi_load(path, &width, &height, &cnt, 0);

    glGenTextures(1, &textures[number]);
    glBindTexture(GL_TEXTURE_2D, textures[number]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

void cubeDestroy()
{
    glDisable(GL_TEXTURE_2D);

    showMask = TRUE;
    gameShow();
    showMask = FALSE;

    RECT rct;
    GetClientRect(hwnd, &rct);

    GLubyte pix[3];
    glReadPixels(rct.right / 2.0, rct.bottom / 2.0, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pix);

    block[pix[0]][pix[1]][pix[2]].active = FALSE;

    glEnable(GL_TEXTURE_2D);
}

void crossDraw()
{
    if (!showMask)
    {
        glDisable(GL_TEXTURE_2D);
        glPushMatrix();
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(2, GL_FLOAT, 0, crossVert);
            glColor3f(0.5, 0.5, 0.5);
            glDrawArrays(GL_LINES, 0, 4);
            glDisableClientState(GL_VERTEX_ARRAY);
        glPopMatrix();
        glEnable(GL_TEXTURE_2D);
    }
}

void rectDraw()
{
    glPushMatrix();
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vertex);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
}

void cubeDraw(unsigned int id, int coordX, int coordY, int coordZ)
{   
    if (!showMask)
        glBindTexture(GL_TEXTURE_2D, textures[id]);

    glPushMatrix();

        if (!showMask)
        {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, texVertex);
        }
        
        // низ
        if ((!block[coordX][coordY][coordZ - 1].active) || (isAbroad(coordZ - 1, 256)))
            rectDraw();

        // вверх
        if ((!block[coordX][coordY][coordZ + 1].active) || (isAbroad(coordZ + 1, 256)))
        {
            glPushMatrix();
                glTranslatef(0, 0, 1);
                rectDraw();
            glPopMatrix();
        }

        // передн€€ часть
        if ((!block[coordX][coordY - 1][coordZ].active) || (isAbroad(coordY - 1, 16)))
        {
            glPushMatrix();
                glRotatef(90, 1, 0, 0);
                rectDraw();
            glPopMatrix();
        }

        // задн€€ часть
        if ((!block[coordX][coordY + 1][coordZ].active) || (isAbroad(coordY + 1, 16)))
        {
            glPushMatrix();
                glTranslatef(0, 1, 0);
                glRotatef(90, 1, 0, 0);
                rectDraw();
            glPopMatrix();
        }

        // права€ часть
        if ((!block[coordX + 1][coordY][coordZ].active) || (isAbroad(coordX + 1, 16)))
        {
            glPushMatrix();
                glTranslatef(1, 0, 0);
                glRotatef(-90, 0, 1, 0);
                rectDraw();
            glPopMatrix();
        }

        // лева€ часть
        if ((!block[coordX - 1][coordY][coordZ].active) || (isAbroad(coordX - 1, 16)))
        {
            glPushMatrix();
                glRotatef(-90, 0, 1, 0);
                rectDraw();
            glPopMatrix();
        }

        if (!showMask)
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();
}

void worldInit()
{
    srand(time(NULL));
    glEnable(GL_TEXTURE_2D);
    loadTexture("textures/dirt.png", 0);
    loadTexture("textures/grass_top.png", 1);

    for (int i = 0; i < LENGHT_BORDER; i++)
    {
        for (int j = 0; j < WIDTH_BORDER; j++)
        {
            for (int k = 0; k < HEIGHT_BORDER; k++)
            {
                block[i][j][k].pos.x = i;
                block[i][j][k].pos.y = j;
                block[i][j][k].pos.z = k;

                block[i][j][k].id = rand() % 2;

                if (block[i][j][k].id == 1)
                {
                    block[i][j][k].color.r = 0.25;
                    block[i][j][k].color.g = 0.75;
                    block[i][j][k].color.b = 0.25;
                }
                else
                {
                    block[i][j][k].color.r = 0.75;
                    block[i][j][k].color.g = 0.75;
                    block[i][j][k].color.b = 0.75;
                }


                if (k < 60)
                    block[i][j][k].active = TRUE;
                else
                    block[i][j][k].active = FALSE;
            }
        }
    }
}

void worldShow()
{
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            for (int k = 0; k < 256; k++)
            {
                glPushMatrix();
                    glTranslatef(block[i][j][k].pos.x, block[i][j][k].pos.y, block[i][j][k].pos.z);

                    if (showMask)
                        glColor3ub(block[i][j][k].pos.x, block[i][j][k].pos.y, block[i][j][k].pos.z);
                    else
                        glColor3f(block[i][j][k].color.r, block[i][j][k].color.g, block[i][j][k].color.b);

                    if (block[i][j][k].active)
                        cubeDraw(block[i][j][k].id, i, j, k);
                glPopMatrix();
            }
        }
    }
}

void gameInit()
{
    RECT rct;
    GetClientRect(hwnd, &rct);
    winResize(rct.right, rct.bottom);

    glEnable(GL_DEPTH_TEST);
    glLineWidth(4);

    worldInit();
    showMask = FALSE;
}

void gameShow()
{
    if (showMask)
        glClearColor(0, 0, 0, 0);
    else
        glClearColor(0.53, 0.81, 0.98, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
        glPushMatrix();
            if (GetForegroundWindow() == hwnd)
                playerMove();

            Camera_Apply();
            worldShow();
        glPopMatrix();
        glTranslatef(0, 0, -0.2);
        glScalef(0.05, 0.05, 0.05);
        crossDraw();
    glPopMatrix();
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0, "GLSample", "Cube Game", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    gameInit();

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            gameShow();

            SwapBuffers(hDC);
            Sleep(1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_DESTROY:
        return 0;

    case WM_KEYDOWN:
    {
        switch (wParam)
        {
            case VK_ESCAPE:
                PostQuitMessage(0);
                break;
        }

        break;
    }
    
    case WM_SIZE:
    {
        winResize(LOWORD(lParam), HIWORD(lParam));
        break;
    }

    case WM_SETCURSOR:
    {
        SetCursor(FALSE);
        break;
    }

    case WM_LBUTTONDOWN:
    {
        cubeDestroy();
        break;
    }

    case WM_RBUTTONDOWN:
    {
        // TODO implement
        break;
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
