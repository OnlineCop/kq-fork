-- shrine - "The water shrine, with the tunnel that leads to Esteria"

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
  if (zn == 0) then
    if progress.tunnel < 4 then
      combat(69)
    end
  elseif (zn == 1) then
    change_map("tunnela", "entrance_w")
  elseif (zn == 2) then
    change_map("tunnelc", "entrance_e")
  end
end