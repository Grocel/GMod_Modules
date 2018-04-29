if not TEST then
	if not Bass3Test then return end

	Bass3Test.Load()
	return
end

function TEST.get( _, _, args )
	print( "BASS3.GetProxy: " )
	print( BASS3.GetProxy( ) )
end

function TEST.set( _, _, args )
	local Server = args[1]
	local Port = tonumber(args[2])
	local Name = args[3]
	local Pass = args[4]

	print( "BASS3.SetProxy IN: ", Server, Port, Name, Pass )
	print( "BASS3.SetProxy: ", BASS3.SetProxy( Server, Port, Name, Pass ) )
	print( "BASS3.GetProxy: " )
	print( BASS3.GetProxy( ) )
end

TEST._ = TEST.set
