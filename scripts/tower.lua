-- tower - "Oracle's tower, southwest of mountain fort"

-- /*
-- {
--
-- P_ORACLE: Spoke to Oracle in the Tower
--   (0) Haven't spoken to her yet
--   (1) She told you your quest, requested help from monsters
--   (2) You helped with monsters (stopped them from getting to the portal) (not done)
--
-- P_ORACLEMONSTERS: Status of the monsters in the caves below
--   (0) You haven't even seen the monsters take the Statue thru the portal
--   (1) Monsters thru portal: Haven't told Oracle about it
--   (2) Monsters thru portal: Told Oracle about it
--   (3) Followed monsters thru portal: monsters still in cave
--   (4) Oracle asks you to take care of monsters; they're still there
--   (5) Monsters are gone (not done)
--
-- P_FTOTAL: Total number of floor switches activated
-- P_FLOOR1..P_FLOOR4: Status of this floor switch
-- Hint: press bottom left and top right.
--
-- SI_BSTONES: # of Black stones in your inventory
-- SI_WSTONES: # of White stones in your inventory
-- P_STONE1..P_STONE4: Whether this stone is in your inventory
-- P_WALL1..P_WALL4: Which stone (if any) is in this wall slot
-- Hint: Stones should be placed in the holes in this order
-- from left to right: black, white, white, black.
--
-- P_DOOROPEN: (4: Single treasure, left floor switch)
-- P_DOOROPEN2: (3: Treasure room, bottom)
-- P_TREASUREROOM: (3: Treasure room, top)
--
-- P_TOWEROPEN: Whether you can return and talk to the Oracle
--   (0) Never spoke to Oracle, cannot enter Tower
--   (1) Never spoke to Oracle, can enter Tower
--   (2) Spoke to Oracle, cannot enter Tower
--   (3) Spoke to Oracle, can enter Tower (not done)
--
-- P_DRAGONDOWN: Whether the stone dragon is alive or not
--
-- P_DENORIAN: Status of the Denorian statue
--   (0) You have not spoken to the Dville town council
--   (1) If you refused to help the Denorians
--   (2) You've agreed to help: haven't found Demnas
--   (3) You found Demnas: haven't found the troll
--   (4) You found the troll
--   (5) Broken statue returned to the Denorians
--   (6) 2nd half of broken statue returned to Denorians (not done)
--
-- still need to complete P_ORACLE and P_ORACLEMONSTERS
-- }
-- */


function autoexec()
  refresh()
end


function entity_handler(en)
  local x, y
  -- Oracle
  if (en == 0) then
    -- /*
    -- {
    -- When you talk to the Oracle, she will discuss the following situations with you:
    --   1: Civil war in the goblin lands from the missing OracleStatue
    --   2: Monsters are trying to get into the TravelPoint
    -- These are actually all related, but she does not know that; she suspects as much, though
    --
    -- P_ORACLE: Spoke to Oracle in the Tower
    --   (0) Have not spoken to her yet
    --   (1) Spoke to her; she requested help from monsters
    --   (2) You helped with monsters (stopped them from getting to the portal)
    --
    -- P_ORACLEMONSTERS: Status of the monsters in the caves below
    --   (0) You haven't seen the monsters take the Statue thru the portal
    --   (1) Monsters thru portal: Have not told Oracle about it yet
    --   (2) Monsters thru portal: Told Oracle about it
    --   (3) Followed monsters thru portal: monsters still in cave
    --   (4) Oracle asks you to take care of monsters; they're still there
    --   (5) Monsters are gone
    --
    -- P_DENORIAN: Status of the stolen statue
    --   (0) You have not spoken to the Dville town council
    --   (1) If you refused to help the Denorians
    --   (2) You've agreed to help: haven't found Demnas
    --   (3) You found Demnas: haven't found the troll
    --   (4) You found the troll
    --   (5) Broken statue returned to the Denorians
    --   (6) 2nd half of broken statue returned to Denorians
    -- }
    -- */

    if (get_progress(P_ORACLEMONSTERS) == 0) then
      -- 0: // You haven't seen the monsters take the Statue thru the portal
      if (get_progress(P_ORACLE) == 0) then
        bubble(en, _"$0, thank you for coming.")
        if (get_numchrs() == 1) then
          bubble(HERO1, _"Hello, um... Oracle. How did you know I was coming?")
        else
          bubble(HERO1, _"Hello, um... Oracle. How did you know we were coming?")
        end
        bubble(en, _"Tsorin sent a messenger here, of course.")
        bubble(en, _"I'm surprised that you came up to see me, seeing as how the TravelPoint is back in the caves from whence you came.")
        if (get_progress(P_DENORIAN) == 0) then
          -- 0: // Haven't spoken to Denorian Council, so know nothing about
          --    // where to look.
          bubble(en, _"I am glad you stopped by, though. There is civil unrest in the goblin lands to the south.")
          bubble(en, _"The Statue, which I gave to the Denorians as a gift, has been stolen. Because of that, there is much unrest.")
          if (get_numchrs() == 1) then
            bubble(HERO1, _"I am a bit hurried in my quest to find... well, something of importance.")
          else
            bubble(HERO1, _"We are a bit hurried in our quest to find... well...")
            bubble(HERO2, _"To find, um... something of importance.")
          end
          bubble(en, _"Yes, but would you consider helping my people find their missing Statue?")
          if (prompt(en, 2, 1, "Anything you can do would",
                     "help.",
                     "  yes",
                     "  no") == 0) then
            bubble(en, _"Thank you so much. Please talk to the head of the Denorian Council to find out more information.")
          else
            bubble(en, _"That is too bad. But if you would like to help in the future, please talk to the head of the Denorian Council.")
          end
          bubble(en, _"The Denorian village is south of here, by the mountain stream.")
        elseif (get_progress(P_DENORIAN) == 1) then
          -- 1: // You refused the Denorian's plea and were kicked out of town
          bubble(en, _"I am glad you stopped by, though. There is civil unrest in the goblin lands to the south.")
          if (get_numchrs() == 1) then
            bubble(HERO1, _"Yes, I know. The Denorian Council told me about the missing statue.")
          else
            bubble(HERO1, _"Yes, we know. The Denorian Council told us about the missing statue.")
          end
          bubble(en, _"Oh! So you've agreed to help?")
          bubble(HERO1, _"Well ...mumble... no...")
          if (get_numchrs() == 1) then
            bubble(HERO1, _"I am a bit hurried in my quest to find... well, something of importance.")
          else
            bubble(HERO2, _"Not exactly...")
            bubble(HERO1, _"We are a bit hurried in our quest to find... well, something of importance.")
          end
          bubble(en, _"Oh, I see. Your quest must be very important, then. Very well, I shall ask someone else.")
        elseif (get_progress(P_DENORIAN) == 2) then
          -- 2: // You told the Denorian Council you would help (haven't found
          --    // anything yet)
          bubble(en, _"I am glad you stopped by, though. There is civil unrest in the goblin lands to the south.")
          if (get_numchrs() == 1) then
            bubble(HERO1, _"Yes, I know. The Denorian Council told me about the missing statue but I haven't found anything yet.")
          else
            bubble(HERO1, _"Yes, we know. The Denorian Council told us about the missing statue.")
            bubble(HERO2, _"We haven't found anything yet, however.")
          end
          bubble(en, _"Don't worry; I'm sure you'll find something. Good luck to you!")
        elseif (get_progress(P_DENORIAN) == 3) then
          -- 3: // You found Demnas (but not the troll), but didn't report it
          --    // to the Denorians; you still have the the broken statue with
          --    // you.
          bubble(en, _"I am glad you stopped by, though. I hear some of the civil unrest in the goblin lands has let up somewhat.")
          bubble(HERO1, _"Yes, a Narakian named Demnas supposedly stole the missing statue.")
          bubble(en, _"And did you get the statue back?")
          bubble(HERO1, _"Well, only half of it. It seems it was broken in half before we ever got to it.")
          if (get_numchrs() == 1) then
            bubble(en, _"Which means someone may still have it.")
          else
            bubble(HERO2, _"Which means someone probably still has it.")
          end
          bubble(en, _"Could you do me a favor?")
          if (prompt(en, 2, 0, "Could you find out who has",
                     "the other half of the statue?",
                     "  yes",
                     "  no") == 0) then
            bubble(en, _"Thank you so very much.")
          else
            bubble(en, _"Well, that's alright. You've done enough already. Thank you so very much.")
          end
        elseif (get_progress(P_DENORIAN) == 4) then
          -- 4: You found the troll, too, but you did not return the broken
          --    statue to the Denorians. You still have it with you.
          bubble(en, _"I am glad you stopped by, though. I hear...")
          bubble(en, _"...wait!", "What is that statue you have there?")
          if (get_numchrs() == 1) then
            bubble(HERO1, _"This is the Denorian Statue. I rescued it from a guy named Demnas and his troll.")
          else
            bubble(HERO1, _"This is the Denorian Statue. We rescued it from a guy named Demnas and his troll.")
          end
          bubble(en, _"So you did... What happened to it? Part of it is missing.")
          if (get_progress(P_TALK_CORIN) == 0) then
            -- // You fought the troll but didn't talk to Corin, so still
            -- // don't know about Malkaron. Which was stupid, since he was
            -- // standing RIGHT THERE.
            if (get_numchrs() == 1) then
              bubble(HERO1, _"I'm not too sure. He accused me of trying to steal the second half of it from him.")
            else
              bubble(HERO2, _"We're not too sure. He accused us of trying to steal the second half of it from him.")
            end
            bubble(en, _"Second half? It sounds as if someone else may have gotten to him first.")
            if (get_numchrs() == 1) then
              bubble(HERO1, _"That's what I think as well.")
            else
              bubble(HERO2, _"That's what we think as well.")
            end
            bubble(en, _"Well, either way, if you could return that half back to the Denorian village, it will help with the unrest.")
          else
            -- // You fought the troll and Corin told you about Malkaron's
            -- // army stealing the statue.
            bubble(HERO1, _"It was Malkaron's armies. They had a spat with Demnas and when the statue broke, they stole half of it.")
            if (get_pidx(0) == CORIN) then
              bubble(HERO1, _"Demnas thought I was with them and locked me in with a troll.")
            else
              bubble(HERO1, _"Demnas thought CORIN was with them and locked him in with a troll.")
            end
            bubble(en, _"Oh, how awful!")
            bubble(HERO1, _"It's alright. Everything worked out. But now we have to consider Malkaron's involvement in this.")
            bubble(en, _"Yes, I certainly hope you get to the bottom of this.")
            bubble(en, _"Would you return the statue to the Denorian village? It is a very important treasure.")
          end
        elseif (get_progress(P_DENORIAN) == 5) then
          -- 5: The broken statue was returned to the village
          bubble(en, _"I am glad you stopped by. The Denorians sent word that you returned part of their statue.")
          bubble(HERO1, _"Yea, well, someone had to do something about that whole mess...")
          if (get_progress(P_TALK_CORIN) == 0) then
            bubble(en, _"An important question is, who has the other half?")
            if (get_numchrs() == 1) then
              bubble(HERO1, _"That's a good question. I'm searching for the same person... or persons, for that matter.")
            else
              bubble(HERO1, _"That's a good question. We're searching for the same person.")
              bubble(HERO2, _"Or persons, for that matter.")
            end
            bubble(en, _"Be careful. Whoever is out there may do rash and unpredictable things if he thinks you are following him.")
          else
            bubble(en, _"An important question is, what will we do about Malkaron's followers?")
            if (get_numchrs() == 1) then
              bubble(HERO1, _"That's actually what I'm trying to find out.")
            else
              bubble(HERO1, _"That's actually what we're trying to find out.")
            end
            bubble(en, _"If Malkaron is responsible for all this upheaval, you must be careful.")
            bubble(en, _"One as strong as he may do rash and unpredictable things if he thinks you are following him.")
          end
          if (get_numchrs() == 1) then
            bubble(HERO1, _"Yes, I was warned to keep my party small.")
          else
            bubble(HERO1, _"Yes, we were warned to keep our numbers small. That's why we're traveling in a party of only two.")
          end
          bubble(en, _"Well, I wish you luck. Thank you for all you've done already.")
        elseif (get_progress(P_DENORIAN) == 6) then
          -- 6: // We can't get here with P_ORACLEMONSTERS == 0, as the player
          --    // would have seen Malkaron's men take the statue thru the
          --    // portal already.
        end

        set_autoparty(1)
        move_entity(HERO1, "oracle", 0)

        if (get_numchrs() == 1) then
          wait_for_entity(HERO1, HERO1)

          bubble(HERO1, _"This whole thing sure seems fishy. I hope I can figure out what's going on.")
          bubble(HERO1, _"I wonder if this statue has anything to do with this missing staff Nostik wants everyone to find?")
          set_ent_facing(HERO1, FACE_DOWN)
          drawmap()
          screen_dump()
          bubble(HERO1, _"...I should probably stop talking to myself too.")
        elseif (get_numchrs() == 2) then
          x, y = marker("oracle")
          move_entity(HERO2, x, y + 1, 0)
          wait_for_entity(HERO1, HERO2)
          orient_heroes()

          bubble(HERO2, _"Hey $0.")
          set_ent_facing(HERO1, FACE_DOWN)
          bubble(HERO1, _"Yeah?")
          bubble(HERO2, _"Do you think that this statue could have something to with this missing staff Nostik wants us to find?")
          bubble(HERO1, _"Whoa, $1. Keep your voice down!")
          bubble(HERO2, _"Oh, right. Sorry.")
          bubble(HERO2, _"So... do you?")
          bubble(HERO1, _"I never thought of that. Let's hope that it doesn't. We have no choice but to go on anyways.")
          bubble(HERO2, _"Yeah, I guess you're right. Let's go.")
        end

        bubble(en, _"Oh, one last thing. I have a request of you before you go.")
        if (get_numchrs() == 2) then
          set_ent_facing(HERO2, FACE_DOWN)
        end
        bubble(en, _"If you can, please get rid of the monsters in the caves below. You can also use my TravelPoint in the caves as well.")
        bubble(en, _"Good luck. I hope to meet with you again.")

        set_autoparty(0)
        set_progress(P_ORACLE, 1)
      elseif (get_progress(P_ORACLE) == 1) then
        bubble(en, _"The monsters in the caves below appeared out of nowhere. Please get rid of them so travelers who wish to use my TravelPoint may do so.")
      else
        -- We will never get P_ORACLE > 1 here since P_ORACLEMONSTERS will
        -- modify that separately.
      end
    elseif (get_progress(P_ORACLEMONSTERS) == 1) then
      -- 1: // Regardless of the status of P_ORACLE, you will tell her of
      --    // Malkaron's monsters with the statue.
      if (get_progress(P_DENORIAN) == 0) then
        -- // You don't know what's going on with the Oracle's statue
        if (get_numchrs() == 1) then
          bubble(HERO1, _"Oracle! I saw Malkaron's monsters take some statue that looked like you through the portal below!")
          bubble(HERO1, _"At least, it looked like part of a statue, anyway.")
        else
          bubble(HERO2, _"Oracle! We saw Malkaron's monsters take some statue that looked like you through the portal below!")
          bubble(HERO1, _"At least, it looked like part of a statue, anyway.")
        end
        bubble(en, _"That must have been the Denorian's statue; that has been stolen from the village to the south.")
      else
        -- // You know that the Oracle's statue had been stolen so you know to
        -- // tell the Oracle
        if (get_numchrs() == 1) then
          bubble(HERO1, _"Oracle! I saw Malkaron's monsters take the stolen statue through the portal below!")
        else
          bubble(HERO1, _"Oracle! We saw Malkaron's monsters take your statue through the portal below!")
        end
      end
      bubble(en, _"You must go after it! That statue is very important to my people.")
      set_progress(P_ORACLEMONSTERS, 2)
    elseif (get_progress(P_ORACLEMONSTERS) == 2) then
      -- 2: // You told the Oracle about the monsters going thru the portal
      bubble(en, _"Please hurry and get that statue back!")
    elseif (get_progress(P_ORACLEMONSTERS) == 3) then
      -- 3: // Followed Malkaron's men thru portal; monsters still in cave
      bubble(en, _"Welcome back.")
      bubble(HERO1, _"Oracle! The monsters went through the portal below with your statue!")
      if (get_numchrs() == 1) then
        bubble(HERO1, _"I went through the portal, but was sent to Maldea somehow.")
      else
        bubble(HERO1, _"We went through the portal, but were was sent to Maldea somehow.")
      end
      bubble(en, _"Yes, the statue probably disrupted the portal somehow. I'm glad to see you made it back safely.")
      if (get_progress(P_ORACLE) == 0) then
        bubble(en, _"I ask you to find the missing statue, if you possibly can.")
      else
        bubble(en, _"Please continue your search for the statue. It is very important to my people.")
      end
      bubble(en, _"And would you be able to take care of the monsters in the caves below?")
      set_progress(P_ORACLEMONSTERS, 4)
    elseif (get_progress(P_ORACLEMONSTERS) == 4) then
      -- 4: // Oracle asked you to take care of monsters; they're still there
      bubble(en, _"I thank you for looking into the missing statue situation.")
      bubble(en, _"There are still monsters lurking around in the caves below. Please get rid of them.")
    elseif (get_progress(P_ORACLEMONSTERS) == 5) then
      -- 5: // Monsters are gone
      bubble(en, _"The monsters are gone! Thank you so much!")
    end

  end
end


function postexec()
  return
end


function refresh()
  local x, y

  showch("treasure1", 33)
  showch("treasure2", 34)
  showch("treasure3", 35)
  showch("treasure4", 36)
  showch("treasure5", 37)
  showch("treasure6", 38)
  showch("treasure7", 39)
  showch("treasure8", 40)
  showch("treasure9", 41)
  showch("treasure10", 42)
  showch("treasure11", 43)
  showch("treasure12", 44)

  -- Dragon guard (1: Main entrance)
  LOC_draw_dragon()

  -- Floor switches (2: Floor switches)
  LOC_set_floor("floor1", P_FLOOR1, "floor1a", "floor1b")
  LOC_set_floor("floor2", P_FLOOR2, "floor2a", "floor2b")
  LOC_set_floor("floor3", P_FLOOR3, "floor3a", "floor3b")
  LOC_set_floor("floor4", P_FLOOR4, "floor4a", "floor4b")

  -- Wall holes (3: Treasure room, top left)
  LOC_set_wall("wall1", P_WALL1)
  LOC_set_wall("wall2", P_WALL2)
  LOC_set_wall("wall3", P_WALL3)
  LOC_set_wall("wall4", P_WALL4)

  -- Door (3: Treasure room, top)
  LOC_set_door("door3_1", P_TREASUREROOM, 40)

  -- Door (3: Treasure room, bottom)
  LOC_set_door("door3_2", P_DOOROPEN2, 10)

  -- Door (4: Single treasure, left floor switch)
  if (get_progress(P_DOOROPEN) == 1) then
    x, y = marker("switch")
    set_btile(x, y, 190)
    set_btile(x + 1, y, 189)
  else
    x, y = marker("switch")
    set_btile(x, y, 189)
    set_btile(x + 1, y, 190)
  end
  LOC_set_door("door4", P_DOOROPEN, 10)

  -- Black stone (1: Main entrance, right)
--  LOC_get_stone("stone1", P_STONE1, 20, 222)

  -- White stone (2: Floor switches, bottom left)
--  LOC_get_stone("stone2", P_STONE2, 21, 221)

  -- White stone (3: Treasure room, top right)
--  LOC_get_stone("stone3", P_STONE3, 22, 221)

  -- Black stone (4: Single treasure, treasure chest)
  if (get_progress(P_STONE4) == 1) then
    showch("stone4", -1)
  end
end


-- Show the status of a chest
function showch(which_marker, which_chest)
  -- Set tiles if -1 passed in as 'which_chest' or if chest already opened
  if (which_chest < 0 or get_treasure(which_chest) == 1) then
    set_mtile(which_marker, 65)
    set_zone(which_marker, 0)
  end
end


function zone_handler(zn)
  -- Combat can be anywhere that other zones do not populate
  if (zn == 0) then
    combat(54)

  -- Front doors (1: Main entrance)
  elseif (zn == 1) then
    if (get_progress(P_ORACLE) > 0) then
      set_progress(P_TOWEROPEN, 2)
    end
    LOC_reset_progress()
    change_map("main", "tower")

  -- Stairs up (1: Main entrance -> 2: Floor switches)
  elseif (zn == 2) then
    warp("dstairs2", 8)

  -- Stairs down (2: Floor switches -> 1: Main Entrance)
  elseif (zn == 3) then
    warp("ustairs1", 8)

  -- Stairs up (2: Floor switches -> 3: Treasure room)
  elseif (zn == 4) then
    warp("dstairs3", 8)

  -- Stairs down (3: Treasure room -> 2: Floor switches)
  elseif (zn == 5) then
    warp("ustairs2", 8)

  -- Stairs up (3: Treasure room -> 4: Single treasure)
  elseif (zn == 6) then
    warp("dstairs4", 8)

  -- Stairs down (4: Single treasure -> 3: Treasure room)
  elseif (zn == 7) then
    warp("ustairs3", 8)

  -- Stairs up (4: Single treasure -> 5: Oracle room)
  elseif (zn == 8) then
    warp("dstairs5", 8)

  -- Stairs down (5: Oracle room -> 4: Single treasure)
  elseif (zn == 9) then
    warp("ustairs4", 8)

  -- Locked doors (3: Treasure room) or (4: Single treasure)
  elseif (zn == 10) then
    bubble(HERO1, _"Locked.")

  -- Oracle bookshelves (5: Oracle room)
  elseif (zn == 11) then
    bubble(HERO1, _"Wow! There are a lot of strange books here.")

  -- Floor switch (2: Floor switches, top left)
  elseif (zn == 12) then
    LOC_floor_switch(P_FLOOR1)

  -- Floor switch (2: Floor switches, top right)
  elseif (zn == 13) then
    LOC_floor_switch(P_FLOOR3)

  -- Floor switch (2: Floor switches, bottom left)
  elseif (zn == 14) then
    LOC_floor_switch(P_FLOOR2)

  -- Floor switch (2: Floor switches, bottom right)
  elseif (zn == 15) then
    LOC_floor_switch(P_FLOOR4)

  -- Wall switch (3: Treasure room, top left)
  elseif (zn == 16) then
    LOC_stoner("wall1", P_WALL1)

  -- Wall switch (3: Treasure room, top right)
  elseif (zn == 17) then
    LOC_stoner("wall2", P_WALL2)

  -- Wall switch (3: Treasure room, bottom left)
  elseif (zn == 18) then
    LOC_stoner("wall3", P_WALL3)

  -- Wall switch (3: Treasure room, bottom right)
  elseif (zn == 19) then
    LOC_stoner("wall4", P_WALL4)

  -- Black stone (1: Main entrance, right)
  elseif (zn == 20) then
    if (get_progress(P_STONE1) == 0) then
      set_progress(P_STONE1, 1)
      add_special_item(SI_BLACKSTONE)
      sfx(5)
      msg(_"Black Stone procured!", 15, 0)
      LOC_get_stone("stone1", P_STONE1, 20, 222)
    end

  -- White stone (2: Floor switches, bottom left)
  elseif (zn == 21) then
    if (get_progress(P_STONE2) == 0) then
      set_progress(P_STONE2, 1)
      add_special_item(SI_WHITESTONE)
      sfx(5)
      msg(_"White Stone procured!", 15, 0)
      LOC_get_stone("stone2", P_STONE2, 21, 221)
    end

  -- White stone (3: Treasure room)
  elseif (zn == 22) then
    if (get_progress(P_STONE3) == 0) then
      set_progress(P_STONE3, 1)
      add_special_item(SI_WHITESTONE)
      sfx(5)
      msg(_"White Stone procured!", 15, 0)
      LOC_get_stone("stone3", P_STONE3, 22, 221)
    end

  -- Treasure chest (4: Single treasure)
  elseif (zn == 23) then
    if (get_progress(P_STONE4) == 0) then
      set_progress(P_STONE4, 1)
      add_special_item(SI_BLACKSTONE)
      sfx(5)
      msg(_"Black Stone procured!", 15, 0)
      refresh()
    end

  -- Floor switch (4: Single treasure, left)
  elseif (zn == 24) then
    if (get_progress(P_DOOROPEN) == 0) then
      set_progress(P_DOOROPEN, 1)
      LOC_set_door("door4", P_DOOROPEN, 10)
      refresh()
    end

  -- Floor switch (4: Single treasure, right)
  elseif (zn == 25) then
    if (get_progress(P_DOOROPEN) == 1) then
      set_progress(P_DOOROPEN, 0)
      sfx(26)
      refresh()
    end

  -- Floor switch (5: Oracle room, left)
  elseif (zn == 26) then
    if (get_progress(P_DOOROPEN2) == 0) then
      bubble(HERO1, _"Oh! I think I stepped on a switch!")
      set_progress(P_DOOROPEN2, 1)
      LOC_set_door("door3_2", P_DOOROPEN2, 10)
      refresh()
    end

  -- Treasure chest (3: Treasure room)
  elseif (zn == 27) then
    chest(33, I_PCURING, 1)
    refresh()

  -- Treasure chest (3: Treasure room)
  elseif (zn == 28) then
    chest(34, I_LTONIC, 1)
    refresh()

  -- Treasure chest (3: Treasure room)
  elseif (zn == 29) then
    chest(35, I_SPEEDBOOTS, 1)
    refresh()

  -- Treasure chest (3: Treasure room)
  elseif (zn == 30) then
    chest(36, I_SWORD3, 1)
    refresh()

  -- Treasure chest (3: Treasure room)
  elseif (zn == 31) then
    chest(37, I_ROBE3, 1)
    refresh()

  -- Treasure chest (3: Treasure room)
  elseif (zn == 32) then
    chest(38, 0, 400)
    refresh()

  -- Treasure chest (3: Treasure room)
  elseif (zn == 33) then
    chest(39, I_B_WHIRLWIND, 1)
    refresh()

  -- Treasure chest (3: Treasure room)
  elseif (zn == 34) then
    chest(40, I_EDAENRA, 1)
    refresh()

  -- Treasure chest (3: Treasure room)
  elseif (zn == 35) then
    chest(41, I_SSTONE, 1)
    refresh()

  -- Treasure chest (3: Treasure room)
  elseif (zn == 36) then
    chest(42, I_MESRA, 1)
    refresh()

  -- Treasure chest (3: Treasure room)
  elseif (zn == 37) then
    chest(43, I_STAFF2, 1)
    refresh()

  -- Treasure chest (3: Treasure room)
  elseif (zn == 38) then
    chest(44, I_GAUNTLET2, 1)
    refresh()

  -- Fire (5: Oracle room)
  elseif (zn == 39) then
    touch_fire(party[0])

  -- Locked door (3: Treasure room)
  elseif (zn == 40) then
    if (get_progress(P_WALL1) == 1 and get_progress(P_WALL2) == 2 and get_progress(P_WALL3) == 2 and get_progress(P_WALL4) == 1) then
      set_progress(P_TREASUREROOM, 1)
      LOC_set_door("door3_1", P_TREASUREROOM, 40)
      refresh()
    end

  -- zn == 41: no enemies attack (3: Treasure room, 5: Oracle room)

  -- Guardian Dragon (1: Main entrance)
  elseif (zn == 42) then
    if (get_progress(P_DRAGONDOWN) == 0) then
      bubble(255, _"The Oracle is not to be disturbed!")
      drawmap()
      screen_dump()
      set_run(0)
      combat(55)
      set_run(1)
      if (get_alldead() == 0) then
        set_progress(P_DRAGONDOWN, 1)
        LOC_draw_dragon()
      else
        return
      end
    end

  -- Save spot (5: Oracle room)
  elseif (zn == 43) then
    set_save(1)
    set_sstone(1)
    sfx(7)

  -- Off of Save spot (5: Oracle room)
  elseif (zn == 44) then
    set_save(0)
    set_sstone(0)

  end
end


function LOC_draw_dragon()
  local a
  local x, y = marker("dragon")

  if (get_progress(P_DRAGONDOWN) == 0) then
    -- Dragon icon tile (223..228) in the for..loop below:
    local b = 223
    for a = x, x + 2, 1 do
      set_ftile(a, y - 2, b)
      set_ftile(a, y - 1, b + 3)
      b = b + 1
    end
    -- Dragon icon tile (229..230) in the for..loop below:
    b = 229
    for a = x, x + 1, 1 do
      set_btile(a, y, b)
      set_obs(a, y, 1)
      set_zone(a, y, 42)
      b = b + 1
    end
  elseif (get_progress(P_DRAGONDOWN) == 1) then
    for a = x, x + 2, 1 do
      set_ftile(a, y - 2, 0)
      set_ftile(a, y - 1, 0)
    end
    for a = x, x + 1, 1 do
      set_btile(a, y, 160)
      set_obs(a, y, 0)
      set_zone(a, y, 0)
    end
  end
end


function LOC_floor_switch(p_floor)
  if (get_progress(p_floor) == 0) then
    if (get_progress(P_FTOTAL) < 2) then
      set_progress(p_floor, 1)
      set_progress(P_FTOTAL, get_progress(P_FTOTAL) + 1)
    end
  else
    set_progress(p_floor, 0)
    set_progress(P_FTOTAL, get_progress(P_FTOTAL) - 1)
  end
  refresh()
end


function LOC_get_stone(stone, p_stone, zone, color)
  local x, y = marker(stone)
  if (get_progress(p_stone) == 0) then
    set_ftile(x, y, color)
    set_zone(x, y, zone)
    set_obs(x, y, 1)
    return
  elseif (get_progress(p_stone) == 1) then
    set_ftile(x, y, 0)
    set_zone(x, y, 0)
    set_obs(x, y, 0)
    return
  end
end


function LOC_reset_progress()
  -- Reset the Black and White stones on the ground
  set_progress(P_STONE1, 0)
  set_progress(P_STONE2, 0)
  set_progress(P_STONE3, 0)
  set_progress(P_STONE4, 0)
  remove_special_item(SI_BLACKSTONE)
  remove_special_item(SI_WHITESTONE)

  -- Reset the stones located in the walls
  set_progress(P_WALL1, 0)
  set_progress(P_WALL2, 0)
  set_progress(P_WALL3, 0)
  set_progress(P_WALL4, 0)

  -- Close the open-by-switch doors
  set_progress(P_DOOROPEN, 0)
  set_progress(P_DOOROPEN2, 0)
  set_progress(P_TREASUREROOM, 0)

  -- Reset the holes on the 2nd floor
  set_progress(P_FTOTAL, 0)
  set_progress(P_FLOOR1, 0)
  set_progress(P_FLOOR2, 0)
  set_progress(P_FLOOR3, 0)
  set_progress(P_FLOOR4, 0)
end


function LOC_set_door(door, p_door, zone)
  local x, y = marker(door)
  if (get_progress(p_door) == 0) then
    set_ftile(x, y - 1, 169)
    set_btile(x, y - 1, 178)
    set_btile(x, y, 170)
    set_obs(x, y - 1, 1)
    set_zone(x, y - 1, zone)
  else
    set_ftile(x, y - 1, 175)
    set_btile(x, y, 176)
    set_obs(x, y - 1, 0)
    set_zone(x, y - 1, 0)
  end
  sfx(26)
end


function LOC_set_floor(floor, p_floor, floor_a, floor_b)
  local x, y = marker(floor)
  local x1, y1 = marker(floor_a)
  local x2, y2 = marker(floor_b)
  if (get_progress(p_floor) == 0) then
    set_btile(x, y, 189)
    set_btile(x1, y1, 0)
    set_obs(x1, y1, 1)
    set_btile(x2, y2, 0)
    set_obs(x2, y2, 1)
  else
    set_btile(x, y, 190)
    set_btile(x1, y1, 160)
    set_obs(x1, y1, 0)
    set_btile(x2, y2, 160)
    set_obs(x2, y2, 0)
  end
  sfx(26)
end


function LOC_set_wall(wall, p_wall)
  local x, y = marker(wall)
  if (get_progress(p_wall) == 0) then
    set_ftile(x, y, 220)  -- No stone
  elseif (get_progress(p_wall) == 1) then
    set_ftile(x, y, 222)  -- White stone
  elseif (get_progress(p_wall) == 2) then
    set_ftile(x, y, 221)  -- Black stone
  end
end


function LOC_stoner(wall, p_wall)
  if (get_progress(p_wall) == 0) then

    -- No stones picked up
    if (not has_special_item(SI_BLACKSTONE) and not has_special_item(SI_WHITESTONE)) then
      return
    end

    -- Black stones picked up but no White stones
    if (has_special_item(SI_BLACKSTONE) and not has_special_item(SI_WHITESTONE)) then
      set_progress(p_wall, 1)
      add_special_item(SI_BLACKSTONE, -1)
      sfx(5)
      LOC_set_wall(wall, p_wall)
      return
    end

    -- White stones picked up but no Black stones
    if (not has_special_item(SI_BLACKSTONE) and has_special_item(SI_WHITESTONE)) then
      set_progress(p_wall, 2)
      add_special_item(SI_WHITESTONE, -1) -- remove 1 white stone
      sfx(5)
      LOC_set_wall(wall, p_wall)
      return
    end

    -- At least one of each color stone picked up
    if (prompt(255, 2, 0, _"What stone will you place?",
                          _"  black",
                          _"  white") == 0) then
      set_progress(p_wall, 1)
      add_special_item(SI_BLACKSTONE, -1)
      sfx(5)
      LOC_set_wall(wall, p_wall)
      return
    else
      set_progress(p_wall, 2)
      add_special_item(SI_WHITESTONE, -1)
      sfx(5)
      LOC_set_wall(wall, p_wall)
      return
    end

  else
    -- Actually, this is already implicitly true
    if ((get_progress(p_wall) ~= 0) and (get_progress(P_TREASUREROOM) == 0)) then
      if (get_progress(p_wall) == 1) then
        -- Remove a White stone
        set_progress(p_wall, 0)
        add_special_item(SI_BLACKSTONE)
        sfx(4)
        LOC_set_wall(wall, p_wall)
      elseif (get_progress(p_wall) == 2) then
      -- Remove a Black stone
        set_progress(p_wall, 0)
        add_special_item(SI_WHITESTONE)
        LOC_set_wall(wall, p_wall)
        return
      end
    end
  end
end
