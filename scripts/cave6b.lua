-- cave6b - "Cave south of underwater tunnel"

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
    change_map("cave6a", "exit")

  elseif (zn == 2) then
    change_map("main", "cave6b")

  elseif (zn == 3) then
    set_save(0)

  elseif (zn == 4) then
    set_save(1)

  end
end
