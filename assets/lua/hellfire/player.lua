function StartRangeAttack(player, d, cx, cy, includesFirstFrame)
    if player._pInvincible and player._pHitPoints == 0 and player == MyPlayer then
        SyncPlrKill(player, DeathReason.Unknown)
        return
    end

    local skippedAnimationFrames = 0
    local flags = player._pIFlags

    local animationFlags = AnimationDistributionFlags.ProcessAnimationPending
    if player._pmode == PLR_MODE.PM_RATTACK then
        animationFlags = animationFlags | AnimationDistributionFlags.RepeatedAction
    end
    NewPlrAnim(player, player_graphic.Attack, d, animationFlags, skippedAnimationFrames, player._pAFNum)

    player._pmode = PLR_MODE.PM_RATTACK
    FixPlayerLocation(player, d)
    SetPlayerOld(player)
    player.position.temp = { cx = cx, cy = cy }
end
