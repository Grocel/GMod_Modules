if SERVER then return end

if not TEST then
	if not Bass3Test then return end

	Bass3Test.Load()
	return
end

function TEST.set( _, _, args )
	local channel = Bass3Test.g_channel
	print( channel )

	if not IsValid( channel ) then return end

	local mode3D = channel:Get3DEnabled()
	print( "Get3DEnabled(): ", mode3D )
	channel:Set3DEnabled(!mode3D)

	mode3D = channel:Get3DEnabled()
	print( "Set3DEnabled() Set: ", mode3D )
end

function TEST.get( _, _, args )
	local channel = Bass3Test.g_channel
	print( channel )

	if not IsValid( channel ) then return end

	local mode3D = channel:Get3DEnabled()
	print( "Set3DEnabled() Set: ", mode3D )
end
