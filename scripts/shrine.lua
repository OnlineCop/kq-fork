-- shrine - "The water shrine, with the tunnel that leads to Esteria"

function autoexec()
  monster_awoken = false
  statues_tried = {}
  local a
  for a = 2, 5, 1 do
    statues_tried[a] = 0
  end

  if progress.tunnel > 2 then
    remove_monster()
    remove_monster_obs()
    remove_monster_zone()
  end
end


function entity_handler(en)
  return
end


function postexec()
  return
end

function check_statues()
  local valid = true
  local a
  for a = 2, 5, 1 do
    if (statues_tried[a] == 0) then
      valid = false
      break
    end
  end
  
  return valid
end

function remove_monster()
  x, y = marker("statue_6")
  set_mtile(x, y, 0)
  set_mtile(x + 1, y, 0)
  set_ftile(x, y - 1, 0)
  set_ftile(x + 1, y - 1, 0)
  set_ftile(x + 2, y - 1, 0)
  set_ftile(x, y - 2, 0)
  set_ftile(x + 1, y - 2, 0)
  set_ftile(x + 2, y - 2, 0)
end

function remove_monster_zone()
  x, y = marker("statue_6")
  set_zone(x, y, 8)
  set_zone(x + 1, y, 0)
  -- set_zone(x, y - 1, 0)
  -- set_zone(x + 1, y - 1, 0)
  -- set_zone(x, y - 2, 0)
  -- set_zone(x + 1, y - 2, 0)
end

function remove_monster_obs()
  x, y = marker("statue_6")
  set_obs(x, y, 0)
  set_obs(x + 1, y, 0)
  -- set_obs(x, y - 1, 0)
  -- set_obs(x + 1, y - 1, 0)
  -- set_obs(x, y - 2, 0)
  -- set_obs(x + 1, y - 2, 0)
end

function handle_statue(zn)
  if statues_tried[zn] == 0 then
    statues_tried[zn] = 1
    if (zn == 2) then
      set_mtile(16, 32, 313)
    elseif (zn == 3) then
      set_mtile(39, 32, 313)
    elseif (zn == 4) then
      set_mtile(16, 19, 313)
    elseif (zn == 5) then
      set_mtile(39, 19, 313)
    end
    
    -- msg(_"The statue's eyes start glowing.", 255, 0)
    
    if check_statues() then
      set_ftile(28, 9, 299)
      bubble(255, _"ROAARR!!")
      monster_awoken = true
    end
  end
end

function zone_handler(zn)
  if (zn == 0) then
    -- combat(24)
  elseif (zn == 1) then
    change_map("grotto2", "ruins")
  elseif (zn > 1 and zn < 6) then
    handle_statue(zn)
  -- elseif (zn == 5) then
    -- handle_statue(zn)
  elseif (zn == 6) then
    if monster_awoken then
      combat(80)
      progress.tunnel = 3
      remove_monster()
      remove_monster_obs()
      remove_monster_zone()
    else
      msg(_"It's a large dragon statue.", 255, 0)
    end
  elseif (zn == 7) then
    bubble(HERO1, _"The ancient builders sure liked statues.")
  elseif (zn == 8) then
    change_map("tunnela", "entrance_e")
  end
end