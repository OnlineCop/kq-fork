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
 * \brief Markers
 * \author OC
 * \date 20100222
 */

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "markers.h"


unsigned int find_marker(const s_marker_array *marray, const char *name)
{
	unsigned int i;

	assert(marray && "s_marker_array is NULL");

	if (name == NULL)
		return -1;  // An empty name is not an error; it is simply not found

	for (i = 0; i < marray->size; ++i)
	{
		if (strcmp(name, marray->array[i].name)) // no match; keep going
			continue;

		return i; // return index of matching marker
	}

	return -1; // no match
}


/*! \brief Load all markers in from packfile
 *
 * Loads individual \sa s_marker objects from the specified PACKFILE.
 *
 * \param[in,out] marray - Current array of markers to be reallocated
 * \param[in]     pf - PACKFILE from whence data are pulled
 * \return        Non-0 on error, 0 on success
 */
size_t load_markers (s_marker_array *marray, PACKFILE *pf)
{
   s_marker *mmarker = NULL;
   size_t i;

   assert (marray && "marray == NULL");
   assert (pf && "pf == NULL");

   if (!marray || !pf) {
      printf ("NULL passed into load_markers()\n");
      return 1;
   }

   marray->size = pack_igetw (pf);
   if (pack_feof (pf)) {
      assert (0 && "pack_igetw() for marray->size received EOF signal.");
      printf ("Expected value for number of markers. Instead, received EOF.\n");
      return 2;
   } else if (marray->size == 0) {
      marray->array = NULL;
      return 0; // Success: okay to have 0 markers in a map
   }

   marray->array = (s_marker *) realloc
      (marray->array, marray->size * sizeof (s_marker));
   for (i = 0; i < marray->size; ++i) {
      mmarker = &marray->array[i];

      pack_fread (mmarker->name, sizeof (mmarker->name), pf);
      mmarker->x = pack_igetw (pf);
      mmarker->y = pack_igetw (pf);

      if (pack_feof (pf)) {
         assert (0 && "pack_igetw() for marker->[xy] received EOF signal.");
         printf ("Encountered EOF during marker read.\n");
         return 3;
      }
   }

   return 0; // Success
}



/*! \brief Save all markers out to packfile
 *
 * Saves individual \sa s_marker objects to the specified PACKFILE.
 *
 * \param[out] marray - Current array of markers from whence data are pulled
 * \param[out] pf - PACKFILE to where data is written
 * \return     Non-0 on error, 0 on success
 */
size_t save_markers (s_marker_array *marray, PACKFILE *pf)
{
   size_t i;

   assert (marray && "marray == NULL");
   assert (pf && "pf == NULL");

   if (!marray || !pf) {
      printf ("NULL passed into save_markers()\n");
      return 1;
   }

   pack_iputw (marray->size, pf);
   if (pack_feof (pf)) {
      assert (0 && "pack_iputw() for marray->size received EOF signal.");
      printf ("Encountered EOF when writing marker array size.\n");
      return 2;
   }

   for (i = 0; i < marray->size; ++i) {
      pack_fwrite (marray->array[i].name, sizeof (marray->array[i].name), pf);
      pack_iputw (marray->array[i].x, pf);
      pack_iputw (marray->array[i].y, pf);

      if (pack_feof (pf)) {
         assert (0 && "pack_iputw() for marker->[xy] received EOF signal.");
         printf ("Encountered EOF when writing marker %dsize.\n", i);
         return 3;
      }
   }

   return 0;
}
