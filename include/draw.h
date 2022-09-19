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

#include "enums.h"

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

class Raster;
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_PixelFormat;
enum eSpellType;

constexpr uint8_t GREY1 = 4;  // pal[4]:  { 16, 16, 16, 0 }
constexpr uint8_t GREY2 = 8;  // pal[8]:  { 33, 33, 33, 0 }
constexpr uint8_t GREY3 = 13; // pal[13]: { 55, 55, 55, 0 }
constexpr uint8_t WHITE = 15; // pal[15]: { 63, 63, 63, 0 }
constexpr uint8_t DTRANS = 2; // pal[2]:  { 8, 8, 8, 0 }
constexpr uint8_t DBLUE = 3;  // pal[3]:  { 12, 12, 12, 0 }
constexpr uint8_t DRED = 6;   // pal[6]:  { 24, 24, 24, 0 }

enum eFontColor
{
    FNORMAL = 0,
    FRED = 1,
    FYELLOW = 2,
    FGREEN = 3,
    FDARK = 4,
    FGOLD = 5,
    FBIG = 6,

    NUM_FONT_COLORS // always last
};

enum eBubbleStyle
{
    B_TEXT = 0,
    B_THOUGHT = 1,
    B_MESSAGE = 2,

    NUM_BUBBLE_STYLES // always last
};

/* These should correspond with the stems found in MISC.
 * Bubbles are either solid (for speech) or wavy (for thoughts).
 */
enum eBubbleStemStyle
{
    STEM_UNDEFINED = -1,

    STEM_BOTTOM_RIGHT = 0,
    STEM_BOTTOM_LEFT = 1,
    STEM_TOP_RIGHT = 2,
    STEM_TOP_LEFT = 3,

    NUM_BUBBLE_STEMS // always last
};

enum class eBoxFill
{
    TRANSPARENT,
    DARK,
    LIGHT
};

struct Rect
{
    int x, y;
    int w, h;
};

class KDraw
{
  public:
    KDraw();
    /*! \brief Set up drawing system.
     *
     * This will store the window, and set up a renderer and texture to go with it.
     *
     * \param   _window KQ's window.
     */
    void set_window(SDL_Window* _window);

    /*! \brief Get KQ's current window. */
    SDL_Window* get_window() const
    {
        return window;
    }

    /*! \brief Resize window.
     *
     * \param   w The new width.
     * \param   h The new height.
     * \param   windowed True to go to windowed mode, false to go to full screen.
     */
    void resize_window(int w, int h, bool windowed);

    /*! \brief Copies from the double buffer to the screen.
     *
     * Handles frame-rate display, stretching and vsync waiting.
     */
    void blit2screen();

    /*! \brief Takes a bitmap and scales it to fit in the color range specified. Output goes to a new bitmap.
     *
     * This is used to make a monochrome version of a bitmap, for example to display a green, poisoned character,
     * or the red 'rage' effect for Sensar.
     *
     * This relies on the palette having continuous lightness ranges of one colour (as the KQ palette does!).
     *
     * An alternative would be to use makecol(), though this would incur a speed penalty.
     *
     * Another alternative would be to precalculate some maps for each case.
     *
     * \param   src Source bitmap.
     * \param   dest Destination bitmap.
     * \param   output_range_start Start of output color range.
     * \param   output_range_end End of output color range.
     */
    void color_scale(Raster* src, Raster* dest, int output_range_start, int output_range_end);

    /*! \brief Convert multiple frames.
     *
     * This is used to color_scale one or more fighter frames.
     *
     * \param   fighter_index Character to convert.
     * \param   output_range_start Start of output range.
     * \param   output_range_end End of output range (exclusive).
     * \param   convert_heroes If ==1 then fighter_index<PSIZE means convert all heroes,
     *          otherwise all enemies.
     */
    void convert_cframes(size_t fighter_index, int output_range_start, int output_range_end, int convert_heroes);

    /*! \brief Restore specified fighter frames to normal color.
     *
     * Restoring colors is done by blitting the 'master copy' from tcframes.
     *
     * \param   fighter_index Character to restore.
     * \param   revert_heroes If ==1 then convert all heroes if fighter_index < PSIZE, otherwise convert all enemies.
     */
    void revert_cframes(size_t fighter_index, int revert_heroes);

    /*! \brief Draw small icon.
     *
     * Just a helper function... reduces the number of places that 'sicons' has to be referenced.
     * Icons are 8x8 sub-bitmaps of sicons, representing items (sword, etc.).
     *
     * \param   where Bitmap to draw to.
     * \param   ino Icon to draw: usually corresponds to s_item::icon, s_special_item::icon, or s_spell::icon.
     * \param   icx X-coord to draw to.
     * \param   icy Y-coord to draw to.
     */
    void draw_icon(Raster* where, int ino, int icx, int icy);

    /*! \brief Draw status icon.
     *
     * Just a helper function... reduces the number of places that 'stspics' has to be referenced.
     * Status icons are 8x8 sub-bitmaps of \p stspics, representing poisoned, etc.
     *
     * \param   where Bitmap to draw to.
     * \param   cc Non-zero if in combat mode (draw using info \p fighter[] rather than \p party[] ).
     * \param   who Character to draw status for.
     * \param   inum The maximum number of status icons to draw:
     *          \p inum ==17 when in combat, \p inum ==8 otherwise.
     * \param   icx X-coord to draw to.
     * \param   icy Y-coord to draw to.
     */
    void draw_stsicon(Raster* where, int cc, int who, eSpellType inum, int icx, int icy);

    /*! \brief Check for forest square.
     *
     * Helper function for draw_char().
     * Returns whether the tile at the specified coordinates is a forest tile (only main map has forest).
     * This could be a headache if the tileset changes!
     *
     * Looks in the \p map_seg[] array.
     *
     * \param   fx X-coord to check.
     * \param   fy Y-coord to check.
     * \returns 1 if it is a forest square, 0 otherwise.
     */
    int is_forestsquare(int fx, int fy);

    /*! \brief Draw the map.
     *
     * Draws the background, character, middle, foreground and shadow layers.
     * The order, and the parallaxing, is specified by the mode.
     * There are 6 modes, as set in the .map file
     *  - 0 Order BMCFS,
     *  - 1 Order BCMFS,
     *  - 2 Order BMCFS, Background parallax
     *  - 3 Order BCMFS, Background & middle parallax
     *  - 4 Order BMCFS, Middle & foreground parallax
     *  - 5 Order BCMFS, Foreground parallax
     *
     * In current KQ maps, only modes 0..2 are used, with the majority being 0.
     * Also handles the Repulse indicator and the map description display.
     *
     * \bug PH: Shadows are never drawn with parallax (is this a bug?).
     */
    void drawmap();

    /*! \brief Draw menu box surrounding the specified coordinates.
     *
     * Given Rect{20, 24, 5, 6}:
     *  - the left border would be drawn at x=[20-8=12]
     *  - the top border would be drawn at y=[24-8=16]
     *  - the right border would be drawn at x=[20+(5+1)*8=68]
     *  - the bottom border would be drawn at y=[24+(6+1)*8=80]
     *
     * \param   where Bitmap to draw to.
     * \param   rect Rect with the x, y, width and height values.
     * \param   color Fill color
     */
    void menubox_inner(Raster* where, Rect rect, eBoxFill color);

    /*! \brief Draw menu box.
     *
     * \param   where Bitmap to draw to.
     * \param   rect Rect with the x, y, width and height values.
     * \param   color Fill color
     */
    void menubox(Raster* where, Rect rect, eBoxFill color);

    /*! \brief Draw menu box.
     *
     * \param   where Bitmap to draw to.
     * \param   x X coord.
     * \param   y Y coord.
     * \param   width Width.
     * \param   height Height.
     * \param   color Fill color
     */
    void menubox(Raster* where, int x, int y, int width, int height, eBoxFill color);

    /*! \brief Display string.
     *
     * Display a string in a particular font on a bitmap at the specified coordinates.
     *
     * \param   where Bitmap to draw to.
     * \param   sx X-coord in pixels.
     * \param   sy Y-coord in pixels.
     * \param   msg String to draw.
     * \param   font_index Font index in range [0..NUM_FONT_COLORS-1].
     */
    void print_font(Raster* where, int sx, int sy, const std::string& msg, eFontColor font_index);

    /*! \brief Calculate font height.
     *
     * \param   index Font index in range [0..NUM_FONT_COLORS-1].
     * \returns Height in pixels.
     */
    int font_height(eFontColor index)
    {
        return index == FBIG ? 12 : 8;
    }

    /*! \brief Calculate text width.
     *
     * \param   index Font index in range [0..NUM_FONT_COLORS-1].
     * \param   s The text to get the length for.
     * \returns How many pixels the given string will take to render.
     */
    int text_length(eFontColor index, const char* s);

    /*! \brief Display number.
     *
     * Display a number using the small font on a bitmap at the specified coordinates and using the specified color.
     *
     * This still expects the number to be in a string... the function's real purpose is to use a different font for
     * numerical display in combat.
     *
     * \param   where Bitmap to draw to.
     * \param   sx X-coord in pixels.
     * \param   sy Y-coord in pixels.
     * \param   msg String to draw.
     * \param   font_index Font index in range [0..4].
     */
    void print_num(Raster* where, int sx, int sy, const std::string& msg, eFont font_index);

    /*! \brief Display speech/thought bubble.
     *
     * Displays text, like bubble_text, but passing the args through the relay function first.
     * \sa bubble_text()
     *
     * \param   fmt Format, B_TEXT or B_THOUGHT.
     * \param   who Character that is speaking.
     * \param   s The text to display.
     */
    void text_ex(eBubbleStyle fmt, int who, const char* s);

    /*! \brief Display speech/thought bubble with portrait.
     *
     * Displays text, like bubble_text, but passing the args through the relay function first.
     * \sa bubble_text()
     *
     * \param   fmt Format, B_TEXT or B_THOUGHT.
     * \param   who Character that is speaking.
     * \param   s The text to display.
     */
    void porttext_ex(eBubbleStyle fmt, int who, const char* s);

    /*! \brief Do user prompt.
     *
     * Draw a text box and wait for a response.  It is possible to offer up to four choices in a prompt box.
     *
     * \param   who Entity that is speaking.
     * \param   numopt Number of choices.
     * \param   bstyle Textbox style (B_TEXT or B_THOUGHT).
     * \param   sp1 Line 1 of text.
     * \param   sp2 Line 2 of text.
     * \param   sp3 Line 3 of text.
     * \param   sp4 Line 4 of text.
     * \returns Index of option chosen [0..numopt-1].
     */
    int prompt(int who, int numopt, eBubbleStyle bstyle, const char* sp1, const char* sp2, const char* sp3,
               const char* sp4);

    /*! \brief Prompt for user input.
     *
     * Present the user with a prompt and a list of options to select from.
     *
     * The prompt is shown, as per text_ex(), and the choices shown in a separate window at the bottom.
     * If the prompt is longer than one box-full, it is shown box-by-box, until the last one, when the
     * choices are shown.
     *
     * If there are more choices than will fit into the box at the bottom, arrows are shown to indicate more pages.
     *
     * Press ALT to select; CTRL does nothing.
     *
     * \param   who Which character is ASKING the question, or 255 if it's a 'general question'.
     * \param   ptext The prompt test.
     * \param   opt[] An array of option strings, null terminated.
     * \param   n_opt The number of options.
     * \returns Option selected, 0= first option etc.
     */
    int prompt_ex(int who, const char* ptext, const char* opt[], int n_opt);

    /*! \brief Alert player.
     *
     * Draw a single-line message in the center of the screen and wait for the confirm key to be pressed or for a
     * specific amount of time.
     *
     * \param   m Message text.
     * \param   icn Icon to display or 255 for none.
     * \param   delay Time to wait (milliseconds?).
     */
    void message(const char* m, int icn, int delay);

    /*! \brief Adjust view.
     *
     * This merely sets the view variables for use in other functions that rely on the view.
     * The view defines a subset of the map, for example when you move to a house in a town,
     * the view contracts to display only the interior.
     *
     * \param   vw Non-zero to enable view, otherwise show the whole map.
     * \param   x1 Top-left of view, in full tiles (such as entity.tilex).
     * \param   y1 Top-left of view, in full tiles (such as entity.tiley).
     * \param   x2 Bottom-right of view [inclusive], in full tiles (such as entity.tilex).
     * \param   y2 Bottom-right of view [inclusive], in full tiles (such as entity.tiley).
     */
    void set_view(int vw, int x1, int y1, int x2, int y2);

    /*! \brief Make a copy of a bitmap.
     *
     * Take a source bitmap and a target. If the target is NULL or too small, re-allocate it.
     * Then blit it.
     *
     * \param   target Bitmap to copy to or NULL.
     * \param   source Bitmap to copy from.
     * \returns Target or a new bitmap.
     */
    Raster* copy_bitmap(Raster* target, Raster* source);

    /*! \brief Lay out some text
     * Splits the given text at word boundaries so no line is longer
     * than the given layout width.
     *
     * \param text a line of text
     * \param layout_width the width in characters
     * \returns the text split into lines
     */
    std::vector<std::string> layout(const std::string& text, int layout_width);

  private:
    /*! \brief Draw border box.
     *
     * Draw the fancy-pants border that I use.
     * I hard-draw the border instead of using bitmaps, because that's just the way I am.
     * It doesn't degrade performance, so who cares :)
     * Border is about 4 pixels thick, fitting inside (x,y)-(x2,y2).
     *
     * \param   where Bitmap to draw to.
     * \param   left Top-left x-coord.
     * \param   top Top-left y-coord.
     * \param   right Bottom-right x-coord.
     * \param   bottom Bottom-right y-coord.
     */
    void border(Raster* where, int left, int top, int right, int bottom);

    /*! \brief Decode String.
     *
     * Extract the next unicode char from a UTF-8 string.
     *
     * \param   iter Text to decode.
     * \param   cp The next character.
     * \returns Pointer to after the next character.
     */
    std::string::const_iterator decode_utf8(std::string::const_iterator iter, uint32_t* cp);

    /*! Boundary adjusted for parallax. */
    struct PBound
    {
        int left;
        int top;
        int right;
        int bottom;
        int x_offset;
        int y_offset;
    };

    /*! \brief Calculate bounds.
     *
     * Calculate bounds based on current view if any and taking into account parallax.
     *
     * \param   is_parallax True if parallax applies to the layer under consideration.
     * \returns A bounding box.
     */
    PBound calculate_box(bool is_parallax);

    /*! \brief Draw background.
     *
     * Draw the background layer.  Accounts for parallaxing.
     * Parallax is on for modes 2 & 3
     */
    void draw_backlayer();

    /*! \brief Draw the heroes on map.
     *
     * It's kind of clunky, but this is also where it takes care of walking in forests and only showing a disembodied
     * head.
     *
     * Does not seem to do any parallaxing.
     */
    void draw_char();

    /*! \brief Draw foreground.
     *
     * Draw the foreground layer.  Accounts for parallaxing.
     * Parallax is on for modes 4 & 5.
     */
    void draw_forelayer();

    /*! \brief Draw box, with different backgrounds and borders.
     *
     * Draw the box as described.
     *
     * \param   where Bitmap to draw to.
     * \param   rect Struct with x, y, w, h members (all integer).
     * \param   bg background fill.
     * \param   bstyle Style of border.
     */
    void draw_kq_box(Raster* where, Rect rect, eBoxFill bg, eBubbleStyle bstyle);

    /*! \brief Draw box, with different backgrounds and borders.
     *
     * Draw the box as described.
     *
     * \param   where Bitmap to draw to.
     * \param   x1 Left edge.
     * \param   y1 Top edge.
     * \param   x2 Right edge (inclusive).
     * \param   y2 Bottom edge (inclusive).
     * \param   bg background fill.
     * \param   bstyle Style of border.
     */
    void draw_kq_box(Raster* where, int x1, int y1, int x2, int y2, eBoxFill bg, eBubbleStyle bstyle);

    /*! \brief Draw middle layer.
     *
     * Draw the middle layer.  Accounts for parallaxing.
     * Parallax is on for modes eMapMode::MAPMODE_1E2p3S and eMapMode::MAPMODE_1P2E3S.
     */
    void draw_midlayer();

    /*! \brief Check whether the player is standing inside a bounding area.
     *
     * If player is inside a bounding area, update the view_area coordinates before drawing to the map.
     * \note This does not appear to have been defined/implemented.
     */
    void draw_playerbound();

    /*! \brief Draw shadows.
     *
     * Draw the shadow layer... this beats making extra tiles.
     * This may be moved in the future to fall between the background and foreground layers.
     * Shadows are never parallaxed.
     */
    void draw_shadows();

    /*! \brief Draw text box.
     *
     * \param   bstyle Style (B_TEXT or B_THOUGHT or B_MESSAGE).
     */
    void draw_textbox(eBubbleStyle bstyle);

    /*! \brief Draw text box with portrait.
     *
     *  Shows the player's portrait and name with the text.
     *
     * \param   bstyle Style (B_TEXT or B_THOUGHT or B_MESSAGE).
     * \param   chr Party member index within party[] and players[] arrays that is doing the talking.
     */
    void draw_porttextbox(eBubbleStyle bstyle, int chr);

    /*! \brief Text box drawing.
     *
     * Generic routine to actually display a text box and wait for a keypress.
     *
     * \param   who Character that is speaking/thinking (ignored for B_MESSAGE style).
     * \param   box_style Style (B_TEXT or B_THOUGHT or B_MESSAGE).
     */
    void generic_text(int who, eBubbleStyle box_style, int isPort);

    /*! \brief Split text into lines.
     *
     * Takes a string and re-formats it to fit into the msgbuf text buffer, for displaying with generic_text().
     *
     * Processes as much as it can to fit in one box, and returns a pointer to the next unprocessed character.
     *
     * \param   buf The string to reformat.
     * \returns The rest of the string that has not been processed, or NULL if it has all been processed.
     */
    const char* relay(const char* buf);

    /*! \brief Calculate bubble position.
     *
     * Calculate where a text bubble should go in relation to the entity who is speaking.
     *
     * \param   entity_index If value is [0..MAX_ENTITIES-1], character that is speaking, otherwise 'general'.
     */
    void set_textpos(uint32_t entity_index);

    /*! \brief Get glyph index.
     *
     * Convert a unicode char to a glyph index.
     * \note Uses inefficient linear search for now.
     *
     * \param   cp Unicode character.
     * \returns Glyph index.
     */
    int get_glyph_index(uint32_t cp);

    /*! \brief Replace all occurrences of "from" with "to" and apply changes back to "str".
     *
     * Origin: http://stackoverflow.com/a/3418285/801098
     *
     * \param   str String to write out to.
     * \param   from Substring to be replace.
     * \param   to Substring to replace all occurrences of 'from' with.
     */
    void replaceAll(std::string& str, const std::string& from, const std::string& to);

    /*! \brief Insert character names.
     *
     * This checks a string for $0, or $1 and replaces with player names.
     *
     * This only handles extremely simple strings; you CAN break it if you try hard enough:
     *      "$$0" or "\\$0" or "\$0" or "$-1", etc.
     *
     * \param   the_string Input string.
     * \returns A string where any $0 or $1 are replaced by player names, or the original string if none found.
     */
    std::string parse_string(const std::string& the_string);

    // The internal processing modes during text reformatting; used in \sa relay()
    enum m_mode
    {
        M_UNDEF,
        M_SPACE,
        M_NONSPACE,
        M_END
    };
    SDL_Window* window;      ///< The target window.
    SDL_Renderer* renderer;  ///< The window's renderer
    SDL_Texture* texture;    ///< The target texture
    SDL_PixelFormat* format; ///< The format of the texture
    int btile;
};

/*  global variables  */
extern uint8_t BLUE;
extern uint8_t DARKBLUE;
extern uint8_t DARKRED;

extern KDraw Draw;
