-- town8 - "not yet named"

-- /*
-- {
-- We're going to learn about the Xenars for the first time here.
--
-- P_WALKING: Layer which the player is walking on
--   (0) You're on an elevated cliff: you walk OVER bridges
--   (1) You're on ground level: you walk UNDER bridges
-- }
-- */

function autoexec()
  set_progress(P_WALKING, 0)
  refresh()
end


function entity_handler(en)
  if (en == 0) then
    bubble(en, _"This is a warm fire.")

  elseif (en == 1) then
    bubble(en, _"I am an NPC. Do you know what that means? It means I have no will of my own, and must obey my programmer.")

  elseif (en == 2) then
    bubble(en, _"I get teased a lot because I'm the only one with blonde hair. Neither of my parents have blonde hair...")

  elseif (en == 3) then
    bubble(en, _"For a long time, we lived in peace. Now that monsters have attacked, we can't even leave the safety of our village.")

  elseif (en == 4) then
    bubble(en, _"I wish I could find some opal armour. I hear there's some sort of magic that can make it very strong.")

  elseif (en == 5) then
    bubble(en, _"I am a silhouette of the Oracle. I can't figure out how you're talking to me, as I'm floating above the water like this. Hmm... must be a bug in this game.")

  elseif (en == 6) then
    shop(24)

  elseif (en == 7) then
    inn(_"Long Journey Inn", 65, 1)

  end
end


function postexec()
  return
end


function refresh()
  showch("treasure1", 91)
  showch("treasure2", 92)
  showch("treasure3", 93)
  showch("treasure4", 94)
  showch("treasure5", 95)
end


-- Show the status of a treasures
function showch(which_marker, which_chest)
  -- Set tiles if chest already opened
  if (get_treasure(which_chest) == 1) then
    set_zone(which_marker, 0)

    -- Only treasure4 needs to keep its obstacle setting
    if (which_marker ~= "treasure4") then
      set_obs(which_marker, 0)
    end
  end
end


function zone_handler(zn)
  if (zn == 1) then
    change_map("main", "town8")

  elseif (zn == 2) then
    set_progress(P_WALKING, 0)
    TOC_switch_layers(zn)

  elseif (zn == 3) then
    set_progress(P_WALKING, 1)
    TOC_switch_layers(zn)

  elseif (zn == 4) then
    chest(91, 0, 1200)
    refresh()

  elseif (zn == 5) then
    chest(92, 0, 150)
    refresh()

  elseif (zn == 6) then
    chest(93, I_SALVE, 1)
    refresh()

  elseif (zn == 7) then
    bubble(HERO1, _"This appears to be a hologram of the Oracle!")

  elseif (zn == 8) then
    warp("dstairs1", 16)

  elseif (zn == 9) then
    warp("dstairs2", 16)

  elseif (zn == 10) then
    chest(94, I_ARMOR8, 1)
    refresh()

  elseif (zn == 11) then
    bubble(HERO1, _"This barrel is filled with rainwater.")

  elseif (zn == 12) then
    touch_fire(party[0])

  elseif (zn == 13) then
    bubble(HERO1, _"Locked.")

  elseif (zn == 14) then
    chest(95, I_MACE3, 1)
    refresh()

  elseif (zn == 15) then
    door_in("inn_1i")

  elseif (zn == 16) then
    door_in("inn_2i")

  elseif (zn == 17) then
    door_out("inn_1o")

  elseif (zn == 18) then
    door_out("inn_2o")

  elseif (zn == 19) then
    door_in("room_1i")

  elseif (zn == 20) then
    door_out("room_1o")

  elseif (zn == 21) then
    door_in("room_2i")

  elseif (zn == 22) then
    door_out("room_2o")

  elseif (zn == 23) then
    door_in("door_3i")

  elseif (zn == 24) then
    door_out("door_3o")

  elseif (zn == 25) then
    door_in("room_4i")

  elseif (zn == 26) then
    door_out("room_4o")

  elseif (zn == 27) then
    door_in("room_5i")

  elseif (zn == 28) then
    door_out("room_5o")

  elseif (zn == 29) then
    door_in("room_6i")

  elseif (zn == 30) then
    door_out("room_6o")

  elseif (zn == 31) then
    warp("room_7d", 8)

  elseif (zn == 32) then
    warp("room_6u", 8)

  elseif (zn == 33) then
    thought(HERO1, _"This spa is relaxing.")

  elseif (zn == 34) then
    touch_fire(party[0])

  end
end


function TOC_switch_layers()
  local x1, y1 = marker("bridge1")
  local x2, y2 = marker("bridge2")

  if (get_progress(P_WALKING) == 0) then
    set_obs(x1 - 1, y1    , 3)
    set_obs(x1    , y1 - 2, 0)
    set_obs(x1    , y1 + 2, 0)
    set_obs(x1 + 1, y1 - 1, 5)
    set_obs(x1 + 1, y1    , 5)
    set_obs(x1 + 1, y1 + 1, 5)

    set_mtile(x1, y1 - 1, 33)
    set_ftile(x1, y1 - 1, 0)
    set_mtile(x1, y1    , 33)
    set_ftile(x1, y1    , 0)
    set_mtile(x1, y1 + 1, 33)
    set_ftile(x1, y1 + 1, 0)

    set_obs(x2    , y2 - 1, 0)
    set_obs(x2 - 1, y2    , 3)
    set_obs(x2 + 1, y2    , 5)
    set_obs(x2    , y2 + 1, 0)

    set_mtile(x2, y2, 33)
    set_ftile(x2, y2, 0)
  elseif (get_progress(P_WALKING) == 1) then
    set_obs(x1 - 1, y1    , 0)
    set_obs(x1    , y1 - 2, 1)
    set_obs(x1    , y1 + 2, 1)
    set_obs(x1 + 1, y1 - 1, 0)
    set_obs(x1 + 1, y1    , 0)
    set_obs(x1 + 1, y1 + 1, 0)

    set_mtile(x1, y1 - 1, 0)
    set_ftile(x1, y1 - 1, 33)
    set_mtile(x1, y1    , 0)
    set_ftile(x1, y1    , 33)
    set_mtile(x1, y1 + 1, 0)
    set_ftile(x1, y1 + 1, 33)

    set_obs(x2    , y2 - 1, 1)
    set_obs(x2 - 1, y2    , 0)
    set_obs(x2 + 1, y2    , 0)
    set_obs(x2    , y2 + 1, 2)

    set_mtile(x2, y2, 0)
    set_ftile(x2, y2, 33)
  end
end
