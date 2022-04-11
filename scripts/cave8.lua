-- cave8 - "Xenar Cave where Binderak is hiding"

-- /*
-- {
-- progress:

-- }
-- */

function autoexec()
  refresh()
end


function loc_credits1(en)
  bubble(255, _"The End.")
  bubble(255, _"KQ was brought to you by:")
  
  bubble(255, "Josh Bolduc - the original creator")
  bubble(255, "(C) 2001 DoubleEdge Software")
  bubble(255, "(C) 2002-2022 KQ Lives Team")
  bubble(255, "http://kqlives.sourceforge.net/")
  bubble(255, "Peter Hull")
  bubble(255, "TeamTerradactyl")
  bubble(255, "Chris Barry")
  bubble(255, "Eduardo Dudaskank")
  bubble(255, "Troy D Patterson")
  bubble(255, "Master Goodbytes")
  bubble(255, "Rey Brujo")
  bubble(255, "Matthew Leverton")
  bubble(255, "Sam Hocevar")
  bubble(255, "Günther Brammer")
  bubble(255, "WinterKnight")
  bubble(255, "Edgar Alberto Molina")
  bubble(255, "Steven Fullmer (OnlineCop)")
  bubble(255, "Shrike")
  bubble(255, "Z9484")
  bubble(255, "Dungeon Crawl 32x32 tiles supplemental")

  wait(75)
  bubble(255, "Fin")
  set_alldead(1)
end


function loc_credits2(en)
  warp("end", 8)
  bubble(255, _"The Staff of Xenarum has been reclaimed!")
  bubble(255, _"And through time the land will be healed.")
  bubble(255, _"The End.")
  bubble(255, _"KQ was brought to you by:")

  bubble(4, "Josh Bolduc - the original creator")
  bubble(5, "(C) 2001 DoubleEdge Software")
  bubble(6, "(C) 2002-2022 KQ Lives Team")
  bubble(7, "http://kqlives.sourceforge.net/")
  bubble(8, "Peter Hull")
  bubble(9, "TeamTerradactyl")
  bubble(10, "Chris Barry")
  bubble(3, "Eduardo Dudaskank")
  bubble(4, "Troy D Patterson")
  bubble(5, "Master Goodbytes")
  bubble(6, "Rey Brujo")
  bubble(7, "Matthew Leverton")
  bubble(8, "Sam Hocevar")
  bubble(9, "Günther Brammer")
  bubble(10, "WinterKnight")
  bubble(3, "Edgar Alberto Molina")
  bubble(4, "Steven Fullmer (OnlineCop)")
  bubble(5, "Shrike")
  bubble(6, "Z9484")
  bubble(7, "Dungeon Crawl 32x32 tiles supplemental")

  wait(75)
  bubble(255, "Fin")
  set_alldead(1)
end

function entity_handler(en)
  if (en == 0) then
    bubble(HERO1, _"We've found you Binderak! Where's the staff of Xenarum?")
    bubble(en, _"Why are you angry at me for stopping Malkaron's advance?")
    if (prompt(HERO1, 2, 0, "...",
    "  Enough talk!",
    "  We need the staff.") == 0) then
        bubble(en, _"I see you can't be reasoned with.")
        set_run(0)
        combat(63)
        set_run(1)
        set_ent_active(0, 0)
        set_ent_active(1, 1)
        set_ent_active(2, 1)
        bubble(1, _"You did it!")
        set_ent_facing(HERO1, FACE_DOWN)
        set_ent_facing(HERO2, FACE_DOWN)
        x, y = marker("nostik_e")
        move_entity(1, x, y, 0)
        wait_for_entity(1, 2)
        bubble(1, _"Ahh the staff of Xenarum, once again in my hands.")
        bubble(1, _"I would like you to introduce to my brother.")
        move_entity(2, x + 1, y, 0)
        wait_for_entity(1, 2)
        bubble(2, _"I'm Malkaron. Pleased to meet you.")
        bubble(2, _"I thank you for recovering the staff for us.")
        bubble(2, _"Mwuhaha")

        loc_credits1()
      else
        bubble(en, _"Why?")
        bubble(HERO1, _"Well Nostik...")
        bubble(en, _"Ha ha")
        bubble(en, _"Malkaron's brother promised you a reward? Priceless.")
        bubble(HERO1, _"No! It can't be.")
        x, y = marker("nostik_e")
        set_ent_active(1, 1)
        set_ent_active(2, 1)
        move_entity(1, x, y, 0)
        move_entity(2, x + 1, y, 0)
        set_ent_facing(HERO1, FACE_DOWN)
        set_ent_facing(HERO2, FACE_DOWN)
        set_ent_facing(en, FACE_DOWN)
        wait_for_entity(1, 2)
        bubble(1, _"Oh but it is.")
        bubble(2, _"Hero types are so easy to manipulate.")
        bubble(1, _"Hand over the staff!")
        bubble(HERO1, _"Never!")
        bubble(2, _"Its too late for you. Surrender!")
        bubble(en, _"I will take Nostik, you go after Malkaron!")
        
        set_staff(1)
        set_run(0)
        combat(64)
        set_run(1)
        set_staff(0)
        loc_credits2()
    end
  end
end


function postexec()
  return
end


function refresh()
  set_ent_id(3, SENSAR)
  set_ent_id(4, SARINA)
  set_ent_id(5, CORIN)
  set_ent_id(6, AJATHAR)
  set_ent_id(7, CASANDRA)
  set_ent_id(8, TEMMIN)
  set_ent_id(9, AYLA)
  set_ent_id(10, NOSLOM)

  -- TODO make dead party members ghosts
  -- if (party[0] == NOSLOM) then
  --   if get_party_hp(get_pidx(0)) <= 0 then
  --     set_ent_transl(10, 1)
  --   end
  -- elseif (party[1] == NOSLOM) then
  --   hero = HERO2
  -- end
end


function zone_handler(zn)
  if (zn == 1) then
    change_map("main", "xenarcave")

  elseif (zn == 2) then
    bubble(HERO1, _"This barrel is filled with water.")
    
  elseif (zn == 3) then
    bubble(HERO1, _"Seems like Binderak likes to write poetry.")
    
  elseif (zn == 4) then
    bubble(HERO1, _"It contains some foodstuffs.")
    -- give_xp(NOSLOM, 10000, 1)
  
  elseif (zn == 5) then
    bubble(HERO1, _"It contains some random supplies.")
    -- chest(200, I_B_DEATH, 1)
  end
end

