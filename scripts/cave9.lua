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
end


function zone_handler(zn)
  if (zn == 0) then
    -- combat(62)

  elseif (zn == 1) then
    change_map("main", "crystal_exit")

  elseif (zn == 2) then
    if (progress.crystal < 1) then
      bubble(HERO1, _"A piece of this crystal might prove useful.")
      add_special_item(SI_CRYSTAL)
      msg(_"Strange crystal procured", 255, 0)
      progress.crystal = 1
    end
  end
end

