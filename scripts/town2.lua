-- town2 - "Randen"

-- /*
-- {
-- progress:
-- blade: Whether woman mentioned the Phoenix Blade
--   (0) You have not spoken to her
--   (1) She has now
--
-- fightonbridge:
--   (0) Have not visited the bridge, not fought
--   (1)..(3) (not used) See bridge[2].lua for more
--   (4) Defeated monster on bridge, have not slept at Inn
--   (5) Slept at inn; bridge is passable
--
-- foundmayor:
--   (0) Have not found the mayor
--   (1) Found the mayor, he does not want company
--   (2) Found mayor & used WARPSTONE to return; he awaits your company
--   (3) You have spoken to the mayor
--
-- mayorguard1:
--   (0) The first of the mayor's guards is still being held in the orc camp
--   (1) The first guard was released: gives you money
--   (2) The first guard already gave you money for releasing him
--
-- mayorguard2:
--   (0) The second of the mayor's guards is still being held in the orc camp
--   (1) The second guard was released
--
-- sidequest1: Whether you finished the 1st sidequest to seal the portal in
--               the tunnel
--   (0) Still letting monsters through, Ajathar doesn't join
--   (1) The Portal is sealed shut, Ajathar joins
--
-- sidequest2: Whether you finished the 2nd sidequest to save the mayor
--   (0) Mayor still missing
--   (1) Mayor found, Casandra joins
--
-- showbridge:
--   (0) Bridge is incomplete
--   (1) Monsters on bridge defeated; slept at Inn: bridge is passable
--   (2) Bridge is totally complete, bridge(2).(lua|map) no longer used at all
--
-- talk_ajathar:
--   (0) Have not spoken to Ajathar when you entered the map
--   (1) You have spoken to him at least once already
--
-- warpstone: The teleporter from Ajantara <-> Randen
--   (0) Have not used it yet
--   (1) Stepped on the warp stone and created its counterpart in Randen
-- }
-- */


function autoexec()
  -- Warpstone is found late in the game, so some things are now available
  -- that were not available earlier
  if progress.warpstone == 1 then
    -- Move the guard guarding the houses in the north-east section of town
    -- over one tile so we can get in
    place_ent(6, "guard")

    -- The bridge repairs will now be completed
    if progress.showbridge == 1 then
      progress.showbridge = 2
    end

    -- If the Mayor had been found, he will now have recovered from his ordeal
    -- and will be willing to speak to you
    if progress.foundmayor == 1 then
      progress.foundmayor = 2
    end
  end

  refresh()
end


function entity_handler(en)
  if (en == 0) then -- blonde woman, top-left house
    if progress.warpstone == 1 then
      bubble(en, _"Back again, are you?")
    else
      bubble(en, _"You must be an adventurer. There has been an unusual number of you people around lately.")
    end
    if progress.foundmayor > 0 then
      bubble(en, _"Oh, and thank you for finding the mayor.")
    end

  elseif (en == 1) then -- old lady, house N of Inn
    if progress.foundmayor == 0 then
      bubble(en, _"My husband is late again.")
    elseif progress.foundmayor == 1 then
      bubble(en, _"I'm getting so worried...")
      bubble(en, _"You don't think Malkaron has something to do with it?")
    elseif progress.foundmayor == 2 then
      bubble(en, _"Where is that man?")
    else
      bubble(en, _"Oh... that bad man, making me worry like this, I'm gonna kill him!")
    end

  elseif (en == 2) then -- brunette town man, south entrance greeter
    if progress.fightonbridge > 4 then
      if progress.showbridge == 2 then
        bubble(en, _"Thank you so much for restoring the trade route to the citizens of Randen!")
      else
        bubble(en, _"Welcome to Randen.")
      end
    else
      bubble(en, _"This is the town of Randen. We're mostly a coastal trade town, but with the bridge out, trade is extremely slow.")
      bubble(en, _"Do you think it has something to do with Malkaron? We've heard a lot of rumors about him as of late...")
    end

  elseif (en == 3) then -- brunette woman, red dress, N of weapon store, next to child
    if progress.foundmayor > 1 and progress.mayorguard1 > 0 and
      progress.mayorguard2 > 0 then
      bubble(en, _"My husband is so excited that everyone returned safely. He tends to get forgetful when he's like this. Sometimes he doesn't even lock up after himself on his way to work.")
      set_obs("room_5door", 0)
      set_zone ("room_5door", 0)
    elseif progress.fightonbridge > 4 then
      bubble(en, _"Good day.")
    else
      if progress.blade == 0  then
        -- PH: Just my little joke hehe
        bubble(en, _"I'm just preparing some vegetables.")
        bubble(HERO1, _"That's a strange knife you've got there.")
        bubble(en, _"What? Oh, this. Yes, it's a Phoenix Blade.")
        bubble(en, _"I found it lying about under a pile of leaves in the forest.")
        bubble(HERO1, _"Isn't there supposed to be someone out looking for that?")
        bubble(en, _"I heard that, too, but it's been ages and no-one has turned up.")
        bubble(en, _"I would give it back, but I need it for slicing carrots!")
        bubble(HERO1, _"Your secret's safe with me.")
        progress.blade = 1
      else
        bubble(en, _"Oh, goodness, I didn't make enough for company... sorry.")
      end
    end

  elseif (en == 4) then -- brunette child, blue shirt, N of weapon store, next to woman in red dress
    if progress.fightonbridge > 4 then
      if progress.showbridge == 2 then
        bubble(en, _"Wow! Mom says you're the ones who beat up the bad guys! You're so cool!")
      else
        bubble(en, _"Hi!")
      end
    else
      bubble(en, _"I'm hungry, I hope mom is done making lunch soon.")
    end

  elseif (en == 5) then -- blonde man, green shirt, SE corner of town (trees)
    if progress.fightonbridge > 4 then
      if progress.warpstone == 1 then
        bubble(en, _"Business is good.")
      else
        bubble(en, _"Now that the bridge is repaired I'm back in business!")
      end

      if (get_treasure(9) == 0) then
        bubble(en, _"I hear you had something to do with speeding along the bridge's construction. I'd like you to have this.")
        chest(9, I_B_SHOCK, 1)
      end

      if progress.showbridge < 2 then
        bubble(en, _"You should look around there to see if the workers left anything behind.")
      else
        -- If the bridge has been completed and you did not get one or more of the treasures on the incomplete bridge, he will give them to you
        if (get_treasure(8) == 0) then
          bubble(en, _"I found this Olginar Seed on the bridge. You can have it.")
          chest(8, I_OSEED, 1)
        end
        if (get_treasure(90) == 0) then
          bubble(en, _"Oh, and I found this curious artifact there as well...")
          chest(90, I_REGENERATOR, 1)
        end
      end
    else
      bubble(en, _"If you haven't already noticed, the bridge across Brayden river is gone.")
      bubble(en, _"It appears that some travelers were stopped by Malkaron's brigands on the bridge, resulting in a volley of magic that left it in cinders.")
      set_ent_script(en, "W25")
      wait_for_entity(en)
      bubble(en, _"A new bridge is supposed to be built soon. This town can't survive for long without our major trade route.")
    end

  elseif (en == 6) then -- guard, NE corner of town, guarding mayor house
    if progress.warpstone == 1 then
      if progress.foundmayor < 2 then
        bubble(en, _"Our mayor is still missing. We're not sure what to do at this point.")
      elseif progress.foundmayor == 2 then
        bubble(en, _"$0! The mayor is back now, thank you! He wants to see you right away!")
      else
        bubble(en, _"Thank you again for everything!")
      end
    else
      if progress.foundmayor == 0 then
        if progress.fightonbridge > 4 then
          bubble(en, _"The mayor has yet to return from Andra. A few of our men are out looking for him as we speak.")
        else
          bubble(en, _"The mayor was going to Andra to talk to the Council there. I think that he should have been back by now.")
          bubble(en, _"We may have to go and look for him soon. We need to make sure that he is safe.")
        end
      elseif progress.foundmayor < 2 then
        bubble(en, _"The mayor is back now, thanks to you. However, the mayor is not seeing any visitors for a while.")
        bubble(en, _"He is still recovering from his ordeal.")
      else
        -- The warp stone has NOT been activated, but you saved and talked to the mayor once already
        bubble(en, _"The mayor welcomes you to visit any time!")
        move_entity(en, "guard")
        wait_for_entity(en)
        set_ent_script(en, "F0")
        wait_for_entity(en)
      end
    end

  elseif (en == 7) then -- brunette woman, green dress, W of mayor house, by top portal
    if progress.warpstone == 1 then
      bubble(en, _"Are you enjoying your stay?")
    else
      if progress.foundmayor == 0 then
        bubble(en, _"The mayor left for Andra with an adventurer that he had just hired and his usual contingent of guards.")
        bubble(en, _"I wonder if they avoided the trouble at the bridge?")
      else
        bubble(en, _"Thank you!")
      end
    end

  elseif (en == 8) then -- brunette man, brown shirt, inn guest in SW of town
    bubble(en, _"I wonder why those adventurers were stopped at the bridge? I'm not sure I want to try crossing it just yet.")

  elseif (en == 9) then -- blonde man, green shirt, magic shop patron
    bubble(en, _"How long does it take to build a bridge?")

  elseif (en == 10) then -- brunette man, blue shirt, by cave entrance (becomes Ajathar)
    LOC_join_ajathar(en)
    refresh()

  elseif (en == 11) then -- green soldier, rescued from Ork camp, behind counter, building W of mayor
    bubble(en, _"Hey, you can't be back here! What are you trying to do, steal from me?")

  elseif (en == 12) then -- brown soldier, rescued from Ork camp, left room, building W of mayor
    if progress.mayorguard1 == 1 then
      bubble(en, _"Thanks for rescuing me back there! Here, have this:")
      set_gp(get_gp() + 1000)
      msg(_"You acquired 1000 gold pieces!")
      progress.mayorguard1 = 2
    else
      bubble(en, _"Those forces of Malkaron's sure are tough!")
    end

  elseif (en == 13) then -- blonde woman, white dress, by S entrance (becomes Casandra)
    LOC_join_casandra(en)
    refresh()

  elseif (en == 14) then -- brunette mayor, gray shirt, Mayor house in NE corner of town
    if progress.foundmayor < 2 then
      bubble(en, _"How did you get in here past my locked door?")
    elseif progress.foundmayor == 2 then
      bubble(en, _"Oh, $0, thank you for rescuing me!")
      progress.foundmayor = 3
    else
      bubble(en, _"I had a spa installed in my house! What do you think?")
    end

  end
end


function postexec()
  return
end


function refresh()
  local x, y
  showch("treasure1", -1)
  showch("treasure2", -1)

  showch("treasure3", 96)
  showch("treasure4", 3)
  showch("treasure5", 4)
  showch("treasure6", 5)
  showch("treasure7", 31)
  showch("treasure8", 46)
  showch("treasure9", 97)

  x, y = marker("travelpoint")
  if progress.warpstone == 1 then
    set_mtile(x, y, 0)
  else
    set_zone (x, y, 0)
  end

  -- This NPC will only appear if you spoke with him in the camp
  if progress.mayorguard1 == 0 then
    set_ent_active(12, 0)
  end

  -- Ditto, plus make sure you cannot speak to a "ghost" over the counter
  if progress.mayorguard2 == 0 then
    x, y = get_ent_tile(11)
    set_ent_active(11, 0)
    set_zone (x, y + 1, 0)
  end

  if (not LOC_manor_or_party(AJATHAR)) then
    -- Make the NPC look like Ajathar if he has not been recruited yet
    set_ent_id(10, AJATHAR)
  else
    -- Otherwise, remove him from screen
    set_ent_active(10, 0)
  end

  if progress.foundmayor > 0 then
    if not LOC_manor_or_party(CASANDRA) then
      -- Casandra should be available to join your party
      set_ent_id(13, CASANDRA)
    else
      set_ent_active(13, 0)
    end

    if progress.showbridge > 1 then
      set_ent_active(8, 0)
      set_ent_active(9, 0)
    end

    x, y = marker("mayor_o");
    set_obs(x, y - 1, 0)
  end

end


-- Show the status of a treasures
function showch(which_marker, which_chest)

  -- /*
  -- The zones 'which_chest == -1' means that there is no "chest" where the
  -- current treasure is found, but is something like a flower patch that
  -- can't be walked upon.  If those treasures have been found, remove the
  -- obstacle at that location, as well as the triggering zone.
  -- */

  if (which_chest < 0) then
    -- Evaluate the chest-less treasure at this location
    set_obs(which_marker, 0)

    -- We have already found the "treasure chest-less" secret treasure here,
    -- so get rid of the 'activating zone' for it
    set_zone(which_marker, 0)
  elseif (get_treasure(which_chest) == 1) then
    set_mtile(which_marker, 265)

    -- We have already collected this treasure,
    -- so get rid of the 'activating zone' for it
    set_zone(which_marker, 0)
  end
end


function zone_handler(zn)
  if (zn == 1) then
    change_map("cave1", "exit")

  elseif (zn == 2) then
    door_in("room_1i")

  elseif (zn == 3) then
    door_in("room_3i")

  elseif (zn == 4) then
    door_in("room_2i")

  elseif (zn == 5) then
    door_in("room_4i")

  elseif (zn == 6) then
    door_in("inn_i")

  elseif (zn == 7) then
    door_in("shop_1i")

  elseif (zn == 8) then
    door_in("shop_4i")

  elseif (zn == 9) then
    bubble(HERO1, _"Locked.")

  elseif (zn == 10) then
    door_out("room_1o")

  elseif (zn == 11) then
    door_out("room_3o")

  elseif (zn == 12) then
    door_out("room_2o")

  elseif (zn == 13) then
    door_out("room_4o")

  elseif (zn == 14) then
    door_out("inn_o")

  elseif (zn == 15) then
    door_out("shop_1o")

  elseif (zn == 16) then
    door_out("shop_4o")

  elseif (zn == 17) then
    -- /* PH added code to check if you do stay over night.
    --    This is done indirectly; if your GP goes down it
    --    means you must have slept here. */
    local old_gp = get_gp()
    inn("Wayside Inn", 30, 1)
    -- This means you MUST stay at the inn before the bridge gets repaired.
    if (get_gp() < old_gp) then
      if progress.fightonbridge == 4 then
        progress.fightonbridge = 5
        progress.showbridge = 1
      end
    end

  elseif (zn == 18) then
    shop(4)

  elseif (zn == 19) then
    shop(5)

  elseif (zn == 20) then
    warp("hall_stairs1", 8)

  elseif (zn == 21) then
    warp("room_5i", 8)

  elseif (zn == 22) then
    warp("room_4stairs", 8)

  elseif (zn == 23) then
    warp("hall_stairs2", 8)

  elseif (zn == 24) then
    chest(3, I_KNIFE2, 1)
    refresh()

  elseif (zn == 25) then
    chest(4, 0, 250)
    refresh()

  elseif (zn == 26) then
    chest(5, I_SALVE, 1)
    refresh()

  elseif (zn == 27) then
    chest(7, I_PURITYGEM, 1)
    refresh()

  elseif (zn == 28) then
    change_map("main", "town2")

  elseif (zn == 29) then
    chest(10, I_MHERB, 1)
    refresh()

  elseif (zn == 30) then
    book_talk(party[0])

  elseif (zn == 31) then
    chest(11, I_LTONIC, 1)
    refresh()

  elseif (zn == 32) then
    book_talk(party[0])

  elseif (zn == 33) then
    bubble(HERO1, _"Hmmm... books about herbs.")

  elseif (zn == 34) then
    if progress.foundmayor > 1 then
      door_in("mayor_i")
    else
      bubble(HERO1, _"Locked.")
    end

  elseif (zn == 35) then
    bubble(HERO1, _"Inns always have boring books.")

  elseif (zn == 36) then
    change_map("town6", "travelpoint")

  elseif (zn == 37) then
    touch_fire(party[0])

  elseif (zn == 38) then
    warp("shop_4hidden", 8)

  elseif (zn == 39) then
    warp("room_5door", 8)

  elseif (zn == 40) then
    chest(31, 0, 150)
    refresh()

  elseif (zn == 41) then
    chest(46, I_STRSEED, 2)
    refresh()

  elseif (zn == 42) then
    chest(96, 0, 250)
    set_obs("treasure3", 0)
    refresh()

  elseif (zn == 43) then
    if progress.mayorguard2 > 0 then
      bubble(11, _"Thank you for rescuing me!")
    end

  elseif (zn == 44) then
    LOC_door("door_guard1")

  elseif (zn == 45) then
    LOC_door("door_guard2")

  elseif (zn == 46) then
    chest(97, 0, 1500)
    refresh()

  elseif (zn == 47) then
    door_in("shop_2i")

  elseif (zn == 48) then
    door_out("shop_2o")

  elseif (zn == 49) then
    door_in("shop_3i")

  elseif (zn == 50) then
    door_out("shop_3o")

  elseif (zn == 51) then
    door_out("mayor_o")

  end
end


function LOC_door(which_marker)
  sfx(25)

  local x, y = marker(which_marker)
  set_btile(x, y, 0)
  set_mtile(x, y + 1, 519)
  set_ftile(x, y, 518)
  set_zone(x, y, 0)
  set_obs(x, y, 0)
end


function LOC_join_ajathar(en)
  local id
  local hero = 9

  if progress.sidequest1 == 0 then
    if progress.talk_ajathar == 0 then
      bubble(HERO1, _"Hello! You haven't ventured very far!")
      bubble(en, _"I have been maintaining a constant prayer vigil at this point. It should discourage the monsters from emerging into the town.")
      bubble(HERO1, _"And what if they DO come out?")
      bubble(en, _"My training also included swordsmanship.")
      progress.talk_ajathar = 1
    elseif progress.talk_ajathar == 1 then
      bubble(en, _"I hope I am doing the right thing here.")
    end
    return
  end

  if progress.talk_ajathar == 0 then
    bubble(en, _"Be careful! That tunnel is infested with monsters.")
    bubble(HERO1, _"Fortunately, I have been successful in closing the Portal. No more monsters will trouble us now.")
    progress.talk_ajathar = 1
  else
    bubble(HERO1, _"You can rest easy now. I have closed the Portal that let the monsters through.")
  end
  bubble(en, _"Great! Can I offer my services?")

  -- Give Ajathar his default equipment
  set_all_equip(AJATHAR, I_MACE2, I_SHIELD1, I_HELM1, I_ROBE2, I_BAND1, 0)
  id = select_team{AJATHAR}
  -- Add the characters that were not selected to the manor
  add_to_manor(id)

  if (id[1]) then
    local script = "L1U2L2U1K"
    set_ent_id(en, id[1])
    set_ent_script(en, "L1U1"..script)

    if (id[2]) then
      -- Two heroes were de-selected
      set_ent_id(hero, id[2])
      set_ent_active(hero, 1)

      local x, y = get_ent_tile(en)
      place_ent(hero, x, y + 1)
      bubble(en, _"If you need us, we'll be back at the manor.")
      set_ent_speed(hero, 4)
      set_ent_speed(en, 4)
      set_ent_script(hero,  "L1U2"..script)
      wait_for_entity(hero, en)
    else
      -- One hero was de-selected
      bubble(en, _"If you need me, I'll be back at the manor.")
      wait_for_entity(en, en)
    end
  end
  set_ent_active(hero, 0)
  set_ent_active(en, 0)
  progress.players = progress.players + 1
end


function LOC_join_casandra(en)
  local id
  local hero = 8

  bubble(en, _"$0, thank you for helping us escape from the orc's camp!")
  bubble(HERO1, _"Don't mention it. How did you get caught up in that mess anyway?")
  bubble(en, _"Well, like I mentioned earlier, the mayor needed a bodyguard while he delivered an urgent message to some guy named Tsorin in Andra.")
  bubble(en, _"That's when we were caught by the orcs. The Mayor ate the note when we were under attack. I have no idea what it was about.")
  bubble(HERO1, _"Hmm, still sounds a bit fishy.")
  bubble(en, _"I'd gladly join you to find out what this is all about!")

  -- Give Casandra her default equipment
  set_all_equip(CASANDRA, I_MACE2, I_SHIELD1, I_HELM1, I_ROBE2, I_BAND1, 0)
  id = select_team{CASANDRA}
  -- Add the characters that were not selected to the manor
  add_to_manor(id)

  if (id[1]) then
    set_ent_id(en, id[1])
    set_ent_script(en, "U8K")

    if (id[2]) then
      -- Two heroes were de-selected
      set_ent_id(hero, id[2])
      set_ent_active(hero, 1)

      local x, y = get_ent_tile(en)
      place_ent(hero, x, y - 1)
      bubble(en, _"If you need us, we'll be back at the manor.")
      set_ent_speed(hero, 4)
      set_ent_speed(en, 4)
      set_ent_script(hero,  "U9K")
      wait_for_entity(hero, en)
    else
      -- One hero was de-selected
      bubble(en, _"If you need me, I'll be back at the manor.")
      wait_for_entity(en, en)
    end
  end
  set_ent_active(hero, 0)
  set_ent_active(en, 0)
  progress.players = progress.players + 1
end
