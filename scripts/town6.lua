-- town6 - "Ajantara"

function autoexec()
  if (get_progress(P_OPALSHIELD) == 0) then
    -- Remove the guards from the map
    set_ent_active(5, 0)
    set_ent_active(6, 0)
  end

  if (get_progress(P_OPALSHIELD) == 1 and get_progress(P_SAVEBREANNE) < 3) then
    -- Remove Breanne
    set_ent_active(9, 0)
  end

  refresh()
end


function entity_handler(en)
  if (en == 0) then
    LOC_lord_ajantara(en)

  elseif (en == 1) then
    LOC_lady_ajantara(en)

  elseif (en == 2) then
    bubble(en, _"This is a lovely little town. Don't you think so?")

  elseif (en == 3) then
    if (get_progress(P_OPALSHIELD) == 0) then
      bubble(en, _"Wanna play tag with me?")
      return
    end
    if (get_progress(P_SAVEBREANNE) < 3) then
      bubble(en, _"I miss Breanne.")
    else
      bubble(en, _"This town rocks!")
    end

  elseif (en == 4) then
    bubble(en, _"Sorry, I'm far too busy and you have terrible breath.")

  elseif (en == 5) then
    if (get_progress(P_SAVEBREANNE) < 3) then
      bubble(en, _"There has been a lot of Cult activity around these parts lately.")
    else
      bubble(en, _"Has the Cult really been rousted?")
    end

  elseif (en == 6) then
    bubble(en, _"Lord and Lady Ajantara have hired us to watch the town.")

  elseif (en == 7) then
    if (get_progress(P_OPALSHIELD) == 0) then
      bubble(en, _"Lord Ajantara's grandfather founded this town.")
      return
    end
    if (get_progress(P_SAVEBREANNE) == 0) then
      if (get_progress(P_TALKGRAMPA) == 0) then
        bubble(en, _"Lord and Lady Ajantara's daughter, Breanne, was kidnapped last night. They are looking for someone to help find her!")
        set_progress(P_TALKGRAMPA, 1)
      else
        bubble(en, _"You should go and speak with the Ajantaras.")
      end
    else
      if (get_progress(P_SAVEBREANNE) < 3) then
        bubble(en, _"You truly are kind.")
      else
        bubble(en, _"It's a lovely day isn't it?")
      end
    end

  elseif (en == 8) then
    bubble(en, _"Everything in this town is owned by Lord and Lady Ajantara.")

  elseif (en == 9) then
    if (get_progress(P_OPALSHIELD) == 0) then
      bubble(en, _"Hiya!")
      return
    end
    bubble(en, _"Thank you very much. Please feel free to come and visit any time.")

  end
end


function postexec()
  return
end


function refresh()
  showch("treasure1", 67)
  showch("treasure2", 68)
  showch("treasure3", 69)
  showch("treasure4", 70)
end


-- Show the status of a treasures
function showch(which_marker, which_chest)
  -- Set tiles if chest already opened
  if (get_treasure(which_chest) == 1) then
    set_zone(which_marker, 0)
  end

  -- Only treasure4 needs to change its obstacle setting
  if (which_marker == "treasure4") then
    set_obs(which_marker, 0)
  end
end


function zone_handler(zn)
  if (zn == 1) then
    change_map("main", "town6")

  elseif (zn == 2) then
    door_in("store_1i")

  elseif (zn == 3) then
    door_in("store_2i")

  elseif (zn == 4) then
    door_in("inn_i")

  elseif (zn == 5) then
    door_in("shop_1i")

  elseif (zn == 6) then
    door_in("room_1i")

  elseif (zn == 7) then
    door_in("shop_2i")

  elseif (zn == 8) then
    door_in("room_2i")

  elseif (zn == 9) then
    door_in("family_i")

  elseif (zn == 10) then
    door_out("store_1o")

  elseif (zn == 11) then
    door_out("store_2o")

  elseif (zn == 12) then
    door_out("inn_o")

  elseif (zn == 13) then
    door_out("shop_1o")

  elseif (zn == 14) then
    door_out("room_1o")

  elseif (zn == 15) then
    door_out("shop_2o")

  elseif (zn == 16) then
    door_out("room_2o")

  elseif (zn == 17) then
    door_out("family_o")

  elseif (zn == 18) then
    bubble(HERO1, _"Locked.")

  elseif (zn == 19) then
    shop(15)

  elseif (zn == 20) then
    shop(16)

  elseif (zn == 21) then
    if (get_progress(P_TOWN6INN) > 0) then
      inn(_"Ajantara Inn", 1, 1)
    else
      inn(_"Ajantara Inn", 150, 1)
    end

  elseif (zn == 22) then
    shop(17)

  elseif (zn == 23) then
    shop(18)

  elseif (zn == 24) then
    chest(67, I_B_BLIZZARD, 1)

  elseif (zn == 25) then
    chest(68, I_EDAENRA, 1)

  elseif (zn == 26) then
    chest(69, I_NLEAF, 1)

  elseif (zn == 27) then
    chest(70, I_HPUP, 1)
    refresh()

  elseif (zn == 28) then
    book_talk(party[0])

  elseif (zn == 29) then
    LOC_complain()

  elseif (zn == 30) then
    if (get_progress(P_WARPSTONE) == 0) then
      set_progress(P_WARPSTONE, 1)
    end
    change_map("town2", "travelpoint")

  elseif (zn == 31) then
    touch_fire(party[0])

  -- Door in the item shop
  elseif (zn == 32) then
    bubble(HERO1, _"Locked.")

  end
end


function LOC_complain()
  if (get_progress(P_SAVEBREANNE) == 1) then
    if (get_numchrs() == 1) then
      bubble(HERO1, _"Ugh... I getting real sick of this running around crap! Why is nothing I need where it's supposed to be?")
      bubble(HERO1, _"I'll bet that even if I had waited a month and come here for the Band, the Ajantara's daughter would have just been kidnapped.")
      bubble(HERO1, _"When will this end?")
    else
      face_each_other(HERO1, HERO2)
      wait(50)
      bubble(HERO1, _"You know what? I'm getting real sick of this running around crap!")
      bubble(HERO2, _"What are you talking about?")
      bubble(HERO1, _"The same thing happened with the Helmet. The minute I show up to get the stupid thing... it's gone.")
      bubble(HERO1, _"It's as if someone is purposely trying to make me go crazy.")
      bubble(HERO2, _"Don't you think that you're exaggerating just a little?")
      bubble(HERO2, _"Sure it may seem that things are out of our control, but what we're doing is of the utmost importance.")
      bubble(HERO2, _"This is our chance to do something good. I mean, originally this quest was nothing more than a fetch mission.")
      bubble(HERO2, _"However, as we've pursued our quest we've managed to do some good along the way. That's what really counts.")
      bubble(HERO1, _"Hmm... I guess you're right. I just don't like being out of control. We still don't really know what's going on!")
      bubble(HERO2, _"I'm sure we'll come to understand what this is all about in time. If nothing else, all this running around is making us stronger.")
      bubble(HERO2, _"We'll be better prepared for Malkaron and anybody else who gets in our way.")
      bubble(HERO1, _"That's true... I have learned a lot since we've started, and I sure feel much better about myself.")
      bubble(HERO2, _"So does that mean that you're ready to go?")
      bubble(HERO1, _"Yes! This isn't about me... this is about rescuing an innocent little girl and saving the world.")
      bubble(HERO2, _"Saving the world?")
      bubble(HERO1, _"Stuff it, I'm on a roll here.")
    end
    set_progress(P_SAVEBREANNE, 2)
  end
end


function LOC_lady_ajantara(en)
  if (get_progress(P_OPALSHIELD) == 0) then
    bubble(en, _"Welcome... enjoy your stay.")
    return
  end
  if (get_progress(P_SAVEBREANNE) == 0) then
    bubble(en, _"Oh poor Breanne!")
  else
    if (get_progress(P_SAVEBREANNE) < 3) then
      bubble(en, _"Good luck to you!")
    else
      bubble(en, _"Breanne is a dear.")
    end
  end
end


function LOC_lord_ajantara(en)
  if (get_progress(P_OPALSHIELD) == 0) then
    bubble(en, _"How do you like our little town?")
    return
  end
  if (get_progress(P_SAVEBREANNE) == 0) then
    bubble(en, _"You must help save our daughter! We'll give you anything, just please bring her back to us.")
    if (get_progress(P_TALKGRAMPA) == 1) then
      bubble(HERO1, _"We were told she was kidnapped, but by who? Where did they take her?")
      bubble(en, _"The ones who took her are members of a cult... the Cult of the Shadow Walker. They took her to their tower on Sharia island.")
    else
      bubble(HERO1, _"What happened to her? Where did she go?")
      bubble(en, _"She was kidnapped by cult members from the Cult of the Shadow Walker. They took her to their tower on Sharia island.")
    end
    bubble(en, _"They have the travel stone to the island guarded by a giant!")
    bubble(HERO1, _"Why did they take her?")
    bubble(en, _"They took her because she is descended from the legendary half-dragon king Elgar.")
    bubble(en, _"They are trying to summon an avatar of the Shadow Walker, and they plan on using Breanne to do it!")
    bubble(HERO1, _"Doesn't that mean that one of you is also descended from Elgar?")
    bubble(en, _"No. We adopted Breanne eight years ago. She doesn't even know that we are not her real parents!")
    bubble(HERO1, _"Fear not, we will get your daughter back safely, and we'll put a stop to this summoning as well!")
    bubble(HERO1, _"Just one thing.")
    bubble(en, _"Yes?")
    bubble(HERO1, _"Where is the travel stone to Sharia island?")
    bubble(en, _"Oh, it's on the other side of Kragar's Pass.")
    bubble(en, _"I'll send word to the guards to let you through because normally only caravans are allowed into the Pass.")
    bubble(HERO1, _"Oh. Okay. Thanks!")
    bubble(HERO1, _"By the way, do you know anything about an Opal Band? I was told that it was in this town. I need to borrow it.")
    bubble(en, _"If you save my daughter, you can have it!")
    bubble(HERO1, _"Really? Great! Where is it?")
    bubble(en, _"It belongs to my daughter. She is probably wearing it.")
    bubble(HERO1, _"Of course. Well, time to go!")
    set_progress(P_SAVEBREANNE, 1)
    return
  end
  if (get_progress(P_SAVEBREANNE) == 4) then
    bubble(en, _"Things are finally back to normal.")
    return
  end
  if (get_progress(P_SAVEBREANNE) < 3) then
    bubble(en, _"Please be careful.")
  else
    bubble(en, _"Breanne told us she gave you the Band. We're glad everything has worked out so great. Thank you very much!")
    bubble(en, _"I know it's not much, but to show our appreciation, we want you to feel welcome in our town.")
    bubble(en, _"Therefore, any time you need to stay at our inn, you'll only be charged 1 GP!")
    set_progress(P_TOWN6INN, 1)
    bubble(HERO1, _"That's very kind of you. Thank you!")
    bubble(en, _"Oh, and by the way. You should take this.")
    sfx(5)
    msg(_"Cave Key procured", 255, 0)
    set_progress(P_CAVEKEY, 1)
    add_special_item(SI_CAVEKEY)
    drawmap()
    screen_dump()
    bubble(HERO1, _"What's this for?")
    bubble(en, _"That will unlock the doors in the mountain pass.")
    bubble(en, _"After thinking about this Opal stuff for a while, I remembered a story about the Opal Armor.")
    bubble(en, _"Apparently, there is a magical suit of Opal Armor locked away deep in the mountain.")
    bubble(en, _"That key will unlock the sealed entrances to the dungeons below.")
    bubble(HERO1, _"That's perfect! The next item we were on our way to find was the Opal Armor.")
    bubble(en, _"Well, I'm glad we could help. However, I must warn you to be extra cautious on this quest.")
    bubble(en, _"A dark race of Larinon live in the dungeons down there and they are most unpleasant.")
    bubble(HERO1, _"Thanks for the warning, and thank you again for the key.")
    set_progress(P_SAVEBREANNE, 4)
  end
end
