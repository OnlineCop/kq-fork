-- goblin - "Goblin village somewhere on the map"

function autoexec()
  return
end


function entity_handler(en)
  if (en == 0) then
    bubble(en, _"I'm a goblin. Fear me.")

  elseif (en == 1) then
    bubble(en, _"This village is often overlooked by Malkaron's armies, so we are relatively safe.")

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

  end
end
