/***************************************************************************\
 * This map editor is for the KQ game, supported by the kqlives community. *
 * Comments and suggestions about the editor are welcome.  Please join our *
 * mailing list: kqlives-main@lists.sourceforge.net                        *
 *                                                                         *
 * Visit our website: http://kqlives.sourceforge.net/index.php             *
 *                                                                         *
 * Mapent.c will contain all routines regarding entity movement, placement *
 * or direction.  This also includes scripted movement processing, etc.    *
\***************************************************************************/


#include "mapdraw.h"

/* Number of entities, index of currently-selected entity */
unsigned int current_ent = 0;


/*! \brief Offset the x/y coodinates of entities
 *
 * Adjusts the location of all entities on the map
 */
void displace_entities (void)
{
   int response, displace_x, displace_y, a;

   /* Draw a box and ask for x-coord adjustment */
   make_rect (double_buffer, 3, 17);
   print_sfont (6, 6, "Displace entities", double_buffer);
   print_sfont (6, 12, "X adjust: ", double_buffer);

   blit2screen ();
   response = get_line (66, 12, strbuf, 4);

   /* If the user hits ESC, break out of the function entirely */
   if (response == 0)
      return;

   /* This number CAN be negative OR blank! */
   displace_x = atoi (strbuf);

   /* This is incase we need to redraw the map, the information will still be
    * visible to the user
    */
   sprintf (strbuf, "X adjust: %d", displace_x);
   print_sfont (6, 12, strbuf, double_buffer);

   /* Clear the box and ask for y-coord adjustment */
   print_sfont (6, 18, "Y adjust: ", double_buffer);

   blit2screen ();
   response = get_line (66, 18, strbuf, 4);

   /* If the user hits ESC, break out of the function entirely */
   if (response == 0)
      return;

   /* This number CAN be negative OR blank! */
   displace_y = atoi (strbuf);

   /* This moves all the entities */
   for (a = 0; a < number_of_ents; a++) {
      /* Confirm that the input x-coords are within the map */
      if (gent[a].tilex + displace_x >= gmap.xsize)
         gent[a].tilex = gmap.xsize - 1;
      else if (gent[a].tilex + displace_x < 0)
         gent[a].tilex = 0;
      else
         gent[a].tilex += displace_x;

      gent[a].x = gent[a].tilex * TILE_W;

      /* Confirm that the input y-coords are within the map */
      if (gent[a].tiley + displace_y >= gmap.ysize)
         gent[a].tiley = gmap.ysize - 1;
      else if (gent[a].tiley + displace_y < 0)
         gent[a].tiley = 0;
      else
         gent[a].tiley += displace_y;

      gent[a].y = gent[a].tiley * TILE_H;
   }
}                               /* displace_entities () */


/* \brief Display the entity menu
 *
 * Display the stats for the current entity in the menu at the bottom of the
 * screen.
 *
 * \param   ent_index Index of current entity
 */
void draw_entdata (const int ent_index)
{
   int ent_x, ent_y;
   const char *dir_facing[4] = { "S", "N", "W", "E" };
   const char *ent_movemode[4] = { "stand (0)", "wander (1)", "script (2)", "chase (3)" };
   const char *ent_atype[2] = { "normal", "constant" };
   const char *ent_obsmode[2] = { "ignore obstructions", "obstructive" };
   const char *ent_transl[2] = { "opaque", "translucent" };
   const char *do_dont[2] = { "", "don't " };

   /* Move the window so the user can see the selected entity and make sure
      the view isn't out of the boundaries */
   window_x = gent[ent_index].tilex - (htiles / 2);
   window_y = gent[ent_index].tiley - (vtiles / 2);

   normalize_view ();
   clear (double_buffer);
   draw_map ();

   /* The white horizontal line that seperates the bottom menu */
   hline (double_buffer, 0, vtiles * TILE_H + 1, htiles * TILE_W + 1, 255);

   /* The white verticle bar that seperates the right tileset */
   vline (double_buffer, htiles * TILE_W + 1, 0, vtiles * TILE_H + 1, 255);

   ent_x = (gent[ent_index].tilex - window_x) * TILE_W;
   ent_y = (gent[ent_index].tiley - window_y) * TILE_H;
   rect (double_buffer, ent_x - 1, ent_y - 1, ent_x + ENT_W, ent_y + ENT_H, 255);

   /* Print the number of entities and currently-selected one in right menu */
   sprintf (strbuf, "Total: %d", number_of_ents);
   print_sfont (htiles * TILE_W + 6, 0, strbuf, double_buffer);
   sprintf (strbuf, "Current: %d", ent_index);
   print_sfont (htiles * TILE_W + 6, 6, strbuf, double_buffer);

   /* Options in the first column */
   sprintf (strbuf, "1 - x = %d", gent[ent_index].tilex);
   print_sfont (column[0], row[0], strbuf, double_buffer);
   sprintf (strbuf, "2 - y = %d", gent[ent_index].tiley);
   print_sfont (column[0], row[1], strbuf, double_buffer);
   sprintf (strbuf, "3 - %s", ent_movemode[gent[ent_index].movemode]);
   print_sfont (column[0], row[2], strbuf, double_buffer);
   sprintf (strbuf, "4 - delay = %d", gent[ent_index].delay);
   print_sfont (column[0], row[3], strbuf, double_buffer);
   sprintf (strbuf, "5 - speed = %d", gent[ent_index].speed);
   print_sfont (column[0], row[4], strbuf, double_buffer);
   sprintf (strbuf, "6 - atype = %d (%s)", gent[ent_index].atype,
            ent_atype[gent[ent_index].atype]);
   print_sfont (column[0], row[5], strbuf, double_buffer);
   sprintf (strbuf, "0 - %s", gent[ent_index].script);
   print_sfont (column[0], row[6], strbuf, double_buffer);

   /* Options in the second column */
   sprintf (strbuf, "7 - change entity sprite (%d)", gent[ent_index].chrx);
   print_sfont (column[2], row[0], strbuf, double_buffer);
   sprintf (strbuf, "8 - direction facing (%s)",
            dir_facing[gent[ent_index].facing]);
   print_sfont (column[2], row[1], strbuf, double_buffer);
   sprintf (strbuf, "9 - %s", ent_obsmode[gent[ent_index].obsmode]);
   print_sfont (column[2], row[2], strbuf, double_buffer);
   sprintf (strbuf, "S - %ssnap back", do_dont[gent[ent_index].snapback]);
   print_sfont (column[2], row[3], strbuf, double_buffer);
   sprintf (strbuf, "F - %sface hero", do_dont[gent[ent_index].facehero]);
   print_sfont (column[2], row[4], strbuf, double_buffer);
   sprintf (strbuf, "T - %s", ent_transl[gent[ent_index].transl]);
   print_sfont (column[2], row[5], strbuf, double_buffer);

   blit2screen ();
}                               /* draw_entdata () */


/*! \brief Draw the entities
 *
 * Draws the entities onto the screen and takes into consideration any layer
 * effects that need to take place (see map_mode_text[] for details).
 * \author PH
 * \date 20031205
 */
void draw_ents (void)
{
   int d, x0, y0;
   BITMAP *ent;

   x0 = window_x * TILE_W;
   y0 = window_y * TILE_H;

   for (d = 0; d < number_of_ents; d++) {
      /* Draw only the entities within the view-screen */
      if ((gent[d].tilex >= window_x) && (gent[d].tilex < window_x + htiles)
          && (gent[d].tiley >= window_y)
          && (gent[d].tiley < window_y + vtiles)) {
         ent = eframes[gent[d].chrx][gent[d].facing * 3];
         /* Draw either a normal sprite or a translucent one */
         if (gent[d].transl == 0)
            draw_sprite (double_buffer, ent, gent[d].tilex * TILE_W - x0,
                         gent[d].tiley * TILE_W - y0);
         else
            draw_trans_sprite (double_buffer, ent, gent[d].tilex * TILE_W - x0,
                               gent[d].tiley * TILE_H - y0);
      }
   }
}                               /* draw_ents () */


/*! \brief Remove an entity from the map
 *
 * Rub the useless guy out
 *
 * \param   ent_x Entity's x-coord
 * \param   ent_y Entity's y-coord
 */
void erase_entity (const int ent_x, const int ent_y)
{
   int a, ent_index = -1;

   /* This function does nothing if there aren't already entities to remove */
   if (number_of_ents == 0)
      return;

   for (a = 0; a < number_of_ents; a++)
      /* Get the index number of the last-drawn entity from this spot */
      if (gent[a].tilex == ent_x && gent[a].tiley == ent_y)
         ent_index = a;

   /* There is no entity under the mouse */
   if (ent_index == -1)
      return;

   /* This re-numerates the index of entities */
   for (a = ent_index + 1; a <= number_of_ents; a++) {
      gent[a - 1] = gent[a];
      gent[a].chrx = 0;         /* Entity's identity (what s/he looks like) */
      gent[a].x = 0;            /* x-coord on map */
      gent[a].y = 0;            /* y-coord on map */
      gent[a].tilex = 0;        /* x-coord tile that entity is standing on */
      gent[a].tiley = 0;        /* y-coord tile that entity is standing on */
      gent[a].eid = 0;          /* Entity type (fighter, enemy, normal) */
      gent[a].active = 0;       /* "Alive" and on-screen or not */
      gent[a].facing = 0;       /* Direction facing */
      gent[a].moving = 0;       /* In the middle of a move */
      gent[a].movcnt = 0;       /* How far along into the move s/he is */
      gent[a].framectr = 0;     /* Counter for determining animation frame */
      gent[a].movemode = 0;     /* Stand, wander, script, or chasing */
      gent[a].obsmode = 0;      /* Affected by obstacles (some can walk through walls/other NPCs) */
      gent[a].delay = 0;        /* Movement delay (between steps) */
      gent[a].delayctr = 0;     /* Counter for movement delay */
      gent[a].speed = 0;        /* How hyperactive the entity is */
      gent[a].scount = 0;       /* Hyperactivity counter */
      gent[a].cmd = 0;          /* Move, wait, facing command */
      gent[a].sidx = 0;         /* Script index number */
      gent[a].extra = 0;        /* ??? */
      gent[a].chasing = 0;      /* Entity is following another */
      gent[a].cmdnum = 0;       /* Number of times we need to repeat 'cmd' */
      gent[a].atype = 0;        /* (unused) */
      strcpy (gent[a].script, "");      /* Movement script (pacing, etc.) */
   }

   /* Oh yea, and do this too */
   number_of_ents--;
}                               /* erase_entity () */


/*! \brief Prepares the entity array for new entries
 *
 * This is called for each time the user wants to reset the entities;
 */
void init_entities (void)
{
   int p;

   /* Clear all entities' personalities */
   for (p = 0; p < 50; p++) {
      /* Entity's identity (what s/he looks like) */
      gent[p].chrx = 0;
      /* x-coord on map */
      gent[p].x = 0;
      /* y-coord on map */
      gent[p].y = 0;
      /* x-coord tile that entity is standing on */
      gent[p].tilex = 0;
      /* y-coord tile that entity is standing on */
      gent[p].tiley = 0;
      /* Entity type (fighter, enemy, normal) */
      gent[p].eid = 0;
      /* "Alive" and on-screen or not */
      gent[p].active = 0;
      /* Direction facing */
      gent[p].facing = 0;
      /* In the middle of a move */
      gent[p].moving = 0;
      /* How far along into the move s/he is */
      gent[p].movcnt = 0;
      /* Counter for determining animation frame */
      gent[p].framectr = 0;
      /* Stand, wander, script, or chasing */
      gent[p].movemode = 0;
      /* Affected by obstacles (some can walk through walls/other NPCs) */
      gent[p].obsmode = 0;
      /* Movement delay (between steps) */
      gent[p].delay = 0;
      /* Counter for movement delay */
      gent[p].delayctr = 0;
      /* How hyperactive the entity is */
      gent[p].speed = 0;
      /* Hyperactivity counter */
      gent[p].scount = 0;
      /* Move, wait, facing command */
      gent[p].cmd = 0;
      /* Script index number */
      gent[p].sidx = 0;
      /* ??? */
      gent[p].extra = 0;
      /* Entity is following another */
      gent[p].chasing = 0;
      /* Number of times we need to repeat 'cmd' */
      gent[p].cmdnum = 0;
      /* (unused) */
      gent[p].atype = 0;
      /* Movement script (pacing, etc.) */
      strcpy (gent[p].script, "");
   }                            /* for (p = 0; p < 50; p++) */

   /* Reset the entity count */
   number_of_ents = 0;
}                               /* init_entities () */


/*! \brief Place an entity on the map
 *
 * Give the little lemming a home
 *
 * \param   ent_x Entity's x-coord
 * \param   ent_y Entity's y-coord
 */
void place_entity (int ent_x, int ent_y)
{
   int a, someone_there = 0;

   /* Don't allow too many NPCs on the map */
   if (number_of_ents >= MAX_ENT)
      return;

   /* This will prevent user from placing more than 1 entity on a coord */
   for (a = 0; a < number_of_ents; a++)
      if (gent[a].tilex == ent_x && gent[a].tiley == ent_y)
         someone_there = 1;

   /* NPC already found; do nothing */
   if (someone_there)
      return;

   /* Set its personality/attributes/stats */
   gent[number_of_ents].chrx = current_ent;     /* What it looks like */
   gent[number_of_ents].tilex = ent_x;  /* which tile it's standing on */
   gent[number_of_ents].tiley = ent_y;  /* ..same.. */
   gent[number_of_ents].x = ent_x * TILE_W; /* Will be the same as tilex unless moving */
   gent[number_of_ents].y = ent_y * TILE_H; /* ..same.. */
   gent[number_of_ents].active = 1;     /* Showing on map or not */
   gent[number_of_ents].eid = 255;      /* */
   gent[number_of_ents].movemode = 0;   /* 0=stand, 1=wander, 2=script, 3=chase */
   gent[number_of_ents].delay = 50;     /* Length of time between commands */
   gent[number_of_ents].speed = 3;      /* Walking speed */
   gent[number_of_ents].obsmode = 1;    /* 0=not obstacle, 1=obstacle */
   gent[number_of_ents].facing = 0;     /* 0=S, 1=N, 2=W, 3=E */
   strcpy (gent[number_of_ents].script, "");    /* Pre-defined movement (pace, dance...) */
   number_of_ents++;

}                               /* place_entity () */


/*! \brief Update stats for entities
 *
 * Allow the user to give each entity its own personality
 */
void update_entities (void)
{
   int response, done;

   /* Entity's index */
   unsigned int et = 0;

   /* Key press */
   int c;

   /* Temporary window coordinates */
   int t_window_x = window_x, t_window_y = window_y;

   /* Updating entities ineffective if no one is there */
   if (number_of_ents == 0) {
      cmessage ("No entities!");
      wait_enter ();
      return;
   }

   done = 0;
   while (!done) {
      draw_entdata (et);
      c = (readkey () >> 8);

      /* Stop updating entities */
      if (c == KEY_ESC)
         done = 1;

      /* Select the previous entity in the queue */
      if (c == KEY_DOWN) {
         if (et > 0)
            et--;
         else
            et = number_of_ents - 1;
      }

      /* Select the following entity in the queue */
      if (c == KEY_UP) {
         if (et < number_of_ents - 1)
            et++;
         else
            et = 0;
      }

      /* Change the x-coord */
      if (c == KEY_1) {
         rectfill (screen, 48, row[0], 71, row[1] - 1, 0);
         hline (screen, 48, row[1] - 1, 71, 255);
         response = get_line (48, row[0], strbuf, 4);

         /* Make sure the line isn't blank */
         if (response == 0)
            return;

         gent[et].tilex = atoi (strbuf);
         gent[et].x = gent[et].tilex * TILE_W;
      }

      /* Change the y-coord */
      if (c == KEY_2) {
         rectfill (screen, 48, row[1], 71, row[2] - 1, 0);
         hline (screen, 48, row[2] - 1, 71, 255);
         response = get_line (48, row[1], strbuf, 4);

         /* Make sure the line isn't blank */
         if (response == 0)
            return;

         gent[et].tiley = atoi (strbuf);
         gent[et].y = gent[et].tiley * TILE_H;
      }

      /* Change the method of movement (Stand, Wander, Script, Chase) */
      if (c == KEY_3) {
         if (gent[et].movemode < 3)
            gent[et].movemode++;
         else
            gent[et].movemode = 0;
      }

      /* Change the movement delay */
      if (c == KEY_4) {
         rectfill (screen, 72, row[3], 95, row[4] - 1, 0);
         hline (screen, 72, row[4] - 1, 95, 255);
         response = get_line (72, row[3], strbuf, 4);

         /* Make sure the line isn't blank */
         if (response == 0)
            return;

         gent[et].delay = atoi (strbuf);
      }

      /* Change the entity's speed */
      if (c == KEY_5) {
         if (gent[et].speed < 7)
            gent[et].speed++;
         else
            gent[et].speed = 1;
      }

      /* Change the atype (unused) */
      if (c == KEY_6) {
         gent[et].atype = (gent[et].atype == 0) ? 1 : 0;
      }

      /* Change the entity's sprite (what it looks like) */
      if (c == KEY_7) {
         if (gent[et].chrx < MAX_EPICS - 1)
            gent[et].chrx++;
         else
            gent[et].chrx = 0;
      }

      /* Change the direction entity is facing */
      if (c == KEY_8) {
         if (gent[et].facing < 3)
            gent[et].facing++;
         else
            gent[et].facing = 0;
      }

      /* Entity is walk-through-able or solid */
      if (c == KEY_9) {
         gent[et].obsmode = (gent[et].obsmode == 0) ? 1 : 0;
      }

      /* Change the entity's scripted movement */
      if (c == KEY_0) {
         rectfill (screen, 24, row[6], 319, SH - 5, 0);
         hline (screen, 24, SH - 5, 319, 255);
         response = get_line (24, row[6], strbuf, 61);

         /* Make sure the line isn't blank */
         if (response == 0)
            return;

         strcpy (gent[et].script, strbuf);
      }

      /* After you talk to this entity, it will return to the direction it
         was facing before you ever talked to him/her */
      if (c == KEY_S) {
         gent[et].snapback = (gent[et].snapback == 0) ? 1 : 0;
      }

      /* Face the hero when s/he is talking to this entity */
      if (c == KEY_F) {
         gent[et].facehero = (gent[et].facehero == 0) ? 1 : 0;
      }

      /* Make this entity transparent */
      if (c == KEY_T) {
         gent[et].transl = (gent[et].transl == 0) ? 1 : 0;
      }
   }                            /* while (!done) */
   window_x = t_window_x;
   window_y = t_window_y;
}                               /* update_entities () */
