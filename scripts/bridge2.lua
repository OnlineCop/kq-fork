-- bridge2 - "On Brayden river Randen and Andra, complete"

-- /*
-- {
-- Which globals should we have for the (incomplete) bridge?
--
-- progress:
-- fightonbridge
--   (0)..(4) [Not calculated]: when this is <=4, we will not use bridge2
--   (5) Slept at the inn, bridge is pretty close to being done
--   (6) [Not calculated]: when this is >= 6, we will not even enter the map
--
-- loseronbridge
--   (0) Have not spoken to man who forgot his sword
--   (1) Spoke to him after defeating the monsters
--
-- asleeponbridge
--   (0) Have not spoken to man sleeping on bridge
--   (1) Man is asleep again
-- }
-- */


function autoexec()
  refresh()
end


function entity_handler(en)
  if (en == 0) then
    bubble(en, _"There have been no further threats. Thank you.")

  elseif (en == 1) then
    if progress.loseronbridge == 0 then
      bubble(en, _"It's a good thing you helped us out with that monster!")
      bubble(HERO1, _"Oh, it was nothing...")
      bubble(en, _"No, really! I don't have a sword!")
      wait(50)
      bubble(en, _"I probably shouldn't have told you that.")
      progress.loseronbridge = 1
    elseif progress.loseronbridge == 1 then
      bubble(HERO1, _"So what happened to your sword?")
      bubble(en, _"...")
      wait(50)
      bubble(en, _"My dog ate it.")
      progress.loseronbridge = 2
    elseif progress.loseronbridge == 2 then
      bubble(en, _"Hey, look! I found a board with a nail in it!")
      thought(HERO1, _"Oh, good grief!")
      bubble(en, _"Fear my wrath! Hi-yah!")
    end

  elseif (en == 2) then
    bubble(en, _"Just a few more planks and this will be finished!")

  elseif (en == 3) then
    bubble(en, _"I... zzz... can't stay awak... zzz...")

  elseif (en == 4) then
    bubble(en, _"*YAWN!* That other guy's yawning is contagious!")

  elseif (en == 5) then
    bubble(en, _"My breaks are too short. I could really use another one.")

  elseif (en == 6) then
    bubble(en, _"I've planted flowers underneath the bridge.")

  elseif (en == 7) then
    bubble(en, _"I'm an architect. I'm building these pillars to reinforce the bridge.")

  elseif (en == 8) then
    if progress.bangthumb == 0 then
      bubble(en, _"Yes, wha...")
      msg(_"WHAM!", 255, 0)
      bubble(en, _"Yow!! My thumb! I banged my thumb!")
      if (get_ent_tilex(HERO1) > get_ent_tilex(en)) then
        set_ent_facing(en, FACE_RIGHT)
      elseif (get_ent_tiley(HERO1) > get_ent_tiley(en)) then
        set_ent_facing(en, FACE_DOWN)
      end
      bubble(en, _"I hope that you're satisfied!")
    else
      bubble(en, _"Owww, my poor thumb...")
    end

  elseif (en == 9) then
    bubble(en, _"Some moron rode a wagon over here with STONE RIMS! I'd like to find the no-good, lousy...")

  end
end


function postexec()
  return
end


function refresh()
  showch("treasure1", 8)
  showch("treasure2", 90)
end


-- Show the status of a treasures
function showch(which_marker, which_chest)
  -- Set tiles if -1 passed in as 'which_chest' or if chest already opened
  if (get_treasure(which_chest) == 1) then
    set_zone(which_marker, 0)
  end
  if (which_marker == "treasure1") then
    set_obs(which_marker, 0)
  end
  if (which_marker == "treasure2") then
    set_ftile(which_marker, 0)
  end
end


function zone_handler(zn)
  if (zn == 1) then
    change_map("main", "bridge", -1, 0)

  elseif (zn == 2) then
    -- // TT: This is still here incase the player didn't get it on the first
    -- //     (incomplete) bridge.  They can only get it once.
    chest(8, I_OSEED, 2)
    refresh()

  elseif (zn == 3) then
    chest(90, I_REGENERATOR, 1)
    refresh()

  elseif (zn == 4) then
    change_map("main", "bridge", 1, 0)

  end
end

