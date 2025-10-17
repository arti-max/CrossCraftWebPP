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
#include <emscripten/html5.h>

EM_BOOL Mouse::mouseDownCallback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
    
    // Добавляем событие нажатия в очередь
    MouseEvent event;
    event.button = e->button;
    event.state = true;  // Нажатие
    event.x = e->targetX;
    event.y = e->targetY;
    Mouse::events.push(event);
    
    return EM_TRUE;
}

EM_BOOL Mouse::mouseUpCallback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
    
    // Добавляем событие отпускания в очередь
    MouseEvent event;
    event.button = e->button;
    event.state = false;  // Отпускание
    event.x = e->targetX;
    event.y = e->targetY;
    Mouse::events.push(event);
    
    return EM_TRUE;
}


// Обработчики событий
EM_BOOL Mouse::mouseClickCallback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
    return EM_TRUE;
}

EM_BOOL Mouse::mouseMoveCallback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
    if (Mouse::grabbed) {
        // В режиме pointer lock используем movementX/Y
        Mouse::deltaX += e->movementX;
        Mouse::deltaY += e->movementY;
    } else {
        // Обычное движение мыши
        double newX = e->targetX;
        double newY = e->targetY;
        Mouse::deltaX = newX - Mouse::mouseX;
        Mouse::deltaY = newY - Mouse::mouseY;
        Mouse::mouseX = newX;
        Mouse::mouseY = newY;
    }
    return EM_TRUE;
}

EM_BOOL Mouse::pointerlockChangeCallback(int eventType, const EmscriptenPointerlockChangeEvent *e, void *userData) {
    bool wasGrabbed = Mouse::grabbed;
    bool shouldBeGrabbed = e->isActive;
    
    
    Mouse::grabbed = shouldBeGrabbed;
    
    if (shouldBeGrabbed && !wasGrabbed) {
        Mouse::deltaX = 0.0;
        Mouse::deltaY = 0.0;
    } else if (!shouldBeGrabbed && wasGrabbed) {
        
        // ИСПРАВЛЕНО: Добавляем ESC событие для синхронизации с CrossCraft
        // Это гарантирует что CrossCraft узнает об освобождении мыши
        // EM_ASM({
        //     // Отправляем событие ESC в C++  
        //     const escEvent = new KeyboardEvent('keydown', {
        //         key: 'Escape',
        //         code: 'Escape',
        //         keyCode: 27,
        //         which: 27
        //     });
        //     document.dispatchEvent(escEvent);
        // });
    }
    
    return EM_TRUE;
}

EM_BOOL Mouse::pointerlockErrorCallback(int eventType, const void *reserved, void *userData) {
    std::cout << "❌ Pointer lock ERROR!" << std::endl;
    return EM_TRUE;
}

#endif

void Mouse::create() {
    std::cout << "Mouse::create() - Setting up Emscripten callbacks" << std::endl;
    
#ifdef __EMSCRIPTEN__
    emscripten_set_mousedown_callback("#canvas", nullptr, true, mouseDownCallback);
    std::cout << "✓ Mouse down callback registered" << std::endl;
    
    emscripten_set_mouseup_callback("#canvas", nullptr, true, mouseUpCallback);
    std::cout << "✓ Mouse up callback registered" << std::endl;
    
    emscripten_set_click_callback("#canvas", nullptr, true, mouseClickCallback);
    std::cout << "✓ Click callback registered" << std::endl;
    
    emscripten_set_mousemove_callback("#canvas", nullptr, true, mouseMoveCallback);
    std::cout << "✓ Mouse move callback registered" << std::endl;
    
    // Pointer lock callbacks
    emscripten_set_pointerlockchange_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, true, pointerlockChangeCallback);
    std::cout << "✓ Pointer lock change callback registered" << std::endl;
    
    emscripten_set_pointerlockerror_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, true, pointerlockErrorCallback);
    std::cout << "✓ Pointer lock error callback registered" << std::endl;
#endif
}

void Mouse::destroy() {
    window = nullptr;
    grabbed = false;
    while (!events.empty()) {
        events.pop();
    }
    std::cout << "Mouse destroyed" << std::endl;
}

void Mouse::init(GLFWwindow* win) {
    window = win;
    std::cout << "Mouse initialized (GLFW callbacks disabled for web)" << std::endl;
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
    // Не нужно в web
}

void Mouse::setGrabbed(bool grab) {
    
#ifdef __EMSCRIPTEN__
    if (grab && !grabbed) {
        requestPointerLock();
    } else if (!grab && grabbed) {
        exitPointerLock();
    }
#endif
}

bool Mouse::isGrabbed() {
    return grabbed;
}

void Mouse::poll() {
    // Не нужно в Emscripten - события обрабатываются асинхронно
}

#ifdef __EMSCRIPTEN__
void Mouse::requestPointerLock() {
    
    EM_ASM({
        console.log('🎯 EM_ASM: Requesting pointer lock...');
        const canvas = document.getElementById('canvas');
        if (canvas) {
            
            // Поддержка разных браузеров и версий API
            const requestPointerLock = canvas.requestPointerLock ||
                                     canvas.mozRequestPointerLock ||
                                     canvas.webkitRequestPointerLock;
            
            if (requestPointerLock) {
                try {
                    const result = requestPointerLock.call(canvas);
                    
                    // Новый API возвращает Promise
                    if (result && typeof result.then === 'function') {
                        result.then(function() {
                            console.log('✅ Pointer lock request SUCCESS (Promise)');
                        }).catch(function(err) {
                            console.error('❌ Pointer lock request FAILED (Promise):', err);
                        });
                    } else {
                        // Старый API не возвращает Promise
                        console.log('✅ Pointer lock request sent (Legacy API)');
                    }
                } catch (error) {
                    console.error('❌ Exception during pointer lock request:', error);
                }
            } else {
                console.error('❌ Pointer lock API not available');
            }
        } else {
            console.error('❌ Canvas element not found');
        }
    });
}

void Mouse::exitPointerLock() {
    
    EM_ASM({
        
        const exitPointerLock = document.exitPointerLock ||
                              document.mozExitPointerLock ||
                              document.webkitExitPointerLock;
        
        if (exitPointerLock) {
            exitPointerLock.call(document);
            console.log('✅ Exit pointer lock called');
        } else {
            console.error('❌ Exit pointer lock not available');
        }
    });
}
#endif
