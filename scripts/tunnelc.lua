-- shrine - "The water shrine, with the tunnel that leads to Esteria"

function autoexec()
  has_battled = false
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
      combat(70)
    end
  elseif (zn == 1) then
    change_map("tunnelb", "entrance_w")
  elseif (zn == 2) then
    change_map("shrine2", "tunnel_exit")
  elseif (zn == 3 and has_battled ~= true) then
    if progress.tunnel < 4 then
      set_run(0)
      combat(71)
      has_battled = true
      set_run(1)
    end
  end
end