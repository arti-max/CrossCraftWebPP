#pragma once
#include <GLFW/glfw3.h>
#include <queue>

struct KeyEvent {
    int key;
    bool state;
};

class Keyboard {
private:
    static std::queue<KeyEvent> events;
    static KeyEvent currentEvent;
    static GLFWwindow* window;

public:
    static void init(GLFWwindow* win);
    static bool next();
    static int getEventKey();
    static bool getEventKeyState();
    static bool isKeyDown(int key);
    
    // GLFW callback
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
