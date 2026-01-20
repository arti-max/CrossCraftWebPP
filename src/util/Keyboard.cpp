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
    
    // bool glfwState = glfwGetKey(window, key) == GLFW_PRESS;
    // bool ourState = keyStates[key];
    
    // if (glfwState != ourState) {
    //     keyStates[key] = glfwState;
        
    //     KeyEvent correctionEvent;
    //     correctionEvent.key = key;
    //     correctionEvent.state = glfwState;
    //     correctionEvent.character = 0;
    //     events.push(correctionEvent);
    // }
    
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
    
    if (action == GLFW_PRESS) {
        if (!keyStates[key]) { 
            event.state = true;
            keyStates[key] = true;
            events.push(event);
        }
    } else if (action == GLFW_RELEASE) {
        event.state = false;
        keyStates[key] = false;
        events.push(event);
    } else if (action == GLFW_REPEAT) {
        if (repeatEventsEnabled) {
            event.state = true;
            events.push(event);
        }
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

void Keyboard::clearEvents() {
    while (!events.empty()) {
        events.pop();
    }
}

const char* Keyboard::getKeyName(int key) {
    switch (key) {
        // Printable keys
        case GLFW_KEY_SPACE: return "SPACE";
        case GLFW_KEY_APOSTROPHE: return "'";
        case GLFW_KEY_COMMA: return ",";
        case GLFW_KEY_MINUS: return "-";
        case GLFW_KEY_PERIOD: return ".";
        case GLFW_KEY_SLASH: return "/";
        case GLFW_KEY_0: return "0";
        case GLFW_KEY_1: return "1";
        case GLFW_KEY_2: return "2";
        case GLFW_KEY_3: return "3";
        case GLFW_KEY_4: return "4";
        case GLFW_KEY_5: return "5";
        case GLFW_KEY_6: return "6";
        case GLFW_KEY_7: return "7";
        case GLFW_KEY_8: return "8";
        case GLFW_KEY_9: return "9";
        case GLFW_KEY_SEMICOLON: return ";";
        case GLFW_KEY_EQUAL: return "=";
        case GLFW_KEY_A: return "A";
        case GLFW_KEY_B: return "B";
        case GLFW_KEY_C: return "C";
        case GLFW_KEY_D: return "D";
        case GLFW_KEY_E: return "E";
        case GLFW_KEY_F: return "F";
        case GLFW_KEY_G: return "G";
        case GLFW_KEY_H: return "H";
        case GLFW_KEY_I: return "I";
        case GLFW_KEY_J: return "J";
        case GLFW_KEY_K: return "K";
        case GLFW_KEY_L: return "L";
        case GLFW_KEY_M: return "M";
        case GLFW_KEY_N: return "N";
        case GLFW_KEY_O: return "O";
        case GLFW_KEY_P: return "P";
        case GLFW_KEY_Q: return "Q";
        case GLFW_KEY_R: return "R";
        case GLFW_KEY_S: return "S";
        case GLFW_KEY_T: return "T";
        case GLFW_KEY_U: return "U";
        case GLFW_KEY_V: return "V";
        case GLFW_KEY_W: return "W";
        case GLFW_KEY_X: return "X";
        case GLFW_KEY_Y: return "Y";
        case GLFW_KEY_Z: return "Z";
        case GLFW_KEY_LEFT_BRACKET: return "[";
        case GLFW_KEY_BACKSLASH: return "\\";
        case GLFW_KEY_RIGHT_BRACKET: return "]";
        case GLFW_KEY_GRAVE_ACCENT: return "`";
        case GLFW_KEY_WORLD_1: return "WORLD1";
        case GLFW_KEY_WORLD_2: return "WORLD2";

        // Function keys
        case GLFW_KEY_ESCAPE: return "ESC";
        case GLFW_KEY_ENTER: return "ENTER";
        case GLFW_KEY_TAB: return "TAB";
        case GLFW_KEY_BACKSPACE: return "BACKSPACE";
        case GLFW_KEY_INSERT: return "INSERT";
        case GLFW_KEY_DELETE: return "DELETE";
        case GLFW_KEY_RIGHT: return "RIGHT";
        case GLFW_KEY_LEFT: return "LEFT";
        case GLFW_KEY_DOWN: return "DOWN";
        case GLFW_KEY_UP: return "UP";
        case GLFW_KEY_PAGE_UP: return "PAGE UP";
        case GLFW_KEY_PAGE_DOWN: return "PAGE DOWN";
        case GLFW_KEY_HOME: return "HOME";
        case GLFW_KEY_END: return "END";
        case GLFW_KEY_CAPS_LOCK: return "CAPS LOCK";
        case GLFW_KEY_SCROLL_LOCK: return "SCROLL LOCK";
        case GLFW_KEY_NUM_LOCK: return "NUM LOCK";
        case GLFW_KEY_PRINT_SCREEN: return "PRINT SCREEN";
        case GLFW_KEY_PAUSE: return "PAUSE";
        case GLFW_KEY_F1: return "F1";
        case GLFW_KEY_F2: return "F2";
        case GLFW_KEY_F3: return "F3";
        case GLFW_KEY_F4: return "F4";
        case GLFW_KEY_F5: return "F5";
        case GLFW_KEY_F6: return "F6";
        case GLFW_KEY_F7: return "F7";
        case GLFW_KEY_F8: return "F8";
        case GLFW_KEY_F9: return "F9";
        case GLFW_KEY_F10: return "F10";
        case GLFW_KEY_F11: return "F11";
        case GLFW_KEY_F12: return "F12";
        case GLFW_KEY_F13: return "F13";
        case GLFW_KEY_F14: return "F14";
        case GLFW_KEY_F15: return "F15";
        case GLFW_KEY_F16: return "F16";
        case GLFW_KEY_F17: return "F17";
        case GLFW_KEY_F18: return "F18";
        case GLFW_KEY_F19: return "F19";
        case GLFW_KEY_F20: return "F20";
        case GLFW_KEY_F21: return "F21";
        case GLFW_KEY_F22: return "F22";
        case GLFW_KEY_F23: return "F23";
        case GLFW_KEY_F24: return "F24";
        case GLFW_KEY_F25: return "F25";
        case GLFW_KEY_KP_0: return "KP 0";
        case GLFW_KEY_KP_1: return "KP 1";
        case GLFW_KEY_KP_2: return "KP 2";
        case GLFW_KEY_KP_3: return "KP 3";
        case GLFW_KEY_KP_4: return "KP 4";
        case GLFW_KEY_KP_5: return "KP 5";
        case GLFW_KEY_KP_6: return "KP 6";
        case GLFW_KEY_KP_7: return "KP 7";
        case GLFW_KEY_KP_8: return "KP 8";
        case GLFW_KEY_KP_9: return "KP 9";
        case GLFW_KEY_KP_DECIMAL: return "KP .";
        case GLFW_KEY_KP_DIVIDE: return "KP /";
        case GLFW_KEY_KP_MULTIPLY: return "KP *";
        case GLFW_KEY_KP_SUBTRACT: return "KP -";
        case GLFW_KEY_KP_ADD: return "KP +";
        case GLFW_KEY_KP_ENTER: return "KP ENTER";
        case GLFW_KEY_KP_EQUAL: return "KP =";
        case GLFW_KEY_LEFT_SHIFT: return "LSHIFT";
        case GLFW_KEY_LEFT_CONTROL: return "LCTRL";
        case GLFW_KEY_LEFT_ALT: return "LALT";
        case GLFW_KEY_LEFT_SUPER: return "LSUPER";
        case GLFW_KEY_RIGHT_SHIFT: return "RSHIFT";
        case GLFW_KEY_RIGHT_CONTROL: return "RCTRL";
        case GLFW_KEY_RIGHT_ALT: return "RALT";
        case GLFW_KEY_RIGHT_SUPER: return "RSUPER";
        case GLFW_KEY_MENU: return "MENU";

        default: return "???";
    }
}