-- cave3a - "First part of cave on way to Oracle's tower"

-- // P_TRAVELPOINT: Whether we've just come through the TravelPoint

function autoexec()
  local x1, y1 = marker("exit")
  local x2, y2 = marker("uldoor1")
  local herox, heroy = get_ent_tile(HERO1)
  if (herox == x1 and heroy == y1) then
    set_warp(1, 264, 57)
  elseif (herox == x2 and heroy == y2) then
    set_warp(1, 264, 60)
  end

  if (get_progress(P_ORACLEMONSTERS) > 0) then
    local a
    for a = 0, 4, 1 do
      set_ent_active(a, 0)
    end
  end

  if (get_progress(P_TRAVELPOINT) == 1) then
    set_progress(P_TRAVELPOINT, 0)
  else
    set_desc(0)
  end

  set_progress(P_WALKING, 0)

  refresh()
end


function entity_handler(en)
  if (en == 0) then
    bubble(en, _"You cannot stop us! Begone!")
  elseif (en == 1) then
    bubble(en, _"This statue is ours! You cannot have it!")
  elseif (en == 2) then
    bubble(HERO1, _"This is the other broken half of the stolen Oracle Statue!")
  elseif (en == 3) then
    bubble(en, _"We will die before we give you the statue!")
  elseif (en == 4) then
    bubble(en, _"A curse be upon you... a curse upon you all!")
  end
end


function postexec()
  return
end


function refresh()
  showch("treasure1", 17)
  showch("treasure2", 18)
  showch("treasure3", 19)

  if (get_progress(P_WARPEDTOT4) > 0) then
    local x, y = marker("dldoor1")
    set_ftile(x, y - 1, 119)    -- set_ftile(13, 49, 119)
    set_obs(x, y - 1, 0)    -- set_obs(13, 49, 0)
  end

  TOC_switch_layers()
end


-- Show the status of a chest
function showch(which_marker, which_chest)
  -- Set tiles if -1 passed in as 'which_chest' or if chest already opened
  if (which_chest < 0 or get_treasure(which_chest) == 1) then
    set_mtile(which_marker, 41)
    set_zone(which_marker, 0)
  end
end


function zone_handler(zn)
  if (zn == 0) then
    combat(8)

  elseif (zn == 1) then
    change_map("main", "cave3", 0, 2)

  elseif (zn == 2) then
    warp("halldoor", 8)

  elseif (zn == 3) then
    warp("dldoor2", 8)

  elseif (zn == 4) then
    change_map("cave3b", "entrance")

  elseif (zn == 5) then
    warp("uldoor3", 8)

  elseif (zn == 6) then
    chest(18, I_NLEAF, 1)
    refresh()

  elseif (zn == 7) then
    chest(17, I_PCURING, 1)
    refresh()

  elseif (zn == 8) then
    chest(19, I_WOODCLOAK, 1)
    refresh()

  elseif (zn == 9) then
    warp("uldoor2", 8)

  elseif (zn == 10) then
    if (get_progress(P_WARPEDTOT4) == 0) then
      msg(_"The door creaks open noisily.", 255, 0)
      set_progress(P_WARPEDTOT4, 1)
      refresh()
    else
      warp("mrdoor1", 8)
      if (get_progress(P_ORACLEMONSTERS) == 0) then
        LOC_monsters_statue()
      end
    end

  -- zn == 11 is a no-combat zone

  elseif (zn == 12) then
    warp("dldoor1", 8)

  elseif (zn == 13) then
    if (get_progress(P_WARPEDTOT4) < 3) then
      set_progress(P_ORACLEMONSTERS, 3)
      change_map("town4", "bad_portal")
    else
      set_progress(P_TRAVELPOINT, 1)
      change_map("town7", "travelpoint")
    end

  elseif (zn == 14) then
    warp("usportal1", 8)

  elseif (zn == 15) then
    change_map("cave3b", "exit")

  elseif (zn == 16) then
    warp("urstairs1", 8)

  elseif (zn == 17) then
    warp("drstairs1", 8)

  elseif (zn == 18) then
    change_map("main", "cave3", 0, -1)

  elseif (zn == 19) then
    if (get_progress(P_ORACLEMONSTERS) == 1) then
      local or1, or2
      if (get_numchrs() == 1) then
        or1 = "I"
        or2 = "I"
      else
        or1 = "We"
        or2 = "we"
      end

      bubble(HERO1, _"No, really. "..or1.._" need to tell the Oracle about this before "..or2.._" go through this TravelPoint!")
    end

  elseif (zn == 20) then
    local cancombat = 0

    -- 25% chance of battle by looking into the empty chests
    cancombat = pick({pr=25, value = 1}, {pr=75, value = 0}).value

    if (cancombat == 0) then
      bubble(HERO1, _"Looks like it's empty.")
    else
      msg(_"Something from inside grabs you!", 255, 0)
      combat(60)
    end

  elseif (zn == 21) then
    warp("dsportal1", 8)

  elseif (zn == 22) then
    bubble(HERO1, _"What a strange, glowing portal we have here...")

  elseif (zn == 23) then
    set_progress(P_WALKING, 0)
    refresh()

  elseif (zn == 24) then
    set_progress(P_WALKING, 1)
    refresh()

  elseif (zn == 25) then
    local x, y = marker("dsportal1")
    bubble(HERO1, _"This wall looks funny...")
    msg(_"You push at the wall. Something shifts.", 255, 0)
    set_zone(x + 16, y + 2, 0)
    set_zone(x + 17, y + 2, 0)
    set_obs(x + 16, y + 2, 0)
    set_obs(x + 17, y + 2, 0)

  end
end


function LOC_monsters_statue()
  local a
  set_progress(P_ORACLEMONSTERS, 1)

  set_ent_script(0, "R5K")
  set_ent_script(1, "R6K")
  set_ent_script(2, "R7K")
  set_ent_script(3, "R8K")
  set_ent_script(4, "U1R8K")

  bubble(HERO1, _"Hey, who's that?!")

  if (get_numchrs() == 1) then
    set_ent_script(HERO1, "U2")
    wait_for_entity(HERO1, HERO1)
  else
    set_ent_script(HERO1, "U2")
    set_ent_script(HERO2, "U1")
    wait_for_entity(HERO1, HERO2)
    orient_heroes()
  end

  wait_for_entity(0, 4)
  for a = 0, 4, 1 do
    set_ent_movemode(a, 2)
  end

  if (get_progress(P_DENORIAN) == 0) then
    bubble(HERO1, _"That looked like the missing statue Tsorin was talking about!")
    bubble(HERO1, _"And those were Malkaron's men!")
  else
    bubble(HERO1, _"That looked like Malkaron's men with the Denorian's statue!")
  end

  if (get_numchrs() == 1) then
    bubble(HERO1, _"I've got to report this to the Oracle!")
  else
    bubble(HERO1, _"We've got to report this to the Oracle!")
  end

end


function TOC_switch_layers()
  local x1, y1 = marker("monster_portal")
  local a

  if (get_progress(P_WALKING) == 0) then
    set_obs(x1 - 2, y1 - 1, 4)
    set_obs(x1 + 2, y1 - 1, 4)
    set_ftile(x1 - 2, y1 - 1, 45)
    set_ftile(x1 + 2, y1 - 1, 47)
    set_mtile(x1 - 2, y1 - 1, 0)
    set_mtile(x1 + 2, y1 - 1, 0)

    for a = x1 - 1, x1 + 1, 1 do
      set_obs(a, y1 - 1, 4)
      set_ftile(a, y1 - 1, 46)
      set_mtile(a, y1 - 1, 0)
    end
  else
    set_obs(x1 - 2, y1 - 1, 1)
    set_obs(x1 + 2, y1 - 1, 1)
    set_ftile(x1 - 2, y1 - 1, 0)
    set_ftile(x1 + 2, y1 - 1, 0)
    set_mtile(x1 - 2, y1 - 1, 45)
    set_mtile(x1 + 2, y1 - 1, 47)

    for a = x1 - 1, x1 + 1, 1 do
      set_obs(a, y1 - 1, 2)
      set_ftile(a, y1 - 1, 0)
      set_mtile(a, y1 - 1, 46)
    end
  end
end
