-- camp - "Orc forest encampment south of Randen"

-- /*
-- {
-- progress:
-- foundmayor:
--   (0) Mayor is still behind bars
--   (1) Mayor has been freed and is now home
-- mayorguard1:
--   (0) The first of the mayor's guards is still being held
--   (1) The first guard was released (and spoken to)
-- mayorguard2:
--   (0) The second of the mayor's guards is still being held
--   (1) The second guard was released (and spoken to)
-- }
-- */


function autoexec()
  local en

  en = 37
  if progress.sidequest2 == 0 then
    set_ent_id(en, CASANDRA)
  else
    set_ent_active(en, 0)
    set_ent_tiley(2, get_ent_tiley(en))
  end

  if progress.foundmayor > 0 then
    for en = 0, 4, 1 do
      set_ent_active(en, 0)
    end
    for en = 25, 28, 1 do
      set_ent_active(en, 0)
    end
  end

  if progress.mayorguard1 > 0 then
    set_ent_active(5, 0)
  end

  if progress.mayorguard2 > 0 then
    set_ent_active(6, 0)
  end

  refresh()
end


function entity_handler(en)
  if (en == 0 or en == 1 or en == 3 or en == 4) then
    bubble(en, _"It sure it clammy in here...")

  elseif (en == 2) then
    -- This should never happen, but just incase...
    LOC_rescue_mayor(en)

  elseif (en == 5) then
    if progress.mayorguard1 == 0 then
      bubble(en, _"Wow, thanks for helping me! When we get back to my place, feel free to stop by sometime!")
      progress.mayorguard1 = 1
    else
      bubble(en, _"My place is right next to the mayor's.")
    end

  elseif (en == 6) then
    if progress.mayorguard2 == 0 then
      bubble(en, _"Thanks for your help! I should be able to find my way out.")
      progress.mayorguard2 = 1
    else
      bubble(en, _"Don't worry about me. I'm still looking for the exit...")
    end

  elseif (en == 7 or en == 8) then
    if (LOC_orc_battle(1, en) == 1) then
      return
    end

  elseif (en >= 9 and en <= 11) then
    if (LOC_orc_battle(2, en) == 1) then
      return
    end

  elseif (en >= 12 and en <= 15) then
    if (LOC_orc_battle(3, en) == 1) then
      return
    end

  elseif (en >= 16 and en <= 18) then
    if (LOC_orc_battle(4, en) == 1) then
      return
    end

  elseif (en >= 19 and en <= 22) then
    if (LOC_orc_battle(5, en) == 1) then
      return
    end

  elseif (en == 23 or en == 24 or (en >= 29 and en <= 36) or en == 38) then
    if (LOC_orc_battle(8, en) == 1) then
      return
    end
    set_ent_active(en, 0)

  elseif (en >= 25 and en <= 28) then
    if (LOC_orc_battle(7, en) == 1) then
      return
    end

  elseif (en == 37) then
    LOC_rescue_mayor(en)
  end
end


function postexec()
  return
end


function refresh()
  local x, y

  showch("treasure1", 55)
  showch("treasure2", 56)
  showch("treasure3", 57)
  showch("treasure4", 58)
  showch("treasure5", 59)
  showch("treasure6", 60)
  showch("treasure7", 61)

  if progress.foundmayor > 0 then
    x, y = marker("cage")
    set_ftile(x, y - 3, 0)
    set_obs(x, y - 3, 0)
  end
end


-- Show the status of a chest
function showch(which_marker, which_chest)
  -- Set tiles if -1 passed in as 'which_chest' or if chest already opened
  if (which_chest < 0 or get_treasure(which_chest) == 1) then
    set_mtile(which_marker, 265)
    set_zone(which_marker, 0)
  end
end


function zone_handler(zn)
  if (zn == 1) then
    change_map("main", "camp", 0, -1)

  elseif (zn == 2) then
    change_map("main", "camp", 0, 1)

  elseif (zn == 3) then
    if (LOC_orc_battle(1, 8) == 1) then
      return
    end

  elseif (zn == 4) then
    if (LOC_orc_battle(3, 12) == 1) then
      return
    end

  elseif (zn == 5) then
    if (LOC_orc_battle(4, 16) == 1) then
      return
    end

  elseif (zn == 6) then
    if (LOC_orc_battle(5, 20) == 1) then
      return
    end

  elseif (zn == 7) then
    if (LOC_orc_battle(6, 17) == 1) then
      return
    end

  elseif (zn == 8) then
    chest(55, I_NLEAF, 2)
    refresh()

  elseif (zn == 9) then
    chest(56, 0, 250)
    refresh()

  elseif (zn == 10) then
    chest(57, I_RRUNE, 1)
    refresh()

  elseif (zn == 11) then
    chest(58, I_SALVE, 2)
    refresh()

  elseif (zn == 12) then
    chest(59, I_EDROPS, 1)
    refresh()

  elseif (zn == 13) then
    chest(60, I_SPEAR1, 1)
    refresh()

  elseif (zn == 14) then
    chest(61, I_ARMOR2, 1)
    refresh()

  elseif (zn == 15) then
    touch_fire(party[0])

  end
end


function LOC_fight_cleanup(fight_num)
  local x, y
  if (fight_num == 1) then
    set_ent_active(7, 0)
    set_ent_active(8, 0)
    x, y = marker("fight_1")
    set_zone(x, y - 1, 0)
    set_zone(x + 1, y - 1, 0)
  elseif (fight_num == 2) then
    set_ent_active(9, 0)
    set_ent_active(10, 0)
    set_ent_active(11, 0)
  elseif (fight_num == 3) then
    set_ent_active(12, 0)
    set_ent_active(13, 0)
    set_ent_active(14, 0)
    set_ent_active(15, 0)
    x, y = marker("fight_3")
    set_zone(x, y - 1, 0)
    set_zone(x + 1, y - 1, 0)
  elseif (fight_num == 4 or fight_num == 6) then
    set_ent_active(16, 0)
    set_ent_active(17, 0)
    set_ent_active(18, 0)
    set_zone("fight_4a", 0)
    x, y = marker("fight_4b")
    set_zone(x, y, 0)
    set_zone(x, y + 1, 0)
  elseif (fight_num == 5) then
    set_ent_active(19, 0)
    set_ent_active(20, 0)
    set_ent_active(21, 0)
    set_ent_active(22, 0)
    x, y = marker("fight_5")
    set_zone(x, y + 1, 0)
    set_zone(x + 1, y + 1, 0)
  elseif (fight_num == 7) then
    set_ent_active(25, 0)
    set_ent_active(26, 0)
    set_ent_active(27, 0)
    set_ent_active(28, 0)

  end
end


function LOC_orc_battle(fight_num, en)
  local x, y, which_combat = 0

  if (fight_num == 1) then
    x, y = marker("fight_1")
    move_entity(7, x + 1, y, 0)
    move_entity(8, x, y, 0)
    wait_for_entity(7, 8)
    set_ent_facing(7, FACE_UP)
    set_ent_facing(8, FACE_UP)
    bubble(en, _"You're not welcome here!")
    which_combat = 2
  elseif (fight_num == 2) then
    if (get_numchrs() > 1) then
      bubble(en, _"Intruders!")
    else
      bubble(en, _"Intruder!")
    end
    x, y = marker("fight_2")
    move_entity(10, x + 1, y + 1, 0)
    move_entity(11, x, y, 0)
    wait_for_entity(10, 11)
    set_ent_facing(10, FACE_UP)
    set_ent_facing(11, FACE_UP)
    which_combat = 3
  elseif (fight_num == 3) then
    x, y = marker("fight_3")
    set_ent_movemode(14, 2)
    move_entity(12, x + 1, y, 0)
    move_entity(13, x, y, 0)
    move_entity(14, x, y + 1, 0)
    move_entity(15, x + 1, y + 1, 0)
    wait_for_entity(12, 15)
    set_ent_facing(12, FACE_UP)
    set_ent_facing(13, FACE_UP)
    set_ent_facing(14, FACE_UP)
    set_ent_facing(15, FACE_UP)
    which_combat = 4
  elseif (fight_num == 4) then
    x, y = marker("fight_4a")
    move_entity(16, x, y - 1, 0)
    move_entity(17, x - 1, y + 1, 0)
    if (get_numchrs() == 2) then
      if (get_ent_tiley(HERO2) > get_ent_tiley(HERO1)) then
        move_entity(18, x, y + 2, 0)
      else
        move_entity(18, x, y + 1, 0)
      end
    end
    wait_for_entity(16, 18)
    which_combat = 3
  elseif (fight_num == 5) then
    x, y = marker("fight_5")
    move_entity(19, x + 1, y)
    move_entity(20, x, y)
    move_entity(21, x, y - 1)
    move_entity(22, x + 1, y - 1)
    wait_for_entity(19, 22)
    set_ent_facing(19, FACE_DOWN)
    set_ent_facing(20, FACE_DOWN)
    set_ent_facing(21, FACE_DOWN)
    set_ent_facing(22, FACE_DOWN)
    which_combat = 4
  elseif (fight_num == 6) then
    -- Move this monster as close as possible to you before fighting
    x, y = get_ent_tile(HERO1)
    move_entity(16, x - 1, y - 1)

    x, y = marker("fight_4b")
    set_ent_speed(16, 6)
    move_entity(17, x + 1, y)
    move_entity(18, x + 1, y + 1)
    wait_for_entity(16, 18)
    set_ent_facing(16, FACE_DOWN)
    set_ent_facing(17, FACE_LEFT)
    set_ent_facing(18, FACE_LEFT)
    which_combat = 3
  elseif (fight_num == 7) then
    bubble(en, _"You have to get through us to get to them.")
    which_combat = 5
  elseif (fight_num == 8) then
    if (get_numchrs() > 1) then
      bubble(en, _"Intruders!")
    else
      bubble(en, _"Intruder!")
    end
    which_combat = 1
  end

  if (which_combat == 0) then
    return 0
  end

  drawmap()
  screen_dump()

  set_run(0)
  combat(which_combat)
  set_run(1)

  if (get_alldead() == 1) then
    return 1
  end

  LOC_fight_cleanup(fight_num)
  return 0
end


function LOC_rescue_mayor(en)
  local a, x, y

  x, y = marker("cage")
  bubble(en, _"Hey, $0! Am I ever glad to see you! We're in a bit of a jam here.")
  if (en ~= 2) then
    bubble(en, _"I took a small job to help guard the mayor on his trip to Andra.")
  end
  bubble(en, _"We were on the Brayden bridge when we were ambushed by these orcs. During the fight, one of them used a fire spell.")
  bubble(en, _"The bridge was engulfed in flames and we were trapped.")
  bubble(HERO1, _"But how did you end up here?")
  bubble(en, _"Well, we had no choice but to go back the way we came, and we ran into the whole tribe.")
  bubble(en, _"We were horribly out-numbered and forced to surrender. They brought us here and then most of them left for somewhere else.")
  bubble(HERO1, _"Hmm... this almost sounds as though the whole thing was planned.")
  bubble(en, _"Hmm...")
  if (en ~= 2) then
    bubble(HERO1, _"Excuse me, Mr. Mayor.")
    bubble(2, _"Yes?")
  end
  bubble(HERO1, _"Do you think that this could have possibly been a planned attack with the purpose of trying to kidnap you?")
  bubble(2, _"I don't see why they would want to kidnap me, but uh... could we possibly get out of here now?")
  if (en ~= 2) then
    set_ent_facing(en, FACE_UP)
    bubble(en, _"Yes sir!")
    set_ent_facing(en, FACE_DOWN)
    bubble(en, _"I'll go back to Randen with the mayor. Why don't you meet me there after?")
  end
  bubble(HERO1, _"Sure.")

  progress.foundmayor = 1
  progress.sidequest2 = 1
  refresh()
  set_autoparty(1)

  move_entity(HERO1, x + 1, y)
  if (get_numchrs() == 2) then
    move_entity(HERO2, x + 1, y + 1)
  end
  wait_for_entity(HERO1, HERO2)
  set_ent_facing(HERO1, FACE_LEFT)
  set_ent_facing(HERO2, FACE_LEFT)

  set_autoparty(0)
  orient_heroes()

  -- TT: This keeps the dead brigands from trying to move too
  for a = 5, 36, 1 do
    set_ent_movemode(a, 0)
  end

  if (en == 2) then
    x, y = get_ent_tile(2)
    move_entity(2, x, y - 1)
    wait_for_entity(2)
  end

  set_ent_script(37, "D5")
  set_ent_script(4, "L1D4")
  set_ent_script(3, "D1L1D3")
  set_ent_script(2, "R1D1L1D2")
  set_ent_script(1, "R2D1L1D1")
  set_ent_script(0, "R3D1L1F0")

  wait_for_entity(0, 37)

  set_ent_script(37, "L4D10K")
  set_ent_script(4, "D1L4D10K")
  set_ent_script(3, "D2L4D10K")
  set_ent_script(2, "D3L4D10K")
  set_ent_script(1, "D4L4D10K")
  set_ent_script(0, "D5L4D10K")

  wait_for_entity(0, 37)

  set_ent_active(en, 0)

  for a = 0, 4, 1 do
    set_ent_active(a, 0)
  end
end
