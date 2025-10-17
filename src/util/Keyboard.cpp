#include "Keyboard.hpp"
#include <iostream>
#include <cctype>

std::queue<KeyEvent> Keyboard::events;
KeyEvent Keyboard::currentEvent;
GLFWwindow* Keyboard::window = nullptr;
std::array<bool, GLFW_KEY_LAST + 1> Keyboard::keyStates = {};
bool Keyboard::repeatEventsEnabled = true;

void Keyboard::create() {
    keyStates.fill(false);
    repeatEventsEnabled = true;
    std::cout << "Keyboard created" << std::endl;
}

void Keyboard::destroy() {
    window = nullptr;
    keyStates.fill(false);
    while (!events.empty()) {
        events.pop();
    }
    std::cout << "Keyboard destroyed" << std::endl;
}

void Keyboard::init(GLFWwindow* win) {
    window = win;
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCharCallback(window, charCallback);
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
}

bool Keyboard::next() {
    if (events.empty()) {
        return false;
    }
    
    currentEvent = events.front();
    events.pop();
    return true;
}

int Keyboard::getEventKey() {
    return currentEvent.key;
}

bool Keyboard::getEventKeyState() {
    return currentEvent.state;
}

char Keyboard::getEventCharacter() {
    return currentEvent.character;
}

bool Keyboard::isKeyDown(int key) {
    if (!window || key < 0 || key > GLFW_KEY_LAST) {
        return false;
    }
    
    bool glfwState = glfwGetKey(window, key) == GLFW_PRESS;
    bool ourState = keyStates[key];
    
    if (glfwState != ourState) {
        keyStates[key] = glfwState;
        
        KeyEvent correctionEvent;
        correctionEvent.key = key;
        correctionEvent.state = glfwState;
        correctionEvent.character = 0;
        events.push(correctionEvent);
    }
    
    return keyStates[key];
}

void Keyboard::enableRepeatEvents(bool enable) {
    repeatEventsEnabled = enable;
}

void Keyboard::clearStuckKeys() {
    if (!window) return;
    
    for (int key = 0; key <= GLFW_KEY_LAST; ++key) {
        if (keyStates[key]) {
            bool actualState = glfwGetKey(window, key) == GLFW_PRESS;
            if (!actualState) {
                keyStates[key] = false;
                
                KeyEvent releaseEvent;
                releaseEvent.key = key;
                releaseEvent.state = false;
                releaseEvent.character = 0;
                events.push(releaseEvent);
                
                std::cout << "Cleared stuck key: " << key << std::endl;
            }
        }
    }
}

void Keyboard::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key < 0 || key > GLFW_KEY_LAST) return;
    
    KeyEvent event;
    event.key = key;
    event.character = 0;
    
    switch (action) {
        case GLFW_PRESS:
            event.state = true;
            keyStates[key] = true;
            events.push(event);
            break;
            
        case GLFW_RELEASE:
            event.state = false;
            keyStates[key] = false;
            events.push(event);
            break;
            
        case GLFW_REPEAT:
            if (repeatEventsEnabled) {
                event.state = true;
                events.push(event);
            }
            break;
    }
}

void Keyboard::charCallback(GLFWwindow* window, unsigned int codepoint) {
    if (codepoint <= 127) {
        KeyEvent event;
        event.key = 0;
        event.state = true;
        event.character = static_cast<char>(codepoint);
        events.push(event);
    }
}
