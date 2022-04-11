-- cave6b - "Cave south of underwater tunnel"

function autoexec()
  return
end


function entity_handler(en)
  return
end


function refresh()
  showch("treasure1", 169)
end


-- Show the status of a chest
function showch(which_marker, which_chest)
  -- Set tiles if -1 passed in as 'which_chest' or if chest already opened
  if (which_chest < 0 or get_treasure(which_chest) == 1) then
    set_btile(which_marker, 95)
    set_zone(which_marker, 0)
  end
end

function postexec()
  return
end


function zone_handler(zn)
  if (zn == 0) then
    -- combat(6)
    
  elseif (zn == 1) then
    change_map("cave6a", "exit")
    
  elseif (zn == 2) then
    if progress.trident == 0 then
      -- combat(6)
      
      msg(_"Magic Trident procured", 255, 0)
      add_special_item(SI_TRIDENT)
      progress.trident = 1

      chest(169, 0, 2400)
      refresh()
    end

  elseif (zn == 3) then
    set_save(0)
    set_sstone(0)

  elseif (zn == 4) then
    set_save(1)
    set_sstone(1)
  end
end
