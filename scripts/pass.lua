-- pass - "Karnok's pass, between Sunarin and Pulcannen"

function autoexec()
  if (get_progress(P_SAVEBREANNE) > 0) then
    -- Move guard on left side of pass
    place_ent(0, get_ent_tilex(0) - 1, get_ent_tiley(0))
    set_ent_facing(0, FACE_RIGHT)
    -- Move guard on right side of pass
    place_ent(1, get_ent_tilex(1) + 1, get_ent_tiley(1) - 1)
    set_ent_facing(1, FACE_LEFT)
  end
  refresh()
end


function entity_handler(en)
  if (en == 0 or en == 1) then
    if (get_progress(P_SAVEBREANNE) == 0) then
      bubble(en, _"This pass is reserved for use by caravans only.")
    else
      bubble(en, _"You are free to use the pass. Just be careful.")
    end
  elseif (en == 2) then
    -- Cabin bloke
    LOC_miner(en)
  end
end


function postexec()
  return
end


function refresh()
  if (get_progress(P_CAVEKEY) > 0) then
    -- Move Rufus into his house
    place_ent(2, 152, 12)
  end

  showch("treasure1", 73)
  showch("treasure2", 74)
  showch("treasure3", 82)
  showch("treasure4", 83)
end


-- Show the status of a chest
function showch(which_marker, which_chest)
  -- Set tiles if -1 passed in as 'which_chest' or if chest already opened
  if (which_chest < 0 or get_treasure(which_chest) == 1) then
    set_mtile(which_marker, 39)
    set_zone(which_marker, 0)
  end
end


function zone_handler(zn)
  local x, y
--  if (zn == 0) then
--    combat(50)

--  elseif (zn == 1) then
  if (zn == 1) then
    change_map("main", "pass_w")

  elseif (zn == 2) then
    change_map("main", "pass_e")

  elseif (zn == 3) then -- northern door
    LOC_door("door3", "cave5", "door1")

  elseif (zn == 4) then
    chest(73, I_VITSEED, 1)
    refresh()

  elseif (zn == 5) then
    chest(74, I_ERUNE, 1)
    refresh()

  -- Western door
  elseif (zn == 6) then
    LOC_door("door1", "cave5", "door2")

  elseif (zn == 7) then
    LOC_door("door2", "cave5", "entrance")

  -- Door into cabin
  elseif (zn == 8) then
    x, y = marker("cabin_in")
    -- Avoid parallax problems: remove the background temporarily
    set_background(0)
    door_in(x, y)

  -- Door out of the cabin
  elseif (zn == 9) then
    x, y = marker("cabin_out")
    -- Reset the background for correct parallax
    set_background(1)
    door_out(x, y)

  -- Treasure
  elseif (zn == 10) then
    chest(82, 0, 500)
    refresh()

  -- Treasure
  elseif (zn == 11) then
    chest(83, I_SALVE, 1)
    refresh()

  elseif (zn == 12) then
    touch_fire(party[0])
  end
end


function LOC_door(door, map, mark)
  local x, y = marker(door)

  if (get_progress(P_CAVEKEY) == 0) then
    bubble(HERO1, "Locked.")
    return
  end

  -- Open the door before going in
  set_mtile(x, y - 1, 57)
  set_mtile(x, y, 33)

  sfx(26)
  drawmap()
  screen_dump()

  change_map(map, mark)
end


function LOC_miner(en)
  if (get_progress(P_TALKRUFUS) == 0) then
    bubble(en, _"Howdy!")
    bubble(HERO1, _"Hello. Is this your cabin?")
    bubble(en, _"It sure is! Th' name's Rufus. I work in the abandoned mines in these parts.")
    bubble(en, _"But I'll warn ya now. There's something nasty hiding in them thar mines.")
    bubble(HERO1, _"Like what?")
    bubble(en, _"I guess I've said enough...")
    set_progress(P_TALKRUFUS, 1)
    if (get_progress(P_CAVEKEY) == 0) then
      bubble(HERO1, _"Don't worry. I was just passing through, anyway.")
    end
  elseif (get_progress(P_TALKRUFUS) == 1) then
    bubble(HERO1, _"Hello again.")
    bubble(en, _"Don't forget what I told ya about them mines.")
    if (get_progress(P_CAVEKEY) == 0) then
      bubble(HERO1, _"I won't.")
    end
  end

  if (get_progress(P_TALKRUFUS)<2) then
    bubble(HERO1, _"But, I really need to get in there.")
    set_progress(P_TALKRUFUS, 2)
    bubble(en, _"We-e-ell...")
    bubble(en, _"I reckon that you and me might be able to do a little business here.")
  else
    bubble(en, _"So, back for some more of the dynamite, are you?")
  end

  shop(23)
end
