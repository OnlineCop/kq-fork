-- cave4 - "Cave south of Oracle's tower and west of Denorian village"

-- /*
-- {
-- P_BRONZEKEY: Key into the room with Corin and the troll
--   (0) Don't have the key yet
--   (1) Got the key from Demnas
--   (2) Used the key to open the bronze door
--
-- P_DEMNASDEAD: Whether you defeated Demnas
--   (0) Still kickin'
--   (1) Dead
--
-- P_DENORIAN: Status of Denorian's statue
--   (0)..(2) Setup in dville.lua
--   (3) You have the Oracle's broken statue, haven't met troll
--   (4) Defeated troll
--
-- P_TALK_CORIN: If you've spoken to Corin
--   (0) Haven't spoken to him yet
--   (1) He tells you about the connection of the troll and Malkaron's armies
--
-- }
-- */

function autoexec()
  copy_tile_all(39, 92, 21, 46, 5, 8)
  refresh()
end


function entity_handler(en)
  if (en == 0) then
    LOC_talk_demnas(en)

  elseif (en == 1) then
    -- You have not spoken to Corin about the troll
    if (get_progress(P_TALK_CORIN) == 0) then
      if (get_progress(P_DENORIAN) < 4) then
        -- You have not yet faced the troll down the stairs
        bubble(en, _"Oh, $0, it's great to see you!")
        bubble(en, _"I traced the missing statue here to Demnas, but before I could confront him, a horde of Malkaron's followers attacked him.")
        bubble(HERO1, _"Malkaron's minions have been here?")
        bubble(en, _"Yes, and during the battle, the statue was broken in half and Malkaron's men took off with it.")
        bubble(en, _"Demnas was so enraged that when he found me, he locked me in this room with this troll.")
        bubble(HERO1, _"A troll? So there really IS a troll? How did you defeat it?")
        bubble(en, _"I haven't. I hit it with a sleep spell as soon as I was locked in here.")
        set_ent_facing(en, FACE_RIGHT)
        bubble(en, _"It's down the stairs right there. Why don't you go take care of it now?")
        set_progress(P_TALK_CORIN, 1)
      else
        -- You have defeated the troll
        bubble(en, _"$0! I saw you run down the stairs there before I could tell you about the troll!")
        bubble(HERO1, _"Yea, the troll was asleep, but it wasn't much of a problem to beat.")
        bubble(en, _"My, my. So modest.")
        if (get_numchrs() == 1) then
          bubble(HERO1, _"Well, what can I say? I'm just that good!")
        else
          bubble(HERO2, _"Well, what can we say? We're just that good!")
        end
        bubble(en, _"If that's so, then let me join your party!")
        LOC_join_corin(en)
      end
    -- Corin told you about the troll
    elseif (get_progress(P_TALK_CORIN) == 1) then
      -- You have not fought the troll yet
      if (get_progress(P_DENORIAN) < 4) then
        bubble(en, _"I'll wait up here. I'm not ready to take on a troll just yet.")
      -- You fought the troll
      else
        bubble(en, _"Good work! Let me join your team!")
        LOC_join_corin(en)
      end
    else
      bubble(en, _"Hi, $0. Let me join your team!")
      LOC_join_corin(en)
    end
    refresh()

  elseif (en == 2) then
    -- Statue will not talk, but Demnas attacks if you try to grab it
    en = 0
    bubble(en, _"Not so fast!")
    set_ent_facing(HERO1, FACE_DOWN)
    LOC_talk_demnas(en)

  end
end


function postexec()
  return
end


function refresh()
  showch("treasure1", 62)
  showch("treasure2", 63)
  showch("treasure3", 64)
  showch("treasure4", 65)
  showch("treasure5", 66)

  if (get_progress(P_DEMNASDEAD) > 0) then
    set_ent_active(0, 0)
  end

  if (get_progress(P_BRONZEKEY) == 2) then
    set_zone("door4", 2)
    set_obs("door4", 0)
  end

  if (not LOC_manor_or_party(CORIN)) then
    -- Make the NPC look like Corin if he has not been recruited yet
    set_ent_id(1, CORIN)
  else
    -- Otherwise, remove him from the screen
    set_ent_active(1, 0)
  end

  if (get_progress(P_DENORIAN) > 2) then
    set_ent_active(2, 0)
  end
end


function zone_handler(zn)
  if (zn == 0) then
    combat(10)

  elseif (zn == 1) then
    change_map("main", "cave4")

  elseif (zn == 2) then
    LOC_doors_in("door1")
    LOC_doors_in("door2")
    LOC_doors_in("door3")
    LOC_doors_in("door4")
    LOC_doors_in("door5")
    LOC_doors_in("door6")
    LOC_doors_in("door7")
    LOC_doors_in("door8")

  elseif (zn == 3) then
    LOC_doors_out("door1")
    LOC_doors_out("door2")
    LOC_doors_out("door3")
    LOC_doors_out("door4")
    LOC_doors_out("door5")
    LOC_doors_out("door6")
    LOC_doors_out("door7")
    LOC_doors_out("door8")

  elseif (zn == 4) then
    if (get_progress(P_BRONZEKEY) == 0) then
      bubble(HERO1, _"Locked.")
    elseif (get_progress(P_BRONZEKEY) == 1) then
      bubble(HERO1, _"What luck! The key from that crazy Demnas guy unlocks this door!")
      set_progress(P_BRONZEKEY, 2)
      remove_special_item(SI_BRONZEKEY)
      refresh()
    end

  elseif (zn == 5) then
    bubble(HERO1, _"Aw... it's full of normal junk.")

  elseif (zn == 6) then
    chest(62, I_SHADECLOAK, 1)
    refresh()

  elseif (zn == 7) then
    chest(63, I_B_GLOOM, 1)
    refresh()

  elseif (zn == 8) then
    chest(64, I_STRSEED, 1)
    refresh()

  elseif (zn == 9) then
    chest(65, I_SALVE, 2)
    refresh()

  elseif (zn == 10) then
    chest(66, I_ARMOR4, 1)
    refresh()

  elseif (zn == 11) then
    -- /* Before this, you saw the room where the troll was, but you couldn't
    --  * look into it (a problem with set_foreground()). This copies the
    --  * tiles from the bottom-right of the screen to the appropriate place.
    --  */
    copy_tile_all(45, 92, 21, 46, 5, 8)
    warp("ustairs", 8)

    -- /* Now, so you don't see this after you leave that room, send back
    --  * the tiles it used to have in the appropriate place. (We'll just
    --  * call autoexec() since it does it already.
    --  */
  elseif (zn == 12) then
    autoexec()
    warp("dstairs", 8)

  elseif (zn == 13) then
    if (get_progress(P_DENORIAN) < 4) then
      bubble(HERO1, _"The Denorians were right. There really WAS a troll. Looks like it's already dead, though.")
      msg(_"The troll suddenly lunges at you... it was only asleep!", 0, 0)
      set_run(0)
      msg("TODO: There needs to be a combat here...", 0, 0)
      set_run(1)

      bubble(HERO1, _"If this troll really DID steal the statue from the Denorians, it was probably being directed by that scumbag Demnas.")
      bubble(HERO1, _"He used this poor bugger and then left him here to rot.")
      wait(100)
      bubble(HERO1, _"Well, I should go back to the village now and report all this.")
      set_progress(P_DENORIAN, 4)
    else
      bubble(HERO1, _"This cell is now his tomb.")
    end

  elseif (zn == 14) then
    -- Nothing: no random battles here

  elseif (zn == 15) then
    if (get_progress(P_DENORIAN) > 2) then
      msg(_"The statue is no longer here.", 58, 0)
    else
      bubble(HERO1, _"This statue of the Oracle appears to be broken in half!")
    end
  end
end


-- Show the status of a chest
function showch(which_marker, which_chest)
  -- Set tiles if -1 passed in as 'which_chest' or if chest already opened
  if (which_chest < 0 or get_treasure(which_chest) == 1) then
    set_btile(which_marker, 256)
    set_zone(which_marker, 0)
  end
end


function LOC_doors_in(which_marker)
  sfx(26)

  set_foreground(0)
  set_btile(which_marker, 240)
  set_mtile(which_marker, 251)
end


function LOC_doors_out(which_marker)
  sfx(26)

  set_btile(which_marker, 250)
  set_mtile(which_marker, 0)
  set_foreground(1)
end


function LOC_join_corin(en)
  local id

  -- // Give Corin him default equipment
  set_all_equip(CORIN, I_MACE2, I_SHIELD1, I_HELM1, I_ROBE2, I_BAND1, 0)
  id = select_team{CORIN}
  -- // Add the characters that weren't selected to the manor
  add_to_manor(id)

  if (id[1]) then
    set_ent_id(en, id[1])
    set_ent_speed(en, 4)

    if (id[2]) then
      -- Two heroes were de-selected
      set_ent_id(0, id[2])
      set_ent_active(0, 1)
      set_ent_speed(0, 4)
      if (get_ent_tiley(HERO1) > get_ent_tiley(1)) then
        -- Hero is below Corin
        set_ent_tilex(0, get_ent_tilex(en) + 1)
        set_ent_tiley(0, get_ent_tiley(en))
      else
        -- Hero is anywhere else
        set_ent_tilex(0, get_ent_tilex(en))
        set_ent_tiley(0, get_ent_tiley(en) + 1)
      end

      bubble(en, _"If you need us, we'll be back at the manor.")

      set_ent_movemode(0, 2)
      set_ent_movemode(en, 2)
      move_entity(0,  37, 73, 1)
      move_entity(en, 37, 73, 1)
      wait_for_entity(0, en)
    else
      -- One hero was de-selected
      bubble(en, _"If you need me, I'll be back at the manor.")
      set_ent_movemode(en, 2)
      move_entity(en, 37, 73, 1)
      wait_for_entity(en, en)
    end
  end
  set_progress(P_PLAYERS, get_progress(P_PLAYERS) + 1)

end


function LOC_talk_demnas(en)
  bubble(en, _"So you're back trying to steal the other half of the statue from me, are you?")
  bubble(HERO1, _"Huh? Have we met before?")
  bubble(en, _"Filthy swine! You cannot have this statue!")
  bubble(HERO1, _"Does this mean you're the one responsible for the disappearance of the statue from a village near here?")
  bubble(en, _"Uh... Are... Are you accusing me of stealing this statue?!")
  bubble(HERO1, _"Well, the thief was traced back to this cave.")
  bubble(en, _"Oh, so now I'm a thief?!!")
  bubble(HERO1, _"That depends... did you take the statue or not?")
  bubble(en, _"What if I did?")
  bubble(HERO1, _"Well, then you will give it back... or it will be taken by force!")
  bubble(en, _"That has been tried before already. You prove to be as stupid as you look.")
  bubble(en, _"Now I, Demnas, will destroy you!")
  drawmap()
  screen_dump()
  set_run(0)
  combat(11)
  set_run(1)
  if (get_alldead() == 1) then
    return
  end
  set_progress(P_DEMNASDEAD, 1)
  refresh()
  drawmap()
  screen_dump()

  sfx(5)
  msg(_"Bronze key procured", 255, 0)
  set_progress(P_BRONZEKEY, 1)
  add_special_item(SI_BRONZEKEY)

  sfx(5)
  msg(_"Broken Denorian Statue procured", 255, 0)
  set_progress(P_DENORIAN, 3)
  add_special_item(SI_DENORIANSTATUE)
  refresh()
end
