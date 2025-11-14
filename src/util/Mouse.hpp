#pragma once
#include <GLFW/glfw3.h>
#include <queue>

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

struct MouseEvent {
    int button;
    bool state;
    double x;
    double y;
    double dwheel;
};

class Mouse {
private:
    static std::queue<MouseEvent> events;
    static MouseEvent currentEvent;
    static double mouseX, mouseY;
    static double deltaX, deltaY;
    static double deltaWheel;
    static bool grabbed;
    static GLFWwindow* window;

    #ifdef __EMSCRIPTEN__
        static EM_BOOL mouseDownCallback(int eventType, const EmscriptenMouseEvent *e, void *userData);
        static EM_BOOL mouseUpCallback(int eventType, const EmscriptenMouseEvent *e, void *userData);
        static EM_BOOL mouseClickCallback(int eventType, const EmscriptenMouseEvent *e, void *userData);
        static EM_BOOL mouseMoveCallback(int eventType, const EmscriptenMouseEvent *e, void *userData);
        static EM_BOOL mouseWheelCallback(int eventType, const EmscriptenWheelEvent *e, void *userData);
        static EM_BOOL pointerlockChangeCallback(int eventType, const EmscriptenPointerlockChangeEvent *e, void *userData);
        static EM_BOOL pointerlockErrorCallback(int eventType, const void *reserved, void *userData);
        
        static void requestPointerLock();
        static void exitPointerLock();
    #endif

public:
    Mouse(GLFWwindow* win);
    ~Mouse();

    static void create();
    static void destroy();
    static void init(GLFWwindow* win);
    static bool next();
    static int getEventButton();
    static bool getEventButtonState();
    static double getEventX();
    static double getEventY();
    static double getEventDWheel();
    static double getX();
    static double getY();
    static double getDX();
    static double getDY();
    static double getDWheel();
    static void setCursorPosition(int x, int y);
    static void setGrabbed(bool grab);
    static bool isGrabbed();
    static void poll();

};
