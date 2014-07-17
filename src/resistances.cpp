#include "resistances.h"
#include "templates.h"
#include <algorithm>


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


// Initialization constructor, one argument per eResistance.
Resistances::Resistances(
    int resist_earth,
    int resist_black,
    int resist_fire,
    int resist_thunder,
    int resist_air,
    int resist_white,
    int resist_water,
    int resist_ice,
    int resist_poison,
    int resist_blind,
    int resist_charm,
    int resist_paralyze,
    int resist_petrify,
    int resist_silence,
    int resist_sleep,
    int resist_time)
{
    _resistances[RESIST_EARTH]      = resist_earth;
    _resistances[RESIST_BLACK]      = resist_black;
    _resistances[RESIST_FIRE]       = resist_fire;
    _resistances[RESIST_THUNDER]    = resist_thunder;
    _resistances[RESIST_AIR]        = resist_air;
    _resistances[RESIST_WHITE]      = resist_white;
    _resistances[RESIST_WATER]      = resist_water;
    _resistances[RESIST_ICE]        = resist_ice;
    _resistances[RESIST_POISON]     = resist_poison;
    _resistances[RESIST_BLIND]      = resist_blind;
    _resistances[RESIST_CHARM]      = resist_charm;
    _resistances[RESIST_PARALYZE]   = resist_paralyze;
    _resistances[RESIST_PETRIFY]    = resist_petrify;
    _resistances[RESIST_SILENCE]    = resist_silence;
    _resistances[RESIST_SLEEP]      = resist_sleep;
    _resistances[RESIST_TIME]       = resist_time;
}


// Copy constructor.
Resistances::Resistances(const Resistances& other) :
    _resistances(other._resistances)
{
    //
}


// Destructor.
Resistances::~Resistances()
{
    //
}


// Assignment operator.
Resistances& Resistances::operator=(const Resistances& other)
{
    this->_resistances = other._resistances;
    return *this;
}


// Load resistance data from the specified FILE, returning 0 on success or non-0 on failure.
int Resistances::Load(FILE* file)
{
    signed int tmp = 0;
    int loadSuccessResult = 0;

    try
    {
        fscanf(file, "%d", &_resistances[RESIST_EARTH]);
        fscanf(file, "%d", &_resistances[RESIST_BLACK]);
        fscanf(file, "%d", &_resistances[RESIST_FIRE]);
        fscanf(file, "%d", &_resistances[RESIST_THUNDER]);
        fscanf(file, "%d", &_resistances[RESIST_AIR]);
        fscanf(file, "%d", &_resistances[RESIST_WHITE]);
        fscanf(file, "%d", &_resistances[RESIST_WATER]);
        fscanf(file, "%d", &_resistances[RESIST_ICE]);
        fscanf(file, "%d", &_resistances[RESIST_POISON]);
        fscanf(file, "%d", &_resistances[RESIST_BLIND]);
        fscanf(file, "%d", &_resistances[RESIST_CHARM]);
        fscanf(file, "%d", &_resistances[RESIST_PARALYZE]);
        fscanf(file, "%d", &_resistances[RESIST_PETRIFY]);
        fscanf(file, "%d", &_resistances[RESIST_SILENCE]);
        fscanf(file, "%d", &_resistances[RESIST_SLEEP]);
        fscanf(file, "%d", &_resistances[RESIST_TIME]);
    }
    catch (int exceptionNum)
    {
        loadSuccessResult = 1;
    }

    return loadSuccessResult;
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


// Return how much all resistances combined equal.
signed int Resistances::GetCombinedResistanceAmounts()
{
    std::map<eResistance, signed int>::iterator it, itEnd;
    signed int sum = 0;

    for (it = _resistances.begin(); it != _resistances.end(); ++it)
    {
        sum += it->second;
    }

    return sum;
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


// Add the specified amount of resistance to the elemental attack.
signed int Resistances::AddResistanceAmount(eResistance resistance, signed int value)
{
    std::map<eResistance, signed int>::iterator it;
    signed int newValue = 0;

    it = _resistances.find(resistance);
    if (it != _resistances.end())
    {
        int minValue = MIN_RESISTANCE;
        int maxValue = (it->first <= RESIST_ICE ? MAX_RESISTANCE_GROUP1 : MAX_RESISTANCE_GROUP2);
        // Add the new value to the current resistance, clamping minimum and maximum values.
        newValue = clamp<signed int>(_resistances[resistance] + value, minValue, maxValue);

        _resistances[resistance] = newValue;
    }

    return newValue;
}
