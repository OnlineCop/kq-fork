-- cult - "Cult of the Shadow Walker base on island west of Sunarin"

function autoexec()
  local a

  if (get_progress(P_AVATARDEAD) < 2) then
    set_zone("front_door", 32)
    set_obs("front_door", 1)
  else
    set_zone("fight", 31)
    set_obs("front_door", 0)
    for a = 0, 19, 1 do
      set_ent_active(a, 0)
    end
    set_ent_active(23, 0)
  end

  set_ent_active(20, 0)
  set_ent_active(21, 0)
  set_ent_active(22, 0)
  refresh()
end


function entity_handler(en)
  if (en == 0) then
    if (get_progress(P_IRONKEY) == 0) then
      bubble(0, _"What th...")
      bubble(HERO1, _"Where's the girl?")
      bubble(0, _"Do you really think that I'm going to tell you?")
      bubble(HERO1, _"That depends... do you want to live?")
      bubble(0, _"Bah! Don't be deceived by the robes. I'm all muscle under here.")
      drawmap()
      screen_dump()
      set_run(0)
      combat(22)
      set_run(1)
      if (get_alldead() == 1) then
        return
      end
      drawmap()
      screen_dump()
      set_ent_active(0, 0)
      set_progress(P_IRONKEY, 1)
      add_special_item(SI_IRONKEY)
      sfx(5)
      msg(_"Iron key procured", 255, 0)
    end

  end
end


function postexec()
  return
end


function refresh()
  showch("treasure1", 75)
  showch("treasure2", 76)
  showch("treasure3", 77)
  showch("treasure4", 78)
  showch("treasure5", 79)
end


-- Show the status of a chest
function showch(which_marker, which_chest)
  -- Set tiles if -1 passed in as 'which_chest' or if chest already opened
  if (which_chest < 0 or get_treasure(which_chest) == 1) then
    set_mtile(which_marker, 65)
    set_zone(which_marker, 0)
  end
end


function zone_handler(zn)
  local x, y
  if (zn == 0) then
    if (get_progress(P_AVATARDEAD) < 2) then
--      combat(19)
    end

  elseif (zn == 1) then
    change_map("main", "cult")

  elseif (zn == 2) then
    warp("dstairs2", 8)

  elseif (zn == 3) then
    warp("ustairs1", 8)

  elseif (zn == 4) then
    warp("dstairs3", 8)

  elseif (zn == 5) then
    warp("ustairs2", 8)

  elseif (zn == 6) then
    warp("ustairs4", 8)

  elseif (zn == 7) then
    warp("dstairs1", 8)

  elseif (zn == 8) then
    warp("ustairs5", 8)

  elseif (zn == 9) then
    warp("dstairs4", 8)

  elseif (zn == 10) then
    warp("ustairs6", 8)

  elseif (zn == 11) then
    LOC_freed(20, 21, 22, 23)

  elseif (zn == 12) then
    LOC_open_door("door1")

  elseif (zn == 13) then
    LOC_open_door("door2")

  elseif (zn == 14) then
    LOC_open_door("door3")

  elseif (zn == 15) then
    LOC_open_door("door4")

  elseif (zn == 16) then
    LOC_open_door("door5")

  elseif (zn == 17) then
    LOC_open_door("door6")

  elseif (zn == 18) then
    if (get_progress(P_IRONKEY) == 0) then
      bubble(HERO1, _"Locked tight.")
    else
      if (get_progress(P_IRONKEY) == 1) then
        bubble(HERO1, _"Unlocked.")
        set_progress(P_IRONKEY, 2)
        remove_special_item(SI_IRONKEY)
      end
      LOC_open_door("door7")
    end

  elseif (zn == 19) then
    LOC_open_door("door8")

  elseif (zn == 20) then
    chest(75, I_ARMOR6, 1)
    refresh()

  elseif (zn == 21) then
    chest(76, 0, 900)
    refresh()

  elseif (zn == 22) then
    chest(77, I_ROBE4, 1)
    refresh()

  elseif (zn == 23) then
    chest(78, I_SHIELD5, 1)
    refresh()

  elseif (zn == 24) then
    chest(79, I_AGRAN, 1)
    refresh()

  elseif (zn == 25) then
    bubble(HERO1, _"Egad, these books are horrible!")

  elseif (zn == 26) then
    bubble(HERO1, _"These are just sick!")

  elseif (zn == 27) then
    bubble(HERO1, _"These books are just plain bad.")

  elseif (zn == 28) then
    bubble(HERO1, _"There is nothing here but common supplies.")

  elseif (zn == 29) then
    if (get_progress(P_AVATARDEAD) == 0) then

      bubble(HERO1, _"Breanne... are you in here?")
      wait(50)
      bubble(23, _"Huh?")
      bubble(HERO1, _"Breanne... are you in here?")
      wait(50)
      set_ent_script(23, "R2D4W50")
      wait_for_entity(23, 23)
      bubble(23, _"Who are you?")
      bubble(HERO1, _"Your parents sent us to rescue you.")
      bubble(23, _"You'll have to get me out later. Those creeps took some of my blood and they are going to do something evil with it.")
      bubble(23, _"You've got to stop them!")
      bubble(HERO1, _"Well... I guess we should hurry then. We'll be back shortly to get you out.")
      bubble(23, _"Thanks! Now hurry!")
      set_progress(P_AVATARDEAD, 1)
      return
    elseif (get_progress(P_AVATARDEAD) == 1) then
      bubble(23, _"What are you waiting for?")
    end

  elseif (zn == 30) then
    LOC_fight(17, 18, 19)

  elseif (zn == 32) then
    bubble(HERO1, _"Hey! The door is sealed!")

  elseif (zn == 33) then
    touch_fire(party[0])

  end
end


function LOC_avatar_abounds()
  local a, b, dx, dy
  local ax, ay, z
  local x, y

  create_df("mpcx.dat", "LFLAME_PCX")
  create_bmp(0, 24, 24)
  df2bmp(0, 0, 16, 0, 0, 24, 24)
  create_bmp(1, 64, 16)
  df2bmp(1, 0, 0, 0, 0, 64, 16)
  destroy_df()

  ax = get_vx()
  ay = get_vy()
  move_camera(1032, 576, 1)

  for a = 1, 23, 1 do
    for b = 0, 3, 3 do
      drawmap()
      for dx = 0, 6, 1 do
        for dy = 1, 5, 1 do
          maskblit(1, b * 16, 0, dx * 16 + 104, dy * 16 + 64, 16, 16)
        end
      end
      for dx = 1, 5, 1 do
        maskblit(1, b * 16, 0, dx * 16 + 104, 64, 16, 16)
        maskblit(1, b * 16, 0, dx * 16 + 104, 160, 16, 16)
      end
      maskblit(0, 0, 0, 148, 128 - a, 24, a)
      screen_dump()
      rest(40)
    end
  end
  destroy_bmp(0)
  destroy_bmp(1)

  for a = 1, 19, 1 do
    set_ent_active(a, 0)
  end

  x, y = marker("avatar")
  set_mtile(x, y - 1, 270)
  set_mtile(x - 1, y, 271)
  set_mtile(x, y, 272)
  set_mtile(x + 1, y, 273)
  set_mtile(x - 1, y + 1, 274)
  set_mtile(x, y + 1, 275)
  set_mtile(x + 1, y + 1, 276)

  move_camera(ax, ay, 1)
end


function LOC_fight(guard1, guard2, guard3)
  bubble(5, _"Hey! Who is that?")
  bubble(5, _"You three take care of them!")
  wait(50)
  set_ent_script(guard1, "L1D1")
  set_ent_script(guard2, "D1L1")
  set_ent_script(guard3, "D1L2")
  wait_for_entity(guard1, guard3)
  wait(50)
  set_run(0)
  combat(20)
  set_run(1)
  if (get_alldead() == 1) then
    return
  end

  x, y = marker("fight")
  set_zone(x, y, 31)
  set_zone(x, y + 1, 31)

  set_ent_active(guard1, 0)
  set_ent_active(guard2, 0)
  set_ent_active(guard3, 0)
  set_mtile("avatar", 250)
  set_autoparty(1)
  set_ent_script(HERO1, "R4U1")
  if (get_numchrs() == 2) then
    set_ent_script(HERO2, "R5")
    wait_for_entity(HERO1, HERO2)
    set_ent_facing(HERO2, FACE_UP)
  else
    wait_for_entity(HERO1, HERO1)
  end
  set_autoparty(0)
  wait(50)
  bubble(HERO1, _"Uh oh!")

  LOC_avatar_abounds()

  drawmap()
  screen_dump()
  wait(50)
  bubble(HERO1, _"The time to attack is now!")
  drawmap()
  screen_dump()
  set_autoparty(1)
  set_ent_script(HERO1, "U2")
  if (get_numchrs() == 2) then
    set_ent_script(HERO2, "U2")
    wait_for_entity(HERO1, HERO2)
  else
    wait_for_entity(HERO1, HERO1)
  end
  set_autoparty(0)
  orient_heroes()
  set_run(0)
  combat(21)
  set_run(1)
  if (get_alldead() == 1) then
    return
  end

  x, y = marker("avatar")
  set_mtile(x, y - 1, 0)
  set_mtile(x - 1, y, 0)
  set_mtile(x, y, 0)
  set_mtile(x + 1, y, 0)
  set_mtile(x - 1, y + 1, 0)
  set_mtile(x, y + 1, 0)
  set_mtile(x + 1, y + 1, 0)
  set_progress(P_AVATARDEAD, 2)
  set_obs("front_door", 0)
  set_zone("front_door", 1)
end



function LOC_freed(guard1, guard2, guard3, breanne)
  if (get_progress(P_AVATARDEAD) == 2) then
    x, y = marker("rescue")

    set_ent_active(guard1, 1)
    set_ent_active(guard2, 1)
    set_ent_active(guard3, 1)
    set_ent_active(breanne, 1)

    place_ent(breanne, x, y)
    set_ent_facing(breanne, FACE_RIGHT)
    set_btile(x, y - 2, 160)
    set_ftile(x, y - 2, 0)
    set_zone (x, y - 2, 0)
    set_obs  (x, y - 2, 0)
  end

  warp("dstairs5", 8)

  if (get_progress(P_AVATARDEAD) == 2) then
    set_autoparty(1)
    if (get_numchrs() == 2) then
      set_ent_script(HERO1, "L1")
      wait_for_entity(HERO1, HERO1)
      set_ent_script(HERO1, "L6")
      set_ent_script(HERO2, "L6")
      wait_for_entity(HERO1, HERO2)
    else
      set_ent_script(HERO1, "L7")
      wait_for_entity(HERO1, HERO1)
    end
    set_autoparty(0)
    orient_heroes()
    bubble(HERO1, _"Hey, you're free!")
    bubble(breanne, _"Yes, some of my father's men were waiting outside. Once the doors became unsealed they figured things were safe.")
    bubble(breanne, _"It seems that you've taken care of the Cult of the Shadow.")
    bubble(HERO1, _"Yeah, that avatar wasn't very strong. It was probably weak due to just having been summoned.")
    bubble(breanne, _"Don't be modest, you did a great job.")
    bubble(breanne, _"By the way, these men here mentioned that you came here because you wanted my Opal Band.")
    bubble(HERO1, _"Well, that was partly the reason we came, but once we found about this Cult and the Avatar, we had to act.")
    bubble(breanne, _"That's okay... I believe you. I want you to have the Band... you've earned it.")
    bubble(HERO1, _"Are you sure?")
    bubble(breanne, _"Yes.")
    drawmap()
    screen_dump()
    sfx(5)
    msg(_"Opal Band procured", 255, 0)
    set_progress(P_OPALBAND, 1)
    add_special_item(SI_OPALBAND)
    drawmap()
    screen_dump()
    bubble(HERO1, _"Thank you very much!")
    bubble(breanne, _"No problem, and good luck!")
    bubble(HERO1, _"See ya!")
    set_ent_script(guard1, "L1U1")
    wait_for_entity(guard1, guard1)
    set_ent_script(guard1, "L3")
    set_ent_script(guard2, "L3")
    set_ent_script(breanne, "L3")
    wait_for_entity(guard1, guard2)
    wait_for_entity(breanne, breanne)
    set_ent_script(guard3, "D1L1")
    wait_for_entity(guard3, guard3)
    set_ent_script(guard1, "L8")
    set_ent_script(guard2, "L8")
    set_ent_script(breanne, "L8")
    set_ent_script(guard3, "L8")
    wait_for_entity(guard1, breanne)
    set_ent_active(guard1, 0)
    set_ent_active(guard2, 0)
    set_ent_active(guard3, 0)
    set_ent_active(breanne, 0)
    set_progress(P_AVATARDEAD, 3)
    set_progress(P_SAVEBREANNE, 3)
  end
end


function LOC_open_door(which_marker)
  local x, y = marker(which_marker)
  sfx(26)
  set_btile(x, y + 1, 176)
  set_ftile(x, y, 175)
  set_zone(x, y, 0)
  set_obs(x, y, 0)
end
