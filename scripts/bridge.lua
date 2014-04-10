-- bridge - "On Brayden river Randen and Andra, incomplete"

-- /*
-- {
-- Which globals should we have for the (incomplete) bridge?
--
-- progress:
-- fightonbridge: Status of monsters infesting the partial bridge
--   (0) Nothing happened on bridge (haven't spoken to worker)
--   (1) Spoke with a worker, haven't tried to leave yet
--   (2) Tried to leave, monster appeared (haven't fought it yet)
--   (3) Defeated monster
--   (4) Left bridge and re-entered; bridge not completed yet
--   (5) [Not calculated]: when this is >= 5, we will use bridge2
--
-- loseronbridge: Guard who forgot his sword
--   (0) Have not spoken to man who forgot his sword
--   (1) Spoke to him after defeating the monsters
--
-- asleeponbridge: Man in the top of the bridge sleeping
--   (0) Have not spoken to man sleeping on bridge
--   (1) Man is asleep again
-- }
-- */


function autoexec()
  refresh()
end


function entity_handler(en)
  local a = progress.fightonbridge
  -- TT comments:
  -- a ==0 before talking to any worker (and before monster)
  -- a ==1 talked to workers but you have not tried to leave
  -- a ==2 tried to leave after a==1 and monster has appeared
  -- a ==3 you defeated the monster
  -- a ==4 after a==3 and you left map and returned
  -- a !=5 in this map; you will use bridge2.(lua|map)

  if (en >= 2) then
    if (a == 0) then
      progress.fightonbridge = 1
    elseif (a == 2) then
      bubble(en, _"There are some weird creatures in the water!")
      return
    elseif (a == 3) then
      bubble(en, _"I think we'll take a break before getting back to work.")
      return
    end
  end

  if (en == 0) then
    if (a == 0 or a == 1) then
      bubble(en, _"We're on the lookout for bandits.")
    elseif (a == 2) then
      bubble(en, _"Um... we don't really have any experience dealing with anything hostile... could you take a look for us?")
    elseif (a == 3) then
      bubble(en, _"Very impressive... it's a good thing you came along. Thanks!")
    elseif (a == 4) then
      bubble(en, _"We should be done soon... come back a little later.")
    end

  elseif (en == 1) then
    if (a < 2) then
      if progress.loseronbridge == 0 then
        bubble(en, _"Those bandits better not show their faces around here again!")
        bubble(HERO1, _"Or you'll thrash 'em right?")
        bubble(en, _"No... they'd better not show up because I forgot my sword!")
        wait(50)
        bubble(en, _"I probably shouldn't have told you that.")
        progress.loseronbridge = 1
      else
        bubble(en, _"Let me know if you see any bandits, will ya?")
      end
    elseif (a == 2) then
      bubble(en, _".....")
    elseif (a == 3) then
      bubble(en, _"Wow!")
    elseif (a == 4) then
      bubble(en, _"The bridge looks like it might be done by tomorrow. Why don't you go have a rest at the inn?")
    end

  elseif (en == 2) then
    if (a < 2) then
      bubble(en, _"This is very hard work!")
    elseif (a == 4) then
      bubble(en, _"That monster popped up right under my nose. Thanks for everything!")
    end

  elseif (en == 3) then
    if (a < 2) then
      bubble(en, _".....")
      if progress.asleeponbridge == 0 then
        wait(50)
        bubble(en, _"Wha...?")
        set_ent_facing(3, FACE_LEFT)
        bubble(en, _"I wasn't sleeping!")
        set_ent_facing(3, FACE_UP)
        progress.asleeponbridge = 1
      end
    elseif (a == 4) then
      bubble(en, _"Zzz...")
    end

  elseif (en == 4) then
    if (a < 2) then
      bubble(en, _"I think that guy up there is asleep.")
    elseif (a == 4) then
      bubble(en, _"Lousy, no-good... sleeping on the job...")
    end

  elseif (en == 5) then
    if (a < 2) then
      bubble(en, _"I really could use a break. It's been almost 15 minutes since the last one!")
    elseif (a == 4) then
      bubble(en, _"Now that the bridge is almost done, I think I'm due for another break.")
    end

  elseif (en == 6) then
    if (a < 2) then
      bubble(en, _"I never should have listened to my mother when she told me to take up construction.")
      bubble(en, _"I should've been a florist like my dad.")
    elseif (a == 4) then
      bubble(en, _"Now you see why I should've been a florist...")
    end

  end
end


function postexec()
  return
end


function refresh()
  showch("treasure1", 8)
end


-- Show the status of a treasures
function showch(which_marker, which_chest)
  -- Set tiles if -1 passed in as 'which_chest' or if chest already opened
  if (get_treasure(which_chest) == 1) then
    set_ftile(which_marker, 0)
    set_zone(which_marker, 0)
  end
end


function zone_handler(zn)
  if (zn == 1) then
    if progress.fightonbridge == 0 or progress.fightonbridge > 2 then
      if progress.fightonbridge == 3 then
        progress.fightonbridge = 4
      end
      change_map("main", "bridge", -1, 0)
      return
    end
    if progress.fightonbridge == 1 then
      bubble(255, "Ahhhhh!!!")
      bubble(255, "Help!")
      set_ent_movemode(2, 2)
      set_ent_movemode(3, 2)
      set_ent_movemode(4, 2)
      set_ent_movemode(5, 2)
      set_ent_movemode(6, 2)
      wait_for_entity(2, 6)
      set_ent_movemode(2, 0)
      set_ent_movemode(3, 0)
      set_ent_movemode(4, 0)
      set_ent_movemode(5, 0)
      set_ent_movemode(6, 0)
      progress.fightonbridge = 2
      x, y = marker("monster")
      set_btile(x, y, 176)
      set_zone(x, y, 3)
      set_obs(x, y, 1)
    end

  elseif (zn == 2) then
    chest(8, I_OSEED, 2)
    refresh()

  elseif (zn == 3) then
    if progress.fightonbridge == 2 then
      can_run = 0
      combat(0)
      can_run = 1
      progress.fightonbridge = 3
      x, y = marker("monster")
      set_btile(x, y, 160)
      set_zone(x, y, 0)
      set_obs(x, y, 1)
    end

  end
end
