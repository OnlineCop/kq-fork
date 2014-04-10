-- main - "World map of Anistal"

function autoexec()
  local x, y
  if progress.showbridge < 2 then
    x, y = marker("bridge")
    if progress.showbridge == 0 then
      set_mtile(x, y, 82)
    end
    set_btile(x - 1, y, 80)
    set_btile(x + 1, y, 80)
    set_obs(x, y, 1)
    set_zone(x - 1, y, 7)
    set_zone(x + 1, y, 71)
  end

  if progress.toweropen == 1 or progress.toweropen == 3 then
    x, y = marker("tower")
    set_obs(x, y - 1, 0)
  end

  if progress.foundmayor > 0 and progress.mayorguard1 > 0 and
    progress.mayorguard2 > 0 then
    x, y = marker("camp")
    set_zone(x, y, 0)
  end

  if progress.opalhelmet == 1 and progress.opalshield == 1 and
    progress.opalband == 1 and progress.opalarmour == 1 then
    x, y = marker("cave6a")
    set_zone(x, y - 1, 73)
  end
end


function entity_handler(en)
  return
end


function postexec()
  return
end


function zone_handler(zn)
  local x, y

  if (zn == 1) then
    change_map("manor", "entrance")

  elseif (zn == 2) then
    if progress.start == 1 then
      if (in_forest(HERO1)) then
        combat(27)
      else
        combat(28)
      end
    end

  elseif (zn == 3) then
    change_map("town1", "entrance")

  elseif (zn == 4) then
    change_map("town2", "entrance")

  elseif (zn == 5) then
    if (in_forest(HERO1)) then
      combat(29)
    else
      combat(30)
    end

  elseif (zn == 6) then
    if (in_forest(HERO1)) then
      combat(31)
    else
      combat(32)
    end

  elseif (zn == 7) then
    if progress.fightonbridge == 5 then
      set_ent_facing(HERO1, FACE_LEFT)
      change_map("bridge2", "entrance")
    else
      set_ent_facing(HERO1, FACE_LEFT)
      change_map("bridge", "entrance")
    end

  elseif (zn == 8) then
    change_map("town3", "entrance")

  elseif (zn == 9) then
    change_map("grotto", "entrance")

  elseif (zn == 10) then
    change_map("fort", "entrance")

  elseif (zn == 11) then
    change_map("fort", "exit")

  elseif (zn == 12) then
    change_map("cave3a", "entrance")

  elseif (zn == 13) then
    change_map("cave3a", "exit")

  elseif (zn == 14) then
    if progress.toweropen == 0 then
      if progress.goblinitem == 1 then
        progress.toweropen = 1
        progress.goblinitem = 2
        remove_special_item(SI_JADEPENDANT)
        bubble(HERO1, _"Hey! The pendant is glowing!")
        bubble(255, _"The doors fly open and the pendant disappears in a puff of smoke.")
      elseif progress.denorian == 2 then
        progress.toweropen = 1
        bubble(HERO1, _"The doors open with a shower of rust.")
      else
        bubble(HERO1, _"The tower appears to be sealed. Maybe we need something to get in here?")
      end
    end

    if progress.toweropen == 2 then
      bubble(HERO1, _"I can't get in here anymore!")
    elseif progress.toweropen > 0 then
      change_map("tower", "entrance")
    end

  elseif (zn == 15) then
    if (in_forest(HERO1)) then
      combat(33)
    else
      combat(34)
    end

  elseif (zn == 16) then
    if (in_forest(HERO1)) then
      combat(35)
    else
      combat(36)
    end

  elseif (zn == 17) then
    change_map("temple1", "entrance")

  elseif (zn == 18) then
    change_map("town4", "entrance")

  elseif (zn == 19) then
    change_map("camp", "entrance")

  elseif (zn == 20) then
    change_map("estate", "entrance")

  elseif (zn == 21) then
    if progress.ayla_quest == 7 then
      progress.ayla_quest = 6
      change_map("town5", "exit")
    else
      change_map("town5", "entrance")
    end

  elseif (zn == 22) then
    if (in_forest(HERO1)) then
      combat(37)
    else
      combat(38)
    end

  elseif (zn == 23) then
    if (in_forest(HERO1)) then
      combat(39)
    else
      combat(40)
    end

  elseif (zn == 24) then
    if (in_forest(HERO1)) then
      combat(41)
    else
      combat(42)
    end

  elseif (zn == 25) then
    if (in_forest(HERO1)) then
      combat(43)
    else
      combat(44)
    end

  elseif (zn == 26) then
    if (in_forest(HERO1)) then
      combat(45)
    else
      combat(46)
    end

  elseif (zn == 27) then
    if (in_forest(HERO1)) then
      combat(47)
    else
      combat(48)
    end

  elseif (zn == 28) then
    if progress.seecoliseum < 2 then
      bubble(HERO1, _"The Coliseum is closed.")
      if progress.seecoliseum == 0 then
        progress.seecoliseum = 1
      end
    else
      change_map("coliseum", "entrance")
    end

  elseif (zn == 29) then
    if progress.denorian == 1 then
      bubble(255, _"You are not allowed in the village.")
      msg(_"You sneak in anyway.", 255, 0)
    end
    change_map("dville", "entrance")

  elseif (zn == 30) then
    change_map("cave4", "entrance")

  elseif (zn == 31) then
    LOC_cave4()

  elseif (zn == 32) then
    change_map("town6", "entrance")

  elseif (zn == 33) then
    warp("colis_w", 16)

  elseif (zn == 34) then
    warp("colis_e", 16)

  elseif (zn == 35) then
    change_map("pass", "entrance")

  elseif (zn == 36) then
    change_map("pass", "exit")

  elseif (zn == 37) then
    change_map("town7", "entrance")

  elseif (zn == 38) then
    LOC_giant()

  elseif (zn == 39) then
    warp("giant_e", 16)

  elseif (zn == 40) then
    change_map("cult", "entrance")

  elseif (zn == 41) then
    change_map("goblin", "entrance")

  elseif (zn == 42) then
    -- Will be fortress for Malkaron
    msg(_"This will be fortress for Malkaron", 255, 0)

  elseif (zn == 43) then
    change_map("town8", "entrance")

  elseif (zn == 44) then
    warp(255, 73, 16)

  elseif (zn == 45) then
    warp(61, 58, 16)

  elseif (zn == 46) then
    warp(29, 33, 16)

  elseif (zn == 47) then
    warp(26, 53, 16)

  elseif (zn == 48) then
    warp(69, 56, 16)

  elseif (zn == 49) then
    warp(37, 81, 16)

  elseif (zn == 50) then
    warp(20, 47, 16)

  elseif (zn == 51) then
    warp(37, 52, 16)

  elseif (zn == 52) then
    warp(44, 71, 16)

  elseif (zn == 53) then
    warp(70, 40, 16)

  elseif (zn == 54) then
    warp(45, 45, 16)

  elseif (zn == 55) then
    warp(20, 53, 16)

  elseif (zn == 56) then
    warp(66, 45, 16)

  elseif (zn == 57) then
    warp(12, 85, 16)

  elseif (zn == 58) then
    warp(26, 47, 16)

  elseif (zn == 59) then
    warp(31, 77, 16)

  elseif (zn == 60) then
    warp(15, 94, 16)

  elseif (zn == 61) then
    warp(33, 119, 16)

  elseif (zn == 62) then
    warp(44, 84, 16)

  elseif (zn == 63) then
    warp(108, 101, 16)

  elseif (zn == 64) then
    warp(28, 118, 16)

  elseif (zn == 65) then
    warp(85, 129, 16)

  elseif (zn == 66) then
    warp(86, 137, 16)

  elseif (zn == 67) then
    warp(18, 135, 16)

  elseif (zn == 68) then
    warp(79, 130, 16)

  elseif (zn == 69) then
    warp(41, 54, 16)

  elseif (zn == 70) then
    warp(12, 24, 16)

  elseif (zn == 71) then
    set_ent_facing(HERO1, FACE_LEFT)
    change_map("bridge2", "exit")

  elseif (zn == 72) then
    LOC_cave6()

  elseif (zn == 73) then
    change_map("cave6a", "entrance")

  elseif (zn == 74) then
    change_map("cave6b", "exit")

  elseif (zn == 75) then
    bubble(HERO1, _"The underwater tunnel should go here.")
    warp("underwater_w", 16)

  elseif (zn == 76) then
    bubble(HERO1, _"The second part of the underwater tunnel should go here.")
    warp("underwater_e", 16)

  elseif (zn == 77) then
    bubble(HERO1, _"This is where the castle town of Xenar goes.")
    bubble(HERO1, _"It's not finished yet.")

  elseif (zn == 78) then
    bubble(HERO1, _"This is the cave behind the Xenar Castle. Sorry, you can't go in there yet.")

  elseif (zn == 79) then
    bubble(HERO1, _"This is as far as the dock goes.")
    warp("dock_n", 16)

  elseif (zn == 80) then
    bubble(HERO1, _"This is as far as the dock goes.")
    warp("dock_s", 16)

  elseif (zn == 81) then
    msg(_"This is where a short pass or cave goes.", 255, 0)
    warp("malk_pass_w", 8)

  elseif (zn == 82) then
    msg(_"This is where a short pass or cave goes.", 255, 0)
    warp("malk_pass_e", 8)

  elseif (zn == 83) then
    msg(_"This is where a new cave goes.", 255, 0)

  elseif (zn == 84) then
    msg(_"This is Binderak's cave.", 255, 0)

  elseif (zn == 85) then
    change_map("sunarin", "entrance")

  end
end


function LOC_cave4()
  local x, y = marker("cave4")
  if progress.denorian == 0 then
    bubble(HERO1, _"Hmm... there's a huge iron door blocking the entrance to this cave.")
  elseif progress.denorian == 1 then
    bubble(HERO1, _"This seems strange. I wonder if this has something to do with the Denorian's request.")
  else
    bubble(HERO1, _"Stones 1, 4 then 3...")
    set_btile(x, y - 1, 54)
    set_zone(x, y - 1, 30)
    set_obs(x, y - 1, 0)
    sfx(26)
    bubble(HERO1, _"Bingo.")
  end
end


function LOC_cave6()
  local x, y = marker("cave6a")
  if progress.opalarmour == 1 and progress.opalhelmet == 1 and
    progress.opalshield == 1 and progress.opalband == 1 then
    set_btile(x, y - 1, 54)
    set_zone(x, y - 1, 73)
    set_obs(x, y - 1, 0)
    sfx(26)
    bubble(HERO1, _"Ah, there we go.")
  else
    bubble(HERO1, _"I think this entrance will open once I have all the opal stuff.")
  end
end


function LOC_giant()
  if progress.giantdead == 0 then
    combat(49)
    if (get_alldead() == 1) then
      return
    else
      progress.giantdead = 1
    end
  else
    warp("giant_w", 16)
  end
end

