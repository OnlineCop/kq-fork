-- temple2 - "Second, underground part of temple north of Andra"

-- TT: Updated map; shifted entire map right 6 and down 4, then updated
--     coords appropriately below (left the xx+6 and xx+4 format in the code
--     until all coordinates can be thoroughly tested).


function autoexec()
  set_ent_active(0, 0)
  if (get_progress(P_TALK_TEMMIN) == 1 or
      get_progress(P_TALK_TEMMIN) == 2) then
    set_ent_id(1, TEMMIN)
  else
    set_ent_active(1, 0)
  end

  refresh()
end


function entity_handler(en)
  if (en == 0) then
    -- We should never encounter a direct conversation with
    -- the blord, but if we do, go ahead and deal with it
    LOC_blord(en)

  elseif (en == 1) then
    LOC_talk_temmin(en)
  end
end


function postexec()
  return
end


function refresh()
  local x, y
  x, y = marker("treasure1")

  showch("treasure1", 21)
  showch("treasure2", 22)
  showch("treasure3", 23)
  showch("treasure4", 24)
  showch("treasure5", 25)
  showch("treasure6", 26)
  showch("treasure7", 27)
  showch("treasure8", 28)
  showch("treasure9", 29)
  showch("treasure10", 30)

  if (get_progress(P_TALK_TEMMIN) == 2) then
    set_ent_facehero(1, 1)
  end
  if (get_progress(P_UNDEADJEWEL) ~= 0) then
    set_btile("jewel", 237)
  end
end


-- Show the status of a chest
function showch(which_marker, which_chest)
  -- Set tiles if -1 passed in as 'which_chest' or if chest already opened
  if (get_treasure(which_chest) == 1) then
    set_mtile(which_marker, 41)
    set_zone(which_marker, 0)
  end
end


function zone_handler(zn)
  if (zn == 0 and get_progress(P_GOBLINITEM) == 0) then
    combat(52)

  elseif (zn == 1) then
    change_map("temple1", "dstairs1")

  elseif (zn == 2) then
    warp("ustairs1", 8)

  elseif (zn == 3) then
    warp("dstairs1", 8)

  elseif (zn == 4) then
    warp("ustairs2", 8)

  elseif (zn == 5) then
    warp("dstairs2", 8)

  elseif (zn == 6) then
    warp("dstairs3", 8)

  elseif (zn == 7) then
    if (get_progress(P_KILLBLORD) == 0) then
      LOC_blord(0)
    else
      warp("ustairs4", 8)
    end

  elseif (zn == 8) then
    warp("ustairs3", 8)

  elseif (zn == 9) then
    chest(21, I_EAGLEEYES, 1)
    refresh()

  elseif (zn == 10) then
    chest(22, 0, 500)
    refresh()

  elseif (zn == 11) then
    chest(23, I_EDAENRA, 1)
    refresh()

  elseif (zn == 12) then
    chest(24, I_IRUNE, 1)
    refresh()

  elseif (zn == 13) then
    chest(25, I_SUIT2, 1)
    refresh()

  elseif (zn == 14) then
    chest(26, I_GAUNTLET1, 1)
    refresh()

  elseif (zn == 15) then
    chest(27, I_WENSAI, 1)
    refresh()

  elseif (zn == 16) then
    chest(28, I_B_VENOM, 1)
    refresh()

  elseif (zn == 17) then
    chest(29, I_RRUNE, 2)
    refresh()

  elseif (zn == 18) then
    chest(30, I_OSEED, 1)
    refresh()

  elseif (zn == 19) then
    warp("dstairs4", 8)

  elseif (zn == 20) then
    LOC_goblin_king(255)

  elseif (zn == 21) then
    sfx(7)
    set_save(1)
    set_sstone(1)

  elseif (zn == 22) then
    set_save(0)
    set_sstone(0)

  end
end


function LOC_blord(en)
  local x, y = marker("dstairs4")

  set_autoparty(1)
  move_entity(HERO1, x, y - 1)
  if (get_numchrs() == 1) then
    wait_for_entity(HERO1, HERO1)
    set_ent_facing(HERO1, FACE_DOWN)
  else
    move_entity(HERO2, x, y - 2)
    wait_for_entity(HERO1, HERO2)
    set_ent_facing(HERO1, FACE_DOWN)
    set_ent_facing(HERO2, FACE_DOWN)
  end
  set_autoparty(0)
  orient_heroes()

  -- Show the enemy guarding the stairs
  set_ent_active(en, 1)
  bubble(en, _"Halt!")
  bubble(en, _"Foolish humans... be gone!")
  drawmap()
  set_run(en)
  combat(53)
  set_run(1)

  bubble(en, _"Argh!")
  drawmap()

  set_ent_active(en, 0)
  set_ent_active(1, 0)

  msg(_"Goblin jewel procured", 19, 0);

  set_progress(P_KILLBLORD, 1)
  add_special_item(SI_UNDEADJEWEL)
  set_progress(P_TALK_TEMMIN, 3)

end


function LOC_goblin_king(en)
  if (get_progress(P_GOBLINITEM) == 1) then
    bubble(HERO1, _"Ooohh... shiny.")
    return
  end

  if (get_progress(P_SIDEQUEST3) == 0) then
    local en = 255

    bubble(HERO1, _"Hey, that gem would fit here.")
    sfx(5)
    set_btile("jewel", 237)
    rest(500)
    bubble(en, _"Thank you.")
    set_ent_facing(0, FACE_DOWN)
    if (get_numchrs() == 2) then
      set_ent_facing(1, FACE_DOWN)
    end
    drawmap()
    screen_dump()
    bubble(HERO1, _"Who are you?")
    bubble(en, _"My name is Kaleg... I am the Goblin king. This tomb is my home.")
    bubble(HERO1, _"I guess that would make you dead then?")
    bubble(en, _"That is correct.")
    bubble(HERO1, _"No problem. I'll just be going now.")
    bubble(en, _"Wait! I wanted to thank you for you help.")
    if (get_numchrs() > 1) then
      bubble(HERO1, _"Well, we're not the ones who figured this all out.")
    else
      bubble(HERO1, _"Well, I'm not the one who figured this all out.")
    end
    bubble(en, _"Regardless, my brethren and I can rest again. Take this.")
    sfx(5)
    msg(_"Jade pendant procured", 255, 0)
    refresh()
    bubble(en, _"This may help you in your quest.")
    bubble(en, _"I must go now. Fare thee well.")
    if (get_numchrs() > 1) then
      bubble(HERO1, _"Hey! What do you know about our quest?")
    else
      bubble(HERO1, _"Hey! What do you know about my quest?")
    end
    wait(50)
    bubble(HERO1, _"Hello?")
    bubble(HERO1, _"Urgh! I hate when they do that!")
    set_progress(P_GOBLINITEM, 1)
    add_special_item(SI_JADEPENDANT)
    set_progress(P_SIDEQUEST3, 1)
    set_progress(P_UNDEADJEWEL, 1)
    remove_special_item(SI_UNDEADJEWEL)
    return
  else
    bubble(HERO1, _"It looks like there should be something here.")
  end
end


function LOC_talk_temmin(en)
  if (get_progress(P_TALK_TEMMIN) == 1) then
    if (get_numchrs() > 1) then
      bubble(en, _"$0! $1! I am pleased to see you.")
    else
      bubble(en, _"$0! I am pleased to see you.")
    end
    bubble(HERO1, _"Greetings Temmin. The priest upstairs said you came down here with the Goblin Jewel to try and stop these undead.")
    bubble(en, _"That is true, but I have failed. I was attacked by a sentient skeleton of considerable strength. It took the Jewel.")
    bubble(HERO1, _"Where is it?")
    bubble(en, _"I don't rightly recall. I ran for my life and didn't pay attention to where I was going.")
    bubble(HERO1, _"Well, let's go get it back.")
    bubble(en, _"You go ahead. I'm resting here and going back to the temple.")
    set_ent_script(en, "F1W50")
    wait_for_entity(en, en)
    bubble(en, _"I am a failure and a coward.")
    bubble(HERO1, _"But...")
    bubble(en, _"Save it. You won't change my mind.")
    set_progress(P_TALK_TEMMIN, 2)
    refresh()
  else
    bubble(en, _"...")
  end
end
