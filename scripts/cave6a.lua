-- cave6a - "Cave north of Ajantara"

-- /*
-- {
-- It would be really good if we could get some sort of cave town 2/ people
-- and the 7th PM.
-- }
-- */


function autoexec()
  return
end


function entity_handler(en)
  return
end


function postexec()
  return
end


function zone_handler(zn)
  if (zn == 1) then
    change_map("main", "cave6a")

  elseif (zn == 2) then
    change_map("cave6b", "entrance")

  end
end
