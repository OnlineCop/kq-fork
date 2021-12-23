-- sunarin - "Sunarin Tower"

function autoexec()
  refresh()
  return
end


-- function entity_handler(en)
--   if (en == 0) then
--   end
-- end


function postexec()
  return
end


function refresh()
  showch("treasure1", 154)
  showch("treasure2", 155)
  showch("treasure3", 156)
  showch("treasure4", 157)
  showch("treasure5", 158)
  showch("treasure6", 159)
  showch("treasure7", 160)
  showch("treasure8", 161)
end


-- Show the status of a chest
function showch(which_marker, which_chest)
  -- Set tiles if -1 passed in as 'which_chest' or if chest already opened
  if (which_chest < 0 or get_treasure(which_chest) == 1) then
    set_mtile(which_marker, 61)
    set_zone(which_marker, 0)
  end
end


function zone_handler(zn)
  if (zn == 1) then
    change_map("cave7", "exit")
  -- elseif (zn == 2) then
  --   LOC_door("door1")
  elseif (zn == 3) then
    touch_fire(party[0]);
  elseif (zn == 4) then
    chest(153, I_KNIFE4, 1)
  elseif (zn == 5) then
    bubble(HERO1, _"I better not risk it.")
  -- elseif (zn == 6) then
  -- elseif (zn == 7) then
  --   bubble(HERO1, _"It's just junk.")
  elseif (zn == 8) then
    chest(154, 0, 5000)
    refresh()
  elseif (zn == 9) then
    chest(155, I_CAP5, 1)
    refresh()
  elseif (zn == 10) then
    chest(156, I_ROD4, 1)
    refresh()
  elseif (zn == 11) then
    chest(157, 0, 3500)
    refresh()
  elseif (zn == 12) then
    chest(158, I_KNIFE3, 1)
    refresh()
    if (progress.sidequest6 < 7) then
      progress.sidequest6 = 7
      bubble(HERO1, _"Haha I would like to see the face of the King when he realizes his treasure is gone.")
      if (get_numchrs() > 1) then
        bubble(HERO2, _"I doubt that would be wise.")
      end
    end
  elseif (zn == 13) then
    chest(159, I_OCEANPEARL, 2)
    refresh()
  elseif (zn == 14) then
    chest(160, I_REGENERATOR, 1)
    refresh()
  elseif (zn == 15) then
    chest(161, I_SHIELD4, 1)
    refresh()
  end
end
