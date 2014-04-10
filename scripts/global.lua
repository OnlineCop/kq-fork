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
  'sidequest7'
}


--Determine which C index corresponds to each quest name.
for i, quest in ipairs(quest_list) do
  quest_list[quest] = i - 1
end


--An interface to the engine's progress storage so we can store progress values
--in savegames. Indices are quest names given in quest_list. Values are
--integers.
progress = {}

--Returns the C index of quest.
local function quest_index(quest)
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


-- Display bubble text; just concatenate all the args and call the _ex function
-- Args ent  Entity number
--      ...  Variable number of arguments - text to show
function bubble(ent, ...)
  s = ""
  for i = 1, arg.n do
    if (i ~= 1) then
      s = s.."\n"
    end
    s = s..arg[i]
  end
  bubble_ex(ent, s)
end


function portbubble(ent, ...)
  s = ""
  for i = 1, arg.n do
    if (i ~= 1) then
      s = s.."\n"
    end
    s = s..arg[i]
  end
  portbubble_ex(ent, s)
end


-- See function bubble()
function thought(ent, ...)
  s = ""
  for i = 1, arg.n do
    if (i ~= 1) then
      s = s.."\n"..arg[i]
    else
      s = s..arg[i]
    end
  end
  thought_ex(ent, s)
end


function portthought(ent, ...)
  s = ""
  for i = 1, arg.n do
    if (i ~= 1) then
      s = s.."\n"..arg[i]
    else
      s = s..arg[i]
    end
  end
  portthought_ex(ent, s)
end


function gameover(ent, ...)
  s = ""
  for i = 1, arg.n do
    if (i ~= 1) then
      s = s.."\n"..arg[i]
    else
      s = s..arg[i]
    end
  end
  portbubble(ent, s)
  gameover_ex()
end


function get_quest_info()
   if LOC_add_quest_item then
      LOC_add_quest_item()
   end

   add_quest_item(_"About...", _"This doesn't do much yet")
   add_quest_item(_"Test1",    _"Some test info")
   add_quest_item(_"Sensar",   _"He rages!")
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
-- If arg is a table it can have a pr field which gives
-- its probability of being picked
-- e.g. print(pick(1,2,3))
--      pick({pr = 5, name = "Rare"}, {pr = 95, name = "Common"}).name
function pick(...)
  cumprob = 0

  for i = 1, arg.n do
    if (istable(arg[i]) ) then
      prob = arg[i].pr or 1
    else
      prob = 1
    end

    cumprob = cumprob + prob
  end

  cumprob = krnd(cumprob)

  for i = 1, arg.n do
    if (istable(arg[i]) ) then
      prob = arg[i].pr or 1
    else
      prob = 1
    end

    cumprob = cumprob - prob

    if (cumprob < 0) then
      return arg[i]
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


