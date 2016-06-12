};



/* Format: {tnum, lvl, per, idx[5]}
 * tnum:    Encounter number in the Encounter table
 * lvl:     Level of monsters
 * per:     When random encounters are specified, this is the cumulative percentage that this one will be selected
 * idx[5]:  There can be up to 5 enemies per battle; index of each enemy (index starts at 1; 0 means NO enemy)
 */
s_erow erows[NUM_ETROWS] =
{
