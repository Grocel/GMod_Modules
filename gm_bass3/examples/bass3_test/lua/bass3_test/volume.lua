if not TEST then
	if not Bass3Test then return end

	Bass3Test.Load()
	return
end

function TEST.v( _, _, args )
	local channel = Bass3Test.g_channel
	print( channel )

	if not IsValid( channel ) then return end

	print( "GetVolume() ", channel:GetVolume( ) )
	local var = tonumber( args[1] ) or 1

	print( "SetVolume() ", var )
	channel:SetVolume( var )
	print( "GetVolume() ", channel:GetVolume( ) )
end

function TEST.vboost( _, _, args )
	local channel = Bass3Test.g_channel
	print( channel )

	if not IsValid( channel ) then return end

	print( "GetVolumeBoost() ", channel:GetVolumeBoost( ) )
	local var = tonumber( args[1] ) or 0

	print( "SetVolumeBoost() ", var )
	channel:SetVolumeBoost( var )
	print( "GetVolumeBoost() ", channel:GetVolumeBoost( ) )
end

function TEST.bl( _, _, args )
	local channel = Bass3Test.g_channel
	print( channel )

	if not IsValid( channel ) then return end

	print( "GetBalance() ", channel:GetBalance( ) )
	local var = tonumber( args[1] ) or 1

	print( "SetBalance() ", var )
	channel:SetBalance( var )
	print( "GetBalance() ", channel:GetBalance( ) )
end

TEST._ = TEST.v
