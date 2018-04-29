if not TEST then
	if not Bass3Test then return end

	Bass3Test.Load()
	return
end

local function tag(_, _, args)
	local channel = Bass3Test.g_channel

	print( channel )
	if not IsValid( channel ) then return end
	local tag = {}

	tag["TAG_HTTP"] = channel:GetTag( BASS3.ENUM.TAG_HTTP, {} ) or "<nil>"

	tag["TAG_ID3"] = channel:GetTag( BASS3.ENUM.TAG_ID3, {} ) or "<nil>"

	tag["TAG_META"] = channel:GetTag( BASS3.ENUM.TAG_META, {} ) or "<nil>"

	tag["TAG_ICY"] = channel:GetTag( BASS3.ENUM.TAG_ICY, {} ) or "<nil>"

	tag["TAG_MF"] = channel:GetTag( BASS3.ENUM.TAG_MF, {} ) or "<nil>"

	tag["TAG_MP4"] = channel:GetTag( BASS3.ENUM.TAG_MP4, {} ) or "<nil>"

	tag["TAG_APE"] = channel:GetTag( BASS3.ENUM.TAG_APE, {} ) or "<nil>"

	tag["TAG_OGG"] = channel:GetTag( BASS3.ENUM.TAG_OGG, {} ) or "<nil>"

	tag["u1"] = channel:GetTag( 8 ) or "<nil>"
	tag["u2"] = channel:GetTag( -1 ) or "<nil>"
	PrintTable( tag )
end

function TEST.autotagr( _, _, args )
	local url = args[1] or ""

	if url == "" then
		url = Bass3Test.DefaultURL
	end

	print( url )
	print( "BASS3.PlayURL", BASS3.PlayURL( url, "", function( channel, err, ... )
		print( "BASS3.PlayURL", channel, err, ... )

		Bass3Test.SetChannal(channel)
		tag( _, _, args )
	end ) )
end

function TEST.autotagf( _, _, args )
	local url = args[1] or ""

	if url == "" then
		url = Bass3Test.DefaultFile
	end

	print( url )
	print( "BASS3.PlayFile", BASS3.PlayFile( "sound/" .. url, 0, function( channel, err, ... )
		print( "BASS3.PlayFile", channel, err, ... )

		Bass3Test.SetChannal(channel)
		tag( _, _, args )
	end ) )
end

TEST._ = tag
TEST.tag = tag
