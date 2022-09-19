/**
   KQ is Copyright (C) 2002 by Josh Bolduc

   This file is part of KQ... a freeware RPG.

   KQ is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2, or (at your
   option) any later version.

   KQ is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with KQ; see the file COPYING.  If not, write to
   the Free Software Foundation,
       675 Mass Ave, Cambridge, MA 02139, USA.
*/

#pragma once

#include <utility>

struct SDL_KeyboardEvent;

class KPlayerInputButton
{
  public:
    KPlayerInputButton(int sc = 0);

    /*! \brief Scancode returned by SDL.
     *
     * Until there is a 'friend' class which we can use to limit access to this, it doesn't
     * make much sense in setting a getter/setter that does not validation checks.
     */
    int scancode;

    /*! \brief Returns true ONLY when the key state transitions from RELEASED to PRESSED.
     *
     * Will only return true on the first frame where it is queried; subsequent calls (until
     * the player releases the key and presses it again) will all return false.
     *
     * This will not return true for key repeats due to holding the key down for long enough.
     * For that, see isRepeating().
     */
    bool operator()();

    /*! \brief Returns true whenever the key is held down.
     *
     * May be called multiple times in the same frame.
     */
    bool isDown() const;

    /*! \brief Returns true whenever the key is down long enough to cause a key repeat event.
     *
     * May be called multiple times in the same frame.
     */
    bool isRepeating() const;

  protected:
    // Only true the first time operator() is called, and remains false until the key has been
    // released and pressed again.
    bool pressed_;

    // True whenever the key is held down.
    bool down_;

    // True when the button is pressed and SDL_KeyboardEvent::repeat is non-zero.
    bool repeat_;

    friend class KPlayerInput;
};

inline bool KPlayerInputButton::operator()()
{
    return std::exchange(pressed_, false);
}

inline bool KPlayerInputButton::isDown() const
{
    return down_;
}

inline bool KPlayerInputButton::isRepeating() const
{
    return repeat_;
}

/**
 * Map player input (from keyboard, joystick, mouse, etc.) to a single input manager for KQ.
 */
class KPlayerInput
{
  public:
    KPlayerInput();

    void ProcessKeyboardEvent(SDL_KeyboardEvent* evt);
    void SetButtonStates(KPlayerInputButton& button, SDL_KeyboardEvent* evt);

    // Flags for determining keypresses and player movement.

    // Moves the cursor or player horizontally (right).
    KPlayerInputButton right;
    // Moves the cursor or player horizontally (left).
    KPlayerInputButton left;
    // Moves the cursor or player vertically (up).
    KPlayerInputButton up;
    // Moves the cursor or player vertically (down).
    KPlayerInputButton down;
    // Exits menus, or opens the game menu.
    KPlayerInputButton besc;
    // Usually the action or "accept" button.
    KPlayerInputButton balt;
    // Usually the run or "cancel" button.
    KPlayerInputButton bctrl;
    // Usually the menu button.
    KPlayerInputButton benter;
    // Displays the (not-yet implemented) help menu (always F1).
    KPlayerInputButton bhelp;
    // Activates cheats (calls cheat.lua) and runs whatever commands are found there (always F10).
    KPlayerInputButton bcheat;
};

extern KPlayerInput PlayerInput;
