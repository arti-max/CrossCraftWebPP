#pragma once
#include <GLFW/glfw3.h>
#include <queue>

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

// Добавь эту структуру
struct MouseEvent {
    int button;
    bool state;
    double x, y;
};

class Mouse {
private:
    static std::queue<MouseEvent> events;
    static MouseEvent currentEvent;
    static double mouseX, mouseY;
    static double deltaX, deltaY;
    static bool grabbed;
    static GLFWwindow* window;

public:
    static void create();
    static void destroy();
    static void init(GLFWwindow* win);
    
    static bool next();
    static int getEventButton();
    static bool getEventButtonState();
    static double getEventX();
    static double getEventY();
    
    static double getX();
    static double getY();
    static double getDX();
    static double getDY();
    static void setCursorPosition(int x, int y);
    
    // ОБНОВЛЕННЫЕ МЕТОДЫ ДЛЯ WEB
    static void setGrabbed(bool grab);
    static bool isGrabbed();
    
    static void poll();
    
    // GLFW callbacks
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double x, double y);

#ifdef __EMSCRIPTEN__
    // Emscripten pointer lock callbacks
    static EM_BOOL pointerlockChangeCallback(int eventType, const EmscriptenPointerlockChangeEvent *e, void *userData);
    static void requestPointerLock();
    static void exitPointerLock();
#endif
};
