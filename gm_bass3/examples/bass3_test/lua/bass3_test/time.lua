if not TEST then
	if not Bass3Test then return end

	Bass3Test.Load()
	return
end

function TEST._( _, _, args )
	local channel = Bass3Test.g_mainpanel
	print( channel )

	if not IsValid( channel ) then return end

	print( "IsBlockStreamed() ", channel:IsBlockStreamed( ) )
	print( "GetTime() ", channel:GetTime( ) )
	print( "GetTime() ", channel:GetTime( ) )

	local var = tonumber( args[1] ) or 0
	print( "SetTime() ", var )

	channel:SetTime( var )

	print( "GetTime() ", channel:GetTime( ) )
	print( "GetTime() ", channel:GetTime( ) )
	print( "GetTime() ", channel:GetTime( ) )
	print( "GetTime() ", channel:GetTime( ) )
	print( "GetTime() ", channel:GetTime( ) )
	print( "GetTime() ", channel:GetTime( ) )
	print( "GetTime() ", channel:GetTime( ) )
end

function TEST.get( _, _, args )
	local channel = Bass3Test.g_mainpanel
	print( channel )

	if not IsValid( channel ) then return end

	print( "IsBlockStreamed() ", channel:IsBlockStreamed( ) )
	print( "GetTime() ", channel:GetTime( ) )
	print( "GetLength() ", channel:GetLength( ) )
	print( "IsEndless() ", channel:IsEndless( ) )
end

TEST._ = TEST.get
