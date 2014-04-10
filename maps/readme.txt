Mapdraw: a map editor for KQ
Mapdump: a .MAP to .PCX (or .BMP) image extractor
Mapdiff: a map comparison utility

Thank you in your interest in the KQ game's map tools:
  MapDraw (the map editor)
  MapDump (displays the map as an image)
  MapDiff (the comparison tool)

Mapdraw has been designed for developers to create the maps for KQ quickly and with as little pain as possible.  Unfortunately at this time, there are no current GUI menus (such as File, Edit, etc.), so you will have to make use of the keyboard shortcuts (use F1 to see a list of possible keys).

Maps contain the following:
  3 layers (background, middle, and foreground)
  Entities (NPCs) and their attributes
  Obstacles
  Shadows
  Zones
  Markers
  Bounding Boxes

The background, middle, and foreground layers are broken down as follows:
- Layer 1, background: 90% of the drawing goes here.  This includes the base of trees, tables, ground tiles (such as dirt or flowers), and buildings.
- Layer 2, middle: Objects and tiles which must be drawn on top of Layer 1 go here.  This includes dishes on a table, shrubs covering an item on the ground, etc.  The Player's character will still walk over the top of these, unlike Layer 3.
- Layer 3, foreground: Drawn over the top of the Player's sprite, such as building and castle overhangs, the top of bookshelves, treetops, etc.

*NOTE: KQ makes use of Parallax, a feature which means that one layer can move more quickly or slowly than another in order to create a spacial depth perception.  (Look at pass.map and cave3b.map as examples.)  This may affect the above descriptions slightly.

Maps also contain the following:
  VISIBLE DURING GAMEPLAY:
  - Shadows are drawn over EVERYTHING, including Layer 3.
    - This is a partially transparent layer where, when a Player or NPC is standing under it, part or all of their sprite will be lightened or darkened (depending on the type of shadow).
    - Examples are the edges of buildings or in caves above Save points.
  - Entities (or NPCs) are most commonly drawn between Layer 2 and Layer 3 (depending on the map's parallax mode).
    - Entities' initial attributes are stored in the MAP files, such as starting coordinates, direction, speed, and some scripted movements.
    - All NPCs attributes can be changed at run-time by scripts.
    - Conversations between NPCs-to-NPCs or NPCs-to-Player are not stored in the MAP files.  Those are handled by the LUA script files.
    - Although Entities/NPCs are stored in the map's data, the Player's characters are not.
  INVISIBLE DURING GAMEPLAY:
  - Obstacles restrict movement, and are not shown.
    - The most common obstacle used is the SQUARE (in the map's editor, at least), where nothing can move onto the tile from any 4 directions during gameplay (including NPCs).
    - The other types are the T-shaped obstacles.  These will block movement only in one direction, meaning you can enter the tile from 3 directions, but cannot move onto or off of the tile from the 4th.
    - Examples where these are used are beds, pillars, and cliff edges, such as in town8.
  - Zones are trigger-points (hotspots), and are not shown.
    - Zones are activated either when the Player's characters steps on it (if it is not blocked by an Obstacle) or, if blocked by an Obstacle, when the Player presses the Action key when facing the tile.
    - The specific action carried out is determined by the LUA script files.  They can be used to change maps, open doors, initiate battles, open treasure chests, etc.
    - There can only be one Zone per tile, but there may be many tiles with the same Zone.  If this occurs, all instances of each Zone (such as "Zone 3") will call the same LUA function.
    - Examples of Zones are where random battles may occur, with some zones (like "Zone 4") calling "weak" monsters and others (like "Zone 5") calling "very strong" ones.
  - Markers are "named coordinates", and are not shown.
    - Markers replace static (hard-coded) coordinates in LUA scripts, because they can be easily moved and updated on the MAP file without changing the LUA script.  This is useful incase a map ever changes size.
    - The LUA scripts can use markers for things such as movement targets (such as in "tower") or copying tiles (such as in "cave4").
    - Markers must have unique names, and tiles can have at most one marker at a time.
    - Markers are not triggered hotspots, like zones, so the player will not be affected if they step on a tile which contains a marker.
    - Examples of marker names: two markers could not be named "entrance" on the same map, but one could be named "entrance_1" while another is "entrance_2".
  - Bounding Boxes are visually-enclosed areas of maps that allow all areas outside of their boundaries to be hidden.
    - Bounding Boxes allow for a "fill" effect anywhere outside of its boundaries.  The "fill" tile simply displays a specified tile from the tilemap surrounding the current area.  Some examples are:
      - Using "trees" when in a forest (example: grotto.map)
      - Using "water" when on an island (example: main.map, around the island with the Manor and Ekla)
    - If a map file has several "rooms" close together, and the Player should not see the interior of any room except the one he or she is immediately in, it may be surrounded by a Bounding Box (see dville.map).


===============================
MAPDRAW:

Now that you understand the basic structure of the MAP file, you can create or modify your maps.  Below is a quick-reference section with the keys and their descriptions.

The map editor is split into three panes:
  The main view-window
  The tile selection area on the right
  The stats at the bottom of the map

===============================
VIEW WINDOW:

The view-window is self-explanatory, so I won't cover it here.

===============================
TILE SELECTION:

The tile selection area on the right contains all the tiles which are available for this tileset.  You can change the tileset being used by clicking on the "Icon:" selection in the Map Stats at the bottom.

You will see a 4x10 grid with the tiles, outlined in a thick red border.  All tilesets should have the first tile blank; it will ALWAYS be used as the default, or "none".  You may click on any of these tiles to select it, and use the +/- keys to advance through the available tiles.  This is a dynamic tileset, which means that if the PCX image being used is ever changed/modified, it will appear here and you will have more (or fewer) tiles to choose from.  Currently, KQ maps only support 16x16 tiles, so you will have to work within those boundaries.

Below the 4x10 grid, you are shown which editing mode you are currently in.  If it is "Layer 1", "Layer 2", "Layer 3", "Entities", "Obstacles", "Shadows", "Zones", "Markers" or "Bound", you are in a mode which will allow you to draw onto the map.  Anything else is simply a viewing mode, and nothing will be modified on the map if you click on a tile in the view-window.

Immediately below the editing mode, you will see the "page" of tiles you are on and the selected tile in (parentheses).

You will also be shown the mouse's x/y coordinates.

===============================
MAP STATS:

Here is a sample of what you will see:

Map:                    Revision:
Icon:                   Start X:    Mult:
Song:                   Start Y:    Div:
ZeroZone:               Width:
Map #:      Warp:       Height:
Mode:       WarpX:      SunStone:
Save:       WarpY:      Last Zone:

"Map" gives you the filename of the current map.
"Icon" tells which tilemap you are using.  Clicking on it will change it.
"Song" is the default song played when the player enters the specified map.
"ZeroZone" means that zone 0 triggers an event.
"Map #" corresponds to the map's identifier # (see MAP_* in include/kq.h for details).
"Mode" is the parallax mode.  This means some layers will move at a different speed than others.
"Save" specifies whether the player can save their game in this map.
"Warp" means that the player can (or can't) use the Warp spell to exit this map.
"WarpX" and "WarpY" are where, when the Warp spell is used, the player ends up at.  This only works when Warp is true.
"Revision" is the version of the map.  Revision 0 did not support Markers, Revision 1 did not support Bounding Boxes.  Current revision is "2".
"Start X" and "Start Y" are the default coordinates on the map where the player will start.
"Width" and "Height" are just that, respectively.
"SunStone" is true when the map can be defined as a "sunny place" where a SunStone will work.
"Last Zone" displays the number of the zone with the largest value.
"Mult" and "Div" are used for parallax/depth-perception.

Attributes are toggled, meaning that if you select Shadows, you will be able to draw just to that layer until you turn it off or select a different option.

When you are using one of the Attributes (Entities, Obstacles, Shadows, Zones, or Markers), you will also see which Attribute # you have currently selected.  You will also have "Current Tile" which means that if you move the mouse over a tile in the map and it has that Attribute, it will tell you its value for that tile.  The last is called "Highlight" which just makes a large red bullet appear over the top of the Attribute, so you can more easily see where it is.

The KEY_+ and KEY_- keys (at the top of the keyboard, or on the number pad) are used to choose which tile (they are on the right-side menu) or Attribute you will draw to the map.

===============================
Keys:

NOTE: These are drawing modes, which allow you to modify the map
1   - Layer 1
2   - Layer 2
3   - Layer 3

NOTE: These are viewing modes, which won't modify the map
4   - Layers 1+2
5   - Layers 1+3
6   - Layers 2+3
7   - Layers 1+2+3
A   - Show all Layers and Attributes
C   - Show map as player would see it (including parallax)

NOTE: Attributes can be "toggled".  You may have all 4 showing at once, but
      you will only affect whichever is currently active.  If an Attribute
      is not showing, pressing the appropriate KEY_* shortcut will display
      it and set it as the "active" Attribute.  If the Attribute is active and
      you press its key again, it will be toggled off.
O   - Toggle Obstacles Attribute
S   - Toggle Shadows Attribute
Z   - Toggle Zone Attribute
E   - Toggle Entities Attribute
M   - Toggle Marker Attribute
B   - Toggle Bounding Box Attribute
F12 - Modify Entity Mode
D   - Displace (or move) all the Entities in the map

F1  - Help file
F2  - Load map
F3  - Save current map
N   - New map
R   - Resize current map
F10 - Map description (what the player sees when entering a town, cave, etc.)

G   - Grab (select) a tile from the map and display it on the Tile Selection on the right
H   - Highlight the current Attribute (works for Obstacles, Shadows, and Zones)

F   - Select the first used zone on a map (usually 0)
L   - Select the last used zone on a map

T   - Block copy (left click starts, right click completes the selection)
P   - Paste the copied block/tile (left click: all Layers/Attributes, right-click pastes to specific Layer/Attribute)
ESC - Cancel copy/paste

F5  - Load a PCX image and convert it to a map
J   - Create a PCX image from one (or more) of the Layers
V   - Visualize (create entire image, minus Entities) of whole map
F6  - Copy any instance of the specified tile into any instance of another tile
F9  - Copy the properties from one Layer to another

F4  - Clear all the tiles from the specified Layer
F7  - Clear all the Obstructions on the map
F8  - Clear all the Shadows on the map
W   - Wipe the contents of the current map

UP ARROW    - Move up 1 space
DOWN ARROW  - Move down 1 space
LEFT ARROW  - Move left 1 space
RIGHT ARROW - Move right 1 space
PGUP        - Move up 1 entire screen
PGDN        - Move down 1 entire screen
BACKSPACE   - Move right 1 entire screen
TAB         - Move left 1 entire screen
HOME        - Move to the top-left corner of the map
END         - Move to the bottom-right corner of the map

Q   - Quit the program

+/- keys:
If you are in Entities mode, selects an entity (a whole lot to choose from)
If you are in Shadows mode, selects a shadow type (11 to choose from)
If you are in Obstacles mode, selects an obstacle (5 to choose from)
If you are in Zones mode, select a zone (up to 255; this is what the LUA files refer to)
Otherwise, it will change the selection of the tiles in the Tile Selection area on the right side of the screen.

===============================
Changes, updates:

20040725:
* Added a Highlight feature to help see where all the Obstacles, Shadows, or Zones of the specified number are located (with a big red dot)
* Count the current Obstacles, Shadows, or Zones and display the user (for example, it helps to see if we've used a specific Zone yet or not)
* Code clarification (more legible), bug fixes
* Zone numbers are formatted depending on the length of the number for easier reading
* Shows where the current copy selection starts and ends for better visability
* Cleaned up the screen; fonts and other spacing issues
* Increased the number of tiles displayed from 20 to 40 in the Tile Selection area
* Added a help file (lots of help by PH on this one)
* Error-checking to make sure images can be loaded; if not, give an error message (hopefully, no more "random" crashes)
* Smart-tileset checking: if the desired tile isn't available, skip to the next one after showing a warning (this is helpful for when you want to add your own tilesets temporarily, then remove it without recompiling the code, or if one of the PCX images isn't found in the current directory)
* Created new file, mapdump, to supercede the need for visual_map() (more flexible)
* Allegro version-specific code (patch by PH)
* Additional error-checking code (hopefully, fewer error messages!)

Older changes:

* You can change the tileset by clicking on the " Icon: *** " PCX name and it
  shows the results immediately across the map.
* Function to select the tile under the mouse cursor (its icon in the tileset
  menu is updated to show which tile you've selected).
  * Added right-click tile-grabbing to "grab" the tile and then set the
    draw_mode to the current layer.
* Added help menu (F1).
* Added extra file open/save checks.


===============================
Known bugs:



Questions and comments (donations, threats...) are of course welcome.

-TeamTerradactyl :: teamterradactyl@users.sourceforge.net

===============================
Todo:

Create an is_modified() function to know if we should prompt to save the map or not
Fix the maps's WarpX/WarpY values.
- In the game, we will need to modify the Warp spell so the player can select where they would like to warp to.  They should be able to warp to any previously-visited town.  Towns will not allow players to warp out of them (the map's Warp == "no"), and some maps may have requirements that must be met before the player can leave there (like the towers where the player may have a Unadium or Goblin object in their posession), so Warp needs to be disabled here.  Else, if Warp is enabled, we need to reset the values of the progress(P_*) associated with that particular map's quest/requirements before the player is taken out of there.  We may be able to use the map's value of "extra_byte" to store whether we need to do any "cleanup" before the player is warped out of there; if there is a value there, then we have to clean it up, if not, we can go ahead and warp.
