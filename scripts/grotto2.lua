-- grotto2 - "Second, larger part of grotto north of Ekla"
-- tunnel:
--   (0) 
--   (1) tunnel unlocked
--   (2) shrine unlocked
--   (3) monster defeated
--   (4) tunnel cleared

function autoexec()
  refresh()
  all_found = true
  if progress.tunnel > 0 and progress.tunnel < 4 then
    LOC_at_table()
    set_ent_active(8, 1)
    set_ent_active(9, 1)
    LOC_create_tent()
  else
    for a = 0, 9, 1 do
      set_ent_active(a, 0)
    end
  end
end


function entity_handler(en)
  -- You are talking to other party members
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
    if progress.tunnel < 2 then
      if (prompt(en, 2, 0, "Ready?",
      "  yes",
      "  no") == 0) then
        -- if all_found == false then
        --   bubble(HERO1, _"But what about the others?")
        --   bubble(en, _"What about 'em? More reward for you.")
        -- end
        bubble(en, _"Ahh let me take a look at this seal...")
        bubble(en, _"...")
        move_entity(8, "ruins")
        wait_for_entity(en, en)
        bubble(en, _"That should do it.")
          sfx(5)
          
          bubble(en, _"Now it's time for you to go in there and do your hero duty and make sure there aren't any nasty surprises in there.")
            progress.tunnel = 2
            change_map("shrine", "entrance")
          end
        else
          bubble(en, _"Have you cleared the tunnel yet?")
          bubble(HERO1, _"Well, no.")
          bubble(en, _"I see.")
        end
  elseif (en == 9) then
    bubble(en, _"I'm just here to carry Nostik's bags.")
    
    bubble(en, _"Would you like to change your party members?")
    select_manor()
    LOC_at_table()

  end
end


function postexec()
  return
end


function refresh()
  if progress.cancelrod > 0 then
    set_mtile("cancel", 265)
    set_zone("cancel", 0)
  end

  if (get_treasure(48) == 1) then
    set_mtile("treasure1", 265)
    set_zone("treasure1", 0)
  end

  if (get_treasure(49) == 1) then
    set_mtile("treasure2", 265)
    set_zone("treasure2", 0)
  end
end


function zone_handler(zn)
  if (zn == 0) then
    combat(24)

  elseif (zn == 1) then
    change_map("grotto", "portal")

  elseif (zn == 2) then
    if progress.tunnel > 1 then
      change_map("shrine", "entrance")
    else
      bubble(HERO1, _"This door is stuck tight.")
    end

  elseif (zn == 3) then
    if progress.cancelrod == 0 then
      progress.cancelrod = 1
      add_special_item(SI_CANCELROD)
      sfx(5)
      msg(_"Rod of Cancellation procured", 255, 0)
      refresh()
    end

  elseif (zn == 4) then
    chest(48, I_MHERB, 1)
    refresh()

  elseif (zn == 5) then
    chest(49, I_HELM1, 1)
    refresh()

  elseif (zn == 6) then
    if (prompt(255, 2, 0, _"Take a nap?",
    _"  yes",
    _"  no") == 0) then
      do_inn_effects(1)
    end
  end
end

function LOC_create_tent()
  x, y = marker("tent")
  set_mtile(x, y, 496)
  set_mtile(x + 1, y, 497)
  set_mtile(x, y - 1, 494)
  set_mtile(x + 1, y - 1, 495)
  set_obs(x, y, 1)
  set_obs(x + 1, y, 1)
  set_obs(x, y - 1, 1)
  set_obs(x + 1, y - 1, 1)
  set_zone(x, y, 6)
  set_zone(x + 1, y, 6)
end

-- Decide who should be standing around Nostik
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
      set_ent_active(a, 0)
      -- all_found = false
    else
      -- Place around the table
      set_ent_active(a, 1)
      set_ent_chrx(a, 255)
      set_ent_id(a, id)
      set_ent_obsmode(a, 1)
    end
  end
end
