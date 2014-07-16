#ifndef _PLAYER_H
#define _PLAYER_H

#include <string>
#include <vector>


class Player
{
    // Public methods
    public:

        // Default constructor, no arguments
        Player();

        // Destructor
        ~Player();


    // Public variables
    public:

        // Entity's name, such as during conversations or displayed over its head.
        std::string name;

        // Current level, in whole numbers
        unsigned int level; // was: lvl

        // Current amount of entity's gained experience within the current level.
        unsigned int experience; // was: xp

        // Amount of experience required for next level-up.
        unsigned int experienceNeeded; // was: next

        // Mana depletion rate (100=1.0, 65=0.65). Mainly affects Noslom.
        unsigned int magicUsageRate; // was: mrp

        // Entity's current amount of health.
        unsigned int hitPoints; // was: hp

        // Entity's maximum amount of health.
        unsigned int maxHitPoints; // was: mhp

        // Entity's current amount of mana.
        unsigned int manaPoints; // was: mp

        // Entity's maximum amount of mana.
        unsigned int maxManaPoints; // was: mmp

        // Entity's individual stats.
        Stats stats; // was: stats[NUM_STATS]

        // Entity's resistances to effects.
        Resistances resistances; // was: res[16]

        // Entity's equipment slots.
        EquipmentSlots equipmentSlots; // was: eqp[6]

        // All spells known to Entity.
        std::vector<Spell> spells; // was: spells[60]
};


#endif  // _PLAYER_H
