if not TEST then
	if not Bass3Test then return end

	Bass3Test.Load()
	return
end

function TEST.u( _, _, args )
	local url = args[1] or ""

	if url == "" then
		url = Bass3Test.DefaultURL
	end

	print( url )
	print( "BASS3.PlayURL", BASS3.PlayURL( url, "", function( channel, err, ... )
		print( "BASS3.PlayURL", channel, err, ... )
		print(type(channel), TypeID(channel))
		Bass3Test.SetChannal(channel)

		if IsValid(channel) then
			channel:Play()
		end
	end ) )
end

TEST.online = TEST.u
TEST.url = TEST.u
TEST._ = TEST.u

function TEST.f( _, _, args )
	local url = args[1] or ""

	if url == "" then
		url = Bass3Test.DefaultFile
	end

	print( url )
	print( "BASS3.PlayFile", BASS3.PlayFile( url, 0, function( channel, err, ... )
		print( "BASS3.PlayFile", channel, err, ... )
		print(type(channel), TypeID(channel))
		Bass3Test.SetChannal(channel)

		if IsValid(channel) then
			channel:Play()
		end
	end ) )
end

TEST.offline = TEST.f
TEST.file = TEST.f

function TEST.u2( _, _, args )
	local channel = Bass3Test.g_channel
	print( channel )

	if not IsValid( channel ) then return end
	local url = args[1] or ""

	if url == "" then
		url = Bass3Test.DefaultURL
	end

	print( url )
	print( "channel:PlayURL", channel:PlayURL( url, 0, function( channel, err, ... )
		print( "channel:PlayURL", channel, err, ... )
		print(type(channel), TypeID(channel))
	end ) )
end

TEST.online = TEST.u2
TEST.url = TEST.u2

function TEST.f2( _, _, args )
	local channel = Bass3Test.g_channel
	print( channel )

	if not IsValid( channel ) then return end
	local url = args[1] or ""

	if url == "" then
		url = Bass3Test.DefaultFile
	end

	print( url )
	print( "channel:PlayFile", channel:PlayFile( url, 0, function( channel, err, ... )
		print( "channel:PlayFile", channel, err, ... )
		print(type(channel), TypeID(channel))
	end ) )
end

TEST.offline2 = TEST.f2
TEST.file2 = TEST.f2

function TEST.e(_, _, args)
	print( "BASS3.CreateChannel()" )

	local channel = BASS3.CreateChannel()
	print( channel )

	Bass3Test.SetChannal(channel)
end

TEST.empty = TEST.e

function TEST.play( _, _, args )
	local channel = Bass3Test.g_channel
	print( channel )

	if not IsValid( channel ) then return end

	print( "GetState() ", channel:GetState( ) )
	local var = tobool( args[1] ) or false

	print( "Play() ", var )
	channel:Play( var )
	print( "GetState() ", channel:GetState( ) )
end

function TEST.pause( _, _, args )
	local channel = Bass3Test.g_channel
	print( channel )

	if not IsValid( channel ) then return end
	print( "GetState() ", channel:GetState( ) )

	print( "Pause()" )
	channel:Pause( )
	print( "GetState() ", channel:GetState( ) )
end

function TEST.stop( _, _, args )
	local channel = Bass3Test.g_channel
	print( channel )

	if not IsValid( channel ) then return end
	print( "GetState() ", channel:GetState( ) )

	print( "Stop()" )
	channel:Stop( )
	print( "GetState() ", channel:GetState( ) )
end

function TEST.remove( _, _, args )
	local channel = Bass3Test.g_channel
	print( channel )

	if not IsValid( channel ) then return end

	channel:Remove( )
	print( "Remove()" )
end
