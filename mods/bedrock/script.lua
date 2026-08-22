-- V1API

function onStart()

end

-- при запуске игры (т.е. при загрузке мода)

function onTick()

end

-- функция при тике игры

function onUpdate(partialTicks)

end

-- ф-ия при рендере, может нужно будет partialTicks для чего-то

function onPlace(x, y, z, id)

end

function onBreak(x, y, z)

end

function onInteract(tileId) -- Вызывается при нажатии пкм, передаёт айди выбранного блока в инвентаре

end

-- вызывается при получении урона мобом. entityId - кто получил, attackerId - кто нанёс урон, damager - кол-во снесённого хп
function onEntityHurt(entityId, attackerId, damage)

end