-- town1 - "Ekla"

-- /*
-- {
-- progress:
-- cancelrod: Possession of Rod of Cancellation
--   (0) Do not have it
--   (1) Got it
--   (2) Returned it to Derig
--
-- darkimpboss: Dark Imp is in the tunnel from Ekla to Randen
--   (0) It is blocking the stairway to Randen
--   (1) You defeated it and the pathway is now clear
--
-- eklawelcome: Corny welcome message when you talk to the man in Ekla
--   (0) He hasn't yet given you his corny "Yes! This makes 8!" speech
--   (1) Now he likes cheese
--
-- portalgone: Whether the portal in the tunnel is still working
--   (0) Still letting monsters through
--   (1) The Portal is sealed shut
--
-- start: Monsters will not attack you until you enter Ekla
--   (0) (not used)
--   (1) Entered Ekla; monsters will now attack randomly on world map
--
-- talkderig: If you've spoken to Derig
--   (0) Never entered grotto
--   (1) Entered grotto; didn't fall down pit
--   (2) Fell down pit in grotto, didn't speak with Derig
--   (3) Fell down pit, spoke with Derig
--   (4) Derig told you about the Rod of Cancellation
--   (5) Returned Unadium coin and Rod of Cancellation to Derig
--   (6) Derig has left Ekla and is back in the Grotto
--
-- talk_tsorin: If you've spoken to Tsorin in Andra (and got his seal)
--   (0) You haven't spoken to him yet
--   (1) Tsorin gave you a note to give to Derig
--   (2) Derig gave you a note to return to Tsorin
--   (3) Tsorin gave you his seal to get through the fort
--   (4) You've shown the seal to the guards at the fort
--   (5) You are free pass through the fort anytime (no contention in goblin lands) (not done)
--
-- ucoin: Unadium Coin from Jen
--   (0) Have not yet spoken with the granddaughter, Jen
--   (1) Spoke to Jen
--   (2) Received coin
--   (3) Returned coin
--
-- warpstone: The teleporter from Ajantara <-> Randen
--   (0) Haven't used it yet
--   (1) Stepped on the warp stone and created its counterpart in Randen
-- }
-- */


function autoexec()
  -- Remove Derig from the screen
  if progress.talkderig < 4 or progress.talkderig > 5 then
    set_ent_active(4, 0)
  else
    -- Move granddaughter to the chair next to the table
    local x, y = marker("by_derig")
    place_ent(2, x - 1, y - 2)

    set_ent_facing(2, FACE_DOWN)
    set_ent_facing(4, FACE_DOWN)
    if progress.talkderig == 4 then
      set_desc(0)
      set_ent_facing(HERO1, FACE_UP)
      set_ent_facing(HERO2, FACE_UP)
      drawmap()
      screen_dump()
    end
  end

  -- Two people are wandering around until you use the transporter from Ajantara to Randen.
  if progress.warpstone == 1 then
    set_ent_active(1, 0)
    set_ent_active(3, 0)
  end

  refresh()
end


function entity_handler(en)
  if (en == 0) then
    LOC_ekla_welcome(en)

  elseif (en == 1) then
    if progress.darkimpboss == 0 then
      bubble(en, _"There is a monster blocking the pass to Randen.")
    else
      bubble(en, _"Now the monster is gone.")
    end -- darkimpboss

  elseif (en == 2) then
    LOC_talk_jen(en)

  elseif (en == 3) then
    if progress.darkimpboss == 0 then
      bubble(en, _"Stock up on weapons, magic, and experience.")
      bubble(en, _"You'll need them against the monster blocking the entrance to Randen.")
    elseif progress.darkimpboss == 1 then
      if progress.portalgone == 0 then
        bubble(en, _"The monster blocking Randen is gone, but there are still monsters underground.")
      elseif progress.portalgone == 1 then
        bubble(en, _"All the monsters are gone from the tunnel!")
      end -- portalgone
    end -- darkimpboss

  elseif (en == 4) then
    LOC_talk_derig(en)

  elseif (en == 5) then
    bubble(en, _"I hid these works of art from Malkaron's forces. When they had come through here, they tried to destroy everything...")

  elseif (en == 6) then
    bubble(en, _"Ever since monsters started appearing, my mother doesn't want me to go outdoors alone. It's so boring in here, though! I want to go out and play!")

  elseif (en == 7) then
    bubble(en, _"My, my. This is beautiful work. How rare! How exquisite! How affordable!")

  end
end


function postexec()
  -- Monsters will not attack on the world map until progress.start == 1
  if progress.start == 0 then
    msg(_"The strong-scented oils wore off.", 255, 0)
    progress.start = 1
  end

  if progress.talkderig == 4 then
    local en = 4
    bubble(en, _"I'm Derig. If I'm here, then you found me in the Grotto.")
    bubble(en, _"To stop the monsters in our underground tunnel, you must seal the portal.")
    bubble(en, _"In order to do that, you can use a Rod of Cancellation to melt it shut.")
    bubble(en, _"I brought you to town to get the Unadium Coin from my granddaughter, which will open the rune back at the Grotto.")
    bubble(en, _"That will transport you to the place where the Rod of Cancellation is.")
    progress.talkderig = 5
  end
end


function refresh()
  -- Chest in magic shop
  if (get_treasure(0) == 1) then
    set_mtile("treasure1", 265)
    set_zone("treasure1", 0)
  end

  -- Cauldron next to item shop
  if (get_treasure(6) == 1) then
    set_zone("treasure2", 0)
  end

  -- Patch of flowers behind houses
  if (get_treasure(98) == 1) then
    set_zone("treasure3", 0)
    set_obs("treasure3", 0)
  end
end


function zone_handler(zn)
  if (zn == 1) then
    change_map("main", "town1")

  elseif (zn == 2) then
    inn("The Blue Boar Inn", 25, 1)

  elseif (zn == 3) then
    shop(0)

  elseif (zn == 4) then
    shop(1)

  elseif (zn == 5) then
    shop(2)

  elseif (zn == 6) then
    bubble(HERO1, _"Locked.")

  elseif (zn == 7) then
    change_map("cave1", "entrance")

  elseif (zn == 8) then
    chest(0, I_B_SCORCH, 1)
    refresh()

  elseif (zn == 9) then
    bubble(255, _"I don't have much.")
    shop(3)

  elseif (zn == 10) then
    door_in("inn_i")

  elseif (zn == 11) then
    door_out("inn_o")

  elseif (zn == 12) then
    door_in("shop_4i")

  elseif (zn == 13) then
    door_out("shop_4o")

  elseif (zn == 14) then
    door_in("shop_2i")

  elseif (zn == 15) then
    door_out("shop_2o")

  elseif (zn == 16) then
    door_in("shop_1i")

  elseif (zn == 17) then
    door_out("shop_1o")

  elseif (zn == 18) then
    door_in("shop_3i")

  elseif (zn == 19) then
    door_out("shop_3o")

  elseif (zn == 20) then
    chest(6, I_NLEAF, 1)
    refresh()

  elseif (zn == 21) then
    book_talk(party[0])

  elseif (zn == 22) then
    door_in("derig_i")

  elseif (zn == 23) then
    door_out("derig_o")

  elseif (zn == 24) then
    touch_fire(party[0])

  elseif (zn == 25) then
    chest(98, 0, 155)
    refresh()

  elseif (zn == 26) then
    thought(HERO1, _"`Erupting Volcano'")

  elseif (zn == 27) then
    thought(HERO1, _"`Mountain Nightscape'")

  elseif (zn == 28) then
    thought(HERO1, _"`My Barn Chimney'")

  elseif (zn == 29) then
    thought(HERO1, _"`The Beach Front'")

  elseif (zn == 30) then
    thought(HERO1, _"`The Eye of Sardine'", _"Hmm, sounds strangely familiar.")

  elseif (zn == 31) then
    thought(HERO1, _"`The Battle Is Over'")

  elseif (zn == 32) then
    thought(HERO1, _"`Forgotten Barn'")

  elseif (zn == 33) then
    thought(HERO1, _"`Fire Sunset'")

  elseif (zn == 34) then
    bubble(5, _"This is my art collection. Any piece of work starting at only 300000 GP!")

  elseif (zn == 35) then
    bubble(HERO1, _"This armour has been highly polished. I can see myself in it!")

  elseif (zn == 36) then
    bubble(HERO1, _"Nothing in here but antique junk.")

  elseif (zn == 37) then
    bubble(HERO1, _"The drawers are locked.")

  elseif (zn == 38) then
    bubble(HERO1, _"This is the price list for all of these items.")

  elseif (zn == 39) then
    bubble(HERO1, _"None of these potions or herbs are useful.")

  elseif (zn == 40) then
    door_in("shop_5i")

  elseif (zn == 41) then
    door_out("shop_5o")

  end
end


function LOC_ekla_welcome(en)
  if progress.warpstone == 0 then
    if progress.eklawelcome == 0 then
      bubble(en, _"Welcome to the town of Ekla.")
      bubble(en, _"Yes! That makes eight. If I welcome enough newcomers to this town, I will get promoted.")
      bubble(en, _"I might get a job sitting in a house all day saying the same thing over and over to anyone who talks to me.")
      bubble(en, _"I should start practicing.")
      progress.eklawelcome = 1
    else
      bubble(en, _"I like cheese.")
    end
  else -- warpstone
    if progress.eklawelcome == 0 then
      bubble(en, _"I welcome people to Ekla.")
      bubble(en, _"Welcome back.")
    else
      bubble(en, _"I welcomed you already. You're in Ekla.")
    end
  end -- warpstone
end


--// Jen helps Town's needs in this order:
--//    1: Monsters (Monsters are still attacking their town)
--//    2: Tunnel (DarkImp is blocking the travel to Randen)
--//    3: Trade (Trade route stopped working between Randen and the rest of the world)
--//    4: No extra assistance needed
function LOC_help_jen_portal(en)
  if progress.cancelrod == 0 then
    LOC_help_town_rod(en)
  elseif progress.darkimpboss == 0 then
    LOC_help_town_boss(en)
  elseif progress.warpstone == 0 then
    LOC_help_town_warpstone(en)
  else
    LOC_help_town_none(en)
  end
end


--// Jen helps Tsorin's needs in this order:
--//    1: Derig (She will help you find him to give him Tsorin's note)
--//    2: Tsorin (She will tell you to give Derig's note to Tsorin)
function LOC_help_jen_tsorin(en)
  if progress.talkderig == 0 then
    LOC_help_tsorin_derig(en)
  else
    LOC_help_tsorin_tsorin(en)
  end
end


--// Town helps Derig's needs in this order:
--//    1: Coin (Jen gives it to you)
--//    2: Rod (Derig tells you how to find it)

function LOC_help_town_derig(en)
  if progress.talkderig < 3 then
    LOC_help_derig_coin(en)
  elseif progress.cancelrod == 0 then
    LOC_help_derig_rod(en)
  else
    LOC_help_derig_portal(en)
  end
end


--// Town's needs in this order:
--//    1: Coin (Need coin to get rid of the monsters)
function LOC_help_town_rod(en)
  if progress.ucoin < 2 then
    LOC_help_town_derig(en)
  end
end


function LOC_talk_derig(en)
  if progress.talk_tsorin == 0 or progress.talk_tsorin > 2 then
    if progress.talkderig == 5 then
      if progress.ucoin == 1 then
        bubble(en, _"Talk to Jen to get the Unadium coin.")
      elseif progress.ucoin == 2 then
        if progress.cancelrod == 0 then
          bubble(en, _"Go get the Rod of Cancellation out of the grotto. Use the rune.")
        elseif progress.cancelrod == 1 then
          if progress.portalgone == 0 then
            bubble(en, _"Now that you have the rod, go down and seal the portal.")
          elseif progress.portalgone == 1 then
            bubble(en, _"You've done it! The portal is gone, and you have returned the Unadium coin and Rod of Cancellation.")
            progress.ucoin = 3
            remove_special_item(SI_UCOIN)
            progress.cancelrod = 2
            remove_special_item(SI_CANCELROD)
            progress.talkderig = 6
            msg(_"Derig takes the Rod of Cancellation and Unadium Coin.", 255, 0)
            bubble(en, _"I'll take these back to the grotto for safe keeping. Thank you.")
          end -- portalgone
        end -- cancelrod
      end -- ucoin
    elseif progress.talkderig == 6 then
      -- Although talkderig==6, Derig will still be on the screen until you leave Ekla and return
      bubble(en, _"I will take these back to the grotto. Thanks again.")
    end -- talkderig
  elseif progress.talk_tsorin == 1 then
    bubble(en, _"Ah yes, $0, I... wait a minute! What is that note you are carrying?")
    msg(_"You show Derig the note.", 18, 0)
    bubble(en, _"Oh, this is very important. Yes, of utmost importance.")
    bubble(en, _"Forgive me, $0, but Tsorin writes that the Oracle's Statue has been stolen. This causes great reason for concern.")
    bubble(HERO1, _"What's the big emergency?")
    bubble(en, _"No time for that now. Please, take this note to Tsorin. I authorize you to pass into the goblin lands.")
    msg(_"Derig continues to mumble worredly.", 255, 0)
    thought(HERO1, _"Boy, this is just too weird all of a sudden.")
    msg(_"Derig hands you a sealed envelope.", 18, 0)
    progress.talk_tsorin = 2
    remove_special_item(SI_NOTE_TSORIN)
    add_special_item(SI_NOTE_DERIG)
  elseif progress.talk_tsorin == 2 then
    bubble(en, _"Please hurry. Take this note to Tsorin immediately!")
  else
    -- // Nothing here, since Derig will only focus on Tsorin's quest if
    -- progress.talk_tsorin < 3
  end -- talk_tsorin
end


-- Jen will provide assistance in this order:
--    1: Tsorin (war is top priority)
--    2: Town (monsters in tunnel)
--    3: Self (she wants her coin back)
--    4: No extra assistance needed
function LOC_talk_jen (en)
  -- Jen will only help you if you are not on Official Business for Tsorin
  if progress.talk_tsorin == 0 or progress.talk_tsorin > 2 then
    -- You have never spoken to Jen before
    if progress.ucoin == 0 then
      if progress.talkderig == 0 then
        -- You have never gone to the grotto.
        bubble(en, _"Jen:", _"I'm Derig's granddaughter. Go find Derig in the grotto north of here.")
      elseif progress.talkderig == 1 then
        -- You entered the grotto, but did not fall in the pit.
        bubble(en, _"Jen:", _"I'm Derig's granddaughter. Go find Derig in the grotto north of here. You've been there once before.")
      elseif progress.talkderig == 2 then
        -- You entered the grotto and fell in pit (you had NOT spoken to Jen first, though).  Someone helped you get out.
        bubble(en, _"You've already been to the grotto.")
        if (get_numchrs() > 1) then
          bubble(HERO1, _"Yes, we fell down a hole and someone pulled us out.")
        else
          bubble(HERO1, _"Yes, I fell down a hole and someone pulled me out.")
        end
        bubble(en, _"That is Derig, my grandfather. Go back and look for him.")
      else
        -- This should never occur
        bubble(en, _"Scripting Error:",
	       "progress.ucoin == 0, but progress.talkderig != 0..2")
      end
      -- Now you have spoken to Jen
      progress.ucoin = 1
    -- You have spoken to Jen at least once
    elseif progress.ucoin == 1 then
      if progress.talkderig == 0 then
        bubble(en, _"I said to go talk to Derig.")
      elseif progress.talkderig == 1 then
        -- You entered the grotto, but did not fall in the pit.
        bubble(en, _"You must find Derig in the grotto. He's there somewhere.")
      elseif progress.talkderig == 2 then
        bubble(en, _"Well? Go find Derig in the grotto!")
      elseif progress.talkderig == 3 then
        bubble(en, _"Okay, that was stupid. You saw him by the fire and you didn't talk to him.")
        if (get_numchrs() == 1) then
          bubble(en, _"GO BACK AND TALK TO HIM! He's sitting by the fire you moron!")
        else
          bubble(en, _"GO BACK AND TALK TO HIM! He's sitting by the fire you morons!")
        end
      elseif progress.talkderig == 4 then
        -- This should never occur
        bubble(en, _"Scripting Error:",
	       "progress.ucoin == 1, but progress.talkderig == 4")
      elseif progress.talkderig == 5 then
        -- Met Derig
        bubble(en, _"Good. Now that you found Derig, here is the Unadium Coin.")
        progress.ucoin = 2
        add_special_item(SI_UCOIN)
        msg(_"Unadium coin procured", 255, 0)
      end -- talkderig
    elseif progress.ucoin == 2 then
      -- You now have the Unadium coin
      if progress.talkderig == 5 then
        if progress.cancelrod == 0 then
          -- You do not have the rod
          bubble(en, _"Go get the Rod of Cancellation.")
        elseif progress.cancelrod == 1 then
          -- You have the rod
          if progress.portalgone == 0 then
            -- The portal is still there
            bubble(en, _"Get rid of the portal now.")
          elseif progress.portalgone == 1 then
            -- The portal is gone
            bubble(en, _"You got rid of the portal! Give my father the coin and rod back and I'll give you a SunStone.")
          end -- portalgone
        end -- cancelrod
      else
        -- This should never occur
        bubble(en, _"Scripting Error:",
	       "progress.ucoin == 2, but progress.talkderig != 5")
      end -- talkderig == 5
    elseif progress.ucoin == 3 then
      -- Returned Rod of Cancellation to Derig
      if (get_treasure(45) == 0) then
        bubble(en, _"Thanks for returning the Rod to my grandfather. Here is a SunStone for you.")
        chest(45, I_SSTONE, 1)
      else
        bubble(en, _"Thanks again.")
      end
    end -- ucoin
  elseif progress.talk_tsorin == 1 then
    bubble(en, _"Jen:", _"You must be looking for Derig, my grandfather. Find him in the grotto north of here.")
  elseif progress.talk_tsorin == 2 then
    bubble(en, _"You should finish Derig's assignment before I give you another.")
  end -- talk_tsorin

-- /*
--  if (get_progress(P_TALK_TSORIN) == 1 or get_progress(P_TALK_TSORIN) == 2) then
--    LOC_help_jen_tsorin(en)
--  elseif (get_progress(P_PORTALGONE) == 0) then
--    LOC_help_jen_portal(en)
--  elseif (get_progress(P_UCOIN) == 3) then
--    LOC_help_jen_coin(en)
--  else
--    LOC_help_jen_none(en)
--  end
-- */
end
