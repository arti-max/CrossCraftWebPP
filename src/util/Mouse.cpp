#include "Mouse.hpp"
#include <iostream>

std::queue<MouseEvent> Mouse::events;
MouseEvent Mouse::currentEvent;
double Mouse::mouseX = 0.0;
double Mouse::mouseY = 0.0;
double Mouse::deltaX = 0.0;
double Mouse::deltaY = 0.0;
bool Mouse::grabbed = false;
GLFWwindow* Mouse::window = nullptr;

void Mouse::init(GLFWwindow* win) {
    window = win;
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    
    glfwGetCursorPos(window, &mouseX, &mouseY);
}

bool Mouse::next() {
    if (events.empty()) {
        return false;
    }
    
    currentEvent = events.front();
    events.pop();
    return true;
}

int Mouse::getEventButton() {
    return currentEvent.button;
}

bool Mouse::getEventButtonState() {
    return currentEvent.state;
}

double Mouse::getEventX() {
    return currentEvent.x;
}

double Mouse::getEventY() {
    return currentEvent.y;
}

double Mouse::getX() {
    return mouseX;
}

double Mouse::getY() {
    return mouseY;
}

double Mouse::getDX() {
    return deltaX;
}

double Mouse::getDY() {
    return deltaY;
}

void Mouse::setGrabbed(bool grab) {
    grabbed = grab;
    if (window) {
        glfwSetInputMode(window, GLFW_CURSOR, grab ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}

bool Mouse::isGrabbed() {
    return grabbed;
}

void Mouse::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    MouseEvent event;
    event.button = button;
    event.state = (action == GLFW_PRESS);
    event.x = mouseX;
    event.y = mouseY;
    events.push(event);
}

void Mouse::cursorPosCallback(GLFWwindow* window, double x, double y) {
    deltaX = x - mouseX;
    deltaY = y - mouseY;
    mouseX = x;
    mouseY = y;
}
