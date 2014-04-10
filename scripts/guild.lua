-- guild - "Home of the Embers in Sunarin"
-- /*
-- P_GUILDSECRET - Have we discovered the secret passage into the Embers' base?
--   (0) Not found it
--   (1) Found it and it's open
--   (2) Found it, gone through, and it's shut again (it will reopen as we approach)
-- P_FOUGHTGUILD - Have we fought the bandits for the helmet?
--   (0) Have not fought
--   (1) Fought and beat them
--   (2) Lose the key to get back in (when leaving)
-- P_OPALHELMET - Did you obtain it?
--   (0) No
--   (1) Yes
-- */
function autoexec()
  refresh()
end


function entity_handler(en)
  if (en == 2) then -- You have met Ayla
    LOC_ayla_join(en)
  end
end


function postexec()
  return
end


function refresh()
  local x, y

  x, y = marker("ustairs1")
  if (get_progress(P_GUILDSECRET) == 1) then
    set_obs  (x + 2, y - 1, 0)
    set_zone (x + 2, y - 1, 0)
    set_mtile(x + 2, y - 2, 0)
    set_mtile(x + 2, y - 1, 0)

    set_ftile(x + 3, y - 2, 242)
    set_ftile(x + 3, y - 1, 241)

    set_obs  (x + 4, y - 1, 1)
    set_zone (x + 4, y - 1, 4)
    set_ftile(x + 4, y - 2, 242)
    set_ftile(x + 4, y - 1, 243)
  elseif (get_progress(P_GUILDSECRET) == 2) then
    set_obs  (x + 2, y - 1, 1)
    set_mtile(x + 2, y - 2, 242)
    set_mtile(x + 2, y - 1, 241)

    set_ftile(x + 3, y - 2, 0)
    set_ftile(x + 3, y - 1, 0)

    set_obs  (x + 4, y - 1, 0)
    set_zone (x + 4, y - 1, 0)
    set_ftile(x + 4, y - 2, 0)
    set_ftile(x + 4, y - 1, 0)
  end

  if (get_progress(P_FOUGHTGUILD) > 0) then
    set_ent_active(0, 0)
    set_ent_active(1, 0)
  end

  if (get_progress(P_OPALHELMET) > 0) then
    set_mtile("helm", 265)
    set_zone("helm", 0)
  end

  -- Should Ayla appear or not?
  if (LOC_manor_or_party(AYLA) or get_progress(P_FOUGHTGUILD) < 1) then
    set_ent_active(2, 0)
  else
    set_ent_active(2, 1)
    set_ent_id(2, AYLA)
  end

  -- I know this is supposed to check inside the individual if..thens to be
  -- proper, but figure we will call it for the remaining treasures too.
  x, y = marker("treasure")
  if (get_treasure(101) == 1) then
    set_mtile(x - 1, y - 1, 265)
  end

  if (get_treasure(102) == 1) then
    set_mtile(x, y - 1, 265)
  end

  if (get_treasure(103) == 1) then
    set_mtile(x + 1, y - 1, 265)
  end

  if (get_treasure(104) == 1) then
    set_mtile(x, y + 1, 265)
  end

  if (get_treasure(105) == 1) then
    set_mtile(x + 1, y + 1, 265)
  end
end


-- Updated 20020929 PH
-- Added test for P_OPALHELMET so
-- that you can re-enter the guild
-- if you do not have the helmet yet
function zone_handler(zn)
  if (zn == 1) then
    if (get_progress(P_OPALHELMET) == 1) then
      set_progress(P_FOUGHTGUILD, 2)
    end
    change_map("town5", "guild_door")

  elseif (zn == 2) then
    warp("dstairs1", 8)

  elseif (zn == 3) then
    if (get_progress(P_GUILDSECRET) == 0) then
      bubble(HERO1, _"Well I'll be...")
      bubble(HERO1, _"The book really IS called 'How to Enter the Ember's Secret Hideout'. Heh... brilliant in its simplicity, I suppose.")
      set_progress(P_GUILDSECRET, 1)
      sfx(26)
      refresh()
    end

  elseif (zn == 4) then
    if (party[0] == AYLA) then
      bubble(HERO1, _"I'm a thief... but even I wouldn't read this garbage!")
    else
      bubble(HERO1, _"Thieves read?!")
    end

  elseif (zn == 5) then
    chest(47, 0, 500)
    refresh()

  elseif (zn == 6) then
    warp("ustairs2", 8)

  elseif (zn == 7) then
    warp("dstairs2", 8)

  elseif (zn == 8) then
    chest(50, I_SWORD4, 1)
    refresh()

  elseif (zn == 9) then
    bubble(HERO1, _"These look interesting.")

  elseif (zn == 10) then
    LOC_hidden_door()

  elseif (zn == 11) then
    if (get_progress(P_FOUGHTGUILD) == 0) then
      LOC_fight()
    end

  elseif (zn == 12) then
    LOC_get_helm()

  elseif (zn == 13) then
    warp("ustairs1", 8)

  elseif (zn == 14) then
    touch_fire(party[0])

  elseif(zn == 15) then
    -- Close the secret door
    sfx(26)
    set_progress(P_GUILDSECRET, 2)
    refresh()

  elseif (zn == 16) then
    -- Open the secret door (a second time)
    sfx(26)
    set_progress(P_GUILDSECRET, 1)
    refresh()

  elseif (zn >= 17 and zn <= 21) then
    -- Gold! Between 170 and 210 GP, depending on the zone number
    chest(101 + zn - 17, 0, zn * 10)

  elseif (zn == 22) then
    if (party[0] == Ayla or party[1] == Ayla) then
      local hero
      if (party[0] == Ayla) then
        hero = HERO1
      else
        hereo = HERO2
      end
      bubble(hero, _"I can't seem to pick this lock.")
    else
      bubble(en, _"The lock on this door looks very strange.")
    end

  elseif (zn == 23) then
    -- This will lead to a room with a TravelPoint which leads to cave1
    warp("dstairs3", 8)

  elseif (zn == 24) then
    warp("dstairs4", 8)

  elseif (zn == 25) then
    bubble(HERO1, _"This note says that there is a portal behind the fireplace.")

  elseif (zn == 26) then
    warp("fireplace2", 8)

  elseif (zn == 27) then
    warp("fireplace1", 8)

  elseif (zn == 28) then
    change_map("cave1", "dstairs1")

  end
end


function LOC_ayla_join(en)
  if (get_progress(P_AYLA_QUEST) == 0) then
    -- This code creates an unsolvable catch-22. Probably to prevent trapping the player in an unfinished quest.
    bubble(en, _"Wha...? Oh, it's you!")
    bubble(HERO1, _"Hello... I recognise you from Nostik's manor, don't I?")
    bubble(en, _"Yes, I broke into the house, but I couldn't find the secret passage.")
    bubble(HERO1, _"You must be pretty good at... uh...")
    bubble(en, _"Thievery? Yea, I am.")
    bubble(HERO1, _"Well, nice meeting you.")
    bubble(en, _"I would join you, but my quest isn't written yet.")
    bubble(HERO1, _"Oh. Ok. Perhaps when this game is finished?")
    bubble(en, _"Yeah, probably then.")
    bubble(HERO1, _"OK.")
  else
    bubble(en, _"Hey, $0. Rumor has it that the guild has a bunch of treasure hoarded somewhere.")
    bubble(HERO1, _"That's possible. Why?")
    bubble(en, _"Let's just suppose that you help me find it. I wouldn't mind joining your little party and helping you out.")
    bubble(HERO1, _"Well, I can always use a little extra help. What do we have to do?")
    bubble(en, _"Let's have a talk around town. I'm sure someone's bound to spill something about the guild's whereabouts.")
    bubble(HERO1, _"Well, alright then.")
    set_ent_active(en, 0)
    set_all_equip(AYLA, I_SWORD4, I_SHIELD3, I_CAP3, I_SUIT3, I_BAND2, 0)
    id = select_team{AYLA}
    --  Add the characters that were deselected to the manor
    add_to_manor(id)

    if (id[1]) then
      -- Need the whole "okay, meet you back at the manor" dialog here
    end
  end
end


function LOC_fight()
  local a, b
  local x, y = marker("fight")

  if (get_progress(P_FOUGHTGUILD) ~= 0) then
    return
  end

  a = get_vx()
  b = get_vy()
  move_camera(816, 288, 2)
  bubble(HERO1, _"Ooh... a chest! But who are these clowns?")
  wait(50)
  bubble(0, _"I'll follow the others to the Coliseum and you wait here for the Guildmaster to return from his visit.")
  bubble(1, _"Yes sir! I'll...")
  bubble(0, _"Who is that?")
  set_ent_facing(1, FACE_RIGHT)
  drawmap()
  screen_dump()
  move_camera(a, b, 1)
  drawmap()
  screen_dump()
  bubble(0, _"Guards!")
  bubble(HERO1, _"Oh oh!")
  wait(50)

  set_ftile(x, y - 1, 369)
  set_ftile(x - 1, y, 372)
  set_ftile(x + 1, y, 371)
  set_ftile(x, y + 1, 370)
  wait(50)
  bubble(HERO1, _"What the...")
  drawmap()
  screen_dump()
  set_run(0)
  combat(26)
  set_run(1)
  if (get_alldead() == 1) then
    return
  end
  set_progress(P_FOUGHTGUILD, 1)
  set_ftile(x, y - 1, 0)
  set_ftile(x - 1, y, 0)
  set_ftile(x + 1, y, 0)
  set_ftile(x, y + 1, 0)
  refresh()
  drawmap()
  screen_dump()
  wait(50)
  bubble(HERO1, _"Those guys are gone!")
  bubble(HERO1, _"But they left the chest behind.")
end


function LOC_get_helm()
  if (get_progress(P_OPALHELMET) == 0) then
    sfx(5)
    msg(_"Opal Helmet procured", 255, 0)
    set_progress(P_OPALHELMET, 1)
    add_special_item(SI_OPALHELMET)
    refresh()
    drawmap()
    screen_dump()
    wait(50)
    bubble(HERO1, _"I don't like this.",
                 _"It was just too easy.")
  end
end


function LOC_hidden_door()
  if (party[0] == Ayla or party[1] == Ayla) then
    local a, b
    local x, y = marker("hidden")
    if (party[0] == Ayla) then
      a = HERO1
      b = HERO2
    else
      a = HERO2
      b = HERO1
    end

    bubble(a, _"The door is just a decoy.")
    move_entity(a, x, y, 0)
    move_entity(b, x, y + 1, 0)
    set_ent_obsmode(a, 0)
    set_ent_obsmode(b, 0)
    wait_for_entity(a, b)
    set_ent_obsmode(a, 1)
    set_ent_obsmode(b, 1)

    sfx(26)
    set_ent_facing(a, FACE_RIGHT)
    bubble(a, _"There we go...")

    set_obs(x + 1, y, 0)
    set_obs(x + 2, y - 2, 0)
    set_zone(x, y - 1, 0)
  else
    bubble(HERO1, _"I can't get in this. I may need someone skilled at picking locks to get inside.")
  end
end
