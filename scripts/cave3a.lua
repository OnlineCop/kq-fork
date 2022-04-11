-- cave3a - "First part of cave on way to Oracle's tower"
-- /*
-- progress.travelpoint: Whether we've just come through the TravelPoint
-- progress.sidequest5 - Have we discovered the secret passage into the Embers' base?
--   (0) Not talked to Sarina.
--   (1) Talked to Sarina and got the Strange Pedant.
--   (2-3) Talked to Sarina but refused to help her close the portal.
--   (4) Sarina can no longer be recruited. Shows up later as a captive of Malkaron?
--   (10) Portal has been shut. Sarina joins the party or returns to the Manor.
-- progress.oraclemonsters - Have we discovered the secret passage into the Embers' base?
--   (5) Disabled the portal
-- */

function autoexec()
  chests_tried = {}
  local a
  for a = 26, 38, 1 do
    chests_tried[a] = 0
  end

  if not LOC_manor_or_party(CASANDRA) and (progress.sidequest5 < 4) then
    set_ent_id(5, SARINA)
  else
    set_ent_active(5, 0)
  end

  has_talked = false

  local x1, y1 = marker("exit")
  local x2, y2 = marker("uldoor1")
  local herox, heroy = get_ent_tile(HERO1)
  if (herox == x1 and heroy == y1) then
    set_warp(1, 264, 57)
  elseif (herox == x2 and heroy == y2) then
    set_warp(1, 264, 60)
  end

  if (progress.oraclemonsters > 0) then
    local a
    for a = 0, 4, 1 do
      set_ent_active(a, 0)
    end
  end

  if (progress.travelpoint == 1) then
    progress.travelpoint = 0
  else
    set_desc(0)
  end

  progress.walking = 0

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
  elseif (en == 5) then
    -- msg(progress.sidequest5)
    if (progress.oraclemonsters == 5) then
      bubble(en, _"Phew thanks. I wasn't sure I was gonna make it.")
      if (party[0] == Temmin) then
        bubble(HERO1, _"You fought with honor.")
        bubble(en, _"Thank you.")
      else
        bubble(HERO1, _"You could have just left and came back with reinforcements.")
        bubble(en, _"I uh... guess I could have done that.")
      end
      
      LOC_join_sarina(en)
      progress.sidequest5 = 10
      set_ent_active(en, 0)

    else
      if (progress.sidequest5 == 0) then
        bubble(en, _"Hey $0, am I glad to see you! I don't think I can handle much more of this.")
        bubble(en, _"I think those treasure chests are connected to this portal in some strange way.")
        bubble(en, _"Here, take this pedant I found on a guard. Try placing it into each treasure chest to see if that's the one that closes it.")
        add_special_item(SI_STRANGEPENDANT)
        bubble(HERO1, _"Why would you think that would work?")
        bubble(en, _"Monsters come out of those chests somehow... I would try it myself but if I leave the portal unguarded I'm sure to be surrounded.")
        bubble(en, _"... oh and the Oracle mentioned something about it.")
        progress.sidequest5 = progress.sidequest5 + 1
      else
        if (has_talked ~= true) then
          has_talked = true
          progress.sidequest5 = progress.sidequest5 + 1
        end
        bubble(en, _"Could you hurry it up.")
      end
    end
  end
end


function postexec()
  return
end


function refresh()
  showch("treasure1", 17)
  showch("treasure2", 18)
  showch("treasure3", 19)

  if (progress.oraclemonsters == 5) then
    set_ftile("monster_portal", 217)
  end

  if (progress.warpedtot4 > 0) then
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
    if (progress.oraclemonsters ~= 5) then
      combat(8)
    end

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
    if (progress.warpedtot4 == 0) then
      msg(_"The door creaks open noisily.", 255, 0)
      progress.warpedtot4 = 1
      refresh()
    else
      warp("mrdoor1", 8)
      if (progress.oraclemonsters == 0) then
        LOC_monsters_statue()
      end
    end

  -- zn == 11 is a no-combat zone

  elseif (zn == 12) then
    warp("dldoor1", 8)

  elseif (zn == 13) then
    if (progress.warpedtot4 < 3) then
      progress.oraclemonsters = 3
      change_map("town4", "bad_portal")
    else
      progress.travelpoint = 1
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
    if (progress.oraclemonsters == 1) then
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
      if (cheese1) then
        msg(_"Something from inside grabs you!", 255, 0)
      end
      combat(61)
    end

  elseif (zn == 21) then
    warp("dsportal1", 8)

  elseif (zn == 22) then
    if (progress.oraclemonsters > 4) then
      bubble(HERO1, _"It's been deactivated.")
    else
      bubble(HERO1, _"What a strange, glowing portal we have here...")
    end

  elseif (zn == 23) then
    progress.walking = 0
    refresh()

  elseif (zn == 24) then
    progress.walking = 1
    refresh()

  elseif (zn == 25) then
    local x, y = marker("dsportal1")
    bubble(HERO1, _"This wall looks funny...")
    msg(_"You push at the wall. Something shifts.", 255, 0)
    set_zone(x + 16, y + 2, 0)
    set_zone(x + 17, y + 2, 0)
    set_obs(x + 16, y + 2, 0)
    set_obs(x + 17, y + 2, 0)
    
  elseif (zn == 26) then
    check_chest(zn)
  elseif (zn == 27) then
    check_chest(zn)
  elseif (zn == 28) then
    check_chest(zn)
  elseif (zn == 29) then
    check_chest(zn)
  elseif (zn == 30) then
    check_chest(zn)
  elseif (zn == 31) then
    check_chest(zn)
  elseif (zn == 32) then
    check_chest(zn)
  elseif (zn == 33) then
    check_chest(zn)
  elseif (zn == 34) then
    check_chest(zn)
  elseif (zn == 35) then
    check_chest(zn)
  elseif (zn == 36) then
    check_chest(zn)
  elseif (zn == 37) then
    check_chest(zn)
  elseif (zn == 38) then
    check_chest(zn)
  elseif (zn == 39) then
    chest(150, I_MANALOCKET, 1)
  elseif (zn == 40) then
    
  end
end

function battle_chest()
  local cancombat = 0
  -- 25% chance of battle by looking into the empty chests
  cancombat = pick({pr=25, value = 1}, {pr=75, value = 0}).value

  if (cancombat == 0) then
    return false
  else
    msg(_"Something from inside grabs you!", 255, 0)
    combat(61)
    return true
  end
end

function check_chest(num)
  if (progress.oraclemonsters < 5) then
    if (progress.sidequest5 == 0) then
      if (battle_chest() ~= true) then
        msg(_"It's empty.", 255, 0)
      end
    else
      msg(_"You place the strange pedant into the chest.")
      chests_tried[num] = true
      local valid = true
      local a
      for a = 26, 38, 1 do
        if (chests_tried[a] == 0) then
          valid = false
          break
        end
      end
      
      -- valid = true
      if (valid == true) then
        sfx(26)
        msg(_"I think that one finally did the trick!")
        remove_special_item(SI_STRANGEPENDANT)
        progress.oraclemonsters = 5
        refresh()
      else
        if (battle_chest() ~= true) then
          msg(_"but nothing happens.")
        end
      end
    end
  end
end


function LOC_monsters_statue()
  local a
  progress.oraclemonsters = 1

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

  if (progress.denorian == 0) then
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

  if (progress.walking == 0) then
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


function level_partner(chr)
  local xp
  xp = get_party_xp(get_pidx(0))
  -- msg ("0 xp "..xp)
  -- msg ("num  "..get_numchrs())
  -- msg ("num  "..get_party_xp(chr))
  for i = 1,get_numchrs()-1,1 do
    xp = xp + get_party_xp(get_pidx(i))
    -- msg ("iter xp "..xp)
  end
  xp = xp / get_numchrs()
  -- msg ("after xp "..xp)
  -- msg ("after xp calc "..(xp - get_party_xp(chr)))
  -- xp = math.floor(xp * 1.1)
  give_xp(chr, xp, 1)
end


function LOC_join_sarina(en)
  local id
  -- // Level up Sarina
  level_partner(SARINA)

  -- // Give Sarina default equipment
  set_all_equip(SARINA, I_SWORD4, I_SHIELD2, I_HELM3, I_ARMOR3, I_BAND2, I_EAGLEEYES)

  id = select_team{SARINA}
  -- // Add the characters that weren't selected to the manor
  add_to_manor(id)

  if (id[1]) then
    set_ent_id(en, id[1])
    set_ent_speed(en, 4)

    if (id[2]) then
      -- Two heroes were de-selected
      set_ent_id(0, id[2])
      set_ent_active(0, 1)
      set_ent_speed(0, 4)
      if (get_ent_tiley(HERO1) > get_ent_tiley(1)) then
        -- Hero is below Sarina
        set_ent_tilex(0, get_ent_tilex(en) + 1)
        set_ent_tiley(0, get_ent_tiley(en))
      else
        -- Hero is anywhere else
        set_ent_tilex(0, get_ent_tilex(en))
        set_ent_tiley(0, get_ent_tiley(en) + 1)
      end

      bubble(en, _"If you need us, we'll be back at the manor.")

      set_ent_movemode(0, 2)
      set_ent_movemode(en, 2)
      move_entity(0,  10, 54, 1)
      move_entity(en, 10, 54, 1)
      wait_for_entity(0, en)
    else
      -- One hero was de-selected
      bubble(en, _"If you need me, I'll be back at the manor.")
      set_ent_movemode(en, 2)
      move_entity(en, 21, 53, 1)
      wait_for_entity(en, en)
    end
  end
  progress.players = progress.players + 1

end