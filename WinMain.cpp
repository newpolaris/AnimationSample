#pragma warning(disable : 28251)
#pragma warning(disable : 28159)

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include "glad.h"
#undef APIENTRY
#include <Windows.h>
#include <iostream>
#include <ShellScalingAPI.h>
#include "Application.h"

// Nuklear
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"
extern "C" {
    NK_API nk_context* nk_win32_init();
    NK_API int nk_win32_handle_event(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, float scaleFactor);
    NK_API void nk_win32_render(int width, int height, int display_width, int display_height);
    NK_API void nk_blank_render(int width, int height, int display_width, int display_height);
    NK_API void nk_win32_shutdown(void);
    NK_API void nk_demo();
}

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC, HGLRC, const int*);

typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int);
typedef int (WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

#if _DEBUG
#include <crtdbg.h>
#pragma comment(linker, "/subsystem:console")
int main(int argc, const char** argv) {
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
    return WinMain(GetModuleHandle(NULL), NULL,
        GetCommandLineA(), SW_SHOWDEFAULT);
}
#else
#ifdef _WIN64
#pragma comment(linker, "/subsystem:windows,5.02")
#else
#pragma comment(linker, "/subsystem:windows,5.01")
#endif
#endif

// App antry & setup
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "Shcore.lib")

Application* gApplication = 0;
GLuint gVertexArrayObject = 0;
GLuint gGpuApplicationStart = 0;
GLuint gGpuApplicationStop = 0;
GLuint gGpuImguiStart = 0;
GLuint gGpuImguiStop = 0;
nk_context* gNkContext = 0;
float gScaleFactor = 1.0f;
float gInvScaleFactor = 1.0f;

// CPU Frame Timers
struct FrameTimer {
    // High level timers
    double frameTime = 0.0;
    float deltaTime = 0.0f;
    // CPU timers
    double frameUpdate = 0.0;
    double frameRender = 0.0;
    double win32Events = 0.0;
    double imguiLogic = 0.0;
    double imguiRender = 0.0;
    double swapBuffer = 0.0;
    // GPU timers
    double imguiGPU = 0.0;
    double appGPU = 0.0;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    HRESULT hr = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

    gApplication = new Application();

    WNDCLASSEX wndclass;
    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wndclass.lpszMenuName = 0;
    wndclass.lpszClassName = L"Win32 Game Window";
    RegisterClassEx(&wndclass);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int clientWidth = 800;
    int clientHeight = 600;

    UINT DPI = GetDpiForSystem();
    gScaleFactor = (float)DPI / 96.0f;
    gInvScaleFactor = 1.0f / gScaleFactor;

    clientWidth = (int)((float)clientWidth * gScaleFactor);
    clientHeight = (int)((float)clientHeight * gScaleFactor);

    RECT windowRect;
    SetRect(&windowRect,
        (screenWidth / 2) - (clientWidth / 2),
        (screenHeight / 2) - (clientHeight / 2),
        (screenWidth / 2) + (clientWidth / 2),
        (screenHeight / 2) + (clientHeight / 2));

    DWORD style = (WS_OVERLAPPED | WS_CAPTION |
        WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);

    AdjustWindowRectEx(&windowRect, style, FALSE, 0);
    HWND hwnd = CreateWindowEx(0, wndclass.lpszClassName,
        L"Game Window", style, windowRect.left,
        windowRect.top, windowRect.right -
        windowRect.left, windowRect.bottom -
        windowRect.top, NULL, NULL,
        hInstance, szCmdLine);
    HDC hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 32;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    HGLRC tempRC = wglCreateContext(hdc);
    wglMakeCurrent(hdc, tempRC);
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    const int attribList[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB, 0,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    HGLRC hglrc = wglCreateContextAttribsARB(hdc, 0, attribList);

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(tempRC);
    wglMakeCurrent(hdc, hglrc);

    if (!gladLoadGL()) {
        std::cout << "Could not initialize GLAD\n";
    }
    else {
        std::cout << "OpenGL Version " << GLVersion.major << "." << GLVersion.minor << "\n";
    }

    // Enable vsync
    PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT =
        (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
    const char* extensions = _wglGetExtensionsStringEXT();
    bool swapControlSupported = strstr(extensions, "WGL_EXT_swap_contro") != 0;

    int vsync = 0;
    if (swapControlSupported) {
        PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
            (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
        PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT =
            (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
        if (wglSwapIntervalEXT(1)) {
            std::cout << "Enabled vsync\n";
            vsync = wglGetSwapIntervalEXT();
        }
        else {
            std::cout << "Could not enable vsync\n";
        }
    }
    else { // !swapControlSupported
        std::cout << "WGL_EXT_swap_control not supported\n";
    }

    // GPU Timers
    bool slowFrame = false;
    bool firstRenderSample = true;
    GLint timerResultAvailable = 0;
    GLuint64 gpuStartTime = 0;
    GLuint64 gpuStopTime = 0;

    glGenQueries(1, &gGpuApplicationStart);
    glGenQueries(1, &gGpuApplicationStop);
    glGenQueries(1, &gGpuImguiStart);
    glGenQueries(1, &gGpuImguiStop);

    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    gApplication->Initialize();
    gNkContext = nk_win32_init();

    // Nuklear messes with these states
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    clientWidth = clientRect.right - clientRect.left;
    clientHeight = clientRect.bottom - clientRect.top;
    glViewport(0, 0, clientWidth, clientHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPointSize(5.0f * gScaleFactor);
    glLineWidth(1.5f * gScaleFactor);
    glBindVertexArray(gVertexArrayObject);

    // CPU timings
    LARGE_INTEGER timerFrequency;
    LARGE_INTEGER timerStart;
    LARGE_INTEGER timerStop;
    LARGE_INTEGER frameStart;
    LARGE_INTEGER frameStop;
    LONGLONG timerDiff;

    FrameTimer display;
    FrameTimer accumulator;
    memset(&display, 0, sizeof(display));
    memset(&accumulator, 0, sizeof(accumulator));
    int frameCounter = 0;

    bool enableFameTiming = true;
    if (!QueryPerformanceFrequency(&timerFrequency)) {
        std::cout << "WinMain: QueryPerformanceFrequency failed\n";
        enableFameTiming = false;
    }

    // Get Display Frequency
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &monitorInfo);

    DEVMODE devMode;
    devMode.dmSize = sizeof(DEVMODE);
    devMode.dmDriverExtra = 0;
    EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    int displayFrequency = (int)devMode.dmDisplayFrequency;
    double frameBudget = (1000.0 / (double)displayFrequency);
    std::cout << "Display frequency: " << displayFrequency << "\n";
    std::cout << "Frame budget: " << frameBudget << " milliseconds\n";

    // Display helpers
    nk_color defaultColor = gNkContext->style.text.color;
    nk_color red = { 255, 0, 0, 255 };
    nk_color orange = { 255, 165, 0, 255 };
    char printBuffer[512];

    // Win loop
    DWORD lastTick = GetTickCount();
    MSG msg;
    while (true) {
        // Win32 events
        QueryPerformanceCounter(&timerStart);
        if (gNkContext != 0 && gVertexArrayObject != 0) {
            nk_input_begin(gNkContext);
        }
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (gNkContext != 0 && gVertexArrayObject != 0) {
            nk_input_end(gNkContext);
        }
        QueryPerformanceCounter(&timerStop);
        timerDiff = timerStop.QuadPart - timerStart.QuadPart;
        accumulator.win32Events += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;

        QueryPerformanceCounter(&frameStart);

        // Update
        QueryPerformanceCounter(&timerStart);

        DWORD thisTick = GetTickCount();
        float dt = float(thisTick - lastTick) * 0.001f;
        lastTick = thisTick;
        if (gApplication != 0) {
            gApplication->Update(dt);
        }
        QueryPerformanceCounter(&timerStop);
        timerDiff = timerStop.QuadPart - timerStart.QuadPart;
        accumulator.frameUpdate += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;

        if (gApplication != 0) {
            // Render
            QueryPerformanceCounter(&timerStart);
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            clientWidth = clientRect.right - clientRect.left;
            clientHeight = clientRect.bottom - clientRect.top;
            glViewport(0, 0, clientWidth, clientHeight);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glPointSize(5.0f * gScaleFactor);
            glLineWidth(1.5f * gScaleFactor);

            glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glBindVertexArray(gVertexArrayObject);
            if (!firstRenderSample) { // Application GPU Timer
                glGetQueryObjectiv(gGpuApplicationStop, GL_QUERY_RESULT, &timerResultAvailable);
                while (!timerResultAvailable) {
                    std::cout << "Waiting on app GPU timer!\n";
                    glGetQueryObjectiv(gGpuApplicationStop, GL_QUERY_RESULT, &timerResultAvailable);
                }
                glGetQueryObjectui64v(gGpuApplicationStart, GL_QUERY_RESULT, &gpuStartTime);
                glGetQueryObjectui64v(gGpuApplicationStop, GL_QUERY_RESULT, &gpuStopTime);
                accumulator.appGPU += (double)(gpuStopTime - gpuStartTime) / 1000000.0;
            }
            float aspect = (float)clientWidth / (float)clientHeight;

            glQueryCounter(gGpuApplicationStart, GL_TIMESTAMP);
            gApplication->Render(aspect);
            glQueryCounter(gGpuApplicationStop, GL_TIMESTAMP);
        }
        QueryPerformanceCounter(&timerStop);
        timerDiff = timerStop.QuadPart - timerStart.QuadPart;
        accumulator.frameRender += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;

        // IMGUI Update
        QueryPerformanceCounter(&timerStart);
        if (gNkContext != 0 && gVertexArrayObject != 0) {
            float imguiXPosition = ((float)clientWidth * gInvScaleFactor) - 205.0f;
            if (nk_begin(gNkContext, "Display Stats", nk_rect(imguiXPosition, 5.0f, 200.0f, 65.0f), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
                nk_layout_row_static(gNkContext, 15, 200, 1);

                sprintf(printBuffer, "Display frequency: %d\0", displayFrequency);
                nk_label(gNkContext, printBuffer, NK_TEXT_LEFT);

                if (vsync != 0) {
                    nk_label(gNkContext, "VSync: on", NK_TEXT_LEFT);
                }
                else {
                    nk_label(gNkContext, "VSync: off", NK_TEXT_LEFT);
                }
                sprintf(printBuffer, "Frame budget: %0.2f ms\0", frameBudget);
                nk_label(gNkContext, printBuffer, NK_TEXT_LEFT);
            }
            nk_end(gNkContext);

            if (nk_begin(gNkContext, "High Level Timers", nk_rect(imguiXPosition, 75.0f, 200.0f, 45.0f),
                NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
                nk_layout_row_static(gNkContext, 15, 200, 1);
                gNkContext->style.text.color = slowFrame ? red : defaultColor;

                sprintf(printBuffer, "Frame Time: %0.5f ms\0", display.frameTime);
                nk_label(gNkContext, printBuffer, NK_TEXT_LEFT);

                sprintf(printBuffer, "Delta Time: %0.5f ms\0", display.deltaTime);
                nk_label(gNkContext, printBuffer, NK_TEXT_LEFT);

                gNkContext->style.text.color = defaultColor;
            }
            nk_end(gNkContext);
        }
        QueryPerformanceCounter(&timerStop);
        timerDiff = timerStop.QuadPart - timerStart.QuadPart;
        accumulator.imguiLogic += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;

        // Imgui Render
        QueryPerformanceCounter(&timerStart);
        if (gNkContext != 0 && gVertexArrayObject != 0) {
            if (!firstRenderSample) { // Imgui GPU Timer
                glGetQueryObjectiv(gGpuImguiStop, GL_QUERY_RESULT, &timerResultAvailable);
                while (!timerResultAvailable) {
                    std::cout << "Waiting on imgui GPU timer!\n";
                    glGetQueryObjectiv(gGpuImguiStop, GL_QUERY_RESULT, &timerResultAvailable);
                }
                glGetQueryObjectui64v(gGpuImguiStart, GL_QUERY_RESULT, &gpuStartTime);
                glGetQueryObjectui64v(gGpuImguiStop, GL_QUERY_RESULT, &gpuStopTime);
                accumulator.imguiGPU += (double)(gpuStopTime - gpuStartTime) / 1000000.0;
            }

            glQueryCounter(gGpuImguiStart, GL_TIMESTAMP);
            nk_win32_render((int)((float)clientWidth * gInvScaleFactor), (int)((float)clientHeight * gInvScaleFactor), clientWidth, clientHeight);
            glQueryCounter(gGpuImguiStop, GL_TIMESTAMP);
        }
        QueryPerformanceCounter(&timerStop);
        timerDiff = timerStop.QuadPart - timerStart.QuadPart;
        accumulator.imguiRender += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;

        if (gApplication != 0) {
            SwapBuffers(hdc);
            if (vsync != 0) {
                glFinish();
            }
        }
        QueryPerformanceCounter(&timerStop);
        timerDiff = timerStop.QuadPart - timerStart.QuadPart;
        accumulator.swapBuffer += (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;

        QueryPerformanceCounter(&frameStop);
        timerDiff = frameStop.QuadPart - frameStart.QuadPart;
        double frameTime = (double)timerDiff * 1000.0 / (double)timerFrequency.QuadPart;
        accumulator.frameTime += frameTime;

        // Profiling house keeping
        firstRenderSample = false;
        if (++frameCounter >= 60) {
            frameCounter = 0;

            display.win32Events = accumulator.win32Events / 60.0;
            display.frameUpdate = accumulator.frameUpdate / 60.0;
            display.frameRender = accumulator.frameRender / 60.0;
            display.imguiLogic = accumulator.imguiLogic / 60.0;
            display.imguiRender = accumulator.imguiRender / 60.0;
            display.swapBuffer = accumulator.swapBuffer / 60.0;
            display.frameTime = accumulator.frameTime / 60.0;
            display.deltaTime = accumulator.deltaTime / 60.0f;
            display.appGPU = accumulator.appGPU / 60.0;
            display.imguiGPU = accumulator.imguiGPU / 60.0;

            memset(&accumulator, 0, sizeof(FrameTimer));
            slowFrame = display.frameTime >= frameBudget;
        }

    } // End of game loop

    if (gApplication != 0) {
        std::cout << "Expected application to be null on exit\n";
        delete gApplication;
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    switch (iMsg) {
    case WM_CLOSE:
        if (gApplication != 0) {
            gApplication->Initialize();
            delete gApplication;
            gApplication = 0;
            DestroyWindow(hwnd);
        }
        else {
            std::cout << "Already shut down!\n";
        }
        break;
    case WM_DESTROY:
        if (gVertexArrayObject != 0) {
            HDC hdc = GetDC(hwnd);
            HGLRC hglrc = wglGetCurrentContext();

            glDeleteQueries(1, &gGpuApplicationStart);
            glDeleteQueries(1, &gGpuApplicationStop);

            glBindVertexArray(0);
            glDeleteVertexArrays(1, &gVertexArrayObject);
            gVertexArrayObject = 0;

            nk_win32_shutdown();
            gNkContext = 0;

            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(hglrc);
            ReleaseDC(hwnd, hdc);

            PostQuitMessage(0);
        }
        else {
            std::cout << "Multiple destroy message\n";
        }
        break;
    case WM_PAINT:
    case WM_ERASEBKGND:
        return 0;
    }

    if (gNkContext != 0 && gVertexArrayObject != 0) {
        if (nk_win32_handle_event(hwnd, iMsg, wParam, lParam, gInvScaleFactor)) {
            return 0;
        }
    }
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
