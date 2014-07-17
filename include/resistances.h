#ifndef _RESISTANCES_H
#define _RESISTANCES_H

#include <allegro.h>
#include <cstdio>
#include <map>


enum eResistance
{
    RESIST_EARTH,     //0
    RESIST_BLACK,     //1
    RESIST_FIRE,      //2
    RESIST_THUNDER,   //3
    RESIST_AIR,       //4
    RESIST_WHITE,     //5
    RESIST_WATER,     //6
    RESIST_ICE,       //7
    RESIST_POISON,    //8
    RESIST_BLIND,     //9
    RESIST_CHARM,     //10
    RESIST_PARALYZE,  //11
    RESIST_PETRIFY,   //12
    RESIST_SILENCE,   //13
    RESIST_SLEEP,     //14
    RESIST_TIME,      //15
};

const signed int MIN_RESISTANCE = -10;
const signed int MAX_RESISTANCE_GROUP1 = 10;
const signed int MAX_RESISTANCE_GROUP2 = 20;


/*! \name Runes/Resistances */
/*  Entity's strengths and weaknesses to certain elements and elemental attacks:
 *  - Negative values mean lots of damage from this type of elemental attack
 *  - 0 means neutral
 *  - Positive values mean very little damage from this type of elemental attack
 */
class Resistances
{
    public:

        // Default constructor, no arguments.
        Resistances();

        // Initialization constructor, one argument per eResistance.
        Resistances(int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int);

        // Copy constructor.
        Resistances(const Resistances&);

        // Destructor.
        ~Resistances();

        // Assignment operator.
        Resistances& operator=(const Resistances&);

        // Load resistance data from the specified FILE, returning 0 on success or non-0 on failure.
        int Load(FILE* file);

        // Load resistance data from the specified PACKFILE, returning 0 on success or non-0 on failure.
        int Load(PACKFILE *);

        // Save resistance data to the specified PACKFILE, returning true on success or false on failure.
        bool Save(PACKFILE *);

        // Return how much of a resistance to the specified elemental attack this entity has.
        signed int GetResistanceAmount(eResistance);

        // Return how much all resistances combined equal.
        signed int GetCombinedResistanceAmounts();

        // Set the amount of resistance to the specified elemental attack.
        void SetResistanceAmount(eResistance, signed int);

        // Add the specified amount of resistance to the elemental attack.
        signed int AddResistanceAmount(eResistance, signed int);

    public:

    protected:
        std::map<eResistance, signed int> _resistances;
};

#endif  // _RESISTANCES_H
