#pragma once
#include <GLFW/glfw3.h>
#include <queue>

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
    static void setGrabbed(bool grab);
    static bool isGrabbed();
    
    // GLFW callbacks
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double x, double y);
};
