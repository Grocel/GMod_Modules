if not TEST then
	if not Bass3Test then return end

	Bass3Test.Load()
	return
end

function TEST.get( _, _, args )
	print( "BASS3.GetAgent: " )
	print( BASS3.GetAgent( ) )
end

function TEST.set( _, _, args )
	local agent = args[1]

	print( "BASS3.SetAgent IN: ", Agent )
	print( "BASS3.SetAgent: ", BASS3.SetAgent( Agent ) )
	print( "BASS3.GetAgent: " )
	print( BASS3.GetAgent( ) )
end

TEST._ = TEST.set
