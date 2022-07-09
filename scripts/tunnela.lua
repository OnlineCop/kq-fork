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
    combat(68)
  elseif (zn == 1) then
    change_map("shrine", "statue_6")
  elseif (zn == 2) then
    bubble(HERO1, _"Alright Trident do your stuff!")
    change_map("tunnelb", "entrance_e")
  elseif (zn == 3) then
    chest(166, I_B_FULLLIFE, 1)
  elseif (zn == 4) then
    chest(167, I_B_LUMINE, 1)
  elseif (zn == 5) then
    set_save(0)
    set_sstone(0)
  elseif (zn == 6) then
    set_save(1)
    set_sstone(1)
  end

  -- elseif (zn > 1 and zn < 6) then
  --   handle_statue(zn)
  -- elseif (zn == 5) then
  --   -- handle_statue(zn)
  -- elseif (zn == 6) then
  --   if monster_awoken then
  --     combat(62)
  --     remove_monster()
  --     remove_monster_obs()
  --     remove_monster_zone()
  --   else
  --     msg("It's a large monstrous statue.", 255, 0)
  --   end
  -- elseif (zn == 7) then
  --   change_map("tunnela", "entrance_w")
  -- end
end