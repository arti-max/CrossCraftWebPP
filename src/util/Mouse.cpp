#include "Mouse.hpp"
#include <iostream>

// Статические переменные
std::queue<MouseEvent> Mouse::events;
MouseEvent Mouse::currentEvent;
double Mouse::mouseX = 0.0;
double Mouse::mouseY = 0.0;
double Mouse::deltaX = 0.0;
double Mouse::deltaY = 0.0;
bool Mouse::grabbed = false;
GLFWwindow* Mouse::window = nullptr;

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

// JavaScript функции через EM_JS
EM_JS(void, js_requestPointerLock, (const char* selector), {
    const element = document.querySelector(UTF8ToString(selector));
    if (element && element.requestPointerLock) {
        element.requestPointerLock();
        console.log('Requesting pointer lock for:', UTF8ToString(selector));
    } else {
        console.error('Cannot request pointer lock for:', UTF8ToString(selector));
    }
});

EM_JS(void, js_exitPointerLock, (), {
    if (document.exitPointerLock) {
        document.exitPointerLock();
        console.log('Exiting pointer lock');
    }
});

EM_JS(int, js_isPointerLocked, (), {
    return document.pointerLockElement ? 1 : 0;
});

#endif

void Mouse::create() {
    std::cout << "Mouse created" << std::endl;
    
#ifdef __EMSCRIPTEN__
    // Настраиваем Emscripten pointer lock callback
    emscripten_set_pointerlockchange_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, false, pointerlockChangeCallback);
    
    // Обработчик движения мыши
    emscripten_set_mousemove_callback("#canvas", nullptr, false, [](int eventType, const EmscriptenMouseEvent *e, void *userData) -> EM_BOOL {
        if (Mouse::grabbed) {  // Только если мышь захвачена
            Mouse::deltaX += e->movementX;
            Mouse::deltaY += e->movementY;
        } else {
            // Обычное движение мыши
            Mouse::deltaX = e->targetX - Mouse::mouseX;
            Mouse::deltaY = e->targetY - Mouse::mouseY;
            Mouse::mouseX = e->targetX;
            Mouse::mouseY = e->targetY;
        }
        return EM_TRUE;
    });
    
    std::cout << "Emscripten mouse callbacks set" << std::endl;
#endif
}

void Mouse::destroy() {
    window = nullptr;
    while (!events.empty()) {
        events.pop();
    }
    std::cout << "Mouse destroyed" << std::endl;
}

void Mouse::init(GLFWwindow* win) {
    window = win;
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    
    glfwGetCursorPos(window, &mouseX, &mouseY);
    std::cout << "Mouse initialized with GLFW window" << std::endl;
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
    double dx = deltaX;
    deltaX = 0.0;
    return dx;
}

double Mouse::getDY() {
    double dy = deltaY;
    deltaY = 0.0;
    return dy;
}

void Mouse::setCursorPosition(int x, int y) {
    if (window) {
        glfwSetCursorPos(window, static_cast<double>(x), static_cast<double>(y));
        mouseX = static_cast<double>(x);
        mouseY = static_cast<double>(y);
        deltaX = 0.0;
        deltaY = 0.0;
    }
}

void Mouse::setGrabbed(bool grab) {
    std::cout << "Mouse::setGrabbed(" << grab << ")" << std::endl;
    grabbed = grab;
    
#ifdef __EMSCRIPTEN__
    if (grab) {
        requestPointerLock();
    } else {
        exitPointerLock();  
    }
#else
    if (window) {
        glfwSetInputMode(window, GLFW_CURSOR, grab ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
#endif
}

bool Mouse::isGrabbed() {
#ifdef __EMSCRIPTEN__
    return js_isPointerLocked() != 0;
#else
    return grabbed;
#endif
}

void Mouse::poll() {
    if (window) {
        glfwGetCursorPos(window, &mouseX, &mouseY);
    }
}

void Mouse::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    MouseEvent event;
    event.button = button;
    event.state = (action == GLFW_PRESS);
    event.x = mouseX;
    event.y = mouseY;
    events.push(event);
    
    // АВТОМАТИЧЕСКИЙ ЗАХВАТ ПО КЛИКУ
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !isGrabbed()) {
        std::cout << "Left click detected - requesting mouse capture" << std::endl;
        setGrabbed(true);
    }
}

void Mouse::cursorPosCallback(GLFWwindow* window, double x, double y) {
#ifdef __EMSCRIPTEN__
    // В web режиме используем movementX/Y через pointer lock
    if (isGrabbed()) {
        // movementX/Y обрабатываются в mousemove callback
    } else {
        deltaX = x - mouseX;
        deltaY = y - mouseY;
        mouseX = x;
        mouseY = y;
    }
#else
    deltaX = x - mouseX;
    deltaY = y - mouseY;
    mouseX = x;
    mouseY = y;
#endif
}

#ifdef __EMSCRIPTEN__
EM_BOOL Mouse::pointerlockChangeCallback(int eventType, const EmscriptenPointerlockChangeEvent *e, void *userData) {
    grabbed = e->isActive;
    std::cout << "Pointer lock changed: " << (grabbed ? "LOCKED" : "UNLOCKED") << std::endl;
    
    if (grabbed) {
        // Сбрасываем дельты при захвате
        deltaX = 0.0;
        deltaY = 0.0;
    }
    
    return EM_TRUE;
}

void Mouse::requestPointerLock() {
    js_requestPointerLock("#canvas");
}

void Mouse::exitPointerLock() {
    js_exitPointerLock();
}
#endif
