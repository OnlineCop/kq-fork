-- goblin - "Goblin village somewhere on the map"

function autoexec()

  if (progress.goblin_monk > 2) then
    set_ent_active(8, 0)
  end
end


function entity_handler(en)
  if (en == 0) then
    bubble(en, _"I'm a goblin. Fear me.")

  elseif (en == 1) then
    bubble(en, _"This village is often overlooked by Malkaron's armies, so we are relatively safe.")
  elseif (en == 4) then
    bubble(en, _"I hope you're not about to cause some ruckus.")
  elseif (en == 6) then
    bubble(en, _"What... you want to stay? We don't normally have vistors. It's going to cost you.")
    inn(_"The Crusty Pot Inn", 95, 1)
  elseif (en == 7) then
    bubble(en, _"You better not lead Malkaron's men here.")
    bubble(HERO1, _"This village is pretty well hidden.")
  elseif (en == 8) then
    if (progress.goblin_monk < 2) then
      bubble(en, _"Some people say inner peace can be found by relinquishing worldly possessions.")
      if (prompt(HERO1, 2, 1, "...",
      "  that's a commendable way to live",
      "  well...") == 0) then
        bubble(en, _"Thanks.")
      else
        if (progress.goblin_monk == 0) then
          bubble(HERO1, _"This is a pretty nice house you have.")
        else
          bubble(HERO1, _"Woah is that two blankets?")
        end
        bubble(en, _"What? You're right!")
        progress.goblin_monk = progress.goblin_monk + 1
      end
    else
      bubble(en, _"You know, your right. If I really want to find peace I can't have little things like these holding me back.")
      bubble(en, _"You can have my house.")
      bubble(en, _"Thanks for helping me find the courage to follow my dreams.")
      bubble(HERO1, _"I uh...")
      move_entity(en, 84, 34, 1)
      progress.goblin_monk = 3
    end
  else
    bubble(en, _"Booga-booga! I'm a goblin! Rowr!")
  end
end


function postexec()
  return
end


function zone_handler(zn)
  if (zn == 1) then
    change_map("main", "goblin")

  elseif (zn == 2) then
    touch_fire(party[0])

  elseif (zn == 3) then
    door_in("room_1i")

  elseif (zn == 4) then
    door_out("room_1o")

  elseif (zn == 5) then
    door_in("room_2i")

  elseif (zn == 6) then
    door_out("room_2o")

  elseif (zn == 7) then
    door_in("room_3i")

  elseif (zn == 8) then
    door_out("room_3o")

  elseif (zn == 9) then
    door_in("room_4i")

  elseif (zn == 10) then
    door_out("room_4o")

  elseif (zn == 11) then
    door_in("room_5i")

  elseif (zn == 12) then
    door_out("room_5o")

  elseif (zn == 13) then
    door_in("room_6i")

  elseif (zn == 14) then
    door_out("room_6o")
    
  elseif (zn == 15) then
    bubble(6, _"What... you want to stay? We don't normally have vistors. It's going to cost you.")
    inn(_"The Crusty Pot", 95, 1)
    
  elseif (zn == 16) then
    if (progress.goblin_monk > 2) then
      if (prompt(255, 2, 0, _"Take a nap?",
                            _"  yes",
                            _"  no") == 0) then
        do_inn_effects(1)
      end
    end
  end
end
