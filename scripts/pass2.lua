-- pass - ""

function autoexec()
  refresh()
end


function entity_handler(en)
  return
  -- if (en == 0) then
  -- end
end


function postexec()
  return
end


function refresh()
  showch("treasure1", 168)
end


-- Show the status of a chest
function showch(which_marker, which_chest)
  -- Set tiles if -1 passed in as 'which_chest' or if chest already opened
  if (which_chest < 0 or get_treasure(which_chest) == 1) then
    set_mtile(which_marker, 39)
    set_zone(which_marker, 0)
  end
end


function zone_handler(zn)
  if (zn == 0) then
    combat(50)

  elseif (zn == 1) then
    change_map("main", "malk_pass_w")

  elseif (zn == 2) then
    change_map("main", "malk_pass_e")

  elseif (zn == 3) then
    chest(168, 0, 768)
    refresh()
  end
end
