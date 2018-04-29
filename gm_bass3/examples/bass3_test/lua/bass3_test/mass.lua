if not TEST then
	if not Bass3Test then return end

	Bass3Test.Load()
	return
end

function TEST._( ply, _, args )
	Bass3Test.g_masschannels = Bass3Test.g_masschannels or {}
	local masschannels = Bass3Test.g_masschannels

	for k, v in pairs(masschannels) do
		if IsValid(v) then
			v:Remove()
		end

		Bass3Test.g_masschannels[k] = nil
	end

	local count = tonumber(args[1] or "0") or 0
	if count <= 0 then
		count = 500
	end

	for i = 1, count do
		print( "BASS3.PlayFile " .. i, BASS3.PlayFile( "sound/mp3_test1.mp3", 0, function( channel, err, ... )
			print( "BASS3.PlayFile " .. i, channel, err, ... )
			masschannels[i] = channel

			if ( masschannels[i] ) then
				masschannels[i]:Play( )
			end
		end ) )
	end
end
