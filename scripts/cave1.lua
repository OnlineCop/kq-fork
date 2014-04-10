-- cave1 - "Underground tunnel from Ekla to Randen"

-- /*
-- {
-- progress:
-- oddwall: Whether the player is told that the wall looks odd
--   (0) Have not yet seen the message
--   (1) Saw the message (only see it 1 time whenever you enter the map)
--   (2) Stairs to the guild available
--
-- darkimpboss: Defeated the monster blocking the tunnel
--   (0) Still there
--   (1) Defeated
--
-- dyingdude: Man in tunnel dying
--   (0) You have not spoken to him
--   (1) Now he is dead
--
-- portalgone: Whether the portal in the tunnel is still working
--   (0) Still letting monsters through
--   (1) The Portal is sealed shut
-- }
-- */

function autoexec()
  if (get_treasure(2) == 0) then
    if progress.oddwall < 2 then
      progress.oddwall = 0
    end
  end

  -- PH added refresh here to get rid of DarkImp
  refresh()
end


function entity_handler(en)
  return
end


function postexec()
  return
end


function refresh()

  -- Pot in SW corner
  if (get_treasure(1) == 1) then
    set_zone("treasure1", 0)
  end

  -- Treasure chest on W
  if (get_treasure(2) == 1) then
    set_mtile("treasure2", 41)
    set_zone("treasure2", 41)
  end

  -- Dark Imp boss in SE corner
  if progress.darkimpboss == 1 then
    set_ftile("imp", 0)
    set_zone("imp", 7)
    set_obs("imp", 0)
  end

  -- Dying man in NE corner
  if progress.dyingdude == 1 or progress.darkimpboss == 1 then
    set_btile("dead", 25)
    set_zone("dead", 0)
    set_obs("dead", 0)
  end

  -- Portal in SW corner
  if progress.portalgone == 1 then
    set_ftile("portal", 217)
  end
end


function zone_handler(zn)
  if (zn == 0) then
    if progress.portalgone == 0 then
      combat(6)
    end

  elseif (zn == 1) then
    change_map("town1", "stairway")

  elseif (zn == 2) then
    change_map("town2", "stairway")

  elseif (zn == 4) then
    chest(1, I_MHERB, 1)
    refresh()

  elseif (zn == 5) then
    chest(2, I_IRUNE, 1)
    refresh()

  elseif (zn == 6) then
    if progress.oddwall < 2 then
      bubble(HERO1, _"These stairs are blocked!")
    else
      change_map("guild", "cave1")
    end

  elseif (zn == 7) then
    -- This is simply a monster-free zone

  elseif (zn == 8) then
    if progress.oddwall == 0 then
      bubble(HERO1, _"Hmm... this wall looks odd.")
      progress.oddwall = 1
    end

  elseif (zn == 9) then
    warp("warp_w", 8)

  elseif (zn == 10) then
    warp("warp_e", 8)

  elseif (zn == 11) then
    if progress.dyingdude == 0 then
      bubble(255, _"... don't go any further. A strange creature has... blocked the path. It just appeared there out of nowhere.")
      progress.dyingdude = 1
    else
      bubble(HERO1, _"He's dead.")
    end

  elseif (zn == 12) then
    if progress.darkimpboss == 0 then
      set_run(0)
      combat(7)
      set_run(1)
      progress.darkimpboss = 1
      refresh()
    end

  elseif (zn == 13) then
    if progress.portalgone == 0 then
      if progress.cancelrod == 1 then
        bubble(HERO1, _"Hmmm... I guess if I just touch it with the rod...")
        do_fadeout(4)
        progress.portalgone = 1
        progress.sidequest1 = 1
        refresh()
        drawmap()
        screen_dump()
        do_fadein(4)
        bubble(HERO1, _"Whoa! The rod melted the portal shut!")
      else
        bubble(HERO1, _"Hmmm... this is very old and has several deep cracks in it.")
      end
    else
      bubble(HERO1, _"I wonder where this came from?")
    end

  end
end

