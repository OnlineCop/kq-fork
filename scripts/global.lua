-- Global functions available to all scripts

--A name for every quest must be inserted here so it can be assigned a unique
--numerical indicator that the engine can recognize. These names will be
--available as indices on the global table 'progress'.
local quest_list = {
  'start',
  'oddwall',
  'darkimpboss',
  'dyingdude',
  'buycure',
  'getpartner',
  'partner1',
  'partner2',
  'showbridge',
  'talkderig',
  'fightonbridge',
  'fellinpit',
  'eklawelcome',
  'loseronbridge',
  'asleeponbridge',
  'altarswitch',
  'killblord',
  'goblinitem',
  'oracle',
  'ftotal',
  'floor1',
  'floor2',
  'floor3',
  'floor4',
  'wall1',
  'wall2',
  'wall3',
  'wall4',
  'dooropen',
  'dooropen2',
  'toweropen',
  'dragondown',
  'treasureroom',
  'undeadjewel',
  'ucoin',
  'cancelrod',
  'portalgone',
  'warpedtot4',
  'oldpartner',
  'boughthouse',
  'talkgelik',
  'opalhelmet',
  'foundmayor',
  'talk_temmin',
  'emberskey',
  'foughtguild',
  'guildsecret',
  'seecoliseum',
  'opalshield',
  'stone1',
  'stone2',
  'stone3',
  'stone4',
  'denorian',
  'c4doorsopen',
  'demnasdead',
  'firsttime',
  'roundnum',
  'battlestatus',
  'useitemincombat',
  'finalpartner',
  'talkgrampa',
  'savebreanne',
  'passguards',
  'ironkey',
  'avatardead',
  'giantdead',
  'opalband',
  'bronzekey',
  'cavekey',
  'town6inn',
  'warpstone',
  'dointro',
  'gotofort',
  'gotoestate',
  'talkbutler',
  'passdoor1',
  'passdoor2',
  'passdoor3',
  'bomb1',
  'bomb2',
  'bomb3',
  'bomb4',
  'bomb5',
  'dynamite',
  'talkrufus',
  'earlyprogress',
  'opaldragonout',
  'opalarmour',

  --'manorparty' names are slots that indicate who is at the manor. The slots
  --are accessed by concatenating 'manorparty' with a number. The values can be
  --0 for nobody or 1..8 for characters 0..7.
  'manorparty0',
  'manorparty1',
  'manorparty2',
  'manorparty3',
  'manorparty4',
  'manorparty5',
  'manorparty6',
  'manorparty7',

  'manor',
  'players',
  'talk_ajathar',
  'blade',
  'ayla_quest',
  'bangthumb',
  'walking',
  'mayorguard1',
  'mayorguard2',
  'talk_tsorin',
  'talk_corin',
  'talkoldman',
  'oraclemonsters',
  'travelpoint',

  -- /* side quests */
  'sidequest1',
  'sidequest2',
  'sidequest3',
  'sidequest4',
  'sidequest5',
  'sidequest6',
  'sidequest7',

  'goblin_monk',
  'trident',
  'tunnel',
  'crystal',
  'malkletter'
}


--Determine which C index corresponds to each quest name.
for i, quest in ipairs(quest_list) do
  quest_list[quest] = i - 1
end


--An interface to the progress of the engine storage so we can store progress values
--in savegames. Indices are quest names given in quest_list. Values are
--integers.
progress = {}

--Returns the C index of quest.
function quest_index(quest)
  local index = tonumber(quest_list[quest])
  if index then
    return index
  else
    error('progress: Invalid quest name: ' .. quest, 3)
  end
end

--Returns the progress amount for the given quest.
function progress:__index(quest)
  return get_progress(quest_index(quest))
end

--Assigns a progress level for a quest.
--quest: A quest name.
--n: The new progress amount. If not a number, it will be interpreted as 0.
function progress:__newindex(quest, n)
  set_progress(quest_index(quest), n)
end

setmetatable(progress, progress)


-- Item identifiers (empty for now. Defined in itemdefs.h)

-- Special Item definitions

SI_UCOIN = 0
SI_CANCELROD = 1
SI_JADEPENDANT = 2
SI_UNDEADJEWEL = 3
SI_WHITESTONE = 4
SI_BLACKSTONE = 5
SI_EMBERSKEY = 6
SI_BRONZEKEY = 7
SI_DENORIANSTATUE = 8
SI_OPALHELMET = 9
SI_OPALSHIELD = 10
SI_IRONKEY = 11
SI_OPALBAND = 12
SI_OPALARMOUR = 13
SI_CAVEKEY = 14
SI_NOTE_TSORIN = 15
SI_NOTE_DERIG = 16
SI_RUSTYKEY = 17
SI_STRANGEPENDANT = 18
SI_EMBERCOIN = 19
SI_TRIDENT = 20
SI_UCOIN2 = 21
SI_CRYSTAL = 22

-- These values correspond to the values found in include/itemdefs.h

I_NOITEM            = 0    -- /* Emtpy */
I_MACE1             = 1    -- /* Mace */
I_MACE2             = 2    -- /* Morningstar */
I_MACE3             = 3    -- /* Frozen Star */
I_MACE4             = 4    -- /* Death's Head */
I_HAMMER1           = 5    -- /* War Hammer */
I_HAMMER2           = 6    -- /* Stun Hammer */
I_HAMMER3           = 7    -- /* Battle Hammer */
I_HAMMER4           = 8    -- /* Thor's Hammer */
I_SWORD1            = 9    -- /* Rapier */
I_SWORD2            = 10   -- /* Short Sword */
I_SWORD3            = 11   -- /* Long Sword */
I_SWORD4            = 12   -- /* Katana */
I_SWORD5            = 13   -- /* Great Sword */
I_SWORD6            = 14   -- /* Dragon Sword */
I_SWORD7            = 15   -- /* Avenger Blade */
I_AXE1              = 16   -- /* Hand Axe */
I_AXE2              = 17   -- /* Battle Axe */
I_AXE3              = 18   -- /* Hunter's Axe */
I_AXE4              = 19   -- /* Slayer's Axe */
I_KNIFE1            = 20   -- /* Knife */
I_KNIFE2            = 21   -- /* Long Knife */
I_KNIFE3            = 22   -- /* Balmar's Dagger */
I_KNIFE4            = 23   -- /* Aichasi Knife */
I_SPEAR1            = 24   -- /* Half Spear */
I_SPEAR2            = 25   -- /* Long Spear */
I_SPEAR3            = 26   -- /* Battle Spear */
I_SPEAR4            = 27   -- /* Chaku Spear */
I_ROD1              = 28   -- /* Iron Rod */
I_ROD2              = 29   -- /* Rod of Fire */
I_ROD3              = 30   -- /* Gloom Rod */
I_ROD4              = 31   -- /* Crystal Rod */
I_ROD5              = 32   -- /* Temmet Rod */
I_STAFF1            = 33   -- /* Staff */
I_STAFF2            = 34   -- /* Soul Staff */
I_STAFF3            = 35   -- /* Defender Staff */
I_STAFF4            = 36   -- /* Pentha Staff */
I_STAFF5            = 37   -- /* Maham Staff */
I_SHIELD1           = 38   -- /* Wooden Shield */
I_SHIELD2           = 39   -- /* Iron Shield */
I_SHIELD3           = 40   -- /* Steel Shield */
I_SHIELD4           = 41   -- /* Tegal Buckler */
I_SHIELD5           = 42   -- /* Aegis Shield */
I_SHIELD6           = 43   -- /* Opal Shield */
I_SHIELD7           = 44   -- /* Unadium Shield */
I_CAP1              = 45   -- /* Cap */
I_CAP2              = 46   -- /* Wizard Cap */
I_CAP3              = 47   -- /* Bandanna */
I_CAP4              = 48   -- /* Ribbon of Ayol */
I_CAP5              = 49   -- /* Mask of Tyras */
I_HELM1             = 50   -- /* Leather Helm */
I_HELM2             = 51   -- /* Iron Helm */
I_HELM3             = 52   -- /* Steel Helm */
I_HELM4             = 53   -- /* Opal Helm */
I_HELM5             = 54   -- /* Unadium Helm */
I_ROBE1             = 55   -- /* Cloth Robe */
I_ROBE2             = 56   -- /* Wizard's Robe */
I_ROBE3             = 57   -- /* Sorceror Robe */
I_ROBE4             = 58   -- /* Arch-Magi Robe */
I_ROBE5             = 59   -- /* Trenta Robes */
I_SUIT1             = 60   -- /* Garment */
I_SUIT2             = 61   -- /* Fighting Suit */
I_SUIT3             = 62   -- /* Battle Suit */
I_SUIT4             = 63   -- /* Flanel Shirt */
I_SUIT5             = 64   -- /* Power Suit */
I_ARMOR1            = 65   -- /* Leather Armor */
I_ARMOR2            = 66   -- /* Bronze Armor */
I_ARMOR3            = 67   -- /* Chain Mail */
I_ARMOR4            = 68   -- /* Gold Armor */
I_ARMOR5            = 69   -- /* Plate Mail */
I_ARMOR6            = 70   -- /* Dragon Armor */
I_ARMOR7            = 71   -- /* Opal Armor */
I_ARMOR8            = 72   -- /* Crystal Armor */
I_ARMOR9            = 73   -- /* Unadium Armor */
I_BAND1             = 74   -- /* Quartz Band */
I_BAND2             = 75   -- /* Adamant Band */
I_BAND3             = 76   -- /* Opal Band */
I_BAND4             = 77   -- /* Unadium Band */
I_GLOVES1           = 78   -- /* Gloves */
I_GLOVES2           = 79   -- /* Ninja Gloves */
I_GLOVES3           = 191  -- /* Satin Gloves */
I_GAUNTLET1         = 80   -- /* Battle Gauntlets */
I_GAUNTLET2         = 81   -- /* Titan Gaunlets */
I_GAUNTLET3         = 82   -- /* Force Gauntlets */
I_SPEEDBOOTS        = 83   -- /* Boots of Speed */
I_HERMES            = 84   -- /* Hermes Shoes */
I_AGRAN             = 85   -- /* Agran Talisman */
I_EAGLEEYES         = 86   -- /* Eagle Eyes */
I_PURITYGEM         = 87   -- /* Gem of Purity */
I_MANALOCKET        = 88   -- /* Mana Locket */
I_MESRA             = 89   -- /* Mesra Feather */
I_OCEANPEARL        = 90   -- /* Ocean Pearl */
I_POWERBRACE        = 91   -- /* Power Brace */
I_PRIESTESS         = 92   -- /* Priestess Charm */
I_REGENERATOR       = 93   -- /* Regenerator */
I_RUBYBROOCH        = 94   -- /* Ruby Brooch */
I_SHADECLOAK        = 95   -- /* Cloak of Shades */
I_DEFCLOAK          = 96   -- /* Defense Cloak */
I_RUNECLOAK         = 97   -- /* Rune Cloak */
I_SPIRITCAPE        = 98   -- /* Spirit Cape */
I_WOODCLOAK         = 99   -- /* Woodland Cloak */
I_WATERRING         = 100  -- /* Water Ring */
I_PALADINRING       = 101  -- /* Paladin's Ring */
I_RILOCRING         = 102  -- /* Riloc's Ring */
I_MHERB             = 103  -- /* Medicinal Herb */
I_NLEAF             = 104  -- /* Neliram Leaf */
I_OSEED             = 105  -- /* Olginar Seed */
I_STRSEED           = 106  -- /* Selingas Seed */
I_AGISEED           = 107  -- /* Amasian Seed */
I_VITSEED           = 108  -- /* Vecindu Seed */
I_INTSEED           = 109  -- /* Ingral Seed */
I_WISSEED           = 110  -- /* Walsiras Seed */
I_EDROPS            = 111  -- /* Elimas Drops */
I_EDAENRA           = 112  -- /* Elixir of Daenra */
I_KBREW             = 113  -- /* Krendar's Brew */
I_LTONIC            = 114  -- /* Larinon Tonic */
I_NPOULTICE         = 115  -- /* Nidana Poultice */
I_PCURING           = 116  -- /* Potion of Curing */
I_SALVE             = 117  -- /* Salve */
I_WENSAI            = 118  -- /* Water of Ensai */
I_HPUP              = 119  -- /* Elixir of Health */
I_MPUP              = 120  -- /* Mystic Elixir */
I_SSTONE            = 121  -- /* Sun Stone */
I_RRUNE             = 122  -- /* Rune of Recovery */
I_WRUNE             = 123  -- /* Rune of Air */
I_ERUNE             = 124  -- /* Rune of Earth */
I_FRUNE             = 125  -- /* Rune of Fire */
I_IRUNE             = 126  -- /* Rune of Water */
I_TP100S            = 127  -- /* Thin sliver of wood */

I_B_CURE1           = 128  -- /* Spell Scroll: Cure1 */
I_B_HOLD            = 129  -- /* Spell Scroll: Hold */
I_B_SHIELD          = 130  -- /* Spell Scroll: Shield */
I_B_SILENCE         = 131  -- /* Spell Scroll: Silence */
I_B_SLEEP           = 132  -- /* Spell Scroll: Sleep */
I_B_BLESS           = 133  -- /* Spell Scroll: Bless */
I_B_VISION          = 134  -- /* Spell Scroll: Vision */
I_B_CURE2           = 135  -- /* Spell Scroll: Cure2 */
I_B_HOLYMIGHT       = 136  -- /* Spell Scroll: Holy Might */
I_B_RESTORE         = 137  -- /* Spell Scroll: Restore */
I_B_FADE            = 138  -- /* Spell Scroll: Fade */
I_B_HASTEN          = 139  -- /* Spell Scroll: Hasten */
I_B_LIFE            = 140  -- /* Spell Scroll: Life */
I_B_SHELL           = 141  -- /* Spell Scroll: Shell */
I_B_WHIRLWIND       = 142  -- /* Spell Scroll: Whirlwind */
I_B_FLOOD           = 143  -- /* Spell Scroll: Flood */
I_B_RECOVERY        = 144  -- /* Spell Scroll: Recovery */
I_B_SHIELDALL       = 145  -- /* Spell Scroll: Shield All */
I_B_SLEEPALL        = 146  -- /* Spell Scroll: Sleep All */
I_B_CURE3           = 147  -- /* Spell Scroll: Cure3 */
I_B_REGENERATE      = 148  -- /* Spell Scroll: Regenerate */
I_B_REPULSE         = 149  -- /* Spell Scroll: Repulse */
I_B_THROUGH         = 150  -- /* Spell Scroll: Through */
I_B_QUICKEN         = 151  -- /* Spell Scroll: Quicken */
I_B_TRUEAIM         = 152  -- /* Spell Scroll: True Aim */
I_B_WALL            = 153  -- /* Spell Scroll: Wall */
I_B_DIVINEGUARD     = 154  -- /* Spell Scroll: Divine Guard */
I_B_TORNADO         = 155  -- /* Spell Scroll: Tornado */
I_B_FULLLIFE        = 156  -- /* Spell Scroll: Full Life */
I_B_CURE4           = 157  -- /* Spell Scroll: Cure4 */
I_B_LUMINE          = 158  -- /* Spell Scroll: Lumine */
I_B_TSUNAMI         = 159  -- /* Spell Scroll: Tsunami */
I_B_VENOM           = 160  -- /* Spell Scroll: Venom */
I_B_SCORCH          = 161  -- /* Spell Scroll: Scorch */
I_B_BLIND           = 162  -- /* Spell Scroll: Blind */
I_B_CONFUSE         = 163  -- /* Spell Scroll: Confuse */
I_B_SHOCK           = 164  -- /* Spell Scroll: Shock */
I_B_GLOOM           = 165  -- /* Spell Scroll: Gloom */
I_B_NAUSEA          = 166  -- /* Spell Scroll: Nausea */
I_B_FROST           = 167  -- /* Spell Scroll: Frost */
I_B_SLOW            = 168  -- /* Spell Scroll: Slow */
I_B_DRAIN           = 169  -- /* Spell Scroll: Drain */
I_B_FIREBLAST       = 170  -- /* Spell Scroll: Fire Blast */
I_B_WARP            = 171  -- /* Spell Scroll: Warp */
I_B_STONE           = 172  -- /* Spell Scroll: Stone */
I_B_LIGHTNING       = 173  -- /* Spell Scroll: Lightning */
I_B_VIRUS           = 174  -- /* Spell Scroll: Virus */
I_B_TREMOR          = 175  -- /* Spell Scroll: Tremor */
I_B_ABSORB          = 176  -- /* Spell Scroll: Absorb */
I_B_DIFFUSE         = 177  -- /* Spell Scroll: Diffuse */
I_B_DOOM            = 178  -- /* Spell Scroll: Doom */
I_B_MALISON         = 179  -- /* Spell Scroll: Malison */
I_B_FLAMEWALL       = 180  -- /* Spell Scroll: Flame Wall */
I_B_BLIZZARD        = 181  -- /* Spell Scroll: Blizzard */
I_B_DEATH           = 182  -- /* Spell Scroll: Death */
I_B_THUNDERSTORM    = 183  -- /* Spell Scroll: Thunder Storm */
I_B_NEGATIS         = 184  -- /* Spell Scroll: Negatis */
I_B_EARTHQUAKE      = 185  -- /* Spell Scroll: Earthquake */
I_B_PLAGUE          = 186  -- /* Spell Scroll: Plague */
I_B_XSURGE          = 187  -- /* Spell Scroll: X-Surge */
I_CHENDIGAL         = 188  -- /* Chendigal */
I_CHENDIGRA         = 189  -- /* Chendigra */
I_DYNAMITE          = 190  -- /* Dynamite */

-- Facing directions, HERO1 and HERO2, and the Hero identifiers are all found
-- in code. These are duplicates. Avoid changing them.

-- Facing directions
FACE_DOWN    = 0
FACE_UP      = 1
FACE_LEFT    = 2
FACE_RIGHT   = 3

-- Special identifiers for bubble()
HERO1             = 200
HERO2             = 201


-- Hero identifiers
SENSAR            = 0
SARINA            = 1
CORIN             = 2
AJATHAR           = 3
CASANDRA          = 4
TEMMIN            = 5
AYLA              = 6
NOSLOM            = 7


-- gettext alias
_ = gettext


-- Add this hero to the manor if not already there
-- hero can be a single value or a table
-- returns the number of heroes that were actually added
function add_to_manor(hero)
  local total, i

  if (not hero) then
    return 0
  end

  if (istable(hero)) then
    total = 0
    i = 1
    while (hero[i]) do
      total = total + add_to_manor(hero[i])
      i = i + 1
    end
    return total
  else
    if (hero < 0 or hero > 7) then
      return 0
    end
    for i = 0, 7 do
      if progress['manorparty' .. i] == hero + 1 then
        return 0
      end
    end
    for i = 0, 7 do
      if progress['manorparty' .. i] == 0 then
        progress['manorparty' .. i] = hero + 1
        return 1
      end
    end
  end
end


-- Return a table containing 'n' (count of arguments) followed by original '...' varargs.
function table.pack(...)
  return { n = select("#", ...), ... }
end


-- Display bubble text; just concatenate all the args and call the _ex function
-- Args ent  Entity number
--      ...  Variable number of arguments - text to show
function bubble(ent, ...)
  s = ""
  local args = table.pack(...)
  for i = 1, args.n do
    if (i ~= 1) then
      s = s.."\n"
    end
    s = s..args[i]
  end
  bubble_ex(ent, s)
end


function portbubble(ent, ...)
  s = ""
  local args = table.pack(...)
  for i = 1, args.n do
    if (i ~= 1) then
      s = s.."\n"
    end
    s = s..args[i]
  end
  portbubble_ex(ent, s)
end


-- See function bubble()
function thought(ent, ...)
  s = ""
  local args = table.pack(...)
  for i = 1, args.n do
    if (i ~= 1) then
      s = s.."\n"..args[i]
    else
      s = s..args[i]
    end
  end
  thought_ex(ent, s)
end


function portthought(ent, ...)
  s = ""
  local args = table.pack(...)
  for i = 1, args.n do
    if (i ~= 1) then
      s = s.."\n"..args[i]
    else
      s = s..args[i]
    end
  end
  portthought_ex(ent, s)
end


function gameover(ent, ...)
  s = ""
  local args = table.pack(...)
  for i = 1, args.n do
    if (i ~= 1) then
      s = s.."\n"..args[i]
    else
      s = s..args[i]
    end
  end
  portbubble(ent, s)
  gameover_ex()
end


function get_quest_info()
   if LOC_get_quest_info then
      LOC_get_quest_info()
   end
   -- Add global quest items here:
   -- add_quest_item("key", "text")
end


-- backward compat
change_mapm = change_map


-- Checks if this ent is in the party, or in the manor, or has never been
-- recruited.
-- who: hero id
-- returns "manor" if in manor, "party" if in party, nil otherwise
function LOC_manor_or_party(who)
  local a
  if (get_pidx(0) == who) then
    return "party"
  elseif (get_numchrs() > 1 and get_pidx(1) == who) then
    return "party"
  end

  --Ideally, this should iterate over a list of character names, but that might
  --require altering the C engine.
  for a = 0, 7 do
    if progress['manorparty' .. a] - 1 == who then
      return "manor"
    end
  end
  return nil
end


-- Pick one of the args
-- If args is a table it can have a pr field which gives
-- its probability of being picked
-- e.g. print(pick(1,2,3))
--      pick({pr = 5, name = "Rare"}, {pr = 95, name = "Common"}).name
function pick(...)
  cumprob = 0

  local args = table.pack(...)
  for i = 1, args.n do
    if (istable(args[i]) ) then
      prob = args[i].pr or 1
    else
      prob = 1
    end

    cumprob = cumprob + prob
  end

  cumprob = krnd(cumprob)

  for i = 1, args.n do
    if (istable(args[i]) ) then
      prob = args[i].pr or 1
    else
      prob = 1
    end

    cumprob = cumprob - prob

    if (cumprob < 0) then
      return args[i]
    end
  end
end


-- Select from heroes in the manor
function select_manor()
  -- Get the current list
  heroes = {}
  for i = 1, 8 do
    v = progress['manorparty' .. i - 1]
    if (v ~= 0) then
      heroes[i] = v - 1
    end
  end
  -- Do the selecting
  heroes = select_team(heroes)
  -- Put back in the list
  for i = 1, 8 do
    if (heroes[i]) then
      v = heroes[i] + 1
    else
      v = 0
    end
    progress['manorparty' .. i - 1] = v
  end
end


-- Swaps the desired character to first in line if they are not already
function swap_character_first(character)
  if (party[1] == character) then
    party[0], party[1] = party[1], party[0]
  end
end


--  Response for reading a book.
function book_talk(ent)
  if (party[0] == Sensar) then
    bubble(HERO1, pick(_"Reading makes me sleepy...",
           _"So many books...",
           _"Reading is for wimps."))

  elseif (party[0] == Temmin) then
    bubble(HERO1, pick(_"If only I had more time...",
           _"So many books...",
           _"Some of these are pretty old."))

  elseif (party[0] == Sarina) then
    bubble(HERO1, pick(_"Ugh... this would take me forever to read.",
           _"I never liked reading.",
           _"Who wrote this trash?"))

  elseif (party[0] == Noslom) then
    bubble(HERO1, pick(_"Fascinating.",
           _"I have this one.",
           _"Romance novels... gack!"))

  elseif (party[0] == Ajathar) then
    bubble(HERO1, pick(_"Hmmm... I don't approve of that.",
           _"I'm too busy to read now.",
           _"How many books can you write that start with 'The Joy of...'?"))

  elseif (party[0] == Ayla) then
    bubble(HERO1, pick(_"I don't have time for this.",
          _"What language is this written in?",
          _"The pages are stuck together!?"))

  elseif (party[0] == Casandra) then
    bubble(HERO1, pick(_"Boring.",
           _"Somebody should burn these.",
           _"Terrible... just terrible."))

  elseif (party[0] == Corin) then
    bubble(HERO1, pick(_"Doesn't anybody leave spellbooks lying around?",
           _"Why would I read this?",
           _"Can't talk... reading."))
  else
    message("Script Error. global.lua:book_talk()", 255, 0)
  end

end


-- This function can be called whenever the hero touches a fire
function touch_fire(ent)
  local x

  if (party[0] == Sensar) then
    bubble(HERO1, pick(_"What th..? Ouch! That's hot!",
           _"There's no way I'm sticking my hand in that fire!",
           _"This feels pretty nice."))

  elseif (party[0] == Temmin) then
    bubble(HERO1, pick(_"Ah, the age-old fire.",
           _"This needs more coal.",
           _"This would be great to read a book next to."))

  elseif (party[0] == Sarina) then
    bubble(HERO1, pick(_"Mmm, wood smoke.",
           _"Smells like burnt hair. Hey wait... that's MY hair!",
           _"Ooh, cozy."))

  elseif (party[0] == Noslom) then
    bubble(HERO1, pick(_"I prefer torches.",
           _"I love the crackle of a good fire.",
           _"I wonder if a spell would make this burn brighter?"))

  elseif (party[0] == Ajathar) then
    bubble(HERO1, pick(_"Hmm... I want marshmallows.",
           _"You call this a fire?!",
           _"Ah, relaxing."))

  elseif (party[0] == Ayla) then
    bubble(HERO1, pick(_"I wonder how hot this is?",
           _"Someone should clean all this soot out of here.",
           _"Well, my face is warm now, but my butt is still freezing!"))

  elseif (party[0] == Casandra) then
    bubble(HERO1, pick(_"Something's burning. I hope it's one of those stupid books!",
           _"The fire is getting low.",
           _"Yessir, this is a fire."))

  elseif (party[0] == Corin) then
    bubble(HERO1, pick(_"I sure like fire.",
           _"Watching this is relaxing.",
           _"This is making me sleepy."))

  else
    message("Script Error. global.lua:touch_fire()", 255, 0)
  end

end


