-- shrine - "The water shrine, with the tunnel that leads to Esteria"

function autoexec()
  all_found = true
  LOC_at_table()
end


function entity_handler(en)

  if (get_ent_id(en) == SENSAR) then
    bubble(en, _"Thinking of Malkaron's army makes me angry.")
  elseif (get_ent_id(en) == SARINA) then
    bubble(en, _"I'm tired of waiting. Let's go.")
  elseif (get_ent_id(en) == CORIN) then
    bubble(en, _"Do you really think Noslom has enough gold to pay all of us?")
  elseif (get_ent_id(en) == AJATHAR) then
    bubble(en, _"I've prepared for this moment.")
  elseif (get_ent_id(en) == CASANDRA) then
    bubble(en, _"Malkaron won't know what hits him.")
  elseif (get_ent_id(en) == TEMMIN) then
    bubble(en, _"It's time to make Malkaron pay for his crimes.")
  elseif (get_ent_id(en) == AYLA) then
    bubble(en, _"I wonder if Malkaron has any good treasure.")
  elseif (get_ent_id(en) == NOSLOM) then
    bubble(en, _"Hmm not everything is adding up. I must be missing something.")

  elseif (en == 8) then
    bubble(en, _"We're close now, I can feel it.")
    set_ent_active(0, 0)
    set_ent_active(1, 0)

  elseif (en == 9) then
    bubble(HERO1, _"Do you have any more of that smelly oil?")
    bubble(en, _"I'm afraid it wouldn't work on the enemies you will face here anyway.")
    bubble(en, _"Would you like to switch who you are travelling with?")
    select_manor()
    LOC_at_table()
  end
end


function postexec()
  if progress.tunnel < 4 then
    progress.tunnel = 4
    bubble(HERO1, _"Phew we made it.")
    bubble(HERO1, _"Now time to collect Nostik.")
    wait(100)
    do_fadeout(4)
  --       refresh()
  --       drawmap()
    screen_dump()
    do_fadein(4)
  end
end


function zone_handler(zn)
  if (zn == 0) then
    -- combat(24)
  elseif (zn == 1) then
    change_map("main", "underwater_w")
   elseif (zn == 2) then
    change_map("tunnelc", "entrance_n")
  elseif (zn == 3) then
    if (prompt(255, 2, 0, _"Take a nap?",
    _"  yes",
    _"  no") == 0) then
      do_inn_effects(1)
    end
  end
end

-- Decide who should be in the shrine
function LOC_at_table()
  local id, a
  for a = 0, 7 do
    -- You have not recruited this person into your team
    id = progress['manorparty' .. a] - 1
    if (id == get_pidx(0)) then
      id = -1
    end
    if (get_numchrs() == 2 and id == get_pidx(1)) then
      id = -1
    end
    if (id < 0) then
      -- Remove entity from the map
      -- msg("Removing " .. a)
      set_ent_active(a, 0)
      -- all_found = false
    else
      -- msg("placing " .. a)
      -- Place around the table
      set_ent_active(a, 1)
      set_ent_chrx(a, 255)
      set_ent_id(a, id)
      set_ent_obsmode(a, 1)
    end
  end
end