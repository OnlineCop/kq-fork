#include <stdio.h>
#include <string.h>
#include "allegro.h"

void load_game (char *);
void load_game_v84 (void);
void load_game_v85_90 (void);
void save_game (char *);
int invchk (int, int);


typedef struct
{
   char name[9];
   int xp, next, lvl, mrp;
   int hp, mhp, mp, mmp;
   int stats[7];
   char res[16];
   unsigned char sts[24];
   int inv[16][2];
   unsigned char eqp[6];
   unsigned char spells[60];
}
old_player;


typedef struct
{
   char name[9];
   int xp, next, lvl, mrp;
   int hp, mhp, mp, mmp;
   int stats[13];
   char res[16];
   unsigned char sts[24];
   unsigned char eqp[6];
   unsigned char spells[60];
}
new_player;

old_player opar[8];

new_player npar[8] = {
   {"Sensar", 0, 70, 1, 100, 40, 40, 0, 0,
    {800, 500, 700, 300, 300, 4000, 9000, 9000, 0, 7500, 0, 100, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
   {"Sarina", 0, 70, 1, 100, 36, 36, 4, 4,
    {600, 600, 600, 400, 400, 6000, 9000, 9000, 0, 6000, 0, 100, 0},
    {0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
   {"Corin", 0, 70, 1, 100, 25, 25, 15, 15,
    {400, 400, 500, 800, 400, 4500, 12000, 9000, 0, 5000, 0, 100, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
   {"Ajathar", 0, 70, 1, 100, 30, 30, 8, 8,
    {600, 500, 500, 400, 800, 5000, 9000, 11000, 0, 5000, 0, 100, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
   {"Casandra", 0, 70, 1, 100, 24, 24, 16, 16,
    {300, 600, 400, 800, 600, 5500, 10000, 10000, 0, 5000, 0, 100, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 8, 0}},
   {"Temmin", 0, 70, 1, 100, 35, 35, 5, 5,
    {700, 500, 800, 300, 500, 5500, 8000, 10000, 0, 6000, 0, 100, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
   {"Ayla", 0, 70, 1, 100, 32, 32, 8, 8,
    {500, 800, 500, 600, 400, 7000, 10000, 8000, 0, 5000, 0, 100, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
   {"Noslom", 0, 70, 1, 65, 22, 22, 18, 18,
    {300, 600, 300, 700, 700, 5500, 10000, 10000, 0, 5000, 0, 100, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
};

unsigned short g_inv[64][2], startx, starty, shopq[50][12];

char curmap[16];
unsigned char *progress, *treasure, tv, gs, windowed = 0,
   stretch_view = 0, wait_retrace = 0;

int gsvol, gmvol, gp, numchrs, smin, shr, pidx[2];
int kup = KEY_UP, kdown = KEY_DOWN, kleft = KEY_LEFT, kright = KEY_RIGHT;
int kalt = KEY_ALT, kctrl = KEY_LCONTROL, kenter = KEY_ENTER, kesc = KEY_ESC;
int jbalt = 0, jbctrl = 1, jbenter = 2, jbesc = 3;

PACKFILE *sdat;

int ic[180] = {
   0, 16, 17, 18, 25, 26, 27, 9, 10, 11, 12, 13, 14, 15, 1, 2, 4, 20, 21, 22,
   23, 33, 35, 37, 5, 6, 8, 28, 29, 30, 31, 32, 38, 39, 40, 41, 42, 43, 44, 45,
   46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
   65, 66, 67, 68, 69, 70, 71, 72, 73, 78, 80, 81, 79, 74, 75, 76, 77, 83, 88,
   90, 89, 84, 85, 93, 94, 87, 86, 92, 91, 95, 97, 98, 99, 96, 102, 100, 101,
   121, 103, 105, 104, 117, 116, 111, 115, 118, 113, 114, 112, 122, 124, 125,
   123, 126, 128, 135, 147, 157, 137, 140, 144, 156, 148, 130, 171, 136, 133,
   139, 141, 149, 145, 150, 151, 153, 152, 176, 129, 162, 131, 132, 134, 166,
   163, 172, 169, 168, 177, 178, 179, 182, 160, 161, 164, 138, 165, 167, 170,
   142, 175, 174, 173, 143, 158, 180, 181, 155, 185, 159, 183, 184, 154, 186,
   146, 187, 106, 107, 108, 109, 110, 119, 120
};

int sc[61] = {
   0, 1, 8, 20, 30, 10, 13, 17, 29, 21, 3, 44, 9, 6, 12, 14, 22, 18, 23, 24,
   26, 25, 49, 2, 35, 4, 5, 7, 39, 36, 45, 42, 41, 50, 51, 52, 55, 33, 34, 37,
   11, 38, 40, 43, 15, 48, 47, 46, 16, 31, 53, 54, 28, 58, 32, 56, 57, 27, 59,
   19, 60
};


void load_game (char *fname)
{
   fprintf (stdout, "Loading %s... ", fname);
   sdat = pack_fopen (fname, F_READ_PACKED);

   if (!sdat) {
      fprintf (stdout, "\nCould not load saved game.\n");
      exit (1);
   }

   pack_fread (&tv, sizeof (tv), sdat);
   fprintf (stdout, "version %d... ", tv);

   if (tv == 91) {
      pack_fclose (sdat);
      fprintf (stdout, "\nThis saved game is up to date already.\n");
      exit (1);
   }

   if (tv != 84 && tv != 85 && tv != 90) {
      pack_fclose (sdat);
      fprintf (stdout, "\nSaved game format is too old.\n");
      exit (1);
   }

   pack_fread (curmap, 16, sdat);

   if (tv == 84)
      load_game_v84 ();
   else
      load_game_v85_90 ();
   pack_fclose (sdat);
   fprintf (stdout, "loaded.\n");

}


void load_game_v84 (void)
{
   int a;

   for (a = 0; a < 8; a++)
      pack_fread (&opar[a], sizeof (old_player), sdat);

   pack_fread (progress, 2000, sdat);
   pack_fread (treasure, 1000, sdat);
   pack_fread (&gsvol, sizeof (gsvol), sdat);
   pack_fread (&gmvol, sizeof (gmvol), sdat);
   pack_fread (&gs, sizeof (gs), sdat);
   pack_fread (&gp, sizeof (gp), sdat);
   pack_fread (&numchrs, sizeof (numchrs), sdat);
   pack_fread (&startx, sizeof (startx), sdat);
   pack_fread (&starty, sizeof (starty), sdat);
   pack_fread (&smin, sizeof (smin), sdat);
   pack_fread (&shr, sizeof (shr), sdat);
   for (a = 0; a < 2; a++)
      pack_fread (&pidx[a], sizeof (pidx[a]), sdat);
}


void load_game_v85_90 (void)
{

   int a, tvar;

   for (a = 0; a < 8; a++)
      pack_fread (&npar[a], sizeof (new_player), sdat);
   pack_fread (progress, 2000, sdat);
   pack_fread (treasure, 1000, sdat);
   pack_fread (shopq, 600 * sizeof (shopq[0][0]), sdat);
   pack_fread (g_inv, 128 * sizeof (g_inv[0][0]), sdat);
   pack_fread (&gsvol, sizeof (gsvol), sdat);
   pack_fread (&gmvol, sizeof (gmvol), sdat);
   if (tv == 85)
      pack_fread (&gs, sizeof (gs), sdat);
   pack_fread (&windowed, sizeof (windowed), sdat);
   if (tv == 85) {
      pack_fread (&tvar, sizeof (tvar), sdat);
      stretch_view = tvar;
   } else {
      pack_fread (&stretch_view, sizeof (stretch_view), sdat);
   }
   pack_fread (&wait_retrace, sizeof (wait_retrace), sdat);
   pack_fread (&kup, sizeof (kup), sdat);
   pack_fread (&kdown, sizeof (kdown), sdat);
   pack_fread (&kleft, sizeof (kleft), sdat);
   pack_fread (&kright, sizeof (kright), sdat);
   pack_fread (&kalt, sizeof (kalt), sdat);
   pack_fread (&kctrl, sizeof (kctrl), sdat);
   pack_fread (&kenter, sizeof (kenter), sdat);
   pack_fread (&kesc, sizeof (kesc), sdat);
   pack_fread (&jbalt, sizeof (jbalt), sdat);
   pack_fread (&jbctrl, sizeof (jbctrl), sdat);
   pack_fread (&jbenter, sizeof (jbenter), sdat);
   pack_fread (&jbesc, sizeof (jbesc), sdat);
   pack_fread (&gp, sizeof (gp), sdat);
   pack_fread (&numchrs, sizeof (numchrs), sdat);
   pack_fread (&startx, sizeof (startx), sdat);
   pack_fread (&starty, sizeof (starty), sdat);
   pack_fread (&smin, sizeof (smin), sdat);
   pack_fread (&shr, sizeof (shr), sdat);
   for (a = 0; a < 2; a++)
      pack_fread (&pidx[a], sizeof (pidx[a]), sdat);
}

void save_game (char *fname)
{

   PACKFILE *gdat;
   int a;

   fprintf (stdout, "Saving %s... ", fname);
   gdat = pack_fopen (fname, F_WRITE_PACKED);
   if (!gdat) {
      fprintf (stdout, "\nCould not save game data.\n");
      exit (1);
   }
   pack_fwrite (&tv, sizeof (tv), gdat);
   pack_fwrite (&numchrs, sizeof (numchrs), gdat);
   pack_fwrite (&gp, sizeof (gp), gdat);
   pack_fwrite (&shr, sizeof (shr), gdat);
   pack_fwrite (&smin, sizeof (smin), gdat);
   for (a = 0; a < 2; a++)
      pack_fwrite (&pidx[a], sizeof (pidx[a]), gdat);
   for (a = 0; a < 8; a++)
      pack_fwrite (&npar[a], sizeof (new_player), gdat);
   pack_fwrite (curmap, 16, gdat);
   pack_fwrite (progress, 2000, gdat);
   pack_fwrite (treasure, 1000, gdat);
   pack_fwrite (shopq, 600 * sizeof (shopq[0][0]), gdat);
   pack_fwrite (g_inv, 128 * sizeof (g_inv[0][0]), gdat);
   pack_fwrite (&gsvol, sizeof (gsvol), gdat);
   pack_fwrite (&gmvol, sizeof (gmvol), gdat);
   pack_fwrite (&windowed, sizeof (windowed), gdat);
   pack_fwrite (&stretch_view, sizeof (stretch_view), gdat);
   pack_fwrite (&wait_retrace, sizeof (wait_retrace), gdat);
   pack_fwrite (&kup, sizeof (kup), gdat);
   pack_fwrite (&kdown, sizeof (kdown), gdat);
   pack_fwrite (&kleft, sizeof (kleft), gdat);
   pack_fwrite (&kright, sizeof (kright), gdat);
   pack_fwrite (&kalt, sizeof (kalt), gdat);
   pack_fwrite (&kctrl, sizeof (kctrl), gdat);
   pack_fwrite (&kenter, sizeof (kenter), gdat);
   pack_fwrite (&kesc, sizeof (kesc), gdat);
   pack_fwrite (&jbalt, sizeof (jbalt), gdat);
   pack_fwrite (&jbctrl, sizeof (jbctrl), gdat);
   pack_fwrite (&jbenter, sizeof (jbenter), gdat);
   pack_fwrite (&jbesc, sizeof (jbesc), gdat);
   pack_fwrite (&startx, sizeof (startx), gdat);
   pack_fwrite (&starty, sizeof (starty), gdat);
   pack_fclose (gdat);
}


int invchk (int i, int q)
{
   int n, v = 64, d = 64;

   if (i == 0 || q == 0)
      return 3;
   for (n = 63; n >= 0; n--) {
      if (g_inv[n][0] == 0)
         v = n;
      if (g_inv[n][0] == i && g_inv[n][1] <= 9 - q)
         d = n;
   }
   if (v == 64 && d == 64)
      return 0;
   if (d < 64) {
      g_inv[d][0] = i;
      g_inv[d][1] += q;
      return 1;
   }
   g_inv[v][0] = i;
   g_inv[v][1] += q;
   return 2;
}


int main (int argc, char **argv)
{
   int a, b, c, binv = 0, beqp = 0, bstg = 0;
   char tfn[4], tex[4], from_name[8], to_name[8];

   if (argc != 2 && argc != 3) {
      fprintf (stdout, "Usage: sgupdate filename [extension]\n");
      fprintf (stdout, "  if extension is omitted, dat is assumed.\n");
      fprintf (stdout,
               "  if included, the extention should not contain the .\n");
      return 1;
   }

   if (strlen (argv[1]) != 3) {
      fprintf (stdout, "File name is not correct.\n");
      return 1;
   }

   strcpy (tfn, argv[1]);

   if (argc == 2)
      strcpy (tex, "dat");
   else {
      if (strcmp (argv[2], "dat") != 0 && strcmp (argv[2], "sav") != 0) {
         fprintf (stdout, "Extension is not correct.\n");
         return 1;
      }
      strcpy (tex, argv[2]);
   }
   sprintf (from_name, "%s.%s", tfn, tex);
   sprintf (to_name, "%s.sav", tfn);
   progress = (unsigned char *) malloc (2000);
   treasure = (unsigned char *) malloc (1000);

   load_game (from_name);

   if (tv == 84) {
      for (a = 0; a < 64; a++) {
         g_inv[a][0] = 0;
         g_inv[a][1] = 0;
      }

      fprintf (stdout, "Doing player conversions.\n");

      for (a = 0; a < 8; a++) {
         strcpy (npar[a].name, opar[a].name);
         npar[a].xp = opar[a].xp;
         npar[a].next = opar[a].next;
         npar[a].lvl = opar[a].lvl;
         npar[a].mrp = opar[a].mrp;
         npar[a].hp = opar[a].hp;
         npar[a].mhp = opar[a].mhp;
         npar[a].mp = opar[a].mp;
         npar[a].mmp = opar[a].mmp;
         for (b = 0; b < 24; b++)
            npar[a].sts[b] = opar[a].sts[b];
         for (b = 0; b < 6; b++) {
            npar[a].eqp[b] = 0;
            if (invchk (ic[opar[a].eqp[b]], 1) == 0)
               beqp++;
         }
         for (b = 0; b < 60; b++)
            npar[a].spells[b] = sc[opar[a].spells[b]];
      }
      if (beqp > 0)
         fprintf (stdout, "  %d failures transferring equipped items.\n",
                  beqp);
      else
         fprintf (stdout, "  no problems transferring equipped items.\n");
      fprintf (stdout, "Converting inventory...\n");
      for (a = 0; a < numchrs; a++) {
         c = pidx[a];
         for (b = 0; b < 16; b++) {
            if (invchk (ic[opar[c].inv[b][0]], opar[c].inv[b][1]) == 0)
               binv++;
         }
      }
      if (binv > 0)
         fprintf (stdout, "  %d failures transferring inventory.\n", beqp);
      else
         fprintf (stdout, "  no problems transferring inventory.\n");
      fprintf (stdout, "Converting storage...\n");
      for (a = 0; a < 96; a++) {
         if (invchk (ic[progress[1800 + a]], progress[1900 + a]) == 0)
            bstg++;
      }
      if (bstg > 0)
         fprintf (stdout, "  %d failures transferring stored items.\n", beqp);
      else
         fprintf (stdout, "  no problems transferring stored items.\n");
      for (a = 1750; a <= 1997; a++)
         progress[a] = 0;
   }
   tv = 91;
   save_game (to_name);
   fprintf (stdout, "saved.\n");
   free (progress);
   free (treasure);
   return 0;
}
