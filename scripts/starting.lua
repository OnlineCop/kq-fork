-- starting - "Various pieces of heroes' homes back in Antoria"

-- /*
-- {
-- P_SKIPINTRO: Whether to show the entire starting again
--   (0) Show the entire starting
--   (1) Do not show everything
-- }
-- */

function autoexec()
  set_desc(0)
  return
end


function entity_handler(en)
  return
end


function postexec()
  if (get_skip_intro() == 1) then
    LOC_choose_hero()
    return
  end

  set_autoparty(1)
  set_vfollow(1)

  -- TT: This makes it a hundred times faster to update scripts and see how
  -- it will appear in real time.  Just comment out everything that you do not
  -- want to sit through every time you recompile the script.
  LOC_storyline(SENSAR)
  LOC_storyline(SARINA)
  LOC_storyline(CORIN)
  LOC_storyline(AJATHAR)
  LOC_storyline(CASANDRA)
  LOC_storyline(TEMMIN)
  LOC_storyline(AYLA)
  LOC_storyline(NOSLOM)

  LOC_meet_nostik()
  LOC_choose_hero()
end


function zone_handler(zn)
  return
end


function LOC_choose_hero()
-- It is unfair to ask the player to choose a character when they know nothing
-- about the game, or what the stats mean. So we automatically choose NOSLOM.
-- The player can set their party however they want later in the game. --WK

  local ptr = 0
  ptr = NOSLOM
  add_chr(ptr)
  set_all_equip(ptr, I_KNIFE1, 0, 0, I_SUIT1, 0, 0)
  change_map("manor", "entrance")

-- We are no longer using this
--[[
  local stop = 0
  local rd = 1
  local a, p

  clear_buffer()
  screen_dump()
  do_fadein(16)
  while (stop == 0) do
    if (rd == 1) then
      clear_buffer()
      for a = 0, 7, 1 do
        if (ptr == a) then
          dark_mbox(a * 32 + 32, 16, 2, 2)
        else
          mbox(a * 32 + 32, 16, 2, 2)
        end
        drawframe(a, 0, a * 32 + 40, 24)
      end
      mbox(80, 64, 18, 5)
      draw_pstat(ptr, 88, 72)
      mbox(80, 120, 18, 8)
      ptext(88, 128, "Strength", 5)
      ptext(88, 136, "Agility", 5)
      ptext(88, 144, "Vitality", 5)
      ptext(88, 152, "Intellect", 5)
      ptext(88, 160, "Sagacity", 5)
      ptext(88, 168, "Speed", 5)
      ptext(88, 176, "Aura", 5)
      ptext(88, 184, "Spirit", 5)
      for p = 0, 7, 1 do
        ptext(184, p * 8 + 128, ":", 5)
        pnum(208, p * 8 + 128, get_party_stats(ptr, p) / 100, 0)
      end
      screen_dump()
    end
    read_controls(0, 0, 1, 1, 1, 0, 0, 0)
    rd = 0
    if (check_key(2) == 1) then
      ptr = ptr - 1
      if (ptr < 0) then
        ptr = 7
      end
      sfx(0)
      rd = 1
    elseif (check_key(3) == 1) then
      ptr = ptr + 1
      if (ptr > 7) then
        ptr = 0
      end
      sfx(0)
      rd = 1
    elseif (check_key(4) == 1) then
      stop = 1
    end
  end
]]--
end


function LOC_meet_nostik()
  local a
  local en = 0

  view_range(1, 116, 12, 141, 31)
  screen_dump()

  set_ent_active(HERO1, 0)

  -- Put a bunch of people around Hunert the butler
  place_ent(3, "butler1")
  set_ent_facing(3, FACE_RIGHT)
  place_ent(4, "butler2")
  set_ent_facing(4, FACE_LEFT)
  place_ent(5, "butler3")
  set_ent_facing(5, FACE_RIGHT)
  place_ent(6, "butler4")
  set_ent_facing(6, FACE_LEFT)
  place_ent(7, "butler5")
  set_ent_facing(7, FACE_UP)
  place_ent(8, "butler6")
  set_ent_facing(8, FACE_UP)
  place_ent(9, "butler7")
  set_ent_facing(9, FACE_UP)
  place_ent(10, "butler8")
  set_ent_facing(10, FACE_UP)

  for a = 0, 7, 1 do
    set_ent_id(3 + a, a)
    set_ent_active(3 + a, 1)
    set_ent_movemode(3 + a, 0)
  end
  set_noe(get_noe() + 8)
  warp("butler", 4)
  set_desc(1)

  bubble(en, _"I would like to thank you all for coming.")
  bubble(en, _"I appreciate all those willing to help. My master has summoned eight of you. Are you all accounted for?")

  set_ent_script(en, "W25F2W25F0W25F3W25F0W30")
  wait_for_entity(en, en)

  bubble(en, _"Excellent. If everyone is ready, I will take you to meet master Nostik...")
  wait(50)

  set_autoparty(1)
end


function LOC_storyline(en)
  local a, b
  add_chr(en)

  if (en == SENSAR) then
    set_ent_facing(HERO1, FACE_UP)
    warp("start_sensar", 16)

    set_ent_speed(HERO1, 4)

    wait(90)

    for a = 0, 1, 1 do
      set_ftile("fire", 537)
      wait(5)
      set_ftile("fire", 538)
      wait(5)
      set_ftile("fire", 539)
      wait(5)
      set_ftile("fire", 538)
      wait(5)
    end

    set_ftile("fire", 537)
    wait(10)

    set_ftile("fire", 0)
    set_ent_script(HERO1, "W40D1")
    wait_for_entity(HERO1, HERO1)

    bubble(HERO1, _"What the..? What was that?")

    wait(25)
    set_ftile("fire", 311)
    set_ent_script(HERO1, "W25F3W25")
    wait_for_entity(HERO1, HERO1)

    bubble(HERO1, _"Hey, a note!")
    set_ftile("fire", 0)

    bubble(HERO1, _"The note is addressed to me!")

    a = get_vx()
    b = get_vy()

    move_camera(a + 64, b + 48, 1)

    thought(HERO1, _"`$0, I am an old friend.",
                   _"This note is intended for your   ",
                   _"eyes only. Haste and discretion  ",
                   _"are of the utmost importance.    ")
    thought(HERO1, _"I implore you to travel to Reisha",
                   _"Mountain with all possible speed.",
                   _"There, a man shall be waiting for",
                   _"your arrival. I beg that you come")
    thought(HERO1, _"at once. Tell no one, as both my ",
                   _"life and yours will be in danger.",
                   _"Please trust me.'                ",
                   _"  -Anonymous                     ")

    move_camera(a, b, 1)

    bubble(HERO1, _"`An old friend?' `Tell no one?' This looks really fishy. And strangely important. I'd better check this out.")

    set_ent_speed(HERO1, 5)
    set_ent_script(HERO1, "D6R4D4")
    wait_for_entity(HERO1, HERO1)

    sfx(25)

  elseif (en == SARINA) then
    warp("start_sarina", 4)

    set_ent_speed(HERO1, 4)

    set_ent_script(HERO1, "L17")
    wait_for_entity(HERO1, HERO1)

    for a = 0, 1, 1 do
      set_ftile("wall", 537)
      wait(10)
      set_ftile("wall", 538)
      wait(10)
      set_ftile("wall", 539)
      wait(10)
      set_ftile("wall", 538)
      wait(10)
    end

    set_ftile("wall", 537)
    wait(20)

    set_ftile("wall", 0)
    bubble(HERO1, _"..huh?")

    set_ent_script(HERO1, "R2F1W30")
    wait_for_entity(HERO1, HERO1)

    bubble(HERO1, "$0:", _"What was that?")
    set_ftile("wall", 310)
    wait(30)
    bubble(HERO1, "$0:", _"That wasn't there a second ago!")

    set_ent_script(HERO1, "U1")
    wait_for_entity(HERO1, HERO1)

    bubble(HERO1, _"Wow, what a strange request.")
    bubble(HERO1, _"On the other hand, maybe there's something here that I can use to help Mom and Dad.")
    bubble(HERO1, _"I'll have to let them know right away!")

    set_ent_speed(HERO1, 5)

    set_ftile("wall", 0)
    set_ent_script(HERO1, "W25D1R15")
    wait_for_entity(HERO1, HERO1)

  elseif (en == CORIN) then
    warp("start_corin", 4)

    set_ent_speed(HERO1, 4)
    set_ent_script(HERO1, "U7")
    wait_for_entity(HERO1, HERO1)

    bubble(HERO1, "$0:", _"Oh, no. I hope this isn't another eviction notice.")

    wait(25)
    set_mtile("door", 0)
    wait(50)
    bubble(HERO1, _"Hey now, what's this?")
    wait(50)

    set_ent_facing(HERO1, FACE_DOWN)
    wait(50)

    bubble(HERO1, _"Hmm... I don't like the sound of this one bit. I wonder if I even know who this anonymous fellow is?")
    bubble(HERO1, _"Well, I'd better get packed. It's a long way to Reisha Mountain.")

    set_ent_speed(HERO1, 5)
    set_ent_script(HERO1, "D7")
    wait_for_entity(HERO1, HERO1)

  elseif (en == AJATHAR) then
    warp("start_ajathar", 4)

    set_ent_speed(HERO1, 4)

    set_ent_script(HERO1, "U7")
    wait_for_entity(HERO1, HERO1)

    bubble(HERO1, "$0:", _"You wanted to see me sir?")
    bubble(1, _"Yes $0. It is time for you to leave the temple and discover the world. I have found the perfect way for you to do that.")
    bubble(HERO1, _"Excellent sir. What am I to do?")
    bubble(1, _"A note addressed to you has mysteriously appeared on the door of your private chambers. It was brought to me.")
    bubble(1, _"Read over this and tell me what you may know about the sender.")

    set_ftile("alter", 311)
    set_ent_script(HERO1, "U1W50")
    wait_for_entity(HERO1, HERO1)
    set_ftile("alter", 0)
    wait(50)
    set_ent_facing(HERO1, FACE_DOWN)
    wait(100)
    set_ent_facing(HERO1, FACE_UP)
    wait(50)

    bubble(HERO1, _"I cannot say, sir. I know no one who lives at Reisha Mountain.")
    bubble(HERO1, _"My knowledge of geography dictates that it is in a secluded mountain range, but that is all.")
    bubble(1, _"Very well, $0. I am giving you an opportunity to investigate this quest.")
    bubble(1, _"Among other things, this will test your training as well as get you out on a little adventure.")
    bubble(HERO1, _"I would be honored sir. Where do I go first?")
    bubble(1, _"Proceed directly to Reisha Mountain. Let's not keep our mystery host waiting.")
    bubble(HERO1, _"I'm on my way.")
    bubble(1, _"Fare thee well then, $0.")

    set_ent_script(HERO1, "D5")
    wait_for_entity(HERO1, HERO1)

  elseif (en == CASANDRA) then
    set_ent_facing(HERO1, FACE_UP)

    warp("start_casandra", 16)

    set_ent_speed(HERO1, 5)

    set_ent_script(HERO1, "U10")
    wait_for_entity(HERO1, HERO1)

    set_ent_speed(HERO1, 4)
    set_ent_script(HERO1, "F2W30F1W30F3W50U2R1")
    wait_for_entity(HERO1, HERO1)

    bubble(HERO1, "$0:", _"Come out, come out, wherever you are, Nella!")

    set_ent_script(HERO1, "R1U1W30F2W30F1W30F3W30")
    wait_for_entity(HERO1, HERO1)

    bubble(HERO1, _"Nella, YOU ARE SO DEAD MEAT WHEN I FIND YOU!")

    set_ftile("room", 537)
    wait(10)
    set_ftile("room", 538)
    wait(10)
    set_ftile("room", 539)
    wait(10)
    set_ftile("room", 538)
    wait(10)
    set_ftile("room", 537)
    wait(10)

    set_ent_script(HERO1, "U1W50")
    wait_for_entity(HERO1, HERO1)

    set_ftile("room", 310)
    wait(50)

    bubble(HERO1, _"Wait a minute...")
    set_ent_script(HERO1, "U5W75")
    wait_for_entity(HERO1, HERO1)
    bubble(HERO1, _"Who put this letter here? Wait a second... I've heard of this `Reisha Mountain' before...")
    set_ftile("room", 0)
    bubble(HERO1, _"I've got to take a look into this right away!")

    set_ent_speed(HERO1, 5)

    set_ent_script(HERO1, "D7L2D13W100K")
    wait_for_entity(HERO1, HERO1)

    set_ent_id(1, 255)
    set_ent_chrx(1, 8)
    set_ent_speed(1, 3)
    set_ent_movemode(1, 2)
    set_ent_tilex(1, 48)
    set_ent_tiley(1, 54)
    set_ent_script(1, "R2F0W50")
    wait_for_entity(1, 1)
    bubble(1, _"$0? Hello?")

  elseif (en == TEMMIN) then
    warp("start_temmin", 4)

    set_ent_speed(HERO1, 4)

    set_ent_script(HERO1, "L9")
    wait_for_entity(HERO1, HERO1)

    bubble(HERO1, "$0:", _"I was told you had a new mission for me, sir.")
    bubble(2, _"Yes $0. A message from Military Intelligence came for you today. You are to go to Reisha Mountain.")
    bubble(HERO1, _"Yes, sir. What are my orders?")
    bubble(2, _"This is the letter which we received.")

    set_ent_script(HERO1, "D1L2F1W60R2U1F2W30")
    wait_for_entity(HERO1, HERO1)

    bubble(HERO1, _"Understood, sir.")
    bubble(2, _"Once you find out what this is all about, report back to us if there is a threat of another invasion.")
    bubble(2, _"We will be safe instead of sorry, is that understood?")
    bubble(HERO1, _"Yes sir. I'll leave immediately, sir.")

    set_ent_script(HERO1, "R9")
    wait_for_entity(HERO1, HERO1)

  elseif (en == AYLA) then
    warp("start_ayla", 4)

    set_ent_speed(HERO1, 4)

    set_ent_script(HERO1, "U6R2W50")
    wait_for_entity(HERO1, HERO1)

    set_ftile("table", 312)
    wait(50)

    bubble(HERO1, "$0:", _"This is so perfect.")
    bubble(HERO1, _"Just as my luck starts running out, along comes an opportunity to get out of town, and possibly make some serious coin on the way.")
    bubble(HERO1, _"Anyone this desperate must have a lot of dough to back it up, and I'm just the one to steal it from him!")

    bubble(HERO1, _"I can just imagine...")
    set_ent_facing(HERO1, FACE_DOWN)
    wait(50)

    bubble(HERO1, _"I'd be the richest thief on the block.")
    bubble(HERO1, _"No! I'd be the richest thief in my OWN CITY! Ha ha ha!")
    wait(50)

  elseif (en == NOSLOM) then
    set_ent_facing(HERO1, FACE_UP)
    warp("start_noslom", 4)

    set_ent_speed(HERO1, 5)

    bubble(HERO1, "$0:", _"This does not look good. I'm almost positive the handwriting on this note is the same as...")
    wait(50)
    set_ent_facing(HERO1, FACE_DOWN)
    wait(50)

    bubble(HERO1, _"Hmm...")
    wait(50)
    bubble(HERO1, _"I have no choice but to check it out. I'd must be getting on my way immediately!")
    set_ent_script(HERO1, "D12")
    wait_for_entity(HERO1, HERO1)
  end

  set_autoparty(0)
  remove_chr(en)
  return
end

