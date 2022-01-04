-- grotto2 - "Second, larger part of grotto north of Ekla"

function autoexec()
  refresh()
  all_found = true
  if progress.tunnel == 1 then
    LOC_at_table()
    set_ent_active(8, 1)
    set_ent_active(9, 1)
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
    -- bubble(en, _"I would be useful to you, since I can use Rage in battle.")
  elseif (get_ent_id(en) == SARINA) then
    bubble(en, _"Let's go.")
    -- bubble(en, _"In battle, I can attack multiple targets at once if I'm equipped with the right weapon.")
  elseif (get_ent_id(en) == CORIN) then
    -- bubble(en, _"I can infuse weapons with magic during battle.")
  elseif (get_ent_id(en) == AJATHAR) then
    -- bubble(en, _"I notice that chanting a prayer during battle can heal your party or dispells the undead monsters.")
    bubble(en, _"I've prepared for this moment.")
  elseif (get_ent_id(en) == CASANDRA) then
    -- bubble(en, _"I can use my Boost ability to strengthen spells when I am attacking.")
    bubble(en, _"It's time.")
  elseif (get_ent_id(en) == TEMMIN) then
    -- bubble(en, _"I am very protective of my team members and will take a beating in their place.")
    bubble(en, _"It's time.")
  elseif (get_ent_id(en) == AYLA) then
    bubble(en, _"It's time.")
    -- bubble(en, _"I'm a thief by trade. You might be surprised what you can steal from enemies!")
  elseif (get_ent_id(en) == NOSLOM) then
    bubble(en, _"It's time.")
    -- bubble(en, _"I have a very keen eye. Not even enemies can hide their weaknesses from me!")

  elseif (en == 8) then
    if (prompt(en, 2, 0, "Ready?",
    "  yes",
    "  no") == 0) then
      if all_found == false then
        bubble(HERO1, _"But what about the others?")
        bubble(en, _"What about 'em? More reward for you.")
      end
      bubble(en, _"Ahh let me take a look at this seal...")
      move_entity(8, "ruins")
      wait_for_entity(en, en)
      bubble(en, _"That should do it.")
      sfx(5)
      
      bubble(en, _"Now it's time for you to go in there and do your hero duty and make sure there aren't any nasty surprises in there.")
      progress.tunnel = 2
      change_map("main", "underwater_w")
    end
  elseif (en == 9) then
    bubble(en, _"I'm just here to carry Nostik's bags.")
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
      change_map("main", "underwater_w")
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

  end
end


-- Decide who should be sitting around the table
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
      all_found = false
    else
      -- Place around the table
      set_ent_active(a, 1)
      set_ent_chrx(a, 255)
      set_ent_id(a, id)
      set_ent_obsmode(a, 1)
    end
  end
end
