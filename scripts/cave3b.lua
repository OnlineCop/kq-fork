-- cave3b - "Second part of cave on way to Oracle's tower"

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
  showch("treasure1", 20)
end


-- Show the status of a chest
function showch(which_marker, which_chest)
  -- Set tiles if -1 passed in as 'which_chest' or if chest already opened
  if (which_chest < 0 or get_treasure(which_chest) == 1) then
    set_mtile(which_marker, 95)
    set_zone(which_marker, 0)
  end
end


function zone_handler(zn)
  if (zn == 0) then
--    combat(9)

  elseif (zn == 1) then
    change_map("cave3a", "uldoor1")

  elseif (zn == 2) then
    chest(20, I_B_LIFE, 1)
    refresh()

  elseif (zn == 3) then
    change_map("cave3a", "drdoor1")

  -- zn == 4 is a no-combat tile

  end
end
