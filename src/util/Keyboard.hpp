#pragma once
#include <GLFW/glfw3.h>
#include <queue>
#include <array>

struct KeyEvent {
    int key;
    bool state;
    char character;
};

class Keyboard {
private:
    static std::queue<KeyEvent> events;
    static KeyEvent currentEvent;
    static GLFWwindow* window;
    static std::array<bool, GLFW_KEY_LAST + 1> keyStates;
    static bool repeatEventsEnabled;

public:
    static void create();
    static void destroy();
    static void init(GLFWwindow* win);
    static bool next();
    static int getEventKey();
    static bool getEventKeyState();
    static char getEventCharacter();
    static bool isKeyDown(int key);
    static void enableRepeatEvents(bool enable);
    
    // GLFW callbacks
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void charCallback(GLFWwindow* window, unsigned int codepoint);
    
    static void clearStuckKeys();
};
