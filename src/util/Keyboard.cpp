#include "Keyboard.hpp"
#include <iostream>

std::queue<KeyEvent> Keyboard::events;
KeyEvent Keyboard::currentEvent;
GLFWwindow* Keyboard::window = nullptr;

void Keyboard::create() {
    std::cout << "Keyboard created" << std::endl;
}

void Keyboard::destroy() {
    window = nullptr;
    while (!events.empty()) {
        events.pop();
    }
    std::cout << "Keyboard destroyed" << std::endl;
}

void Keyboard::init(GLFWwindow* win) {
    window = win;
    glfwSetKeyCallback(window, keyCallback);
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

bool Keyboard::isKeyDown(int key) {
    return window && glfwGetKey(window, key) == GLFW_PRESS;
}

void Keyboard::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    KeyEvent event;
    event.key = key;
    event.state = (action == GLFW_PRESS || action == GLFW_REPEAT);
    events.push(event);
}
