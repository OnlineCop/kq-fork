-- grotto2 - "Second, larger part of grotto north of Ekla"

function autoexec()
  refresh()
end


function entity_handler(en)
  return
end


function postexec()
  return
end


function refresh()
  if progress.cancelrod > 0 then
    set_mtile("cancel", 265)
    set_zone("cancel", 0)
  end

  if (get_treasure(48) == 1) then
    set_mtile("treasure1", 265)
    set_zone("treasure1", 0)
  end

  if (get_treasure(49) == 1) then
    set_mtile("treasure2", 265)
    set_zone("treasure2", 0)
  end
end


function zone_handler(zn)
  if (zn == 0) then
    combat(24)

  elseif (zn == 1) then
    change_map("grotto", "portal")

  elseif (zn == 2) then
    if progress.talkoldman == 3 then
      msg(_"$0 fits the rusty key into the lock.", 255, 0)
      progress.talkoldman = 4
    elseif progress.talkoldman == 4 then
      bubble(HERO1, _"Something's supposed to happen here... it hasn't been programmed yet.")
    else
      bubble(HERO1, _"This door is stuck tight.")
    end

  elseif (zn == 3) then
    if progress.cancelrod == 0 then
      progress.cancelrod = 1
      add_special_item(SI_CANCELROD)
      sfx(5)
      msg(_"Rod of Cancellation procured", 255, 0)
      refresh()
    end

  elseif (zn == 4) then
    chest(48, I_MHERB, 1)
    refresh()

  elseif (zn == 5) then
    chest(49, I_HELM1, 1)
    refresh()

  end
end
