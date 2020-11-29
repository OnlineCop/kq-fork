#pragma once

class KPlayerInput
{
public:
	KPlayerInput();
	~KPlayerInput() = default;

	void readcontrols();

public:
	// Flags for determining keypresses and player movement.

	// Moves the cursor or player horizontally (right).
	int right;
	// Moves the cursor or player horizontally (left).
	int left;
	// Moves the cursor or player vertically (up).
	int up;
	// Moves the cursor or player vertically (down).
	int down;
	// Exits menus, or opens the game menu.
	int besc;
	// Usually the action or "accept" button.
	int balt;
	// Usually the run or "cancel" button.
	int bctrl;
	// Usually the menu button.
	int benter;
	// Displays the (not-yet implemented) help menu.
	int bhelp;
	// Activates cheats (calls cheat.lua) and runs whatever commands are found there.
	int bcheat;

	// Scan codes for the keys (help is always F1)
	int kright, kleft, kup, kdown;
	int kesc, kenter, kalt, kctrl;

	// Joystick buttons
	int jbalt, jbctrl, jbenter, jbesc;
};

extern KPlayerInput PlayerInput;
