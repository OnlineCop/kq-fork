-- dville - "Denorian village southeast of Oracle's tower"

-- /*
-- {
-- P_DENORIAN: Status of the stolen statue
--   (0) You just got there; statue is gone
--   (1) If you refuse after you hear their plea, you are kicked out of town
--   (2) You've agreed to help
--   (3) You found Demnas (but not the troll)
--   (4) You found the troll, too
--   (5) The broken statue is returned to the village
--   (6) The 2nd half of the statue is returned (not done)
-- }
-- */

function autoexec()
  return
end


function entity_handler(en)
  local CorinInParty = 0, cp
  -- Anyone except head of town council kicks you out if you refuse to help
  if (get_progress(P_DENORIAN) == 1 and en ~= 7) then
    bubble(en, _"You're not welcome here!")
    change_map("main", "dville")
    return
  end

  if (en == 0) then
    if (get_progress(P_DENORIAN) < 5) then
      bubble(en, _"Who would want to steal from us?")
    elseif (get_progress(P_DENORIAN) == 5) then
      bubble(en, _"I thought as much.")
    else
      bubble(en, _"We are so happy to have the statue whole once again!")
    end

  elseif (en == 1) then
    if (get_progress(P_DENORIAN) < 5) then
      bubble(en, _"This is terrible.")
    elseif (get_progress(P_DENORIAN) == 5) then
      bubble(en, _"I'm glad the troll isn't a threat to us anymore.")
    else
      bubble(en, _"I'm glad that's over.")
    end

  elseif (en == 2) then
    bubble(en, _"That statue was given to the village after the evil Kalatos was defeated in the Great War.")

  elseif (en == 3) then
    if (get_progress(P_DENORIAN) < 5) then
      bubble(en, _"The village council says that it was a troll who stole the statue. What's a troll?")
    elseif (get_progress(P_DENORIAN) == 5) then
      bubble(en, _"Mommy told me what a troll is. That's so scary!")
    else
      bubble(en, _"The statue is home.")
    end

  elseif (en == 4) then
    if (get_progress(P_DENORIAN) < 5) then
      bubble(en, _"Apparently, there has not been a troll spotted in these parts since the Great War.")
    elseif (get_progress(P_DENORIAN) == 5) then
      bubble(en, _"I wonder if our village is really safe?")
    else
      bubble(en, _"I feel so much more secure now. Thank you for everything you've done!")
    end

  elseif (en == 5) then
    if (get_progress(P_DENORIAN) < 5) then
      bubble(en, _"I can't believe this has happened to us. We are a peaceful people.")
    elseif (get_progress(P_DENORIAN) == 5) then
      bubble(en, _"Now, what to make for supper.")
    else
      bubble(en, _"Thank you for restoring peace to our people.")
    end

  elseif (en == 6) then
    if (get_progress(P_DENORIAN) < 5) then
      bubble(en, _"Where did our statue go?")
    elseif (get_progress(P_DENORIAN) == 5) then
      bubble(en, _"Things can get back to normal now.")
    else
      bubble(en, _"I've lost my hat. I should go ask the statue where to look.")
    end

  elseif (en == 7) then
    LOC_council(en)

  elseif (en == 8) then
    if (get_progress(P_DENORIAN) == 0) then
      bubble(en, _"This fellow sitting with me is the head of the village council. You should speak with him.")
    elseif (get_progress(P_DENORIAN) < 5) then
      bubble(en, _"Good luck.")
    elseif (get_progress(P_DENORIAN) == 5) then
      bubble(en, _"We shall put the statue back as soon as it's whole again!")
    else
      bubble(en, _"Thanks again.")
    end

  elseif (en == 9) then
    if (get_progress(P_DENORIAN) == 0) then
      bubble(en, _"You should hear what the head of the village council has to say.")
    elseif (get_progress(P_DENORIAN) < 5) then
      bubble(en, _"Good luck.")
    elseif (get_progress(P_DENORIAN) == 5) then
      bubble(en, _"It's a relief to have our treasured statue in our hands again.")
    else
      bubble(en, _"Thanks again.")
    end

  elseif (en == 10) then
    if (get_progress(P_DENORIAN) == 0) then
      bubble(en, _"We hope you will help us.")
    elseif (get_progress(P_DENORIAN) < 5) then
      bubble(en, _"Good luck.")
    elseif (get_progress(P_DENORIAN) == 5) then
      bubble(en, _"Although it's broken, the statue still means everything to us.")
    else
      bubble(en, _"Oh, it's wonderful to have the whole statue again!")
    end

  elseif (en == 11) then
    if (get_progress(P_DENORIAN) < 5) then
      bubble(en, _"This is where the statue was taken from.")
    elseif (get_progress(P_DENORIAN) == 5) then
      bubble(en, _"We'll put the statue back here once we have the whole thing.")
    else
      bubble(en, _"Thank you for getting our statue back!")
    end

  elseif (en == 12) then
    if (get_progress(P_DENORIAN) < 5) then
      bubble(en, _"Less council meetings, more cooking!")
    elseif (get_progress(P_DENORIAN) == 5) then
      bubble(en, _"Food always tastes better when it's cooked. Except icecream.")
    else
      bubble(en, _"Let the cookfires burn!")
    end

  elseif (en == 13) then
    if (get_progress(P_DENORIAN) < 5) then
      bubble(en, _"Geez, I wish I was old enough to go after that smelly old troll! I'd show him a thing or two.")
    elseif (get_progress(P_DENORIAN) == 5) then
      bubble(en, _"Nerakians? Where are they from?")
    else
      bubble(en, _"It would be cool to control monsters with your thoughts! ZAAAP!")
    end

  elseif (en == 14) then
    if (get_progress(P_DENORIAN) < 5) then
      bubble(en, _"The council is having another meeting about what to do to get back our statue.")
    elseif (get_progress(P_DENORIAN) == 5) then
      bubble(en, _"I think a celebration is in order.")
    else
      bubble(en, _"Ah, even more reason to celebrate!")
    end

  elseif (en == 15) then
    if (get_progress(P_DENORIAN) < 5) then
      bubble(en, _"You should find the council's hut. They are looking for people to help get the statue back.")
    elseif (get_progress(P_DENORIAN) == 5) then
      bubble(en, _"The council can relax now. Thank you.")
    else
      bubble(en, _"It sure would be nice to have a little more excitement around here. It's too... boring.")
    end

  end
end


function postexec()
  return
end


function zone_handler(zn)
  if (zn == 1) then
    change_map("main", "dville")

  elseif (zn == 2) then
    bubble(HERO1, _"Locked.")

  elseif (zn == 3) then
    door_in("room_1i")

  elseif (zn == 4) then
    door_in("room_2i")

  elseif (zn == 5) then
    door_in("room_3i")

  elseif (zn == 6) then
    door_in("room_4i")

  elseif (zn == 7) then
    door_in("room_5i")

  elseif (zn == 8) then
    door_in("room_6i")

  elseif (zn == 9) then
    door_out("room_1o")

  elseif (zn == 10) then
    door_out("room_2o")

  elseif (zn == 11) then
    door_out("room_3o")

  elseif (zn == 12) then
    door_out("room_4o")

  elseif (zn == 13) then
    door_out("room_5o")

  elseif (zn == 14) then
    door_out("room_6o")

  elseif (zn == 15) then
    touch_fire(party[0])

  end
end


function LOC_council(en)
  if (get_progress(P_DENORIAN) == 0) then
    bubble(en, _"The council would like to ask for your help in retrieving our statue. We will tell you what we know.")
    if (prompt(en, 2, 0, "However, I must first ask if",
               "you are willing to help us?",
               "  yes",
               "  no") == 0) then
      LOC_trollstory(en)
    else
      bubble(en, _"I see. Well, then you should be on your way then. My people will show you out.")
      set_progress(P_DENORIAN, 1)
      change_map("main", "dville")
    end
  elseif (get_progress(P_DENORIAN) == 1) then
    if (prompt(en, 2, 0, "Have you changed your mind?",
               "  yes",
               "  no") == 0) then
      LOC_trollstory(en)
    else
      bubble(en, _"I thought as much. Begone!")
      change_map("main", "dville")
    end
  elseif (get_progress(P_DENORIAN) == 2) then
    bubble(en, _"Good luck.")
  elseif (get_progress(P_DENORIAN) == 3 or
          get_progress(P_DENORIAN) == 4) then
    bubble(en, _"This is incredible... you actually got the statue back...")
    bubble(HERO1, _"...Well, hold on a second. I didn't get the WHOLE statue back; just a part of it. It looks like it had been broken before I got there.")
    bubble(en, _"Regardless, this much is wonderful. I wish we had some way of paying you back.")
    bubble(en, _"Unfortunately our village has next to nothing.")
    bubble(HERO1, _"Don't worry... not everything we adventurers do is for a reward.")
    bubble(en, _"What about the troll?")

    msg(_"You tell the head of the village council what you found.", 0, 0)
    bubble(en, _"Demnas... that name is familiar.")
    bubble(en, _"I remember that a tribe of Narakians once lived around here somewhere. I thought they were all wiped out by Malkaron's armies.")

    msg(_"You shrug.", 0, 0)
    bubble(en, _"Hmm...")
    bubble(en, _"Wait... I think I remember now. Demnas was the name of the Narakian's tribal leader who supposedly went crazy many years ago. It's possible that you found where he has been hiding all of this time.")

    if (get_progress(P_DENORIAN) == 3) then
      bubble(HERO1, _"Well, him being crazy explains why he was rambling on about someone trying to steal the statue from him.")
    else
      bubble(HERO1, _"He was rambling on about someone trying to seal the statue from him.")
      if (get_progress(P_TALK_CORIN) == 0) then
        bubble(HERO1, _"It still doesn't make any sense.")
        bubble(HERO1, _"Wait a minute! I remember that CORIN was down there...")
        if (get_numchrs() == 1) then
          bubble(HERO1, _"I might be able to go back there and talk to him.")
        else
          bubble(HERO1, _"We might be able to go back there and talk to him.")
        end
        bubble(HERO1, _"Maybe he'd know more about it.")
      else
        if (party[0] == CORIN) then
          CorinInParty = HERO1
          cp = "I thought"
        elseif (get_numchrs() == 2 and party[1] == CORIN) then
          CorinInParty = HERO2
          cp = "I thought"
        else
          CorinInParty = HERO1
          cp = "He thought"
        end

        -- // You met with Corin, but he's not in your party
        if (CorinInParty == 0) then
          if (get_numchrs() == 1) then
            bubble(HERO1, _"It didn't make any sense until I met up with CORIN.")
          else
            bubble(HERO1, _"It didn't make any sense until we met up with CORIN.")
          end
        end
        bubble(CorinInParty, cp.." that Malkaron's armies got to Demnas and the troll first because they were able to get away with half of the statue.")
      end
    end
    bubble(en, _"Well, I guess I'm a little relieved that we don't have a troll problem.")
    bubble(HERO1, _"Well, yeah I guess that's good. And you don't have to worry about any crazed Nerakians for awhile either.")
    bubble(en, _"That's good to hear. At the very least I can say that you will always be welcome in our village.")
    bubble(HERO1, _"Thank you.")
    set_progress(P_DENORIAN, 5)
    remove_special_item(SI_DENORIANSTATUE)
  elseif (get_progress(P_DENORIAN) == 5) then
    bubble(en, _"Good luck in your journey. If you come across the other half of our statue...")
  else
    bubble(en, _"Good journey friend.")
  end
end


function LOC_trollstory(en)
  bubble(en, _"Oh, thank you very much! We will not forget your kindness.")
  bubble(en, _"So, here is what we know. The thief is a troll. Our people have tracked the troll to a cave on the other side of the river.")
  bubble(en, _"That is where he can be found.")
  bubble(HERO1, _"Have your people tried to get the statue back themselves?")
  bubble(en, _"Yes, we have tried, but we quickly learned that we were no match for this enemy.")
  bubble(en, _"We could not even overcome the denizens of the cave, never mind the troll himself!")
  bubble(HERO1, _"Are trolls common to this area? I have seen none in my travels.")
  bubble(en, _"That is the strange part of all this. Trolls have not been seen in these parts for nearly a century!")
  bubble(en, _"I must say that we suspect that someone is directing the troll in these matters.")
  bubble(HERO1, _"Well, don't worry. We will get that statue back for you and get to the bottom of this whole thing.")
  bubble(en, _"Oh, and if you haven't seen the cave yet... or even if you have, you'll need to know that there is a trick to opening the door.")
  bubble(en, _"There is a set of stones off to the left of the entrance. You have to press them in the right order to gain access.")
  bubble(en, _"Press the first stone, then the fourth and lastly the third. Then the door will open. Got it?")
  bubble(HERO1, _"Uh... sure.")
  set_progress(P_DENORIAN, 2)
end
