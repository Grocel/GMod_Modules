if SERVER then return end

if not TEST then
	if not Bass3Test then return end

	Bass3Test.Load()
	return
end

function TEST.set( _, _, args )
	local ENV = tonumber( args[1] or "<nil>" )
	local Vol = tonumber( args[2] or "<nil>" )
	local Decay = tonumber( args[3] or "<nil>" )
	local Damp = tonumber( args[4] or "<nil>" )

	print( "BASS3.GetEAX IN: ", ENV, Vol, Decay, Damp )
	print( "BASS3.SetEAX: ", BASS3.SetEAX( ENV, Vol, Decay, Damp ) )
	print( "BASS3.GetEAX: ", BASS3.GetEAX( ) )
end
