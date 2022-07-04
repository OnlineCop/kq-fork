-- cave9 - "Crystal Cave"

-- /*
-- {
-- progress:
-- crystal: Whether the player has a crystal
--   (0) No crystal
--   (1) Got crystal
-- }
-- */

function autoexec()
  refresh()
end

function entity_handler(en)
  return
end


function postexec()
  return
end


function refresh()
  showch("treasure1", 171)
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
    combat(78)

  elseif (zn == 1) then
    change_map("main", "crystal_exit")

  elseif (zn == 2) then
    if (progress.crystal < 1) then
      bubble(HERO1, _"Ahh What's this?")
      set_run(0)
      combat(79)
      set_run(1)
      bubble(HERO1, _"A piece of this crystal might prove useful.")
      add_special_item(SI_CRYSTAL)
      msg(_"Strange crystal procured", 255, 0)
      progress.crystal = 1
    end
  elseif (zn == 3) then
    chest(171, I_HPUP, 1)
    refresh()
  end
end

