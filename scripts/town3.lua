-- town3 - "Andra"

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
-- }
-- */

function autoexec()
  if progress.talk_tsorin == 4 then
    -- // Deactivate the Tsorin character
    set_ent_active(1, 0)
  end
  refresh()
end


function entity_handler(en)
  if (en == 0) then
    bubble(en, _"This is the town of Andra.")

  elseif (en == 1) then
    LOC_talk_tsorin(en)

  elseif (en == 2) then
    bubble(en, _"I wish the weapon shop sold slingshots.")

  elseif (en == 3 or en == 4) then
    bubble(en, _"We're guarding against goblins.")

  elseif (en == 8) then
    bubble(en, _"Caffeine, caffeine, caffeine, caffeine...")

  elseif (en == 9) then
    LOC_talk_oldguy(en)

  end
end


function postexec()
  return
end


function refresh()
  showch("treasure1", 12)
  showch("treasure2", 13)
  showch("treasure3", 14)
  showch("treasure4", 99)
  showch("treasure5", 100)
end


-- Show the status of treasures
function showch(which_marker, which_chest)
  -- Set tiles if chest already opened
  if (get_treasure(which_chest) == 1) then
    set_zone(which_marker, 0)
  end

  -- Only treasure1 needs to change its obstacle setting
  if (which_marker == "treasure1") then
    set_obs(which_marker, 0)
  end

  -- Only treasure4 needs to change its Layer2 tile
  if (which_marker == "treasure4") then
    set_mtile(which_marker, 0)
  end
end


-- Show the status of treasures
function showch(which_marker, which_chest)
  -- Set tiles if chest already opened
  if (get_treasure(which_chest) == 1) then
    set_zone(which_marker, 0)
  end

  -- Only treasure1 needs to change its obstacle setting
  if (which_marker == "treasure1") then
    set_obs(which_marker, 0)
  end
end


function zone_handler(zn)
  if (zn == 1) then
    change_map("main", "town3")

  elseif (zn == 2) then
    door_in("inn_i")

  elseif (zn == 3) then
    door_out("inn_o")

  elseif (zn == 4) then
    door_in("shop_1i")

  elseif (zn == 5) then
    door_out("shop_1o")

  elseif (zn == 6) then
    door_in("room_1i")

  elseif (zn == 7) then
    door_out("room_1o")

  elseif (zn == 8) then
    door_in("shop_2i")

  elseif (zn == 9) then
    door_out("shop_2o")

  elseif (zn == 10) then
    door_in("room_4i")

  elseif (zn == 11) then
    door_out("room_4o")

  elseif (zn == 12) then
    door_in("shop_3i")

  elseif (zn == 13) then
    door_out("shop_3o")

  elseif (zn == 14) then
    bubble(HERO1, _"Locked.")

  elseif (zn == 15) then
    inn("Riverside Inn", 35, 1)

  elseif (zn == 16) then
    shop(6)

  elseif (zn == 17) then
    shop(7)

  elseif (zn == 18) then
    shop(8)

  elseif (zn == 19) then
    shop(9)

  elseif (zn == 20) then
    door_in("shop_4i")

  elseif (zn == 21) then
    door_out("shop_4o")

  elseif (zn == 22) then
    chest(12, 0, 1)
    refresh()

  elseif (zn == 23) then
    chest(14, I_NPOULTICE, 1)

  elseif (zn == 24) then
    book_talk(party[0])

  elseif (zn == 25) then
    chest(13, I_CAP2, 1)

  elseif (zn == 26) then
    bubble(HERO1, _"Various books about magic.")

  elseif (zn == 27) then
    bubble(HERO1, _"The art of battle-magic.")

  elseif (zn == 28) then
    bubble(HERO1, _"How magic can make you rich!")

  elseif (zn == 29) then
    bubble(HERO1, _"Ten reasons why you should never call a wizard 'pencil-neck'.")

  elseif (zn == 30) then
    touch_fire(party[0])

  elseif (zn == 31) then
    bubble(HERO1, _"Inns always have boring books.")

  elseif (zn == 32) then
    chest(99, I_B_SLEEPALL, 1)
    refresh()

  elseif (zn == 33) then
    bubble(HERO1, _"The barrel is empty.")

  elseif (zn == 34) then
    chest(100, 0, 200)
    refresh()

  end
end


function LOC_talk_oldguy(en)
  if progress.goblinitem == 0 then
    bubble(en, _"To the north of here is a temple. It is nearly impenetrable from the outside, even to Malkaron's armies.")
    bubble(en, _"However, monsters have somehow appeared INSIDE the temple. They need help in any way possible.")
    bubble(HERO1, _"I can probably help. What can I do?")
    bubble(en, _"I hear that the monsters are coming through a portal in the caves under the temple. Seal it up to stop the monsters.")
    bubble(HERO1, _"How do I seal the portal?")
    bubble(en, _"I don't know. You should ask someone in the temple.")
  elseif progress.goblinitem == 1 then
    bubble(en, _"Use that Goblin Item to seal the portal in the temple.")
  else
    bubble(en, _"Good work in the temple! We thank you most graciously!")
  end
end -- LOC_talk_oldguy


function LOC_talk_tsorin(en)
  if progress.talk_tsorin == 0 then
    bubble(en, _"Tsorin:", "Thank you for visiting our town. What do you need?")
    bubble(HERO1, _"I'm not sure. To get through the pass to the south, I guess.")
    bubble(en, _"I'm afraid I cannot allow that. There is a civil upheaval in the Goblin Lands and so cannot allow anyone to pass.")
    if (get_numchrs() == 1) then
      bubble(HERO1, _"I'm pretty sure I need to get through there.")
    else
      bubble(HERO1, _"We're pretty sure we need to get through there.")
    end
    msg(_"Tsorin eyes you warily.", 255, 0)
    bubble(en, _"Well, I have to know if I can trust you first. Here, take this note to Derig.")
    msg(_"Tsorin hands you an envelope with his seal on it.", 18, 0)
    bubble(en, _"He lives back in Ekla, and if he says you can pass, I shall let you pass.")
    progress.talk_tsorin = 1
    add_special_item(SI_NOTE_TSORIN)
  elseif progress.talk_tsorin == 1 then
    bubble(en, _"If you have trouble finding Derig, ask around town. Someone's bound to know where he's wandered off to.")
  elseif progress.talk_tsorin == 2 then
    msg(_"You hand the note to Tsorin.", 18, 0)
    bubble(en, _"Hmm, I see. Since Derig says you are trustworthy, I can let you pass through the fort.")
    if (get_numchrs() == 1) then
      bubble(HERO1, _"Can you tell me why I need this security clearance?")
    else
      bubble(HERO1, _"Can you tell us why we need this security clearance?")
    end
    bubble(en, _"The Oracle's Statue was stolen from a village shrine south of here. It caused such problems that civil war has ensued. As a safety to our citizens, we have closed the border to try to capture the thief or thieves.")
    if (get_numchrs() == 1) then
      bubble(HERO1, _"If you want, I can help find this missing statue.")
    else
      bubble(HERO1, _"If you want, we can help you find this missing statue.")
    end
    bubble(en, _"That would be wonderful! Find the Oracle to the south and ask her what she would like you to do.")
    bubble(en, _"Present this Seal to the guards, and tell them that you're acting with my authority.")
    msg(_"Tsorin's Seal procured!", 25, 0)
    bubble(en, _"I'm afraid I cannot send anyone to protect you - you must take the utmost care.")
    progress.talk_tsorin = 3
    remove_special_item(SI_NOTE_DERIG)
  elseif progress.talk_tsorin == 3 then
    bubble(en, _"You can find the Oracle to the south. She will help you find this missing statue.")
  elseif progress.talk_tsorin == 4 then
    bubble(en, _"Thank you for all you've done.")
  end
end -- LOC_talk_tsorin
