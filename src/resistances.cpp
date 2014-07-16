#include "resistances.h"

// Default constructor, no arguments.
Resistances::Resistances()
{
    _resistances[RESIST_EARTH]      = 0;
    _resistances[RESIST_BLACK]      = 0;
    _resistances[RESIST_FIRE]       = 0;
    _resistances[RESIST_THUNDER]    = 0;
    _resistances[RESIST_AIR]        = 0;
    _resistances[RESIST_WHITE]      = 0;
    _resistances[RESIST_WATER]      = 0;
    _resistances[RESIST_ICE]        = 0;
    _resistances[RESIST_POISON]     = 0;
    _resistances[RESIST_BLIND]      = 0;
    _resistances[RESIST_CHARM]      = 0;
    _resistances[RESIST_PARALYZE]   = 0;
    _resistances[RESIST_PETRIFY]    = 0;
    _resistances[RESIST_SILENCE]    = 0;
    _resistances[RESIST_SLEEP]      = 0;
    _resistances[RESIST_TIME]       = 0;
}


// Destructor
Resistances::~Resistances()
{
    //
}


// Load resistance data from the specified PACKFILE, returning 0 on success or non-0 on failure.
int Resistances::Load(PACKFILE *packfile)
{
    int loadSuccessResult = 0;

    try
    {
        _resistances[RESIST_EARTH]      = (signed int)pack_getc(packfile);
        _resistances[RESIST_BLACK]      = (signed int)pack_getc(packfile);
        _resistances[RESIST_FIRE]       = (signed int)pack_getc(packfile);
        _resistances[RESIST_THUNDER]    = (signed int)pack_getc(packfile);
        _resistances[RESIST_AIR]        = (signed int)pack_getc(packfile);
        _resistances[RESIST_WHITE]      = (signed int)pack_getc(packfile);
        _resistances[RESIST_WATER]      = (signed int)pack_getc(packfile);
        _resistances[RESIST_ICE]        = (signed int)pack_getc(packfile);
        _resistances[RESIST_POISON]     = (signed int)pack_getc(packfile);
        _resistances[RESIST_BLIND]      = (signed int)pack_getc(packfile);
        _resistances[RESIST_CHARM]      = (signed int)pack_getc(packfile);
        _resistances[RESIST_PARALYZE]   = (signed int)pack_getc(packfile);
        _resistances[RESIST_PETRIFY]    = (signed int)pack_getc(packfile);
        _resistances[RESIST_SILENCE]    = (signed int)pack_getc(packfile);
        _resistances[RESIST_SLEEP]      = (signed int)pack_getc(packfile);
        _resistances[RESIST_TIME]       = (signed int)pack_getc(packfile);
    }
    catch (int exceptionNum)
    {
        loadSuccessResult = 1;
    }

    return loadSuccessResult;
}


// Save resistance data to the specified PACKFILE, returning true on success or false on failure.
bool Resistances::Save(PACKFILE *packfile)
{
    bool saveSuccessResult = true;

    try
    {
        pack_putc((char)_resistances[RESIST_EARTH],     packfile);
        pack_putc((char)_resistances[RESIST_BLACK],     packfile);
        pack_putc((char)_resistances[RESIST_FIRE],      packfile);
        pack_putc((char)_resistances[RESIST_THUNDER],   packfile);
        pack_putc((char)_resistances[RESIST_AIR],       packfile);
        pack_putc((char)_resistances[RESIST_WHITE],     packfile);
        pack_putc((char)_resistances[RESIST_WATER],     packfile);
        pack_putc((char)_resistances[RESIST_ICE],       packfile);
        pack_putc((char)_resistances[RESIST_POISON],    packfile);
        pack_putc((char)_resistances[RESIST_BLIND],     packfile);
        pack_putc((char)_resistances[RESIST_CHARM],     packfile);
        pack_putc((char)_resistances[RESIST_PARALYZE],  packfile);
        pack_putc((char)_resistances[RESIST_PETRIFY],   packfile);
        pack_putc((char)_resistances[RESIST_SILENCE],   packfile);
        pack_putc((char)_resistances[RESIST_SLEEP],     packfile);
        pack_putc((char)_resistances[RESIST_TIME],      packfile);
    }
    catch (int exceptionNum)
    {
        saveSuccessResult = false;
    }

    return saveSuccessResult;
}


// Return how much of a resistance to the specified elemental attack this entity has.
signed int Resistances::GetResistanceAmount(eResistance resistance)
{
    std::map<eResistance, signed int>::iterator it;
    it = _resistances.find(resistance);
    if (it == _resistances.end())
    {
        return 0;
    }

    return _resistances[resistance];
}


// Set the amount of resistance to the specified elemental attack.
void Resistances::SetResistanceAmount(eResistance resistance, signed int value)
{
    std::map<eResistance, signed int>::iterator it;
    it = _resistances.find(resistance);
    if (it != _resistances.end())
    {
        _resistances[resistance] = value;
    }
}
