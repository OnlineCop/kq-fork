/***************************************************************************\
 * This map editor is for the KQ game, supported by the kqlives community. *
 * Comments and suggestions about the editor are welcome.  Please join our *
 * mailing list: kqlives-main@lists.sourceforge.net                        *
 *                                                                         *
 * Visit our website: http://kqlives.sourceforge.net/index.php             *
 *                                                                         *
 * Mapstructs.c contains markers, bounding areas, etc..                    *
\***************************************************************************/


#include "mapdraw.h"
#include <sstream>


/*! \brief Add or remove a bounding area
 *
 * \param   x - Coordinate of bounding area
 * \param   y - same
 * \param   mouse_b - Which mouse button (left/right) was pressed
 * \param   current - Number of bounding areas currently defined
 */
void add_change_bounding(int x, int y, int mouse_b, int *current)
{
    Bound *b;
    Bound *found = NULL;

    /* Used only to check if boundaries are okay */
    Bound temp;

    /* Does a bounding box exist at this coordinate? */
    found = is_contained_bound(gmap.bounds.array, gmap.bounds.size, x, y, x, y);

    if (mouse_b == 2)
    {
        /* We will unset the active_bound variable if a right-click is ever
         * encountered, regardless if the bounding box in question is found
         * or not.
         */
        active_bound = 0;
    }

    if (found)
    {
        /* There is a bounding box here */
        if (mouse_b == 1)
        {
            /* The user clicked on a bounded area; allow user to select the
             * background tile.  This tile is used everywhere that the bounded
             * box is NOT located.  It is most commonly '0' (black) for the
             * side-rooms, but can be '1' or '3' for other areas (for trees or
             * grass, etc.).
             */
            rename_bound_tile(found);
            return;
        }
        else if (mouse_b == 2)
        {
            /* Delete it */

            /* Move the selector to the previous bounding box if this was the
             * last one on the map
             */
            gmap.bounds.size--;
            if (*current >= gmap.bounds.size)
            {
                *current = gmap.bounds.size - 1;
            }

            memcpy(found, found + 1, (&gmap.bounds.array[gmap.bounds.size] - found) * sizeof(Bound));
            gmap.bounds.array[gmap.bounds.size].btile = 0;
            return;
        }
    }
    else
    {
        /* There is no bounding box here */
        if (mouse_b == 1)
        {
            /* Add a bounding box */
            *current = gmap.bounds.size;

            b = &gmap.bounds.array[gmap.bounds.size];

            if (!active_bound)
            {
                /* We are not already tracking a boundary, so create new */
                b->left = x;
                b->top = y;
                active_bound = 1;
            }
            else
            {
                /* We have started tracking a boundary already, meaning we
                 * already have the top-left coords. Now we just need the
                 * bottom-right ones.
                 */
                if (b->left == x && b->top == y)
                {
                    /* Our bounding rectangle will never be only 1-tile high and
                     * wide, so do not allow that small of an area to be selected.
                     */
                    return;
                }

                if (x <= b->left)
                {
                    temp.left = x;
                    temp.right = b->left;
                }
                else
                {
                    temp.left = b->left;
                    temp.right = x;
                }

                if (y <= b->top)
                {
                    temp.top = y;
                    temp.bottom = b->top;
                }
                else
                {
                    temp.top = b->top;
                    temp.bottom = y;
                }

                /* Don't allow the user to begin (or end) any portion of a bounding
                 * area if it has any points contained in another bounding area's
                 * region.
                 */
                if (is_bound(&gmap.bounds, temp.left, temp.top, temp.right, temp.bottom) == 0)
                {
//            if (!bound_in_bound2 (&temp, gmap.bounds.array, gmap.bounds.size)) {
                    set_bounds(b, temp.left, temp.top, temp.right, temp.bottom, b->btile);
                    gmap.bounds.size++;
                    active_bound = 0;
                }
            }
            return;
        }
        else if (mouse_b == 2)
        {
            /* Nothing to do */
            return;
        }
    }
}



/*! \brief Add or remove a marker
 *
 * \param   x - Coordinate to place marker
 * \param   y - same
 * \param   mouse_b - Whether to add or remove marker (1 to add, 2 to remove)
 * \param   current - Number of markers currently defined
 */
void add_change_marker(int x, int y, int mouse_b, size_t *current)
{
    std::vector<Marker*> markerArray = gmap.markers.Markers();
    Marker* found = gmap.markers.FindMarker(x, y);

    if (found != NULL)
    {
        /* There is a marker here */
        if (mouse_b == 1)
        {
            /* Rename it */
            rename_marker(found);
        }
        else if (mouse_b == 2)
        {
            /* Delete it */
            gmap.markers.RemoveMarker(found);

            /* Move the selector to the previous marker if this was the last
             * marker on the map
             */
            size_t markerArraySize = markerArray.size();
            if (*current >= markerArraySize)
            {
                *current = markerArraySize - 1;
            }

            /* Wait for mouse button to be released */
        }
    }
    else
    {
        /* There is no marker here */
        if (mouse_b == 1)
        {
            std::stringstream ss;
            ss << "Marker_" << markerArray.size();

            /* Add a marker with default name */
            Marker* marker = new Marker();
            marker->X(x);
            marker->Y(y);
            marker->Name(ss.str());

            gmap.markers.AddMarker(marker);
        }
    }
}



/*! \brief Draw rectangle around bounding area
 *
 * \param   where - Location to draw rectangle
 * \param   b - Bounding box's coordinates
 * \param   color - color of the rectangle
 */
void bound_rect(BITMAP *where, Bound b, int color)
{
    Bound rectb;

    rectb.left = (b.left - window_x) * 16;
    rectb.top = (b.top - window_y) * 16;

    if (b.left < window_x + htiles)
    {
        rectb.right = (b.right - window_x) * 16 + 15;
    }
    else
    {
        rectb.right = htiles * 16;
    }

    if (b.top < window_y + htiles)
    {
        rectb.bottom = (b.bottom - window_y) * 16 + 15;
    }
    else
    {
        rectb.bottom = vtiles * 16;
    }

    rect(where, rectb.left, rectb.top, rectb.right, rectb.bottom, color);
}



/*! \brief Locate the bounded area specified by the given direction
 *
 *                    - -3 means first box
 *                    - -1 means previous box
 * \param   direction - 0 means current box
 *                    - 1 means next box
 *                    - 3 means last box
 * \param   current - Number of bounded areas currently defined
 */
int find_bound(int direction, int *current)
{
    // No bounding areas; nothing to do, so return 'not found'
    if (gmap.bounds.size < 1)
    {
        return 0;
    }

    /* Which bounding box to move to */
    if (direction == -3)
    {
        /* First box */
        *current = 0;
    }
    else if (direction == -1)
    {
        /* Previous box */
        if (--*current < 0)
        {
            *current = gmap.bounds.size - 1;
        }
    }
    else if (direction == 0)
    {
        /* Center map on current Box, if it's not too big */

        int x, y;

        x = (gmap.bounds.array[*current].right - gmap.bounds.array[*current].left) / 2 +
            gmap.bounds.array[*current].left;
        y = (gmap.bounds.array[*current].bottom - gmap.bounds.array[*current].top) / 2 +
            gmap.bounds.array[*current].top + 1;

        center_window(x, y);
    }
    else if (direction == 1)
    {
        /* Next Box */
        if (++*current >= gmap.bounds.size)
        {
            *current = 0;
        }
    }
    else if (direction == 3)
    {
        /* Last Box */
        *current = gmap.bounds.size - 1;
    }
    return 1;
}



/*! \brief Locate the marker specified by the given direction
 *
 *                    - -3 means first marker
 *                    - -1 means previous marker
 * \param   direction - 0 means current marker
 *                    - 1 means next marker
 *                    - 3 means last marker
 * \param   current - Number of markers currently defined
 */
int find_next_marker(int direction, size_t *current)
{
    std::vector<Marker*> markerArray = gmap.markers.Markers();

    // No markers; nothing to do, so return 'not found'
    if (markerArray.size() == 0)
    {
        return 0;
    }

    /* Which marker to move to */
    if (direction == -3)
    {
        /* First marker */
        *current = 0;
    }
    else if (direction == -1)
    {
        /* Previous Marker */
        if (*current > 0)
        {
            --*current;
        }
        else
        {
            *current = markerArray.size() - 1;
        }
    }
    else if (direction == 0)
    {
        if (*current < markerArray.size())
        {
            /* Center map on current Marker */
            center_window(markerArray[*current]->X(), markerArray[*current]->Y());
        }
    }
    else if (direction == 1)
    {
        /* Next Marker */
        if (*current < markerArray.size())
        {
            ++*current;
        }
        else
        {
            *current = 0;
        }
    }
    else if (direction == 3)
    {
        /* Last Marker */
        *current = markerArray.size() - 1;
    }
    return 1;
}



/*! \brief Move the window so we can see the currenly-selected box
 *
 * \param   current - Number of bounding boxes currently defined
 * \modify  global coords "window_[xy]" will be updated
 */
void orient_bounds(int current)
{
    int width, height;

    width = gmap.bounds.array[current].right - gmap.bounds.array[current].left;
    height = gmap.bounds.array[current].bottom - gmap.bounds.array[current].top;

    if (width > htiles - 1)
    {
        /* Boundary is larger than window's width */

        /* Move window's left edge to the box's left edge */
        window_x = gmap.bounds.array[current].left;
    }
    else
    {
        /* Boundary fits inside viewable window */

        /* Move window just enough to see the entire box */
        if (gmap.bounds.array[current].left < window_x)
        {
            window_x = gmap.bounds.array[current].left;
        }
        else if (gmap.bounds.array[current].right > window_x + htiles - 1)
        {
            window_x = gmap.bounds.array[current].right - htiles + 1;
        }
    }

    if (height > vtiles - 1)
    {
        /* Boundary is larger than window's height */

        /* Move window's top edge to the box's top edge */
        window_y = gmap.bounds.array[current].top;
    }
    else
    {
        /* Boundary fits inside viewable window */

        /* Move window just enough to see the entire box */
        if (gmap.bounds.array[current].top < window_y)
        {
            window_y = gmap.bounds.array[current].top;
        }
        else if (gmap.bounds.array[current].bottom > window_y + vtiles - 1)
        {
            window_y = gmap.bounds.array[current].bottom - vtiles + 1;
        }
    }
}



/*! \brief Give the bounded area a new background tile number
 *
 * \param   box - Which box to assign the value to
 */
void rename_bound_tile(Bound *box)
{
    int response, done;
    int selected_tile = 0;

    make_rect(double_buffer, 2, 32);
    sprintf(strbuf, "Tile: %d", box->btile);
    print_sfont(6, 6, strbuf, double_buffer);
    print_sfont(6, 12, ">", double_buffer);

    done = 0;
    while (!done)
    {
        blit2screen();
        response = get_line(12, 12, strbuf, 4);

        /* If the user hits ESC, break out of the function entirely */
        if (response == 0)
        {
            return;
        }

        /* Make sure this line isn't blank */
        if (strlen(strbuf) > 0)
        {
            selected_tile = atoi(strbuf);

            /* Make sure the value is valid */
            if (selected_tile < 0 || selected_tile >= ICONSET_SIZE * max_sets)
            {
                sprintf(strbuf, "Invalid tile: %d", selected_tile);
                cmessage(strbuf);
                wait_enter();
            }
            else
            {
                done = 1;
            }
        }
    }
    box->btile = selected_tile;
}                               /* rename_bound_tile() */



/*! \brief Give the marker a new name if the name hasn't been taken already
 *
 * \param   found - Which marker to rename
 */
void rename_marker(Marker* found)
{
    int response, done;

    make_rect(double_buffer, 2, 32);
    print_sfont(6, 6, found->Name().c_str(), double_buffer);
    print_sfont(6, 12, ">", double_buffer);

    done = 0;
    while (!done)
    {
        blit2screen();
        response = get_line(12, 12, strbuf, 31);

        /* If the user hits ESC, break out of the function entirely */
        if (response == 0)
        {
            return;
        }

        /* Make sure this line isn't blank */
        if (strlen(strbuf) == 0)
        {
            cmessage("Do you want to clear the name of this marker? (y/n)");
            if (yninput())
            {
                done = 1;
            }
        }
        else
        {
            done = 1;
        }

        /* Make sure no other markers have the same name */
        Marker* foundByName = gmap.markers.FindMarker(strbuf);
        if (foundByName != NULL)
        {
            cmessage("Another marker has that name. Use another name.");
            yninput();
            done = 0;
            break;
        }
    }
    found->Name(strbuf);
}                               /* rename_marker() */

/* Local Variables:     */
/* mode: c              */
/* comment-column: 0    */
/* indent-tabs-mode nil */
/* tab-width: 4         */
/* End:                 */
