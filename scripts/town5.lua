-- town5 - "Sunarin"

-- /*
-- This is where the entrance to the Embers Guild is located.
-- You will meet Ayla (PM #5), and she'll join you if you help her with the
-- quest.
--
-- It is also the start of Ayla's quest - robbing the palace in the inner city.
-- Controlled by P_AYLA_QUEST:
--   (0) Nothing done
--   (1) Talked to the girl near the gates
--   (2) Talked to the folks in the bar
--   (3) Talked to both
--   (4) Opened the hidden latch on the northern wall
--   (5) Got the disguise and ready to go
--   (6) Allowed into inner city
--   (7) Used to dress Ayla on entering the town (should not be used elsewhere)
--*/

--/*
-- Entity table:
-- 0. Townsperson
-- 1. Townsperson
-- 2. Townsperson
-- 3. Townsperson
-- 4. Disgruntled woman in bar
-- 5. Girl near palace gates
-- 6. Townsperson
-- 7. Dungar
-- 8. Soldier
--*/


function autoexec()
  if (get_progress(P_TALKGELIK) ~= 2) then
    set_ent_active(7, 0)
  end
  if (get_progress(P_FOUGHTGUILD) == 2) then
    set_desc(0)
  end

  if (get_progress(P_AYLA_QUEST) == 6) then
    if (party[0] == Ayla or party[1] == Ayla) then
      if (get_numchrs() == 2) then
        set_ent_tiley(HERO2, get_ent_tiley(HERO1) - 1)
        set_ent_facing(HERO2, FACE_RIGHT)
      end
    end
  end

  if (get_progress(P_SIDEQUEST6) == 1) then
    LOC_remove_gate()
  end

  set_ent_active(8, 0) -- Guard inside city gates
  refresh()
end


function entity_handler(en)
  if (en == 0) then
    bubble(en, _"Do you like this shirt?")

  elseif (en == 1) then
    bubble(en, _"I really like that guy's shirt.")

  elseif (en == 2) then
    bubble(en, _"I have to stop bringing my wife with me.")

  elseif (en == 3) then
    bubble(en, _"I must have had too many drinks.",
              _"My husband is starting to look handsome.")

  elseif (en == 4) then
    if (party[0] == Ayla) then
      LOC_ayla_bar(en)
    else
      bubble(en, _"These two are weird... but at least I don't have to sit alone.")
    end

  elseif (en == 5) then
    if (party[0] == Ayla) then
      LOC_ayla_girl(en)
    else
      if (get_progress(P_SIDEQUEST6) == 1) then
        bubble(en, _"The, uh, gates to the inner city are open now. It's very unusual...")
      else
        bubble(en, _"Those gates to the inner city never open. Strangers are not allowed in.")
      end
    end

  elseif (en == 6) then
    local face

    if (party[0] == Ayla and get_progress(P_AYLA_QUEST) == 5) then
      bubble(en, _"Hey there, hot mama!", "", _"        Rowr!")
      face = get_ent_facing(en)
      msg(_"SLAP!", 255, 0)
        set_ent_facing(en, 0) set_ent_script(en, "W7") wait_for_entity(en, en)
        set_ent_facing(en, 2) set_ent_script(en, "W7") wait_for_entity(en, en)
        set_ent_facing(en, 1) set_ent_script(en, "W7") wait_for_entity(en, en)
        set_ent_facing(en, 3) set_ent_script(en, "W50") wait_for_entity(en, en)
        set_ent_facing(en, face)
        bubble(en, _"Ouch!")
      set_ent_movemode(en, 1)
    else
      bubble(en, _"Welcome to Sunarin!",
                 _"Please feel free to leave at any time.")
    end

  elseif (en == 7) then
    bubble(en, _"Please... help me... I brought soldiers with me... but the Embers killed them. I barely escaped with my life!")
    bubble(HERO1, _"Who are you? And who are the Embers?")
    bubble(en, _"My name is Dungar. The Embers is the short name for the Thieves of the Ember Crest. They run this town.")
    bubble(en, _"I came to get back something they stole from me...")
    bubble(HERO1, _"The Opal Helmet?")
    bubble(en, _"Why yes! The Opal Helmet.",
               _"Who are you? You're not one of them are you?")
    bubble(HERO1, _"No. My name is $0. I am on a quest, and I need to borrow that helmet to traverse the underwater passage.")
    bubble(en, _"I see. Well, if you can get it back, I will certainly let you use it.")
    bubble(HERO1, _"Well, how do we get it back?")
    bubble(en, _"We can discuss that back at my place. It is not safe here.")
    bubble(HERO1, _"Okay.")
    set_progress(P_EMBERSKEY, 1)
    set_progress(P_TALKGELIK, 3)
    change_map("estate", "by_table")
  end

end


function postexec()
-- /*
--  If we remove the Ember's Key, we can't get Ayla later. So, we don't remove
--  the Ember's Key if we don't have Ayla yet.
-- */
  if (get_progress(P_FOUGHTGUILD) == 2 and get_progress(P_AYLA_QUEST) > 0) then
    set_progress(P_FOUGHTGUILD, 3)
    set_progress(P_EMBERSKEY, 0)
    remove_special_item(SI_EMBERSKEY)
    bubble(HERO1, _"Oh no!")
    bubble(HERO1, _"I must have dropped that key inside!")
    bubble(HERO1, _"Oh well, I got what I came for.")
  end

  if (get_progress(P_AYLA_QUEST) == 6) then
    local hero = LOC_get_ayla()

    if (hero ~= 0 and get_ent_chrx(hero) ~= 8) then
      bubble(hero, _"Let me hop into the maid's outfit real quick.")

      do_fadeout(4)
      set_holdfade(1)
      set_progress(P_AYLA_QUEST, 5)
      refresh()
      wait(100)
      set_holdfade(0)
      do_fadein(4)
    end
  end
end


function refresh()
  showch("treasure1", 32)

  LOC_wall()
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
  local x, y

  if (zn == 1) then
    if (party[0] == Ayla or party[1] == Ayla) then
      if (get_progress(P_AYLA_QUEST) == 5) then
        LOC_check_costume()
        set_progress(P_AYLA_QUEST, 6)
      end
    end
    change_map("main", "town5", -1, 0)

  elseif (zn == 2) then
    bubble(HERO1, _"Locked.")

  elseif (zn == 3) then
    door_in("inn", -4, 0)

  elseif (zn == 4) then
    door_in("inn", 4, 0)

  elseif (zn == 5) then
    door_in("shop", -3, 0)

  elseif (zn == 6) then
    door_in("shop", 3, 0)

  elseif (zn == 7) then
    door_out("inn1")

  elseif (zn == 8) then
    door_out("inn2")

  elseif (zn == 9) then
    door_out("shop1")

  elseif (zn == 10) then
    door_out("shop2")

  elseif (zn == 11) then
    inn(_"River's Way Inn", 70, 1)

  elseif (zn == 12) then
    shop(13)

  elseif (zn == 13) then
    shop(14)

  elseif (zn == 14) then
    bubble(HERO1, _"Inn books. Worst ever.")

  elseif (zn == 15) then
    bubble(HERO1, _"What language is this?")

  elseif (zn == 16) then
    chest(32, I_PRIESTESS, 1)
    refresh()

  elseif (zn == 17) then -- Palace gates
    if (party[0] == Ayla) then
      LOC_ayla_gates()
    else
      bubble(HERO1, _"The gates are closed and locked.")
    end

  elseif (zn == 18) then
    if (get_progress(P_EMBERSKEY) == 0) then
      bubble(HERO1, _"Locked.")
    else
      -- LOC_change_costume()
      bubble(HERO1, _"Key goes in...")
      change_map("guild", "entrance")
    end

  elseif (zn == 19) then
    touch_fire(party[0].id)

  elseif (zn == 20) then
    -- Maid cottage
    door_in("maid_in")
    bubble(HERO1, _"Hey, that's not a real fire down there. It's just a mirage!")

  elseif (zn == 21) then
    door_out("maid_out")

  elseif (zn == 22) then
    -- Not allowed to leave cottage
    bubble(HERO1, _"The door is locked.")

  elseif (zn == 23) then
    LOC_maid_clothes()

  elseif (zn == 24) then

    -- TODO: Once we find and pull the latch, we should always be able to find and pull it, regardless of if Ayla is in the party
    -- This needs to be re-written to properly handle it:

    -- Wall behind city

    -- Only Ayla will notice anything unusual about the wall.
    local hero = LOC_get_ayla()
    if (hero == 0 or get_progress(P_AYLA_QUEST) < 2) then
      return
    end

    if (get_progress(P_AYLA_QUEST) > 1 and
        get_progress(P_AYLA_QUEST) < 4) then
      bubble(hero, _"Ayla:", "There's a latch on the wall.")
    end

    msg(_"You pull the latch, and the wall slides open.", 255, 0)
    x, y = marker("wall")
    set_obs(x, y, 0)
    set_zone(x, y, 0)
    set_progress(P_AYLA_QUEST, 4)

  elseif (zn == 25) then
    bubble(HERO1, _"Nothing in here but old clothes.")

  elseif (zn == 26) then
    bubble(HERO1, _"Locked.")

  elseif (zn == 27) then
    bubble(HERO1, _"Locked.")

  elseif (zn == 28) then
    bubble(HERO1, _"Locked.")

  elseif (zn == 29) then
    door_in("room_1i")

  elseif (zn == 30) then
    set_progress(P_AYLA_QUEST, 7)
    change_map("main", "town5", 1, 0)

  elseif (zn == 31) then
    door_out("room_1o")

  elseif (zn == 32) then
    bubble(HERO1, _"Nothing of interest here.")

  end
end


-- Conversations between Ayla and the single woman in the bar
function LOC_ayla_bar(woman)
  local pp
  pp = get_progress(P_AYLA_QUEST)
  if (pp == 0) then
    bubble(woman, _"Hello there! I'd buy you a drink but I'm broke.")
    bubble(HERO1, _"I'd buy you one but it looks like you've had a few already. Goodbye...")
    bubble(woman, _"Don't be like that. I've just had to pay out half my gold in taxes.")
    bubble(HERO1, _"What did you say?")
    bubble(woman, _"Our ruler takes all our wealth and keeps it in the Inner City.",
           _"Only the servants get to see what goes on in there.",
           _"That woman over there, she's a maid and she gets to live in a nice cottage just opposite Vezdin's shop.")
    bubble(HERO1, _"She's certainly got some money to spend on drink.")
    thought(HERO1, "I might just pay a little visit to that cottage while the maid's in here.")
    set_progress(P_AYLA_QUEST, 2)
    bubble(woman, _"I know, it's just unfair!")
  elseif (pp == 1) then
    bubble(HERO1, _"I don't suppose you work in the palace?")
    bubble(woman, _"I wish. That woman over there is a maid. Only a maid and she lives in a lovely cottage opposite Vezdin's shop.")
    bubble(HERO1, _"Do  you know anything about the wealth that's supposed to be in there?")
    bubble(woman, _"Keep your voice down! I bet you've been talking to that little miss near the palace gates, haven't you?")
    bubble(HERO1, _"Yes, but I much prefer the company in here.")
    set_progress(P_AYLA_QUEST, 3)
  elseif (pp == 2 or pp == 5) then
    bubble(woman, pick(_"Y'know, you're my best friend!",
                  _"I love this bar - I just wish it would stop spinning.",
                  _"My boyfriend used to say: Work is the curse of the drinking classes. That's funny, don't you think?"))
  elseif (pp == 3) then
    bubble(woman, _"I always talk too much when I've been drinking. Don't pay any attention to what I said.")
  elseif (pp == 4) then
    bubble(woman, _"Oh, I am honoured! Haven't you got better things to do over at the palace?")
  end
end


function LOC_ayla_gates()
  local x, y, guard

  x, y = marker("gate")
  guard = 8
  bubble(255, _"WHO GOES THERE?!?")

  if (get_ent_chrx(HERO1) ~= 0) then
    if (get_progress(P_AYLA_QUEST) == 5) then
      bubble(HERO1, _"Just a little serving-girl, reporting for duty.")
      bubble(255, _"Wait there, I'm coming.")
      set_ent_active(guard, 1)
      move_entity(guard, x, y)
      wait_for_entity(guard, guard)
      bubble(guard, _"Come through.")
      sfx(25)
      warp(x, y - 1, 8)
      set_ent_facing(HERO1, FACE_DOWN)
      set_ent_facing(guard, FACE_UP)
      bubble(guard, _"Well, you are a cute little thing, aren't you?")
      bubble(HERO1, _"Cute??")
      bubble(guard, _"I'm sorry, miss, I didn't mean to offend. Follow me.")
      ----
      bubble(255, "CUT!")
      bubble(255, "That's a wrap for today.",
                  "PH hasn't written the rest of this script yet!")
      set_vfollow(0)
      set_ent_script(guard, "R12")
      bubble(255, _"Ayla, great work. Sir Alec, you were fabulous, sweetie.")
      bubble(guard, _"I'm still not sure of my motivation in this scene...")
      wait_for_entity(guard, guard)
      LOC_check_costume()
      set_progress(P_AYLA_QUEST, 7)
      change_map("main", "town5", 1, 0)
      set_vfollow(1)
    else
      bubble(255, _"Gates are closed. Come back later.")
    end
  else
    bubble(HERO1, _"I was wondering if you did tours around the castle?")
    bubble(255, _"I will show you the inside of the dungeon if you bother me again. Clear off!")
    bubble(HERO1, _"I'll take that as a 'no', then.")
  end
end


-- Conversations between Ayla and the girl near the palace gates
function LOC_ayla_girl(girl)
  local pp
  pp = get_progress(P_AYLA_QUEST)
  if (pp == 0) then
    bubble(girl, _"Those gates to the inner city never open. Strangers are not allowed in.")
    bubble(HERO1, _"I suppose ... there must be some valuable stuff in there?")
    bubble(girl, _"Oh, yes, this town should be prosperous, but our ruler takes all our gold in taxes and keeps it walled up in there.")
    bubble(girl, _"That's why everyone here is so surly.")
    bubble(HERO1, _"You said strangers weren't allowed in. Can't locals come and go as they please?")
    bubble(girl, _"Only servants, they ... hey, why are you so interested?")
    bubble(HERO1, _"No reason. I'm just passing through this town and I was curious, that's all.")
    thought(HERO1, "Hmm... curious about the riches they've got in there. Surely the mission can wait for a while.")
    set_progress(P_AYLA_QUEST, 1)
  elseif (pp == 1) then
    bubble(girl, _"You know, I'll call the guards if I see you round here again.")
  elseif (pp == 2) then
    bubble(girl, _"Those gates to the inner city never open. Strangers are not allowed in.")
    bubble(HERO1, _"Can you show me where the servant's entrance is, then?")
    bubble(girl, _"Why would you want to know a thing like that?")
    bubble(HERO1, _"Erm... it's for a book I'm writing. That's it, I'm a famous author.")
    bubble(girl, _"Well, it won't be a very interesting book, then. Servants just knock on the gates and the guard lets them in.")
    bubble(HERO1, _"Thanks very much. I'll be sure to put you in the credits.")
    thought(HERO1, "Every so often I am amazed by the gullibility of my fellow humans.")
    set_progress(P_AYLA_QUEST, 3)
  elseif (pp == 3) then
    bubble(girl, pick(_"You're always hanging around here. I'm becoming a little suspicious.",
           _"Have you been talking to those losers in the bar? You stink of beer!",
           _"Whatever you want, I'm not interested."))
  elseif (pp == 4) then
    bubble(girl, _"Hello. You must be a new employee. But you look strangely familiar...")
  elseif (pp == 5) then
    bubble(girl, _"Looking at you again, you look REALLY familiar.  Hmm...")
  end
end


function LOC_check_costume()
  local hero = LOC_get_ayla()

  if (hero ~= 0 and get_ent_chrx(hero) ~= 0) then
    bubble(hero, _"Just wait a second whilst I change out of this costume.")

    do_fadeout(4)
    set_holdfade(1)
    set_ent_chrx(hero, 0)
    wait(100)
    set_holdfade(0)
    do_fadein(4)
  end
end


function LOC_get_ayla()
  local hero

  if (party[0] == Ayla) then
    hero = HERO1
  elseif (party[1] == Ayla) then
    hero = HERO2
  else
    hero = 0
  end

  return hero
end


function LOC_maid_clothes()
  -- Clothes chest in cottage, Ayla must be in lead to find them
  if (party[0] == Ayla) then
    if (get_progress(P_AYLA_QUEST) > 0 and get_progress(P_AYLA_QUEST) < 5) then
      sfx(5)
      msg(_"Maid's outfit procured", 12, 0)

      x, y = marker("maid_in")
      move_entity(HERO1, x + 4, y)

      wait_for_entity(HERO1, HERO1)
      set_ent_facing(HERO1, FACE_DOWN)

      bubble(HERO1, _"Would you look at this ratty old thing? Oh well...")

      move_entity(HERO1, x + 3, y)
      wait_for_entity(HERO1, HERO1)

      msg(_"Ayla changes into the maid's outfit.", 255, 0)

      do_fadeout(4)
      set_holdfade(1)
      set_progress(P_AYLA_QUEST, 5)
      refresh()

      wait(100)
      set_holdfade(0)
      do_fadein(4)
      thought(HERO1, _"Hmm... it's a little tight around the arms...")
    else
      bubble(HERO1, _"Nothing here worth stealing.")
    end
  else
    bubble(HERO1, _"Nothing in here but old clothes.")
  end
end


function LOC_remove_gate()
  -- Get rid of the gate doors
  local y
  local x, a = marker("gate")
  for y = a - 1, a + 1, 1 do
    set_zone(x - 1, y, 0)
    set_obs(x - 1, y, 0)
  end
end


function LOC_wall()
  if (get_progress(P_AYLA_QUEST) > 3) then
    local x, y = marker("wall")
    local hero = LOC_get_ayla()

    set_zone(x, y, 0)
    set_obs(x, y, 0)

    -- Put Ayla into her disguise
    if (get_progress(P_AYLA_QUEST) == 5 and hero ~= 0) then
      set_ent_chrx(hero, 8)
    end
  end
end
