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

/*! \file
 * \brief Equipment menu stuff.
 *
 * This file contains code to handle the equipment menu including dropping and optimizing the items carried.
 */

#include "eqpmenu.h"

#include "draw.h"
#include "gfx.h"
#include "input.h"
#include "itemmenu.h"
#include "kq.h"
#include "menu.h"
#include "setup.h"

KEquipMenu EquipMenu;

namespace
{

// There is an 8-pixel "frame" around each edge of the menus.
constexpr int FrameBorder = 8;

// The fonts use 8x8 monospaced glyphs (may be different colors).
constexpr int FontW = 8;
constexpr int FontH = 8;

// Half of the font sizes (usually for centering text).
constexpr int FontW2 = FontW / 2;
constexpr int FontH2 = FontH / 2;

/*! \brief Calculate number of pixels needed to center text within a specified character width.
 *
 * Assumes fonts used are FontW pixels wide.
 *
 * \param   text String to be centered.
 * \param   maxNumChars Maximum length of string that text must fit within (NOT pixel size!).
 * \returns Left offset in pixels, not to fall below 0.
 */
int getCenteredTextXoffset(const std::string& text, unsigned int maxNumChars)
{
    const int textLeftEdge = FontW2 * (maxNumChars - text.size());
    return std::max<int>(0, textLeftEdge);
}

size_t longestString(const std::vector<std::string>& inputs)
{
    size_t longest = 0;
    for (const auto& input : inputs)
    {
        longest = std::max<size_t>(longest, input.size());
    }
    return longest;
}

/*! \brief Limit a string to a maximum length; then append text to end of it.
 *
 * If input were "abcd", maxLength were 3, and appendString were ":", then the resulting string
 * would be "abc:" as it would first truncate "abcd" to "abc" and then append ":".
 *
 * If input were "xy", maxLength were 3, and appendString were ":", then the resulting string
 * would be "xy:" as "xy" is already shorter than 3 characters, and then ":" is appended.
 *
 * \param   input Original string to possibly truncate, if it exceeds maxLength characters.
 * \param   maxLength Max number of (possibly Unicode) characters to allow, not including any
 *          text appended onto the string by appendString.
 * \param   addEllipsis Add an '…' ellipsis at the location text is truncated, before appendString.
 * \param   appendString Optional characters to append onto the final string.
 * \returns String that had first been truncated to maxLength characters, then appendString
 *          appended onto it (new string length may exceed maxLength characters).
 */
std::string truncate_string(std::string input, int maxLength, bool addEllipsis, const std::string& appendString)
{
    if (maxLength > 0 && input.size() > maxLength)
    {
        // Tab (0x09) is rendered as an ellipsis '…'. See 'glyph_lookup' map in draw.cpp.
        input[maxLength - 1] = '\t';
    }

    // The '.*' in this printf notation truncates the (possibly Unicode) text if necessary.
    // The precision is the parameter right before the text to be formatted (here: maxLength).
    sprintf(input, "%.*s%s", maxLength, input.c_str(), appendString.c_str());

    return input;
}

/*! \brief Limit each string in the vector to a maximum length; then append text to end of each.
 *
 * If inputs[0] were "abcd", maxLength were 3, and appendString were ":", then the resulting string
 * would be "abc:" as it would first truncate "abcd" to "abc" and then append ":".
 *
 * If inputs[1] were "xy", maxLength were 3, and appendString were ":", then the resulting string
 * would be "xy:" as "xy" is already shorter than 3 characters, and then ":" is appended.
 *
 * \param   inputs Original strings to possibly truncate, if they exceed maxLength characters.
 * \param   maxLength Max number of (possibly Unicode) characters to allow, not including any
 *          text appended onto the string by appendString.
 * \param   addEllipsis Add an '…' ellipsis at the location text is truncated, before appendString.
 * \param   appendString Optional characters to append onto each string in inputs.
 * \returns Vector of strings that have first been truncated to maxLength characters, then
 *          appendString appended onto each (new string lengths may exceed maxLength characters).
 */
std::vector<std::string> truncate_strings(std::vector<std::string> inputs, int maxLength, bool addEllipsis,
                                          const std::string& appendString)
{
    for (auto& input : inputs)
    {
        input = truncate_string(input, maxLength, addEllipsis, appendString);
    }

    return inputs;
}

// Need input state machine:
//  1. EquipInputState::eInputState::ActionBar
//      CANCEL: Move to [EquipInputState::eInputState::Exit]
//      CONFIRM:
//      - If over "Equip": move to [EquipInputState::eInputState::ChooseSlot]
//      - If over "Remove": move to [EquipInputState::eInputState::ChooseSlot]
//      - If over "Optimize": remain in [EquipInputState::eInputState::ActionBar]
//      - If over "Clear": remain in [EquipInputState::eInputState::ActionBar]
//      LEFT_ARROW: remain in [EquipInputState::eInputState::ActionBar]
//      RIGHT_ARROW: remain in [EquipInputState::eInputState::ActionBar]
//
//  2. EquipInputState::eInputState::ChooseSlot
//      CANCEL: Move to [EquipInputState::eInputState::ActionBar]
//      CONFIRM:
//      - If over "Equip":
//          - If t_inv IS empty: remain in [EquipInputState::eInputState::ChooseSlot]
//          - If t_inv NOT empty: move to [EquipInputState::eInputState::SelectEquipment]
//      - If over "Remove":
//          - If deequip() fails: remain in [EquipInputState::eInputState::ChooseSlot]
//          - If deequip() succeeds: move to [EquipInputState::eInputState::ActionBar]
//      - Else: (do stuff, and then) remain in [EquipInputState::eInputState::ChooseSlot]
//      UP_ARROW: remain in [EquipInputState::eInputState::ChooseSlot]
//      DOWN_ARROW: remain in [EquipInputState::eInputState::ChooseSlot]
//
//  3. EquipInputState::eInputState::SelectEquipment
//      CANCEL: Move to [EquipInputState::eInputState::ChooseSlot]
//      CONFIRM:
//      - If equip() succeeds: move to [EquipInputState::eInputState::ChooseSlot]
//      - If equip() fails: remain in [EquipInputState::eInputState::SelectEquipment]
//      UP_ARROW: remain in [EquipInputState::eInputState::SelectEquipment]
//      DOWN_ARROW: remain in [EquipInputState::eInputState::SelectEquipment]
//
//  4. EquipInputState::eInputState::Exit
//      Exit equip_menu()
struct EquipInputState
{
    enum eInputState
    {
        ActionBar,
        ChooseSlot,
        SelectEquipment,
        Exit
    };

    eInputState currentState = eInputState::ActionBar;
    bool isFocusOnActionBar = true;
    bool isActionBarStopped = false;
    uint8_t player_index = 0;
    eEquipment slot = eEquipment::EQP_WEAPON;
    uint16_t cursorYoffset = 0;
    uint16_t scrollYoffset = 0;
};
EquipInputState equipInputState;

} // anonymous namespace

KEquipMenu::KEquipMenu()
    : tstats {}
    , tres {}
    , t_inv {}
    , activeActionBar { eActionBar::AB_NONE }
{
    // The top menu is 35 chars long (4x 8-char columns, with 1-char padding between each) to hold
    // the "Equip", "Optimize", "Remove", "Empty" options the player may choose from.
    constexpr int MaxMenuWidth = 4 * 8 + 3;

    // {20, 12, 35, 1}
    topMenu = Rect { 20, 12, MaxMenuWidth, 1 };
    const int row2Yoffset = topMenu.y + FontH * topMenu.h + FrameBorder * 2;

    // There are two menus below the top one.
    // 1. On the left, the Equipped menu shows what the selected party member has equipped in each
    //    slot (weapons, shields, etc.).
    // 2. On the right, the Portrait menu shows the party member's 40x40 portrait and 8-char name.
    // Both menus are NUM_EQUIPMENT [6] chars tall.

    // The Equipped menu is 25 chars long: MaxMenuWidth [35] minus the width [8] of the Portrait
    // menu, minus the 2 "frame borders" drawn between these menus.

    // {20, 36, 25, 6}
    equippedMenu = Rect { topMenu.x, row2Yoffset, 25, eEquipment::NUM_EQUIPMENT };

    // The Portrait menu is 8 chars long.
    // Its left edge starts to the right of the Equipped menu, plus 2x frame borders between them.
    const int portraitXoffset = FontW * equippedMenu.w + FrameBorder * 2;

    // {228, 36, 8, 6}
    portraitMenu = Rect { topMenu.x + portraitXoffset, row2Yoffset, 8, eEquipment::NUM_EQUIPMENT };

    // There are three menus on the bottom.
    // 1. On the left, the Available menu shows which equipment the hero has available for the
    //    selected equipment slot (weapon, shield, etc.).
    // 2. On the top-right, the 13 stats (strength, agility, etc.) show how the equipment will
    //    affect this hero if it were to be equipped.
    // 3. On the bottom-right, a field displaying "Resist up" or "Resist down" (or blank).

    const int row3Yoffset = equippedMenu.y + FontH * equippedMenu.h + FrameBorder * 2;

    // The Equippable menu is 20 chars long and NUM_ITEMS_PER_PAGE [16] tall.

    // {20, 100, 20, 16}
    equippableMenu = Rect { topMenu.x, row3Yoffset, 20, NUM_ITEMS_PER_PAGE };

    // The Prevew menu is 13 chars long.
    // Its left edge starts to the right of the Equippable menu, plus 2x frame borders.
    const int previewXOffset = FontW * equippableMenu.w + FrameBorder * 2;

    // {196, 100, 13, 13}
    previewMenu = Rect { equippableMenu.x + previewXOffset, row3Yoffset, 13, eStat::NUM_STATS };

    // equippedMenu{12, 28, 25, 6}
    const int row4Yoffset = previewMenu.y + FontH * previewMenu.h + FrameBorder * 2;

    // The Resist menu is 13 chars long.
    // {196, 220, 13, 1}
    resistMenu = Rect { equippableMenu.x + previewXOffset, row4Yoffset, 13, 1 };
}

void KEquipMenu::calc_equippreview(uint8_t player_index, eEquipment eqp_type, uint8_t item)
{
    const ePIDX pidxC = pidx[player_index];
    auto& eqp_slot = party[pidxC].eqp[eqp_type];
    uint8_t tmp = eqp_slot;
    eqp_slot = item;
    kmenu.update_equipstats();
    for (uint8_t stats_index = 0; stats_index < eStat::NUM_STATS; ++stats_index)
    {
        tstats[stats_index] = fighter[player_index].stats[stats_index];
    }
    for (uint8_t res_index = 0; res_index < eResistance::R_TOTAL_RES; ++res_index)
    {
        tres[res_index] = fighter[player_index].res[res_index];
    }
    eqp_slot = tmp;
    kmenu.update_equipstats();
}

void KEquipMenu::calc_possible_equip(const ePIDX pidxC, eEquipment slot)
{
    t_inv.clear();
    for (size_t g_inv_index = 0, g_inv_size = g_inv.size(); g_inv_index < g_inv_size; ++g_inv_index)
    {
        auto [item, quantity] = g_inv[g_inv_index];
        // Check if we have any items at all
        if (item == I_NOITEM || quantity == 0)
        {
            continue;
        }

        const auto& equipment = items[item];
        if (equipment.type == slot && equipment.eq[pidxC] != 0)
        {
            // Note that this will add shields when the hero is equipped with a two-handed weapon,
            // but logic elsewhere will prevent the player from selecting it.
            t_inv.push_back(g_inv_index);
        }
    }
}

void KEquipMenu::processChooseEquipmentInputs(uint8_t player_index, eEquipment slot)
{
}

bool KEquipMenu::deequip(const ePIDX pidxC, eEquipment slot)
{
    if (slot == eEquipment::EQP_NONE)
    {
        return false;
    }

    uint8_t existing_item = party[pidxC].eqp[slot];
    if (existing_item != I_NOITEM && check_inventory(existing_item, 1) != 0)
    {
        party[pidxC].eqp[slot] = I_NOITEM;
        return true;
    }
    return false;
}

void KEquipMenu::draw_equipmenu_top()
{
    // Semi-transparent box surrounding the options at the top of the screen.
    Draw.menubox_inner(double_buffer, topMenu, eBoxFill::TRANSPARENT);

    constexpr int MaxLength = 8;

    // Truncate strings only to the width of the menu box; the logic below will display the full
    // text when the player highlights that particular option.
    static const std::vector<std::string> TopOptions = truncate_strings(
        {
            _("Equip"),
            _("Optimize"),
            _("Remove"),
            _("Empty"),
        },
        topMenu.w, true, "");
    static const std::vector<std::string> TruncatedTopOptions = truncate_strings(TopOptions, MaxLength, true, "");

    if (equipInputState.currentState == EquipInputState::eInputState::ActionBar)
    {
        // First, draw the text for the non-highlighted options.
        //
        // The text length is clamped here to not exceed MaxLength (8) characters.
        for (size_t actionbar_index = 0; actionbar_index < eActionBar::AB_TOTAL; ++actionbar_index)
        {
            // Skip the text for the highlighted option.
            if (actionbar_index == activeActionBar)
            {
                continue;
            }

            const int menuLeftEdge = (MaxLength + 1) * FontW * actionbar_index;
            // Center text within the menu.
            const int textLeftEdge = getCenteredTextXoffset(TruncatedTopOptions[actionbar_index], MaxLength);

            Draw.print_font(double_buffer, topMenu.x + menuLeftEdge + textLeftEdge, topMenu.y,
                            TruncatedTopOptions[actionbar_index], FGOLD);
        }

        const std::string& activeOption = TopOptions[activeActionBar];

        // Second, draw an opaque box surrounding the highlighted option's text.
        //
        // Its width is clamped between [8..35] characters to avoid exceeding equippedFrame's bounds.

        // Copy the highlighted option's text, truncating to fit within the menu if necessary.
        const int activeMenuWidth = std::max<int>(MaxLength, activeOption.size());

        // Shift the menu to the left if it would spill over equippedFrame's right edge.
        const int menuLeftEdge = FontW * std::min<int>((MaxLength + 1) * activeActionBar, topMenu.w - activeMenuWidth);

        // Third, draw the frame around the highlighted option, followed by its text.

        const Rect highlightFrame { topMenu.x + menuLeftEdge, topMenu.y, activeMenuWidth, topMenu.h };
        Draw.menubox_inner(double_buffer, highlightFrame, eBoxFill::DARK);

        // Fourth, draw the text for the highlighted option.

        const int textLeftEdge = getCenteredTextXoffset(activeOption, activeMenuWidth);
        Draw.print_font(double_buffer, topMenu.x + textLeftEdge + menuLeftEdge, topMenu.y, activeOption, FGOLD);
    }
    else
    {
        // After player selects one of "Equip" or "Remove", print only that title text in the top menubox.

        // Copy the highlighted option's text, truncating to fit within the menu if necessary.
        const int maxTextLength = std::min<int>(topMenu.w, TopOptions[activeActionBar].size());
        sprintf(strbuf, "%.*s", maxTextLength, TopOptions[activeActionBar].c_str());

        const int textLeftEdge = getCenteredTextXoffset(strbuf, topMenu.w);
        Draw.print_font(double_buffer, topMenu.x + textLeftEdge, topMenu.y, strbuf, FGOLD);
    }
}

void KEquipMenu::draw_equipmenu_equipped(const KPlayer& hero)
{
    // Semi-transparent box surrounding the Equipped options on the top-left of the screen.
    Draw.menubox_inner(double_buffer, equippedMenu, eBoxFill::TRANSPARENT);

    constexpr int MaxTextLength = 8;

    // This frame contains 5 distinct columns:
    //  1. 1 "empty" char where the player's cursor is drawn.
    //  2. Up to MaxTextLength chars for the equipment slot names.
    //  3. 2 chars padding for ':' followed by a blank space.
    //  4. 1 char where the equipment icon is drawn.
    //  5. The remaining chars are for the equipment name.
    //
    // All combined, the char count should not exceed equippedMenu.w characters.

    constexpr int cursorXoffset = 1;    // blank space for cursor
    constexpr int iconBlankXoffset = 1; // blank space before icon

    // The 6 equipment slot names will be capped at MaxTextLength [8] chars long, then ':' appended.
    static const std::vector<std::string> EquipOptions = truncate_strings(
        {
            _("Hand1"),
            _("Hand2"),
            _("Head"),
            _("Body"),
            _("Arms"),
            _("Other"),
        },
        MaxTextLength, true, ":");

    static const int longestOption = std::min<int>(MaxTextLength, longestString(EquipOptions));
    static const int iconXoffset = cursorXoffset + longestOption + iconBlankXoffset;
    static const int iconLeftEdge = FontW * iconXoffset;
    static const int textLeftEdge = FontW * (iconXoffset + 1);

    const int maxEquipmentTextLength = equippedMenu.w - iconXoffset - 1;

    for (size_t eqp_index = 0; eqp_index < eEquipment::NUM_EQUIPMENT; ++eqp_index)
    {
        const int rowYoffset = FontH * eqp_index;

        // Draw the yellow text in column 1.
        Draw.print_font(double_buffer, equippedMenu.x + FontW * cursorXoffset, equippedMenu.y + rowYoffset,
                        EquipOptions[eqp_index], FGOLD);

        const uint8_t items_index = hero.eqp[eqp_index];
        const auto& equipment = items[items_index];

        // Draw the equipment's icon just before column 2.
        Draw.draw_icon(double_buffer, equipment.icon, equippedMenu.x + iconLeftEdge, equippedMenu.y + rowYoffset);

        // Draw the equipment's name in column 2.
        strbuf = truncate_string(equipment.item_name, maxEquipmentTextLength, true, "");
        Draw.print_font(double_buffer, equippedMenu.x + textLeftEdge, equippedMenu.y + rowYoffset, strbuf, FNORMAL);
    }
}

void KEquipMenu::draw_equipmenu_portrait(const KPlayer& hero, Raster* portrait)
{
    constexpr int NameMaxLength = 8;

    // Semi-transparent box surrounding the Portrait on the top-left of the screen.
    Draw.menubox_inner(double_buffer, portraitMenu, eBoxFill::TRANSPARENT);

    // Display the party member's face and name on the right, centered.
    const int portraitLeftEdge = (FontW * portraitMenu.w - portrait->width) / 2;
    const std::string heroName = truncate_string(hero.name, NameMaxLength, true, "");
    const int nameXoffset = getCenteredTextXoffset(heroName, portraitMenu.w);
    const int nameYoffset = FontH * (portraitMenu.h - 1);

    // Each s_heroinfo::portrait is 40x40 pixels.
    draw_sprite(double_buffer, portrait, portraitMenu.x + portraitLeftEdge, portraitMenu.y);

    Draw.print_font(double_buffer, portraitMenu.x + nameXoffset, portraitMenu.y + nameYoffset, heroName, FNORMAL);
}

void KEquipMenu::draw_equipmenu(ePIDX pidxC)
{
    // Redraw the Equip menu, applying the player's earlier inputs to determine which text to
    // render, as well as which sections need to display frames around active/highlighted text.
    // This has been broken into submenu tasks, to better handle localized text.

    draw_equipmenu_top();
    draw_equipmenu_equipped(party[pidxC]);
    draw_equipmenu_portrait(party[pidxC], players[pidxC].portrait);
}

void KEquipMenu::draw_equippable(uint8_t player_index, eEquipment slot, uint16_t pptr)
{
    // Semi-transparent box surrounding the available equipment on the bottom-left of the screen.
    Draw.menubox_inner(double_buffer, equippableMenu, eBoxFill::TRANSPARENT);
    const ePIDX pidxC = pidx[player_index];
    const auto& partyMember = party[pidxC];

    if (slot == eEquipment::EQP_NONE)
    {
        t_inv.clear();
    }
    else
    {
        calc_possible_equip(pidxC, slot);
    }

    constexpr int cursorXoffset = 1; // blank space for cursor
    constexpr int iconXoffset = 1;
    const int iconLeftEdge = FontW * cursorXoffset;
    const int textLeftEdge = FontW * (cursorXoffset + iconXoffset);

    // -5 to account for "cursor" and icon on left, plus "x#" and up/down "more" arrows on right.
    const int maxEquipmentLength = equippableMenu.w - 5;
    const int quantityLeftEdge = FontW * (equippableMenu.w - 3);

    // This is used to determine whether to draw equipment text below in FNORMAL or FDARK color.
    const uint8_t equippedWeapon = partyMember.eqp[EQP_WEAPON];
    const uint8_t equippedShield = partyMember.eqp[EQP_SHIELD];
    bool heroHasTwoHandedWeaponEquipped =
        (equippedWeapon != I_NOITEM && items[equippedWeapon].hnds == eWeaponRestrict::HAND_DOUBLE);
    bool heroHasShieldEquipped = (equippedShield != I_NOITEM);

    auto eptr = std::min<uint16_t>(t_inv.size(), pptr + NUM_ITEMS_PER_PAGE);
    for (uint16_t t_inv_index = pptr; t_inv_index < eptr; ++t_inv_index)
    {
        const auto g_inv_index = t_inv[t_inv_index];
        if (g_inv_index >= g_inv.size())
        {
            continue;
        }
        auto [items_index, quantity] = g_inv[g_inv_index];
        const int itemname_y = t_inv_index - pptr;
        const int rowYoffset = FontH * itemname_y;
        const auto& selected_item = items[items_index];

        Draw.draw_icon(double_buffer, selected_item.icon, equippableMenu.x + iconLeftEdge,
                       equippableMenu.y + rowYoffset);

        // Sometimes, the hero may be equipped with a two-handed weapon, and will be unable to
        // equip an item in this list (like a shield). Still show this item but greyed out (other
        // logic will prevent the player from selecting the item and play the SND_BAD effect if
        // they try anyway).
        const auto eqp_type = selected_item.type;
        bool showItemDisabled = false;
        if (selected_item.type == eEquipment::EQP_SHIELD && heroHasTwoHandedWeaponEquipped)
        {
            showItemDisabled = true;
        }
        else if (selected_item.type == eEquipment::EQP_WEAPON && selected_item.hnds == eWeaponRestrict::HAND_DOUBLE &&
                 heroHasShieldEquipped)
        {
            showItemDisabled = true;
        }
        eFontColor canBeEquippedColor = showItemDisabled ? FDARK : FNORMAL;

        strbuf = truncate_string(selected_item.item_name, maxEquipmentLength, true, "");
        Draw.print_font(double_buffer, equippableMenu.x + textLeftEdge, equippableMenu.y + rowYoffset, strbuf,
                        canBeEquippedColor);
        if (quantity > 1)
        {
            sprintf(strbuf, "^%d", quantity);
            Draw.print_font(double_buffer, equippableMenu.x + quantityLeftEdge, equippableMenu.y + rowYoffset, strbuf,
                            canBeEquippedColor);
        }
    }

    // Draw up & down arrows if needed.
    if (pptr > 0)
    {
        static const int upArrowXoffset = FontW * (equippableMenu.w - 1);
        static const int upArrowYoffset = 0;
        draw_sprite(double_buffer, upptr, equippableMenu.x + upArrowXoffset, equippableMenu.y + upArrowYoffset);
    }
    if (pptr + NUM_ITEMS_PER_PAGE < t_inv.size())
    {
        static const int downArrowXoffset = FontW * (equippableMenu.w - 1);
        static const int downArrowYoffset = FontH * (equippableMenu.h - 1);
        draw_sprite(double_buffer, dnptr, equippableMenu.x + downArrowXoffset, equippableMenu.y + downArrowYoffset);
    }
}

void KEquipMenu::draw_equippreview(uint8_t player_index, eEquipment slot, uint8_t eqp_item)
{
    // Semi-transparent box surrounding the stats previews on the lower-right of the screen.
    Draw.menubox_inner(double_buffer, previewMenu, eBoxFill::TRANSPARENT);

    constexpr int MaxAcronymLength = 3;
    // Localized text will be capped at MaxAcronymLength [3] letters long, and a ':' will be appended.
    static const std::vector<std::string> EquipAttributes =
        truncate_strings({ _("Str"), _("Agi"), _("Vit"), _("Int"), _("Sag"), _("Spd"), _("Aur"), _("Spi"), _("Att"),
                           _("Hit"), _("Def"), _("Evd"), _("Mdf") },
                         MaxAcronymLength, false, ":");

    if (slot < eEquipment::EQP_NONE)
    {
        calc_equippreview(player_index, slot, eqp_item);
    }
    else
    {
        kmenu.update_equipstats();
    }

    // This frame contains 4 distinct columns:
    //  1. 4 chars for the 3-letter EquipAttributes stat abbreviations, plus ':'.
    //  2. 3 chars for current stat values [0..999], right-aligned.
    //  3. 3 chars for " > " between current and preview stat values.
    //  4. 3 chars for preview stat values [0..999], right-aligned.
    //
    // All combined, the char count should not exceed previewMenu.w [13] characters.

    static const int previewLeftEdge = FontW * (previewMenu.w - 3);

    for (uint8_t stats_index = 0; stats_index < eStat::NUM_STATS; ++stats_index)
    {
        const int currentToolStat = fighter[player_index].stats[stats_index];
        const int highlightedToolStat = (slot != eEquipment::EQP_NONE) ? tstats[stats_index] : currentToolStat;

        const int rowYoffset = FontH * stats_index;

        // On the left, print the stat acronym and ':', the current stat, and " > " all in white.
        sprintf(strbuf, "%4s%3d > ", EquipAttributes[stats_index].c_str(), currentToolStat);

        Draw.print_font(double_buffer, previewMenu.x, previewMenu.y + rowYoffset, strbuf, FNORMAL);

        // On the right, display the currently-selected tool's stats in one of 3 colors, to show
        // the player that the stats may be the same (white), higher (green) or lower (red).
        sprintf(strbuf, "%3d", highlightedToolStat);

        eFontColor statFontColor = FNORMAL;
        if (highlightedToolStat > currentToolStat)
        {
            statFontColor = FGREEN;
        }
        else if (highlightedToolStat < currentToolStat)
        {
            statFontColor = FRED;
        }

        Draw.print_font(double_buffer, previewMenu.x + previewLeftEdge, previewMenu.y + rowYoffset, strbuf,
                        statFontColor);
    }

    // Semi-transparent box surrounding the "Resist up/down" message on the bottom-right.
    Draw.menubox_inner(double_buffer, resistMenu, eBoxFill::TRANSPARENT);

    if (slot < eEquipment::EQP_NONE)
    {
        int sum_current_res = 0;
        int sum_preview_res = 0;

        for (uint8_t res_index = 0; res_index < eResistance::R_TOTAL_RES; ++res_index)
        {
            sum_current_res += fighter[player_index].res[res_index];
            sum_preview_res += tres[res_index];
        }

        if (sum_current_res != sum_preview_res)
        {
            static const std::vector<std::string> resists {
                _("Resist up"),
                _("Resist down"),
            };
            const size_t resists_index = sum_preview_res > sum_current_res ? 0 : 1;
            strbuf = truncate_string(resists[resists_index], resistMenu.w, true, "");

            const int resistXoffset = getCenteredTextXoffset(strbuf, resistMenu.w);
            Draw.print_font(double_buffer, resistMenu.x + resistXoffset, resistMenu.y, strbuf, FNORMAL);
        }
    }
}

bool KEquipMenu::equip(ePIDX pidxC, uint32_t selected_item_index)
{
    if (selected_item_index >= g_inv.size() || pidxC <= ePIDX::PIDX_UNDEFINED || pidxC >= MAXCHRS)
    {
        return false;
    }

    const auto items_index = g_inv[selected_item_index].item;
    const auto& selected_item = items[items_index];
    const auto eqp_type = selected_item.type;
    if (eqp_type == eEquipment::EQP_NONE)
    {
        return false;
    }

    auto& partyMember = party[pidxC];
    const auto existing_item = partyMember.eqp[eqp_type];

    if (selected_item.eq[pidxC] == 0)
    {
        return false;
    }
    if (eqp_type == eEquipment::EQP_SHIELD)
    {
        const uint8_t weapon = partyMember.eqp[EQP_WEAPON];
        // Can't equip a shield if holding a two-handed weapon
        if (weapon != I_NOITEM && items[weapon].hnds == eWeaponRestrict::HAND_DOUBLE)
        {
            return false;
        }
    }
    else if (eqp_type == eEquipment::EQP_WEAPON)
    {
        const uint8_t shield = partyMember.eqp[EQP_SHIELD];
        // Can't equip a two-handed weapon if holding a shield
        if (shield != I_NOITEM && selected_item.hnds == eWeaponRestrict::HAND_DOUBLE)
        {
            return false;
        }
    }
    if (existing_item != I_NOITEM)
    {
        g_inv.add(existing_item);
    }
    // Place in the slot
    partyMember.eqp[eqp_type] = items_index;
    g_inv.removeIndex(selected_item_index);
    return true;
}

void KEquipMenu::processInputs()
{
    switch (equipInputState.currentState)
    {
    case EquipInputState::eInputState::ActionBar: {
        if (PlayerInput.left())
        {
            processInputsCb_ActionBarLeft();
        }
        if (PlayerInput.right())
        {
            processInputsCb_ActionBarRight();
        }
        if (PlayerInput.balt())
        {
            processInputsCb_ActionBarAlt();
        }
        if (PlayerInput.bctrl())
        {
            processInputsCb_ActionBarCtrl();
        }
        break;
    }
    case EquipInputState::eInputState::ChooseSlot: {
        if (PlayerInput.up())
        {
            processInputsCb_ChooseSlotUp();
        }
        if (PlayerInput.down())
        {
            processInputsCb_ChooseSlotDown();
        }
        if (PlayerInput.balt())
        {
            processInputsCb_ChooseSlotAlt();
        }
        if (PlayerInput.bctrl())
        {
            processInputsCb_ChooseSlotCtrl();
        }
        break;
    }
    case EquipInputState::eInputState::SelectEquipment: {
        if (PlayerInput.up() || PlayerInput.up.isRepeating())
        {
            processInputsCb_SelectEquipmentUp();
        }
        if (PlayerInput.down() || PlayerInput.down.isRepeating())
        {
            processInputsCb_SelectEquipmentDown();
        }
        if (PlayerInput.balt())
        {
            processInputsCb_SelectEquipmentAlt();
        }
        if (PlayerInput.bctrl())
        {
            processInputsCb_SelectEquipmentCtrl();
        }
        break;
    }
    default:
        break;
    }
}

void KEquipMenu::processInputsCb_ChooseSlotUp()
{
    // Move between "Hand1", "Hand2", "Head", "Body", "Arms", "Other" slots.
    eEquipment slot = eEquipment::EQP_NONE;
    switch (equipInputState.slot)
    {
    case eEquipment::EQP_WEAPON:
        slot = eEquipment::EQP_SPECIAL;
        break;
    case eEquipment::EQP_SHIELD:
        slot = eEquipment::EQP_WEAPON;
        break;
    case eEquipment::EQP_HELMET:
        slot = eEquipment::EQP_SHIELD;
        break;
    case eEquipment::EQP_ARMOR:
        slot = eEquipment::EQP_HELMET;
        break;
    case eEquipment::EQP_HAND:
        slot = eEquipment::EQP_ARMOR;
        break;
    case eEquipment::EQP_SPECIAL:
        slot = eEquipment::EQP_HAND;
        break;
    default:
        slot = eEquipment::EQP_SPECIAL;
        break;
    }
    equipInputState.slot = slot;
    play_effect(KAudio::eSound::SND_CLICK, 128);
}

void KEquipMenu::processInputsCb_SelectEquipmentUp()
{
    // Move around available inventory list.
    if (equipInputState.cursorYoffset > 0)
    {
        // Move up active page.
        --equipInputState.cursorYoffset;
    }
    else
    {
        // Move into previous page.
        if (equipInputState.scrollYoffset > 0)
        {
            --equipInputState.scrollYoffset;
        }
    }
    play_effect(KAudio::eSound::SND_CLICK, 128);
}

void KEquipMenu::processInputsCb_ChooseSlotDown()
{
    // Move between "Hand1", "Hand2", "Head", "Body", "Arms", "Other" slots.
    eEquipment slot = eEquipment::EQP_NONE;
    switch (equipInputState.slot)
    {
    case eEquipment::EQP_WEAPON:
        slot = eEquipment::EQP_SHIELD;
        break;
    case eEquipment::EQP_SHIELD:
        slot = eEquipment::EQP_HELMET;
        break;
    case eEquipment::EQP_HELMET:
        slot = eEquipment::EQP_ARMOR;
        break;
    case eEquipment::EQP_ARMOR:
        slot = eEquipment::EQP_HAND;
        break;
    case eEquipment::EQP_HAND:
        slot = eEquipment::EQP_SPECIAL;
        break;
    case eEquipment::EQP_SPECIAL:
        slot = eEquipment::EQP_WEAPON;
        break;
    default:
        slot = eEquipment::EQP_WEAPON;
        break;
    }
    equipInputState.slot = slot;
    play_effect(KAudio::eSound::SND_CLICK, 128);
}

void KEquipMenu::processInputsCb_SelectEquipmentDown()
{
    // Move around available inventory list.
    if (equipInputState.cursorYoffset < NUM_ITEMS_PER_PAGE - 1)
    {
        // Move down active page.
        ++equipInputState.cursorYoffset;
    }
    else
    {
        // Move into next page.
        const unsigned int page_ptr = equipInputState.scrollYoffset + equipInputState.cursorYoffset;
        if (page_ptr < t_inv.size() - 1)
        {
            ++equipInputState.scrollYoffset;
        }
    }
    play_effect(KAudio::eSound::SND_CLICK, 128);
}

void KEquipMenu::processInputsCb_ActionBarLeft()
{
    // Move between "Equip", "Optimize", "Remove", "Empty" options.
    eActionBar newAction = eActionBar::AB_NONE;
    switch (activeActionBar)
    {
    case eActionBar::AB_EQUIP:
        newAction = eActionBar::AB_CLEAR;
        break;
    case eActionBar::AB_OPTIMIZE:
        newAction = eActionBar::AB_EQUIP;
        break;
    case eActionBar::AB_REMOVE:
        newAction = eActionBar::AB_OPTIMIZE;
        break;
    case eActionBar::AB_CLEAR:
        newAction = eActionBar::AB_REMOVE;
        break;
    default:
        newAction = eActionBar::AB_CLEAR;
        break;
    }
    activeActionBar = newAction;
    play_effect(KAudio::eSound::SND_CLICK, 128);
}

void KEquipMenu::processInputsCb_ActionBarRight()
{
    // Move between "Equip", "Optimize", "Remove", "Empty" options.
    eActionBar newAction = eActionBar::AB_NONE;
    switch (activeActionBar)
    {
    case eActionBar::AB_EQUIP:
        newAction = eActionBar::AB_OPTIMIZE;
        break;
    case eActionBar::AB_OPTIMIZE:
        newAction = eActionBar::AB_REMOVE;
        break;
    case eActionBar::AB_REMOVE:
        newAction = eActionBar::AB_CLEAR;
        break;
    case eActionBar::AB_CLEAR:
        newAction = eActionBar::AB_EQUIP;
        break;
    default:
        newAction = eActionBar::AB_EQUIP;
        break;
    }
    activeActionBar = newAction;
    play_effect(KAudio::eSound::SND_CLICK, 128);
}

void KEquipMenu::processInputsCb_ActionBarAlt()
{
    const ePIDX pidxC = pidx[equipInputState.player_index];
    // Activate highlighted "Equip", "Optimize", "Remove", "Empty" option.
    switch (activeActionBar)
    {
    case eActionBar::AB_EQUIP: {
        equipInputState.currentState = EquipInputState::eInputState::ChooseSlot;
        break;
    }
    case eActionBar::AB_OPTIMIZE: {
        const bool didOptimize = optimize_equip(equipInputState.player_index);
        play_effect(didOptimize ? KAudio::eSound::SND_EQUIP : KAudio::eSound::SND_BAD, 128);
        break;
    }
    case eActionBar::AB_REMOVE: {
        equipInputState.currentState = EquipInputState::eInputState::ChooseSlot;
        break;
    }
    case eActionBar::AB_CLEAR: {
        bool all_ok = true;

        for (uint8_t eqp_index = 0; eqp_index < eEquipment::NUM_EQUIPMENT; ++eqp_index)
        {
            if (party[pidxC].eqp[eqp_index] == I_NOITEM)
            {
                continue;
            }
            all_ok &= deequip(pidxC, (eEquipment)eqp_index);
        }

        play_effect(all_ok ? KAudio::eSound::SND_UNEQUIP : KAudio::eSound::SND_BAD, 128);
        break;
    }
    default:
        break;
    }
}

void KEquipMenu::processInputsCb_ChooseSlotAlt()
{
    const ePIDX pidxC = pidx[equipInputState.player_index];
    // Cursor is over one of "Hand1", "Hand2", "Head", "Body", "Arms", "Other" slots.
    if (activeActionBar == eActionBar::AB_EQUIP)
    {
        if (t_inv.empty())
        {
            play_effect(KAudio::eSound::SND_BAD, 128);
        }
        else
        {
            // Scrollbar offset from the top of the equipment list. When > 0, it means the player scrolled
            // down enough that there are one or more items above the top of the visible list (should show
            // an "^" chevron to indicate more items above).
            equipInputState.cursorYoffset = 0;
            equipInputState.scrollYoffset = 0;

            equipInputState.currentState = EquipInputState::eInputState::SelectEquipment;
        }
    }
    else if (activeActionBar == eActionBar::AB_REMOVE)
    {
        if (deequip(pidxC, equipInputState.slot))
        {
            play_effect(KAudio::eSound::SND_UNEQUIP, 128);
            equipInputState.currentState = EquipInputState::eInputState::ActionBar;
        }
        else
        {
            play_effect(KAudio::eSound::SND_BAD, 128);
        }
    }
}

void KEquipMenu::processInputsCb_SelectEquipmentAlt()
{
    const ePIDX pidxC = pidx[equipInputState.player_index];
    // Attempt to equip selected equipment in inventory list.
    const auto page_ptr = equipInputState.scrollYoffset + equipInputState.cursorYoffset;
    bool didEquip = false;
    if (page_ptr < t_inv.size())
    {
        if (equip(pidxC, t_inv[page_ptr]))
        {
            didEquip = true;
        }
    }

    if (didEquip)
    {
        equipInputState.currentState = EquipInputState::eInputState::ChooseSlot;
        play_effect(KAudio::eSound::SND_EQUIP, 128);
    }
    else
    {
        play_effect(KAudio::eSound::SND_BAD, 128);
    }
}

void KEquipMenu::processInputsCb_ActionBarCtrl()
{
    // From "Equip, Optimize, etc." top bar to exiting Equip menu entirely.
    equipInputState.currentState = EquipInputState::eInputState::Exit;
}

void KEquipMenu::processInputsCb_ChooseSlotCtrl()
{
    // From "Hand1, Hand2, etc." slot selection to "Equip, Optimize, etc." top bar.
    equipInputState.currentState = EquipInputState::eInputState::ActionBar;
}

void KEquipMenu::processInputsCb_SelectEquipmentCtrl()
{
    // From selecting from available inventory to "Hand1, Hand2, etc." slot selection.
    equipInputState.currentState = EquipInputState::eInputState::ChooseSlot;
}

void KEquipMenu::equip_menu(uint8_t player_index)
{
    equipInputState.player_index = player_index;
    const ePIDX pidxC = pidx[player_index];

    // If isFocusOnActionBar is true, navigation only affects the "Action Bar";
    // otherwise, navigation is down either selecting which gear/slot to modify or the items
    // to consider equipping for that gear/slot.

    activeActionBar = eActionBar::AB_EQUIP;
    play_effect(KAudio::eSound::SND_MENU, 128);

    equipInputState.currentState = EquipInputState::eInputState::ActionBar;
    equipInputState.isFocusOnActionBar = true;
    equipInputState.player_index = player_index;
    equipInputState.slot = eEquipment::EQP_WEAPON;

    while (equipInputState.currentState != EquipInputState::eInputState::Exit)
    {
        Game.ProcessEvents();
        Game.do_check_animation();
        Draw.drawmap();

        draw_equipmenu(pidxC);

        if (equipInputState.currentState == EquipInputState::eInputState::ActionBar)
        {
            // Action Bar: "Equip", "Optimize", "Remove", "Clear"
            const eEquipment slot = eEquipment::EQP_NONE;
            draw_equippable(equipInputState.player_index, slot, 0);
            draw_equippreview(equipInputState.player_index, slot, I_NOITEM);
        }
        else if (equipInputState.currentState == EquipInputState::eInputState::ChooseSlot)
        {
            const eEquipment slot =
                (activeActionBar == eActionBar::AB_REMOVE) ? equipInputState.slot : eEquipment::EQP_NONE;
            draw_equippable(equipInputState.player_index, equipInputState.slot, 0);
            draw_equippreview(equipInputState.player_index, slot, I_NOITEM);

            // Draw the "sword" cursor.
            draw_sprite(double_buffer, menuptr, equippedMenu.x - FontW, equippedMenu.y + FontH * equipInputState.slot);
        }
        else if (equipInputState.currentState == EquipInputState::eInputState::SelectEquipment)
        {
            draw_equippable(equipInputState.player_index, equipInputState.slot, equipInputState.scrollYoffset);

            const unsigned int page_ptr = equipInputState.scrollYoffset + equipInputState.cursorYoffset;
            if (page_ptr < t_inv.size())
            {
                const auto g_inv_index = t_inv[page_ptr];
                draw_equippreview(equipInputState.player_index, equipInputState.slot, g_inv[g_inv_index].item);
            }

            // Draw the "sword" cursor.
            draw_sprite(double_buffer, menuptr, equippableMenu.x - FontW,
                        equippableMenu.y + FontH * equipInputState.cursorYoffset);
        }
        Draw.blit2screen();

        processInputs();
    }
}

bool KEquipMenu::optimize_equip(uint8_t player_index)
{
    const ePIDX pidxC = pidx[player_index];
    // First, de-equip all slots
    for (uint8_t eqp_index = 0; eqp_index < eEquipment::NUM_EQUIPMENT; ++eqp_index)
    {
        if (party[pidxC].eqp[eqp_index] != I_NOITEM)
        {
            if (!deequip(pidxC, (eEquipment)eqp_index))
            {
                return false;
            }
        }
    }

    // Equip Hand1
    int maxx = 0;  // Highest attack/defense currently calculated for each equipment type.
    int maxi = -1; // Index within g_inv[] array for "best" equipment.
    calc_possible_equip(pidxC, eEquipment::EQP_WEAPON);

    // Base the "best" weapon off its Attack stat.
    for (size_t t_inv_index = 0, t_inv_size = t_inv.size(); t_inv_index < t_inv_size; ++t_inv_index)
    {
        const auto g_inv_index = t_inv[t_inv_index];
        if (g_inv_index >= g_inv.size())
        {
            continue;
        }
        int items_index = g_inv[g_inv_index].item;
        int attack_stats = items[items_index].stats[eStat::Attack];
        if (attack_stats > maxx)
        {
            maxx = attack_stats;
            maxi = t_inv_index;
        }
    }
    if (maxi > -1 && maxi < t_inv.size())
    {
        equip(pidxC, t_inv[maxi]);
    }

    // Equip Hand2, Head, Body, Arms
    for (uint8_t eqp_index = eEquipment::EQP_SHIELD; eqp_index <= eEquipment::EQP_HAND; ++eqp_index)
    {
        maxx = 0;
        maxi = -1;
        calc_possible_equip(pidxC, (eEquipment)eqp_index);

        // Base the "best" shield/helmet/chest/arm off its combined physical+magical defense stats.
        for (size_t t_inv_index = 0, t_inv_size = t_inv.size(); t_inv_index < t_inv_size; ++t_inv_index)
        {
            const auto g_inv_index = t_inv[t_inv_index];
            if (g_inv_index >= g_inv.size())
            {
                continue;
            }
            int items_index = g_inv[g_inv_index].item;
            int total_defense_stats =
                items[items_index].stats[eStat::Defense] + items[items_index].stats[eStat::MagicDefense];
            if (total_defense_stats > maxx)
            {
                maxx = total_defense_stats;
                maxi = t_inv_index;
            }
        }
        if (maxi > -1 && maxi < t_inv.size())
        {
            equip(pidxC, t_inv[maxi]);
        }
    }

    // Equip Other
    maxx = 0;
    maxi = -1;
    calc_possible_equip(pidxC, eEquipment::EQP_SPECIAL);

    // Base the "best" special equipment stat off its accumulated elemental resistances.
    for (size_t t_inv_index = 0, t_inv_size = t_inv.size(); t_inv_index < t_inv_size; ++t_inv_index)
    {
        const auto g_inv_index = t_inv[t_inv_index];
        if (g_inv_index >= g_inv.size())
        {
            continue;
        }
        int items_index = g_inv[g_inv_index].item;
        const auto& item = items[items_index];
        int total_stats_and_res = 0;
        for (uint8_t stats_index = 0; stats_index < eStat::NUM_STATS; ++stats_index)
        {
            total_stats_and_res += item.stats[stats_index];
        }
        for (uint8_t res_index = 0; res_index < eResistance::R_TOTAL_RES; ++res_index)
        {
            total_stats_and_res += item.item_resistance[res_index];
        }
        if (total_stats_and_res > maxx)
        {
            maxx = total_stats_and_res;
            maxi = t_inv_index;
        }
    }
    if (maxi > -1 && maxi < t_inv.size())
    {
        const auto g_inv_index = t_inv[maxi];
        equip(pidxC, g_inv_index);
    }
    return true;
}
