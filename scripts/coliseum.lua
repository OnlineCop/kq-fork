-- coliseum - "Coliseum west of Sunarin"

-- /*
-- {
  -- Get the 6th PM after the battle
  -- Of course, we shouldn't let him/her join except if we do a side-quest.
  -- }
  -- */
  
  
  function autoexec()
    set_ent_active(12, 0)
    if (progress.roundnum > 0) then
      x, y = marker("after_battle")
      place_ent(6, x + 1, y + 1)
    end
    if (progress.opalshield == 1 and progress.finalpartner > 0) then
      -- msg(progress.finalpartner)
      -- set_ent_chrx(12, get_pidx(progress.finalpartner))
      set_ent_id(12, progress.finalpartner - 1)
      set_ent_active(12, 1)
    end
end


function entity_handler(en)
  if (en == 0) then
    bubble(en, _"I actually made it to the second round!")

  elseif (en == 1) then
    bubble(en, _"I'm out of practice.")
    
  elseif (en == 2) then
    bubble(en, _"I can't stand waiting like this!")

  elseif (en == 3) then
    bubble(en, _"There's not enough women in these competitions.")

  elseif (en == 4) then
    bubble(en, _"I came all this way and then found out that I didn't have enough money for the registration fee!")

  elseif (en == 5) then
    bubble(en, _"I'm waiting for my next match.")

  elseif (en == 6) then
    local a, b

    if (progress.opalshield == 1) then
      bubble(en, _"This is where you register.")
      bubble(en, _"Hey, wait a minute... you're the one who defeated Trayor. We can't let you register again... you're too good for these guys.")
      return
    end
    if (progress.roundnum == 0) then
      bubble(en, _"This is where you register.")
      if (progress.firsttime == 0) then
        bubble(en, _"Before I explain the rules, I should tell you that the registration fee is 2000 gp!")
        bubble(en, _"If you lose even once, you are done... and will have to register again.")
        bubble(en, _"As well, after you register you can't leave the coliseum grounds. If you leave, you will be forced to register again.")
        bubble(en, _"Now, let's talk about the actual fights!")
        bubble(en, _"The first three battles are more or less just preliminary fights to make sure you have what it takes.")
        bubble(en, _"If you win the first three rounds, you qualify for more advanced matches.")
        bubble(en, _"The more advanced matches are against more skilled opponents and will determine whether or not your skills are balanced enough.")
        bubble(en, _"If you manage to defeat the advanced warriors, you will be given a chance to fight Trayor for the grand prize, an opal shield!")
        a = prompt(en, 2, 0, _"So, are you willing to pay the",
                             _"2000 gp entrance fee?",
                             _"  no  ",
                             _"  yes ")
        progress.firsttime = 1
      else
        a = prompt(en, 2, 0, _"The registration fee is 2000 gp.",
                             _"Are you in?",
                             _"  no  ",
                             _"  yes ")
      end
      if (a == 0) then
        bubble(en, _"Oh well. Come back when you're ready.")
      else
        if (get_gp() >= 2000) then

          set_gp(get_gp() - 2000)
          bubble(en, _"Consider yourself registered. After each battle, come back and talk to me.")
          set_ent_script(en, "U2R1F2")
          wait_for_entity(en, en)
          -- progress.roundnum = 7
          progress.roundnum = 1
          progress.battlestatus = 0
        else
          bubble(en, _"Umm... you don't seem to have enough. I would suggest selling some junk. In any case, you can't register at this point.")
        end
      end
    else
      a = progress.battlestatus
      if (a == 0) then
        if (progress.roundnum == 7) then
          bubble(en, _"Wow! This is it... your next battle is with Trayor. Good luck... you'll need it!")
        else
          bubble(en, _"Battle number "..string.format("%d", progress.roundnum).._". Just head on through that door when you are ready.")
        end
      elseif (a == 1) then
        if (progress.roundnum == 7) then
          bubble(en, _"Astounding! Whoever you are, you're going to go far!")
          bubble(en, _"Well, I guess this is yours.")
          sfx(5)
          msg(_"Opal Shield procured", 255, 0, xofs, yofs)
          progress.opalshield = 1
          add_special_item(SI_OPALSHIELD)
          if (get_ent_tilex(HERO1) == get_ent_tilex(en)) then
            set_ent_script(HERO1, "L1")
            wait_for_entity(HERO1, HERO1)
          end

          if (get_numchrs() == 1) then
            move_entity(HERO1, "prebattle", 0)
            wait_for_entity(HERO1, HERO1)
            set_ent_facing(HERO1, FACE_UP)
          else
            move_entity(HERO1, "prebattle", 0)
            move_entity(HERO2, get_marker_tilex("prebattle"), get_marker_tiley("prebattle") + 1, 0)
            wait_for_entity(HERO1, HERO2)
            set_ent_facing(HERO1, FACE_UP)
            set_ent_facing(HERO2, FACE_UP)
            orient_heroes()
          end
          set_ent_script(en, "L1D2")
          wait_for_entity(en, en)
          bubble(en, _"Good luck in your endeavours.")

          if (progress.oldpartner > 0) then
            progress.finalpartner = progress.oldpartner
          else
            if (party[0] == CASANDRA) then
              if (krnd(10) < 5) then
                progress.finalpartner = TEMMIN + 1
              end
            end
            if (party[0] == TEMMIN) then
              if (krnd(10) < 5) then
                progress.finalpartner = CASANDRA + 1
              end
            end
            while (progress.finalpartner == 0) do
              z = krnd(8)
              if (z ~= party[0].id) then
                progress.finalpartner = z + 1
              end
            end
          end
          set_ent_id(12, progress.finalpartner - 1)
          set_ent_active(12, 1)
        else
          bubble(en, _"Congratulations! You should have a rest or heal up. Come back and talk to me again after you've prepared.")
          progress.battlestatus = 0
          progress.roundnum = progress.roundnum + 1
        end
      elseif (a == 2) then
        bubble(en, _"Ooh... tough luck!")
        progress.battlestatus = 0
        progress.roundnum = 0
        set_ent_script(HERO1, "D3F1")
        wait_for_entity(0, 0)
        set_ent_script(en, "L1D2")
        wait_for_entity(en, en)
        play_map_song()
      end
    end

  elseif (en == 7) then
    bubble(en, _"Losing isn't so bad.")
    bubble(en, _"No wait, let me rephrase that!")
    bubble(en, _"Losing sucks!")

  elseif (en == 8) then
    bubble(en, _"What a waste of money.")
    bubble(HERO1, _"How many matches did you fight?")
    bubble(en, _"Matches? I haven't left the bar yet!")

  elseif (en == 9) then
    bubble(en, _"These fights are quite interesting.")

  elseif (en == 10) then
    bubble(en, _"Wine puts losing in a whole new perspective.")

  elseif (en == 11) then
    bubble(en, _"The Coliseum hasn't been the same since they discontinued betting on the fights.")

  elseif (en == 12) then
    if (progress.oldpartner == 0) then
      bubble(en, _"Wow! I saw you fighting. You cleaned up!")
      bubble(en, _"Oh yeah! I'm here because I ran into some guy named Derig. Well, he actually came looking for me.")
      bubble(en, _"Anyways, he wanted me to come and find you.")
      bubble(HERO1, _"Derig! What does he want now?")
      bubble(en, _"He said that he found out where all of the Opal armor is and he sent me to tell you.")
      bubble(en, _"He also figured that if you didn't have a partner by now, that I should make my services available to you.")
      bubble(en, _"And let me just say that after travelling all this way to find you... I won't take no for an answer!")
      bubble(HERO1, _"Well, in that case, welcome to the team.")

      if (progress.opalhelmet == 0) then
        bubble(HERO1, _"I just got the shield. I haven't got the helmet yet I got sidetracked.")
        bubble(en, _"I see well then, down to business. The Band belongs to a merchant in Ajantara south of here.")
      else
        bubble(HERO1, _"Now, I already found the Opal Helmet and the Opal Shield.")
        bubble(en, _"So what about the other stuff? Do you already know where the Band and Armor are?")
        bubble(HERO1, _"No... no, I don't know where the other pieces are.")
        bubble(en, _"Alrighty then, down to business. The Band belongs to a merchant in Ajantara south of here.")
      end
      bubble(en, _"The Armor, however, is hidden deep in a cavern east of here.")

      if (get_numchrs() > 1) then
        bubble(en, _"Since there are already two of you. I'm going to head back to the manor.")
        move_entity(en, 30, 29, 1)
        wait_for_entity(0, en)
      else
        bubble(HERO1, _"Well, where should we go first?")
        bubble(en, _"Derig said to get the Armor last and since all that leaves is the Band, then we should go for that.")
        bubble(HERO1, _"Fair enough... let's roll.")
        add_chr(progress.finalpartner - 1)
        LOC_partner_up()
        copy_ent(en, HERO2)
        set_ent_active(en, 0)
        orient_heroes()
        drawmap()
        screen_dump()
        msg(_"$1 joined!", 255, 0)
      end
    else
      bubble(en, _"Hey... long time no see. I saw you fighting. You were amazing!")
      bubble(en, _"Oh yeah! I'm here because I ran into some guy named Derig. Well, he actually came looking for me.")
      bubble(en, _"He knew that we were separated and he gave me some information to bring back.")
      bubble(HERO1, _"Speaking of that... where did you go?")
      bubble(en, _"Heh... when we hit the teleporter, it sent me back to Ekla.")
      bubble(en, _"I was on the way back to the teleporter when I ran into Derig... and now I'm here.")
      bubble(HERO1, _"So, what did Derig tell you?")
      bubble(en, _"He told me where all the Opal stuff is... the Shield, the Band, the Armor, and the Helmet.")
      if (progress.opalhelmet == 0) then
        bubble(HERO1, _"I just got the shield. I haven't got the helmet yet I got sidetracked.")
        bubble(en, _"I see well then, down to business. The Band belongs to a merchant in Ajantara south of here.")
        bubble(en, _"The Armor, however, is hidden deep in a cavern east of here, so we should listen to Derig and leave that for last.")
      else
        bubble(HERO1, _"I already snagged the Helmet and the Shield... where are the other parts?")
        bubble(en, _"You already found the Helmet and Shield! Wow! Well, the Band is in a town called Ajantara to the south.")
        bubble(en, _"The Armor is deep in a cavern to the east. And Derig said to get the Armor last, so we should go to Ajantara first. Okay?")
      end

      if (get_numchrs() > 1) then
        bubble(en, _"When I say we, I mean you, because there are already two of you. I'm going to head back to the manor.")
        move_entity(en, 30, 29, 1)
        wait_for_entity(0, en)
      else
        bubble(HERO1, _"No problem... let's go.")
        add_chr(progress.finalpartner - 1)
        LOC_partner_up()
        copy_ent(en, HERO2)
        set_ent_active(en, 0)
        orient_heroes()
        drawmap()
        screen_dump()
        msg(_"$1 joined!", 255, 0)
      end
    end
    add_to_manor(progress.finalpartner - 1)
    progress.finalpartner = 0
  end
end


function postexec()
  return
end


function zone_handler(zn)
  if (zn == 1) then
    progress.roundnum = 0
    progress.battlestatus = 0
    change_map("main", "coliseum")

  elseif (zn == 2) then
    door_in("inn_i")

  elseif (zn == 3) then
    door_in("tournament_i")

  elseif (zn == 4) then
    door_in("shop_i")

  elseif (zn == 5) then
    door_out("inn_o")

  elseif (zn == 6) then
    door_out("tournament_o")

  elseif (zn == 7) then
    door_out("shop_o")

  elseif (zn == 8) then
    inn(_"The Coliseum Inn", 250, 1)

  elseif (zn == 9) then
    shop(22)

  elseif (zn == 10) then
    door_out("battle")

  elseif (zn == 11) then
    set_save(0)

  elseif (zn == 12) then
    bubble(HERO1, _"Hmm. The sun feels very warm right here.")
    set_save(1)

  elseif (zn == 13) then
    touch_fire(party[0])

  elseif (zn == 14) then
    door_in("after_battle")

  elseif (zn == 15) then
    if (progress.battlestatus > 0) then
      bubble(HERO1, _"I'm not ready yet.")
      return
    end
    set_run(0)
    set_can_use_item(0)
    combat(11 + progress.roundnum)
    set_can_use_item(1)
    set_run(1)
    local x, y = marker("battle")
    warp(x, y - 3, 16)
    if (get_alldead() == 1) then
      progress.battlestatus = 2
      set_party_hp(party[0], 1)
      set_alldead(0)
    else
      progress.battlestatus = 1
    end

  end
end


function LOC_partner_up()
  local a
  a = get_party_xp(get_pidx(0)) - get_party_xp(get_pidx(1))
  a = a - krnd(750)
  if (a > 0) then
    give_xp(get_pidx(1), a, 1)
  end

  a = progress.finalpartner - 1

  -- Only set up equipment if you haven't already met them
  if (LOC_manor_or_party(SENSAR) == false) then
    if (a == SENSAR) then
      set_all_equip(0, I_HAMMER2, I_SHIELD3, I_CAP3, I_ARMOR5, I_BAND2, 0)
    elseif (a == SARINA) then
      set_all_equip(1, I_SPEAR2, 0, I_CAP3, I_SUIT3, I_BAND2, 0)
    elseif (a == CORIN) then
      set_all_equip(2, I_ROD3, I_SHIELD3, I_CAP2, I_ROBE3, I_BAND2, 0)
    elseif (a == AJATHAR) then
      set_all_equip(3, I_HAMMER2, I_SHIELD3, I_CAP2, I_ROBE3, I_BAND2, 0)
    elseif (a == CASANDRA) then
      set_all_equip(4, I_STAFF2, 0, I_CAP2, I_ROBE3, I_BAND2, 0)
    elseif (a == TEMMIN) then
      set_all_equip(5, I_SWORD4, I_SHIELD3, I_HELM2, I_ARMOR5, I_BAND2, 0)
    elseif (a == AYLA) then
      set_all_equip(6, I_SWORD4, I_SHIELD3, I_CAP3, I_SUIT3, I_BAND2, 0)
    elseif (a == NOSLOM) then
      set_all_equip(7, I_ROD3, I_SHIELD3, I_CAP2, I_ROBE3, I_BAND2, 0)
    end
  end
end
