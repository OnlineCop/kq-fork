-- town7 - "Pulcannen"

-- // P_TRAVELPOINT: Whether we've just come through the TravelPoint

function autoexec()

  if ((get_progress(P_TALKOLDMAN) == 0) and
      (get_progress(P_OPALHELMET) > 0) and
      (get_progress(P_OPALSHIELD) > 0) and
      (get_progress(P_OPALBAND) > 0) and
      (get_progress(P_OPALARMOUR) > 0)) then
    set_progress(P_TALKOLDMAN, 1)
  end

  if (get_progress(P_TRAVELPOINT) == 1) then
    set_progress(P_TRAVELPOINT, 0)
  end

  refresh()
end


function entity_handler(en)
  if (en == 0) then
    bubble(en, _"You'll find great things here.")

  elseif (en == 1) then
    bubble(en, _"Things here are rather pricey.")

  elseif (en == 2) then
    bubble(en, _"It's quite peaceful here.", "I like it.")

  elseif (en == 3) then
    bubble(en, _"I love the sea.")

  elseif (en == 4) then
    bubble(en, _"Nothing exciting ever happens here.")

  elseif (en == 5) then
    bubble(en, _"This town does not get a lot of visitors.")

  elseif (en == 6) then
    LOC_talk_oldman(en)

  end
end


function postexec()
  return
end


function refresh()
  showch("treasure1", 71)
  showch("treasure2", 72)

  LOC_setup_oldman(6)
end


-- Show the status of a treasures
function showch(which_marker, which_chest)
  -- Set tiles if chest already opened
  if (get_treasure(which_chest) == 1) then
    set_zone(which_marker, 0)
  end

  -- Only treasure1 needs to keep its obstacle setting
  if (which_marker ~= "treasure1") then
    set_obs(which_marker, 0)
  end
end


function zone_handler(zn)
  if (zn == 1) then
    change_map("main", "town7")

  elseif (zn == 2) then
    door_in("shop_1i")

  elseif (zn == 3) then
    door_in("shop_2i")

  elseif (zn == 4) then
    door_in("shop_3i")

  elseif (zn == 5) then
    door_in("room_1i")

  elseif (zn == 6) then
    door_in("room_2i")

  elseif (zn == 7) then
    door_out("shop_1o")

  elseif (zn == 8) then
    door_out("shop_2o")

  elseif (zn == 9) then
    door_out("shop_3o")

  elseif (zn == 10) then
    door_out("room_1o")

  elseif (zn == 11) then
    door_out("room_2o")

  elseif (zn == 12) then
    if (get_progress(P_TALKOLDMAN) > 0) then
      door_in("counter_i")
    else
      bubble(HERO1, _"Locked.")
    end

  elseif (zn == 13) then
    shop(19)

  elseif (zn == 14) then
    shop(20)

  elseif (zn == 15) then
    shop(21)

  elseif (zn == 16) then
    inn(_"Pulcannen Inn", 200, 1)

  elseif (zn == 17) then
    warp("dstairs1", 8)

  elseif (zn == 18) then
    warp("dstairs2", 8)

  elseif (zn == 19) then
    warp("ustairs1", 8)

  elseif (zn == 20) then
    warp("ustairs2", 8)

  elseif (zn == 21) then
    chest(71, I_SALVE, 1)

  elseif (zn == 22) then
    chest(72, I_MPUP, 1)
    refresh()

  elseif (zn == 23) then
    book_talk(party[0])

  elseif (zn == 24) then
    touch_fire(party[0])

  elseif (zn == 25) then
    door_out("counter_o")

  elseif (zn == 26) then
    entity_handler(6)

  elseif (zn == 27) then
    bubble(HERO1, _"This door appears locked.")

  elseif (zn == 28) then
    bubble(HERO1, _"This door appears locked.")

  elseif (zn == 29) then
    bubble(HERO1, _"Locked.")

  elseif (zn == 30) then
    bubble(HERO1, _"This barrel is filled with rainwater.")

  elseif (zn == 31 or zn == 32) then
    bubble(HERO1, _"The chest is locked.")

  elseif (zn == 33) then
    set_progress(P_TRAVELPOINT, 1)
    change_map("cave3a", "portal")

  end
end


-- Allow player to go visit the old man
function LOC_setup_oldman(en)
  if (get_progress(P_TALKOLDMAN) == 1) then
    set_obs("door1", 0)
  elseif (get_progress(P_TALKOLDMAN) > 1) then
    place_ent(en, "around_table")
    set_ent_facing(en, FACE_DOWN)
    local x, y = marker("counter")
    set_zone(x, y - 3, 0)
    set_zone(x, y, 0)
    if (get_progress(P_TALKOLDMAN) == 5) then
      -- // Unlock the doors leading to the old man's counters
      set_obs("hdoor1", 0)
      set_obs("hdoor2", 0)
    end
  end
end


function LOC_talk_oldman(en)
  local x, y = marker("counter")

  if (get_progress(P_TALKOLDMAN) == 0) then
    bubble(en, _"Hey, how did you get in here? Get out!")
  elseif (get_progress(P_TALKOLDMAN) == 1) then
    set_ent_speed(en, 5)
    set_ent_movemode(en, 2)
    -- Move the old man to the counter closest to you
    if (get_ent_tiley(HERO1) == 22) then
      -- Move old man to top (northern) counter
      move_entity(en, x, y - 2, 0)
      wait_for_entity(en, en)
      set_ent_facing(en, FACE_UP)
    else
      -- Move old man to bottom (southern) counter
      move_entity(en, x, y - 1, 0)
      wait_for_entity(en, en)
      set_ent_facing(en, FACE_DOWN)
    end
    bubble(en, _"Wow! You have all the Opal items! Quick, come up here to my study and let me see them!")
    -- Unlock doors that lead behind his counters
    set_obs("hdoor1", 0)
    set_obs("hdoor2", 0)
    set_ent_speed(en, 5)

    move_entity(en, "around_table", 0)
    wait_for_entity(en, en)
    set_ent_facing(en, FACE_DOWN)
    set_ent_movemode(en, 0)

    set_progress(P_TALKOLDMAN, 2)
    refresh()
  elseif (get_progress(P_TALKOLDMAN) == 2) then
    bubble(en, _"I see you have everything! Here, take this key.")
    sfx(5)
    msg(_"Rusty key procured", 63, 0)
    bubble(en, _"This key will open the temple found in the Grotto around Ekla.")
    if (get_numchrs() == 1) then
      bubble(HERO1, _"The Grotto by Ekla? What will I find there?")
    else
      bubble(HERO1, _"The Grotto by Ekla? What will we find there?")
    end
    bubble(en, _"It's an underwater tunnel that leads from the grotto to the island Malkaron is staying.")
    bubble(HERO1, _"Oh. Thanks!")
    set_progress(P_TALKOLDMAN, 3)
    add_special_item(SI_RUSTYKEY)
  elseif (get_progress(P_TALKOLDMAN) == 3) then
    bubble(en, _"I hope the key still fits in the temple down in the Grotto by Ekla. It's a bit rusty.")
  elseif (get_progress(P_TALKOLDMAN) == 4) then
    bubble(en, _"I see the key worked. Excellent.")
    set_progress(P_TALKOLDMAN, 5)
  else
    bubble(en, _"Good luck on your journey.")
  end
end
