-- prison - "Malkaron's prison"

-- /*
-- {
-- progress:

-- }
-- */

function autoexec()
  did_talk = false
  refresh()
end


function entity_handler(en)
  return
end


function postexec()
  return
end


function refresh()
end


function zone_handler(zn)
  if (zn == 1) then
    change_map("main", "prison_out")
    
  elseif (zn == 2) then
    if (did_talk == false) then
      bubble(HERO1, _"Can you hear me?")
      bubble(4, _"Come to free me?")
      bubble(HERO1, _"How would I do that?")
      bubble(4, _"If I knew that do you think I would be in here?")
      bubble(HERO1, _"Good point.")
      bubble(4, _"Word of advice. Don't cross Malkaron.")
      bubble(HERO1, _"But he must be stopped.")
      bubble(4, _"Ahh you must be the hero type.")
      bubble(4, _"There's a cave to the south of here that has crystals that can break magical seals.")
      bubble(HERO1, _"Thanks.")
      bubble(4, _"Don't forget about me when you save the day.")
    else
      bubble(4, _"Quiet the guards might hear you.")
    end
  end
end

