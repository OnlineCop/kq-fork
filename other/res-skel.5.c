};



/* Format: {Map, Zone, Enc, Etnum, Eidx, "BMusic", "BackImg"}
 *   Map:       Where this battle occurs
 *   Zone:      What triggers this battle
 *   Enc:       The 1-in-enc chance there will NOT be combat
 *   Etnum:     Select rows in the Encounter table
 *   Eidx:      Select row out of the etnum; 99 for random
 *   "BMusic":  Music file to play
 *   "BackImg": Background image
 */
s_encounter battles[NUM_BATTLES] =
{
