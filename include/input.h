#pragma once

struct SDL_KeyboardEvent;

class KPlayerInput
{
  public:
    KPlayerInput();
    void ProcessKeyboardEvent(SDL_KeyboardEvent* evt);

    struct button
    {
        button(int sc = 0)
            : scancode(sc)
        {
        }
        int scancode;
        bool down = false;
        bool pressed = false;
        bool isDown() const
        {
            return down;
        }
        bool operator()()
        {
            bool rc = pressed;
            pressed = false;
            return rc;
        }
    };
    // Flags for determining keypresses and player movement.

    // Moves the cursor or player horizontally (right).
    button right;
    // Moves the cursor or player horizontally (left).
    button left;
    // Moves the cursor or player vertically (up).
    button up;
    // Moves the cursor or player vertically (down).
    button down;
    // Exits menus, or opens the game menu.
    button besc;
    // Usually the action or "accept" button.
    button balt;
    // Usually the run or "cancel" button.
    button bctrl;
    // Usually the menu button.
    button benter;
    // Displays the (not-yet implemented) help menu (always F1).
    button bhelp;
    // Activates cheats (calls cheat.lua) and runs whatever commands are found there. (always F10)
    button bcheat;

    // Joystick buttons
    int jbalt, jbctrl, jbenter, jbesc;
};

extern KPlayerInput PlayerInput;
