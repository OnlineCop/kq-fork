/*! \page License
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
 * \brief Setup and menu code
 *
 * \author JB
 * \date ??????
 * \remark Updated  ML Oct-2002
 */

#include <stdio.h>
#include <string.h>

#include "combat.h"
#include "draw.h"
#include "kq.h"
#include "music.h"
#include "platform.h"
#include "res.h"
#include "setup.h"
#include "timing.h"


/*! \name Globals */

/*! Debug level 0..3 */
char debugging = 0;

/*! Speed-up for slower machines */
char slow_computer = 0;

/*  Internal variables  */
static DATAFILE *sfx[MAX_SAMPLES];

/*  Internal functions  */
static int load_samples (void);
static int getavalue (const char *, int, int, int, int);
static int getakey (void);
static void parse_allegro_setup (void);
static void parse_jb_setup (void);


/*! \brief Play sound effects / music if adjusting it */

/* TT: looks like a hack to me! :-) */
/* PH: I cannot tell a lie: it was Matthew... */
#define IF_VOLUME_ALERT() \
   if (!strcmp (capt, "Sound Volume")) {\
      set_volume (cv * 10, 0);\
      play_effect (1, 127);\
   } else if (!strcmp (capt, "Music Volume"))\
      set_music_volume (cv / 25.5);


/*! \brief Draw a setting and its title
 *
 * Helper function for the config menu.
 * The setting title is drawn, then its value right-aligned.
 * \author PH
 * \date 20030527
 * \param   y y-coord of line
 * \param   caption Title of the setting (e.g. "Windowed mode:")
 * \param   value The setting (e.g. "Yes")
 * \param   color The foreground colour of the text
 */
static void citem (int y, const char *caption, const char *value,
                   const int color)
{
   print_font (double_buffer, 48 + xofs, y + yofs, caption, color);
   print_font (double_buffer, SCREEN_H2 - 8 * strlen (value) + xofs, y + yofs, value, color);
}



/*! \brief Display configuration menu
 *
 * This is the config menu that is called from the system
 * menu.  Here you can adjust the music or sound volume, or
 * the speed that the battle gauge moves at.
 */
void config_menu (void)
{
   size_t stop = 0, ptr = 0;
   int p;
   int temp_key = 0;

#ifdef DEBUGMODE
# define MENU_SIZE 18
#else
# define MENU_SIZE 17
#endif
   static const char *dc[MENU_SIZE];

   /* Define rows with appropriate spacings for breaks between groups */
   int row[MENU_SIZE];

   for (p = 0; p < 4; p++)
      row[p] = (p + 4) * 8;     // (p * 8) + 32
   for (p = 4; p < 12; p++)
      row[p] = (p + 5) * 8;     // (p * 8) + 40
   for (p = 12; p < 15; p++)
      row[p] = (p + 6) * 8;     // (p * 8) + 48
   for (p = 15; p < MENU_SIZE; p++)
      row[p] = (p + 7) * 8;     // (p * 8) + 56

   /* Helper strings */
   dc[0] = _("Display KQ in a window.");
   dc[1] = _("Stretch to fit 640x480 resolution.");
   dc[2] = _("Display the frame rate during play.");
   dc[3] = _("Wait for vertical retrace.");
   dc[4] = _("Key used to move up.");
   dc[5] = _("Key used to move down.");
   dc[6] = _("Key used to move left.");
   dc[7] = _("Key used to move right.");
   dc[8] = _("Key used to confirm action.");
   dc[9] = _("Key used to cancel action.");
   dc[10] = _("Key used to call character menu.");
   dc[11] = _("Key used to call system menu.");
   dc[12] = _("Toggle sound and music on/off.");
   dc[13] = _("Overall sound volume (affects music).");
   dc[14] = _("Music volume.");
   dc[15] = _("Animation speed-ups for slow machines.");
   dc[16] = _("Toggle how to allocate CPU usage.");
#ifdef DEBUGMODE
   dc[17] = _("Things you can do only in DebugMode.");
#endif

   unpress ();
   push_config_state ();
   set_config_file (kqres (SETTINGS_DIR, "kq.cfg"));
   while (!stop) {
      check_animation ();
      drawmap ();
      menubox (double_buffer, 88 + xofs, yofs, 16, 1, BLUE);
      print_font (double_buffer, 96 + xofs, 8 + yofs, _("KQ Configuration"), FGOLD);
      menubox (double_buffer, 32 + xofs, 24 + yofs, 30, MENU_SIZE + 3, BLUE);

      citem (row[0], _("Windowed mode:"), windowed == 1 ? _("YES") : _("NO"), FNORMAL);
      citem (row[1], _("Stretch Display:"), stretch_view == 1 ? _("YES") : _("NO"), FNORMAL);
      citem (row[2], _("Show Frame Rate:"), show_frate == 1 ? _("YES") : _("NO"), FNORMAL);
      citem (row[3], _("Wait for Retrace:"), wait_retrace == 1 ? _("YES") : _("NO"), FNORMAL);
      citem (row[4], _("Up Key:"), kq_keyname (kup), FNORMAL);
      citem (row[5], _("Down Key:"), kq_keyname (kdown), FNORMAL);
      citem (row[6], _("Left Key:"), kq_keyname (kleft), FNORMAL);
      citem (row[7], _("Right Key:"), kq_keyname (kright), FNORMAL);
      citem (row[8], _("Confirm Key:"), kq_keyname (kalt), FNORMAL);
      citem (row[9], _("Cancel Key:"), kq_keyname (kctrl), FNORMAL);
      citem (row[10], _("Menu Key:"), kq_keyname (kenter), FNORMAL);
      citem (row[11], _("System Menu Key:"), kq_keyname (kesc), FNORMAL);
      citem (row[12], _("Sound System:"), is_sound ? _("ON") : _("OFF"), FNORMAL);

      p = FNORMAL;
      /* TT: This needs to check for ==0 because 1 means sound init */
      if (is_sound == 0)
         p = FDARK;

      sprintf (strbuf, "%3d%%", gsvol * 100 / 250);
      citem (row[13], _("Sound Volume:"), strbuf, p);

      sprintf (strbuf, "%3d%%", gmvol * 100 / 250);
      citem (row[14], _("Music Volume:"), strbuf, p);

      citem (row[15], _("Slow Computer:"), slow_computer ? _("YES") : _("NO"), FNORMAL);

      if (cpu_usage)
         sprintf (strbuf, _("rest(%d)"), cpu_usage - 1);
      else
         sprintf (strbuf, "yield_timeslice()");
      citem (row[16], _("CPU Usage:"), strbuf, FNORMAL);

#ifdef DEBUGMODE
      if (debugging)
         sprintf (strbuf, "%d", debugging);
      citem (row[17], _("DebugMode Stuff:"), debugging ? strbuf : _("OFF"), FNORMAL);
#endif

      /* This affects the VISUAL placement of the arrow */
      p = ptr;
      if (ptr > 3)
         p++;
      if (ptr > 11)
         p++;
      if (ptr > 14)
         p++;
      draw_sprite (double_buffer, menuptr, 32 + xofs, p * 8 + 32 + yofs);

      /* This is the bottom window, where the description goes */
      menubox (double_buffer, xofs, 216 + yofs, 38, 1, BLUE);
      print_font (double_buffer, 8 + xofs, 224 + yofs, dc[ptr], FNORMAL);
      blit2screen (xofs, yofs);

      readcontrols ();
      if (up) {
         unpress ();
         // "jump" over unusable options
         if (ptr == 15 && is_sound == 0)
            ptr -= 2;
         if (ptr > 0)
            ptr--;
         else
            ptr = MENU_SIZE - 1;
         play_effect (SND_CLICK, 128);
      }
      if (down) {
         unpress ();
         // "jump" over unusable options
         if (ptr == 12 && is_sound == 0)
            ptr += 2;
         if (ptr < MENU_SIZE - 1)
            ptr++;
         else
            ptr = 0;
         play_effect (SND_CLICK, 128);
      }
      if (balt) {
         unpress ();
         switch (ptr) {
         case 0:
#ifdef __DJGPP__
            text_ex (B_TEXT, 255,
                     _("This version of KQ was compiled for DOS and does not support windowed mode"));
#else
            text_ex (B_TEXT, 255,
                     _("Changing the display mode to or from windowed view could have serious ramifications. It is advised that you save first."));
            if (windowed == 0)
               sprintf (strbuf, _("Switch to windowed mode?"));
            else
               sprintf (strbuf, _("Switch to full screen?"));
            p = prompt (255, 2, B_TEXT, strbuf, _("  no"), _("  yes"), "");
            if (p == 1) {
               windowed = !windowed;
               set_config_int (NULL, "windowed", windowed);
               set_graphics_mode ();
            }
#endif
            break;
         case 1:
#ifdef __DJGPP__
            text_ex (B_TEXT, 255,
                     _("This version of KQ was compiled for DOS and does not support stretching"));
#else
            text_ex (B_TEXT, 255,
                     _("Changing the stretched view option could have serious ramifications. It is advised that you save your game before trying this."));
            if (stretch_view == 0)
               sprintf (strbuf, _("Try to stretch the display?"));
            else
               sprintf (strbuf, _("Switch to unstretched display?"));
            p = prompt (255, 2, B_TEXT, strbuf, _("  no"), _("  yes"), "");
            if (p == 1) {
               stretch_view = !stretch_view;
               set_config_int (NULL, "stretch_view", stretch_view);
               set_graphics_mode ();
            }
#endif
            break;
         case 2:
            show_frate = !show_frate;
            set_config_int (NULL, "show_frate", show_frate);
            break;
         case 3:
            wait_retrace = !wait_retrace;
            set_config_int (NULL, "wait_retrace", wait_retrace);
            break;
         case 4:
            while ((temp_key = getakey ()) == 0) {}
            kup = temp_key;
            unpress ();
            temp_key = 0;
            set_config_int (NULL, "kup", kup);
            break;
         case 5:
            while ((temp_key = getakey ()) == 0) {}
            kdown = temp_key;
            unpress ();
            temp_key = 0;
            set_config_int (NULL, "kdown", kdown);
            break;
         case 6:
            while ((temp_key = getakey ()) == 0) {}
            kleft = temp_key;
            unpress ();
            temp_key = 0;
            set_config_int (NULL, "kleft", kleft);
            break;
         case 7:
            while ((temp_key = getakey ()) == 0) {}
            kright = temp_key;
            unpress ();
            temp_key = 0;
            set_config_int (NULL, "kright", kright);
            break;
         case 8:
            while ((temp_key = getakey ()) == 0) {}
            kalt = temp_key;
            unpress ();
            temp_key = 0;
            set_config_int (NULL, "kalt", kalt);
            break;
         case 9:
            while ((temp_key = getakey ()) == 0) {}
            kctrl = temp_key;
            unpress ();
            temp_key = 0;
            set_config_int (NULL, "kctrl", kctrl);
            break;
         case 10:
            while ((temp_key = getakey ()) == 0) {}
            kenter = temp_key;
            unpress ();
            temp_key = 0;
            set_config_int (NULL, "kenter", kenter);
            break;
         case 11:
            while ((temp_key = getakey ()) == 0) {}
            kesc = temp_key;
            unpress ();
            temp_key = 0;
            set_config_int (NULL, "kesc", kesc);
            break;
         case 12:
            if (is_sound == 2)
               sound_init ();
            else {
               if (is_sound == 0) {
                  is_sound = 1;
                  print_font (double_buffer, 92 + 2 + xofs, 204 + yofs,
                              _("...please wait..."), FNORMAL);
                  blit2screen (xofs, yofs);
                  sound_init ();
                  play_music (g_map.song_file, 0);
               }
            }
            set_config_int (NULL, "is_sound", is_sound != 0);
            break;
         case 13:
            if (is_sound == 2) {
               p = getavalue (_("Sound Volume"), 0, 25, gsvol / 10, 1);
               if (p != -1)
                  gsvol = p * 10;

               /* make sure to set it no matter what */
               set_volume (gsvol, 0);
               set_config_int (NULL, "gsvol", gsvol);
            } else
               /* Not as daft as it seems, SND_BAD also wobbles the screen */
               play_effect (SND_BAD, 128);
            break;
         case 14:
            if (is_sound == 2) {
               p = getavalue (_("Music Volume"), 0, 25, gmvol / 10, 1);
               if (p != -1)
                  gmvol = p * 10;

               /* make sure to set it no matter what */
               set_music_volume (gmvol / 250.0);
               set_config_int (NULL, "gmvol", gmvol);
            } else
               play_effect (SND_BAD, 128);
            break;
         case 15:
            /* TT: toggle slow_computer */
            slow_computer = !slow_computer;
            set_config_int (NULL, "slow_computer", slow_computer);
            break;
         case 16:
            /* TT: Adjust the CPU usage:yield_timeslice() or rest() */
            cpu_usage++;
            if (cpu_usage > 2)
               cpu_usage = 0;
            break;
#ifdef DEBUGMODE
         case 17:
            /* TT: Things we only have access to when we're in debug mode */
            if (debugging < 4)
               debugging++;
            else
               debugging = 0;
            break;
#endif
         }
      }
      if (bctrl) {
         unpress ();
         stop = 1;
      }
   }
   pop_config_state ();
}



/*! \brief Release memory used by samples
 * \author  : Josh Bolduc
 * \date ????????
 *
 *  Duh.
 */
void free_samples (void)
{
   size_t index;

   if (is_sound == 0)
      return;

   for (index = 0; index < MAX_SAMPLES; index++)
      unload_datafile_object (sfx[index]);
}



/*! \brief Process keypresses when mapping new keys
 *
 * This grabs whatever key is being pressed and returns it to the caller.
 * PH 20030527 Removed call to keypressed() and added poll_music()
 *
 * \returns the key being pressed, 0 if error (or cancel?)
 */
static int getakey (void)
{
   int a;

   clear_keybuf ();
   menubox (double_buffer, 108 + xofs, 108 + yofs, 11, 1, DARKBLUE);
   print_font (double_buffer, 116 + xofs, 116 + yofs, _("Press a key"), FNORMAL);
   blit2screen (xofs, yofs);

   while (1) {
      poll_music ();
      for (a = 0; a < KEY_MAX; a++) {
         if (key[a] != 0)
            return a;
      }
   }
   return 0;
}



/*! \brief Get value for option
 *
 * Display a bar and allow the user to adjust between fixed limits
 *
 * \param   capt Caption
 * \param   minu Minimum value of option
 * \param   maxu Maximum vlaue of option
 * \param   cv Current value (initial value)
 * \param   sp Show percent. If sp==1, show as a percentage of maxu
 * \returns the new value for option, or -1 if cancelled.
 */
static int getavalue (const char *capt, int minu, int maxu, int cv, int sp)
{
   int stop = 0, a, b;

   if (maxu == 0)
      return -1;

   while (!stop) {
      check_animation ();
      menubox (double_buffer, 148 - (maxu * 4) + xofs, 100 + yofs, maxu + 1, 3, DARKBLUE);
      print_font (double_buffer, 160 - (strlen (capt) * 4) + xofs, 108 + yofs, capt, FGOLD);
      print_font (double_buffer, 152 - (maxu * 4) + xofs, 116 + yofs, "<", FNORMAL);
      print_font (double_buffer, 160 + (maxu * 4) + xofs, 116 + yofs, ">", FNORMAL);
      b = 160 - (maxu * 4) + xofs;
      for (a = 0; a < cv; a++) {
         rectfill (double_buffer, a * 8 + b + 1, 117 + yofs, a * 8 + b + 7,
                   123 + yofs, 50);
         rectfill (double_buffer, a * 8 + b, 116 + yofs, a * 8 + b + 6,
                   122 + yofs, 21);
      }
      if (sp == 1)
         sprintf (strbuf, "%d%%", cv * 100 / maxu);
      else
         sprintf (strbuf, "%d", cv);
      print_font (double_buffer, 160 - (strlen (strbuf) * 4) + xofs,
                  124 + yofs, strbuf, FGOLD);
      blit2screen (xofs, yofs);

      readcontrols ();
      if (left) {
         unpress ();
         cv--;
         if (cv < minu)
            cv = minu;
         IF_VOLUME_ALERT ();
      }
      if (right) {
         unpress ();
         cv++;
         if (cv > maxu)
            cv = maxu;
         IF_VOLUME_ALERT ();
      }
      if (balt) {
         unpress ();
         stop = 1;
      }
      if (bctrl) {
         unpress ();
         return -1;
      }
   }
   return cv;
}



#if (ALLEGRO_VERSION >= 4 && ALLEGRO_SUB_VERSION >= 2)
   // TT: Already defined in setup.h
   // #define kq_keyname scancode_to_name
#else
const char *kq_keyname (int scancode)
{
   if (scancode >= 0 && scancode < (signed) N_KEYNAMES)
      return keynames[scancode];
   else
      return "???";
}
#endif



/*! \brief Load sample files
 * \author JB
 * \date ????????
 *
 * Load the list of samples from the data file.
 *
 * \todo RB FIXME: This must be generated from the kqsnd.h header,
 *          not hardcoded, to make it easier to maintain (a perl script?).
 * \remark Updated : 20020914 - 05:20 (RB)
 * \remark ML 2002-09-22: altered this so it returns an error on failure
 *
 * \returns 0 on success, 1 on failure.
 */
static int load_samples (void)
{
   AL_CONST char *sndfiles[MAX_SAMPLES] = {
      "WHOOSH_WAV", "MENUMOVE_WAV", "BAD_WAV", "ITEM_WAV",
      "EQUIP_WAV", "DEEQUIP_WAV", "BUYSELL_WAV", "TWINKLE_WAV",
      "SCORCH_WAV", "POISON_WAV", "CHOP_WAV", "SLASH_WAV",
      "STAB_WAV", "HIT_WAV", "ICE_WAV", "WIND_WAV",
      "QUAKE_WAV", "BLACK_WAV", "WHITE_WAV", "BOLT1_WAV",
      "FLOOD_WAV", "HURT_WAV", "BMAGIC_WAV", "SHIELD_WAV",
      "KILL_WAV", "DOOROPEN_WAV", "DOOR2_WAV", "STAIRS_WAV",
      "TELEPORT_WAV", "CURE_WAV", "RECOVER_WAV", "ARROW_WAV",
      "BOLT2_WAV", "BOLT3_WAV", "FLAME_WAV", "BLIND_WAV",
      "INN_WAV", "CONFUSE_WAV", "DISPEL_WAV", "DOOM_WAV",
      "DRAIN_WAV", "GAS_WAV", "EXPLODE_WAV"
   };
   size_t index;

   if (is_sound == 0)
      return 1;

   for (index = 0; index < MAX_SAMPLES; index++) {
      sfx[index] = load_datafile_object (SOUND_DATAFILE, sndfiles[index]);
      if (sfx[index] == NULL) {
         sprintf (strbuf, _("Error loading .WAV file: %s.\n"),
                  sndfiles[index]);
         klog (strbuf);
         return 1;
      }
   }
   return 0;
}



/*! \brief Parse allegro file kq.cfg
 *
 * This is like parse_setup(), but using Allegro format files
 *
 * \author PH
 * \date 20030831
 */
static void parse_allegro_setup (void)
{
   const char *cfg = kqres (SETTINGS_DIR, "kq.cfg");

   if (!exists (cfg)) {
      /* config file does not exist. Fall back to setup.cfg */
      /* Transitional code */
      parse_jb_setup ();
      push_config_state ();
      set_config_file (cfg);

      set_config_int (NULL, "skip_intro", skip_intro);
      set_config_int (NULL, "windowed", windowed);

      set_config_int (NULL, "stretch_view", stretch_view);
      set_config_int (NULL, "show_frate", show_frate);
      set_config_int (NULL, "is_sound", is_sound);
      set_config_int (NULL, "use_joy", use_joy);
      set_config_int (NULL, "slow_computer", slow_computer);

      set_config_int (NULL, "kup", kup);
      set_config_int (NULL, "kdown", kdown);
      set_config_int (NULL, "kleft", kleft);
      set_config_int (NULL, "kright", kright);
      set_config_int (NULL, "kesc", kesc);
      set_config_int (NULL, "kalt", kalt);
      set_config_int (NULL, "kctrl", kctrl);
      set_config_int (NULL, "kenter", kenter);

#ifdef DEBUGMODE
      set_config_int (NULL, "debugging", debugging);
#endif

      pop_config_state ();
      return;
   }
   push_config_state ();
   set_config_file (cfg);

   /* NB. JB's config file uses intro=yes --> skip_intro=0 */
   skip_intro = get_config_int (NULL, "skip_intro", 0);
#ifdef __DJGPP__
   /* In DJGPP, it's always non-windowed non-stretched (DOS-era stuff!) */
   windowed = 0;
   stretch_view = 0;
#else
   windowed = get_config_int (NULL, "windowed", 1);
   stretch_view = get_config_int (NULL, "stretch_view", 1);
#endif
   wait_retrace = get_config_int (NULL, "wait_retrace", 1);
   show_frate = get_config_int (NULL, "show_frate", 0);
   is_sound = get_config_int (NULL, "is_sound", 1);
   use_joy = get_config_int (NULL, "use_joy", 0);
   slow_computer = get_config_int (NULL, "slow_computer", 0);
   cpu_usage = get_config_int (NULL, "cpu_usage", 2);
#ifdef KQ_CHEATS
   cheat = get_config_int (NULL, "cheat", 0);
   no_random_encounters = get_config_int (NULL, "no_random_encounters", 0);
   no_monsters = get_config_int (NULL, "no_monsters", 0);
   every_hit_999 = get_config_int (NULL, "every_hit_999", 0);
#endif
#ifdef DEBUGMODE
   debugging = get_config_int (NULL, "debugging", 0);
#endif

   kup = get_config_int (NULL, "kup", KEY_UP);
   kdown = get_config_int (NULL, "kdown", KEY_DOWN);
   kleft = get_config_int (NULL, "kleft", KEY_LEFT);
   kright = get_config_int (NULL, "kright", KEY_RIGHT);
   kesc = get_config_int (NULL, "kesc", KEY_ESC);
   kalt = get_config_int (NULL, "kalt", KEY_ALT);
   kctrl = get_config_int (NULL, "kctrl", KEY_LCONTROL);
   kenter = get_config_int (NULL, "kenter", KEY_ENTER);
   pop_config_state ();
}



/*! \brief Parse setup.cfg
 *
 * Read settings from file
 * Parse the setup.cfg file for key configurations.
 * This file would also contain sound options, but that
 * isn't necessary right now.
 *
 * Remember that setup.cfg is found in the /saves dir!
 */
static void parse_jb_setup (void)
{
   FILE *s;
   int dab = 0;

   /* Default key assignments */
   kup = KEY_UP;
   kdown = KEY_DOWN;
   kright = KEY_RIGHT;
   kleft = KEY_LEFT;
   kalt = KEY_ALT;
   kctrl = KEY_LCONTROL;
   kenter = KEY_ENTER;
   kesc = KEY_ESC;
   jbalt = 0;
   jbctrl = 1;
   jbenter = 2;
   jbesc = 3;
   /* PH Why in the world doesn't he use Allegro cfg functions here? */
   if (!(s = fopen (kqres (SETTINGS_DIR, "setup.cfg"), "r"))) {
      klog (_("Could not open saves/setup.cfg - Using defaults."));
      return;
   }
   fscanf (s, "%s", strbuf);
   while (!feof (s)) {
      if (strbuf[0] == '#')
         fgets (strbuf, 254, s);
#ifdef KQ_CHEATS
      if (!strcmp (strbuf, "cheat")) {
         fscanf (s, "%d", &dab);
         cheat = dab;
      }
#endif
      if (!strcmp (strbuf, "debug")) {
         fscanf (s, "%d", &dab);
         debugging = dab;
      }
      if (!strcmp (strbuf, "intro")) {
         fscanf (s, "%s", strbuf);
         if (!strcmp (strbuf, "no"))
            skip_intro = 1;
      }
      if (!strcmp (strbuf, "windowed")) {
         fscanf (s, "%s", strbuf);
         if (!strcmp (strbuf, "yes"))
            windowed = 1;
      }
      if (!strcmp (strbuf, "stretch")) {
         fscanf (s, "%s", strbuf);
         if (!strcmp (strbuf, "yes"))
            stretch_view = 1;
      }
      if (!strcmp (strbuf, "framerate")) {
         fscanf (s, "%s", strbuf);
         if (!strcmp (strbuf, "on"))
            show_frate = 1;
      }
      if (!strcmp (strbuf, "sound")) {
         fscanf (s, "%s", strbuf);
         if (!strcmp (strbuf, "off"))
            is_sound = 0;
      }
      if (!strcmp (strbuf, "joystick")) {
         fscanf (s, "%s", strbuf);
         if (!strcmp (strbuf, "no"))
            use_joy = 0;
      }
      if (!strcmp (strbuf, "slow_computer")) {
         fscanf (s, "%s", strbuf);
         if (!strcmp (strbuf, "yes"))
            slow_computer = 1;
      }
      if (!strcmp (strbuf, "rightkey")) {
         fscanf (s, "%s", strbuf);
         kright = atoi (strbuf);
      }
      if (!strcmp (strbuf, "leftkey")) {
         fscanf (s, "%s", strbuf);
         kleft = atoi (strbuf);
      }
      if (!strcmp (strbuf, "upkey")) {
         fscanf (s, "%s", strbuf);
         kup = atoi (strbuf);
      }
      if (!strcmp (strbuf, "downkey")) {
         fscanf (s, "%s", strbuf);
         kdown = atoi (strbuf);
      }
      if (!strcmp (strbuf, "sysmenukey")) {
         fscanf (s, "%s", strbuf);
         kesc = atoi (strbuf);
      }
      if (!strcmp (strbuf, "cancelkey")) {
         fscanf (s, "%s", strbuf);
         kctrl = atoi (strbuf);
      }
      if (!strcmp (strbuf, "confirmkey")) {
         fscanf (s, "%s", strbuf);
         kalt = atoi (strbuf);
      }
      if (!strcmp (strbuf, "chrmenukey")) {
         fscanf (s, "%s", strbuf);
         kenter = atoi (strbuf);
      }
      fscanf (s, "%s", strbuf);
   }
   fclose (s);
}



/*! \brief Parse setup file
 *
 * \date 20030831
 * \author PH
 */
void parse_setup (void)
{
   parse_allegro_setup ();
}



/*! \brief Play sample effect
 *
 * Play an effect... if possible/necessary.  If the effect to
 * be played is the 'bad-move' effect, than do something visually
 * so that even if sound is off you know you did something bad :)
 * PH added explode effect.
 *
 * \param   efc Effect to play (index in sfx[])
 * \param   panning Left/right pan - see Allegro's play_sample()
 */
void play_effect (int efc, int panning)
{
   int a, s, xo = 1, yo = 1;
   static const int bx[8] = { -1, 0, 1, 0, -1, 0, 1, 0 };
   static const int by[8] = { -1, 0, 1, 0, 1, 0, -1, 0 };
   static const int sc[] = { 1, 2, 3, 5, 3, 3, 3, 2, 1 };
   SAMPLE *samp;
   PALETTE whiteout, old;

   /* Patch provided by mattrope: */
   /* sfx array is empty if sound is not initialized */
   if (is_sound != 0)
      samp = (SAMPLE *) sfx[efc]->dat;
   else
      samp = NULL;              /* PH not strictly needed but I added it */

   switch (efc) {
   default:
      if (samp)
         play_sample (samp, gsvol, panning, 1000, 0);
      break;
   case SND_BAD:
      fullblit(double_buffer, fx_buffer);

      if (samp)
         play_sample (samp, gsvol, panning, 1000, 0);
      clear_bitmap (double_buffer);
      blit (fx_buffer, double_buffer, xofs, yofs, xofs, yofs, 320, 240);

      if (in_combat == 0) {
         xo = xofs;
         yo = yofs;
      }

      /*        blit (fx_buffer, double_buffer, xo, yo, xo, yo, 320, 240); */

      for (a = 0; a < 8; a++) {
         blit2screen (xo + bx[a], yo + by[a]);
         kq_wait (10);
      }
      fullblit(fx_buffer, double_buffer);
      break;
   case SND_EXPLODE:
      fullblit(double_buffer, fx_buffer);
      clear_bitmap (double_buffer);
      get_palette (old);
      for (a = 0; a < 256; ++a) {
         s = (old[a].r + old[a].g + old[a].b) > 40 ? 0 : 63;
         whiteout[a].r = whiteout[a].g = whiteout[a].b = s;
      }
      blit (fx_buffer, double_buffer, xofs, yofs, xofs, yofs, 320, 240);
      if (samp) {
         play_sample (samp, gsvol, panning, 1000, 0);
      }
      for (s = 0; s < (int) (sizeof (sc) / sizeof (*sc)); ++s) {
         if (s == 1)
            set_palette (whiteout);
         if (s == 6)
            set_palette (old);

         for (a = 0; a < 8; a++) {
            blit2screen (xofs + bx[a] * sc[s], yofs + by[a] * sc[s]);
            kq_wait (10);
         }
      }
      fullblit(fx_buffer, double_buffer);
      break;
   }
}



/*! \brief Set mode
 *
 * Set the graphics mode, taking into account the Windowed and Stretched
 * settings.
 */
void set_graphics_mode (void)
{
   if (stretch_view == 1) {
      if (windowed == 1)
         set_gfx_mode (GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
      else
         set_gfx_mode (GFX_AUTODETECT, 640, 480, 0, 0);
   } else {
      if (windowed == 1)
         set_gfx_mode (GFX_AUTODETECT_WINDOWED, 320, 240, 0, 0);
      else
         set_gfx_mode (GFX_AUTODETECT, 320, 240, 0, 0);
   }

   set_palette (pal);
}



/*! \brief Show keys help
 * Show a screen with the keys listed, and other helpful info
 * \author PH
 * \date 20030527
 */
void show_help (void)
{
   menubox (double_buffer, 116 + xofs, yofs, 9, 1, BLUE);
   print_font (double_buffer, 132 + xofs, 8 + yofs, _("KQ Help"), FGOLD);
   menubox (double_buffer, 32 + xofs, 32 + yofs, 30, 20, BLUE);
   menubox (double_buffer, xofs, 216 + yofs, 38, 1, BLUE);
   print_font (double_buffer, 16 + xofs, 224 + yofs,
               _("Press CONFIRM to exit this screen"), FNORMAL);
   citem (72, _("Up Key:"), kq_keyname (kup), FNORMAL);
   citem (80, _("Down Key:"), kq_keyname (kdown), FNORMAL);
   citem (88, _("Left Key:"), kq_keyname (kleft), FNORMAL);
   citem (96, _("Right Key:"), kq_keyname (kright), FNORMAL);
   citem (104, _("Confirm Key:"), kq_keyname (kalt), FNORMAL);
   citem (112, _("Cancel Key:"), kq_keyname (kctrl), FNORMAL);
   citem (120, _("Menu Key:"), kq_keyname (kenter), FNORMAL);
   citem (128, _("System Menu Key:"), kq_keyname (kesc), FNORMAL);
   do {
      blit2screen (xofs, yofs);
      readcontrols ();
   }
   while (!balt && !bctrl);
   unpress ();
}



/*! \brief Initialize sound system
 * \author JB
 * \date ????????
 * \remark On entry is_sound=1 to initialise,
 *         on exit is_sound=0 (failure) or 2 (success),
 *         is_sound=2 to shutdown,
 *         on exit is_sound=0
 * \remark 20020914 - 05:28 RB : Updated
 *  20020922 - ML : updated to use DUMB
 *  20020922 - ML : Changed to only reserving 8 voices. (32 seemed over-kill?)
 */
void sound_init (void)
{
   if (!sound_avail) {
      is_sound = 0;
      return;
   }
   switch (is_sound) {
   case 1:
      /* set_volume_per_voice (2); */
      init_music ();
      is_sound = load_samples ()? 0 : 2;        /* load the wav files */
      break;
   case 2:
      /* TT: We forgot to add this line, causing phantom music to loop */
      stop_music ();
      free_samples ();
      is_sound = 0;
      break;
   }
}
