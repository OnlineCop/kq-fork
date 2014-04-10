-- fort - "Mountain fort south of Andra"

-- /*
-- {
-- progress:
-- goblinitem: Possession of Goblin Item (which helps seal Monster Portal)
--   (0) Do not have it
--   (1) Got it
--   (2) Returned it to Oracle
--
-- portal2gone: Whether the portal in the temple is still working
--   (0) Still letting monsters through
--   (1) The Portal is sealed shut
--
-- talk_tsorin: If you've spoken to Tsorin in Andra (and got his seal)
--   (0) You haven't spoken to him yet
--   (1) Tsorin gave you a note to give to Derig
--   (2) Derig gave you a note to return to Tsorin
--   (3) Tsorin gave you his seal to get through the fort
--   (4) You've shown the seal to the guards at the fort
--   (5) You are free pass through the fort anytime (no contention in goblin lands)
--
-- In this town, we are going to add the 3rd PM, which will include the sub-
-- plot in the temple.
--
-- }
-- */


function autoexec()
  refresh()
end


function entity_handler(en)
  if (en == 0) then
    bubble(en, _"We have no outposts in goblin territory. If you get into real trouble, you'll have to go all the way back to Andra.")

  elseif (en == 1) then
    bubble(en, _"Make sure to stock up on supplies.")

  elseif (en == 2) then
    if progress.talk_tsorin < 3 then
      bubble(en, _"We cannot let you pass.")
    elseif progress.talk_tsorin == 3 then
      bubble(en, _"The Lieutenant requires Tsorin's authorization before anyone can pass through here.")
    else
      bubble(en, _"You are free to come and go as you please.")
    end

  elseif (en == 3) then
    if progress.talk_tsorin < 3 then
      bubble(en, _"No one is allowed through right now.")
    elseif progress.talk_tsorin == 3 then
      bubble(en, _"Show Tsorin's seal to this man next to me. He will inform the Lieutenant that you have it.")
    else
      bubble(en, _"Be careful in the goblin lands.")
    end

  elseif (en == 4) then
    if progress.talk_tsorin < 3 then
      bubble(en, _"Only Captain Tsorin can authorize you to come through here.")
    elseif progress.talk_tsorin == 3 then
      if (get_numchrs() == 1) then
        bubble(HERO1, _"Tsorin sends his greetings to you. You are to let me pass through to the Oracle's Tower.")
      else
        bubble(HERO1, _"Tsorin sends his greetings to you. You are to let us pass through to the Oracle's Tower.")
      end
      bubble(en, _"I see that you are bearing his seal. Very well.")
      bubble(en, _"I'll tell the Lieutenant you're here.")
      set_ent_speed(en, 4)
      set_ent_script(en, "D10")
      wait_for_entity(en, en)
      progress.talk_tsorin = 4
      refresh()
    elseif progress.talk_tsorin == 4 then
      bubble(en, _"The Lieutenant says you are free to go into goblin lands. Be cautious.")
    else
      bubble(en, _"We have received word that the goblin lands are peaceful now. Thank you for your help.")
    end

  elseif (en == 5) then
    if progress.talk_tsorin == 4 then
      bubble(en, _"Lieutenant:", _"South of here is a river you'll need to cross. Head for the tower in the small group of mountains due west from there. You just have to go through some caves to reach it. Then make your way up to the Oracle. Is that understood?")
      bubble(HERO1, _"Yeah, I guess so... it all seems clear enough.")
      bubble(en, _"Good. Head out whenever you're ready.")
    else
      bubble(en, _"Best of luck to you.")
    end

  end
end


function postexec()
  return
end


function refresh()
  if progress.talk_tsorin > 3 then
    place_ent(4, 39, 14)
    set_ent_facing(4, FACE_DOWN)
  end
end


function zone_handler(zn)
  if (zn == 1) then
    change_map("main", "fort", 0, -1)

  elseif (zn == 2) then
    change_map("main", "fort", 0, 2)

  elseif (zn == 3) then
    door_in("room_i")

  elseif (zn == 4) then
    door_out("room_o")

  elseif (zn == 5) then
    touch_fire(party[0])

  end
end
