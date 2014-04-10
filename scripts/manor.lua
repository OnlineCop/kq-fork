-- manor - "Nostik's manor southwest of Ekla"

-- /*
-- {
-- progress:
-- manor: Status of conversations when you are in the manor
--   (0) New game; you haven't spoken to the butler yet
--   (1) Nostik further explained your mission; you haven't spoken to butler
--   (2) Butler has spoken to you, or you have no new recruits yet
--   (3) You've recruited others and they're waiting around the table
--
-- players: Number of recruits in your party
--   (0) You haven't recruited anyone yet
--   (1)..(7) You have others in your party
--
-- manorparty[0-7]: Which recruits have joined your party
--   (0)..(8) Whether this character has been recruited into your party
--
--
-- Details:
--
-- When the game starts, Nostik debriefs everybody and offers to go into detail
-- for whoever wants to listen.  Everyone else leaves immediately (walks out
-- the door) and you're left at the table.
--
-- Before you can leave yourself, the butler, Hunert, will stop to talk to you
-- and fill you in, as well as give you some money to start the quest.
--
-- When you return to the manor, the dining room should be empty unless you
-- have 'recruited' other party members into your team.  If you have, then
-- those members are sitting in their places around the table.  When you speak
-- to Hunert, the butler, he will allow you to choose your party members and
-- whether they will lead or follow in the 2-person party.
--
-- Quick reference:
--
--  SENSAR   0
--  SARINA   1
--  CORIN    2
--  AJATHAR  3
--  CASANDRA 4
--  TEMMIN   5
--  AYLA     6
--  NOSLOM   7
--  Nostik   8
--  Hunert   9
-- }
-- */


function autoexec()
  if progress.players > 0 then
    progress.manor = 3
  end

  if progress.manor == 0 then
    -- New game; Nostik has not explained the quest yet
    LOC_setup_newgame()
  elseif progress.manor == 1 then
    -- You spoke to Nostik about the purpose of your mission
  elseif progress.manor == 2 then
    -- You already talked to Hunert, but you have no recruits
    for a = 0, 7, 1 do
      set_ent_active(a, 0)
    end
  elseif progress.manor == 3 then
    -- You have recruited at least 1 other party member
    LOC_at_table()

    -- Put Nostik to bed (he is old and feeble)
    place_ent(8, "bed")
    set_ent_facing(8, FACE_DOWN)
  end
end


function entity_handler(en)
  -- You are talking to other party members
  if (get_ent_id(en) == SENSAR) then
    bubble(en, _"I would be useful to you, since I can use Rage in battle.")
  elseif (get_ent_id(en) == SARINA) then
    bubble(en, _"In battle, I can attack multiple targets at once if I'm equipped with the right weapon.")
  elseif (get_ent_id(en) == CORIN) then
    bubble(en, _"I can infuse weapons with magic during battle.")
  elseif (get_ent_id(en) == AJATHAR) then
    bubble(en, _"I notice that chanting a prayer during battle can heal your party or dispells the undead monsters.")
  elseif (get_ent_id(en) == CASANDRA) then
    bubble(en, _"I can use my Boost ability to strengthen spells when I am attacking.")
  elseif (get_ent_id(en) == TEMMIN) then
    bubble(en, _"I am very protective of my team members and will take a beating in their place.")
  elseif (get_ent_id(en) == AYLA) then
    bubble(en, _"I'm a thief by trade. You might be surprised what you can steal from enemies!")
  elseif (get_ent_id(en) == NOSLOM) then
    bubble(en, _"I have a very keen eye. Not even enemies can hide their weaknesses from me!")

  -- Nostik
  elseif (en == 8) then
    LOC_explain_mission3(en)

  -- Butler Hunert
  elseif (en == 9) then
    if progress.manor == 0 or progress.manor == 1 then
      bubble(en, _"Ah yes, Master Nostik asked me to give you this.")
      LOC_talk_butler(en)
    elseif progress.manor == 2 then
      bubble(en, _"Books are an amazing source of knowledge. Nostik has spent many years writing his own.")
    elseif progress.manor >= 3 then
      bubble(en, _"Welcome back, $0. The others are here waiting for you.")
      bubble(en, _"You can exchange your party members here.")

      -- PH, this is where your script comes in?
      select_manor()
      LOC_at_table()
    end
  end

end


function postexec()
  local en = 8
  local x, y = marker("entrance")
  if progress.manor == 0 then
    rest(200)

    bubble(en, _"Alright everyone, I welcome you. Let me get right to the major points of why you're here.")
    LOC_explain_mission(en)

    bubble(en, _"For fear of going out and being spotted, I will stay here if you need anything.")
    bubble(en, _"Good luck all of you.")

    -- TT: make everyone else walk out the door
    move_entity(SENSAR,   x - 4, y, 1)
    move_entity(SARINA,   x - 4, y, 1)
    move_entity(CORIN,    x - 4, y, 1)
    move_entity(AJATHAR,  x - 4, y, 1)
    move_entity(CASANDRA, x - 4, y, 1)
    move_entity(TEMMIN,   x - 4, y, 1)
    move_entity(AYLA,     x - 4, y, 1)
    move_entity(NOSLOM,   x - 4, y, 1)
    for a = 0, 7, 1 do
      set_ent_speed(a, 4)
    end

    wait_for_entity(0, 7)

    bubble(en, _"When you are ready to go, talk to Hunert and he will get you started on your journey.")
  end
end


function zone_handler(zn)
  -- Front door
  if (zn == 1) then
    change_map("main", "manor")

  -- Stairs going up
  elseif (zn == 2) then
    warp("dstairs", 8)

  -- Stairs going down
  elseif (zn == 3) then
    warp("ustairs", 8)

  -- Doors, duh
  elseif (zn == 4) then
    bubble(HERO1, _"Locked.")

  -- Bookshelves
  elseif (zn == 5) then
    bubble(HERO1, _"Wow! This guy reads weird stuff.")

  -- In front of exit
  elseif (zn == 6) then
    local en = 9

    if progress.manor == 0 or progress.manor == 1 then
      local x, y = get_ent_tile(HERO1)
      bubble(en, _"Hey! Hold on!")

      -- Turn around, see who is yelling
      set_ent_script(HERO1, "U1")
      wait_for_entity(HERO1, HERO1)
      bubble(HERO1, _"Huh?")

      -- Butler running speed
      set_ent_speed(en, 5)

      -- Check location on map
      set_ent_script(en, "R3D3L5D8")
      wait_for_entity(en, en)

      move_entity(en, x, y - 1, 0)

      -- Process movement script
      wait_for_entity(en, en)
      set_ent_facing(en, FACE_DOWN)

      -- Butler normal speed
      set_ent_speed(en, 3)

      bubble(en, _"It might be foolish to leave without hearing what I have to say.")
      bubble(en, _"First, Nostik gives you this.")

      -- TT: Added the (en) so the text bubble correctly displays
      LOC_talk_butler(en)
    end
  -- Search fireplaces
  elseif (zn == 7) then
    touch_fire(party[0])
  end
end




-- Decide who should be sitting around the table
function LOC_at_table()
  local id, a
  for a = 0, 7 do
    -- You have not recruited this person into your team
    id = progress['manorparty' .. a] - 1
    if (id == get_pidx(0)) then
      id = -1
    end
    if (get_numchrs() == 2 and id == get_pidx(1)) then
      id = -1
    end
    if (id < 0) then
      -- Remove entity from the map
      set_ent_active(a, 0)
    else
      -- Place around the table
      set_ent_active(a, 1)
      set_ent_chrx(a, 255)
      set_ent_id(a, id)
      set_ent_obsmode(a, 1)
    end
  end
end


function LOC_explain_mission(en)
    local a
    a = prompt(en, 3, 1, _"Do you want the long version or the short version?",
                         _"  long",
                         _"  short",
                         _"  neither")
    if (a == 0) then
      -- long
      bubble(en, _"The world is in an upheaval right now. Malkaron is a military general who, quite suddenly, became unstoppable. His forces were practically invincible. The world was plagued with his destruction. Then just a few months ago, Malkaron withdrew and all this frenzy stopped. He and his armies seemed to have just vanished.")
      bubble(en, _"Peace only lasted only until now, when monsters have begun to appear everywhere. They kill livestock, attack towns and ransack villages. They are looking for something.")
      wait(75)
      bubble(en, _"The fact is, I had, until recently, been an advisor to him in his army. He is furious with me, however, and he's sent out scouts to find me.")
      bubble(en, _"Malkaron had a magical staff which gave him and his armies total invincibility. It turned the weapons and armour of his armies practically indestructible, merely by being in close proximity to the staff. That is why his army was unstoppable.")
      bubble(en, _"Naturally, he kept this a well-guarded secret, but since he carried this staff with him everywhere his armies went, I began to piece the clues together. Malkaron became suspicious, and sent me to the dungeon.")
      LOC_explain_mission2(en)
    elseif (a == 1) then
      -- short
      bubble(en, _"Malkaron is a general whose armies became invincible. They practically tore the world apart until just recently.")
      bubble(en, _"Their forces unexpectedly withdrew a few months ago and it's been peaceful until now.")
      bubble(en, _"It appears that there are monsters springing up everywhere, looking for something.")
      bubble(en, _"Malkaron had a magical staff that made his armies' weapons and armour practically indestructible by mere proximity to it.")
      bubble(en, _"He found out that I knew about this `little secret', and threw me into the dungeon.")
      LOC_explain_mission2(en)
    else
      bubble(en, _"Fine, I'll tell you the minimal amount. Malkaron captured me.")
    end

    bubble(en, _"I escaped by use of my magic, and now I am here.")
end


function LOC_explain_mission2(en)
    wait(75)
    bubble(en, _"This is where you come in. I believe that Malkaron has lost the staff and is now looking for it. Those monsters are his scouts.")
    bubble(en, _"If this is true, I need your help to find it before he does.")
    bubble(en, _"Each of you have a different skill that I believe will be very beneficial to your search.")
    bubble(en, _"You will need to stop the monsters. They are Malkaron's eyes and ears, and if they find the staff before you do, we may be in trouble again.")
    wait(50)
end


function LOC_explain_mission3(en)
  if progress.manor == 0 then
    if (prompt(en, 2, 1, _"Do you need me to explain it again?",
                         _"  yes",
                         _"  no") == 0) then
      LOC_explain_mission(en)
      bubble(en, _"I hope this helps you have a better understanding of what's going on.")
    else
      bubble(HERO1, _"No, I think I get it. But why did you choose me?")
      bubble(en, _"As you may have noticed, you're not the only one. We've been friends a long time, $0, and I know I can trust you with confidence.")
      bubble(HERO1, _"Oh. I thought you selected me because of my brains or because my magic skills were more finely honed than anyone else's...")
      bubble(en, _"Heh-heh. You DO have certain skills which I'm counting on will help you out on your mission, but so do the others, so don't get a big head.")
      bubble(HERO1, _"Yea, yea. So why didn't you just get everybody into one big group and just let all of us go out and find this staff?")
      bubble(en, _"Groups attract attention. If you see a group of three or more people wandering around, you begin to suspect something's up. That's exactly what you'd want to avoid, if you ever want to stay ahead of Malkaron and his minions.")
      bubble(HERO1, _"Oh yea, I hadn't thought of that.")
      progress.manor = 1
    end
    wait(50)
    LOC_explain_mission3(en)
  elseif progress.manor == 1 then
    bubble(en, _"You should be going. Talk to Hunert before you go. He can help start you on your way.")
  elseif progress.manor == 2 then
    bubble(en, _"Good luck, $0.")
  elseif progress.manor == 3 then
    bubble(en, _"Zzz... zzz... zzz...")
  else
    bubble(en, _"Mine aren't the only books on the Staff of Xenarum and other treasures.")
  end
end


function LOC_setup_newgame()
  local a
  local x, y = get_ent_tile(get_pidx(0))

  -- Set up entities 0-7 in manor.map as your team members
  for a = 0, 7, 1 do
    set_ent_active(a, 1)
    set_ent_id(a, a)
    set_ent_obsmode(a, 1)
    set_ent_chrx(a, 255)
  end

  -- Remove the NPC that looks like you, from the map
  set_ent_active(get_pidx(0), 0)

  -- Set the REAL hero in the old NPCs place
  place_ent(HERO1, x, y)

  -- Set your facing direction
  set_ent_facing(HERO1, get_ent_facing(get_pidx(0)))

  -- Center map on your character coords
  calc_viewport(1)

end


function LOC_talk_butler(en)
  drawmap()
  screen_dump()
  sfx(6)
  msg(_"You've acquired 200 gp!", 255, 0)
  set_gp(get_gp() + 200)
  drawmap()
  screen_dump()
  bubble(en, _"Since there are so many monsters wandering around, you may be attacked at random.")
  bubble(HERO1, _"You mean monsters will go on a wild rampage and just start gouging me for no reason?")
  bubble(en, _"Well, yes. That's a good way of putting it.")
  bubble(HERO1, _"That will sure make it hard to sleep at night.")
  bubble(en, _"Heh heh. Maybe so. Try sleeping in a town or village inn. Monsters avoid populated places for some reason.")
  bubble(en, _"Here, I'll put a monster repellant on you that will last long enough for you to make it into town.")
  msg(_"Hunert sprinkles some strong-scented oils on you.", 255, 0)
  bubble(HERO1, _"Whew! That reeks! Why does it have to smell so bad?")
  msg(_"Hunert shrugs", 255, 0)
  bubble(en, _"Hopefully, it will give you a chance to buy some better weapons and armour.")
  bubble(HERO1, _"Well, thank you for the information.")
  progress.manor = 2
end
