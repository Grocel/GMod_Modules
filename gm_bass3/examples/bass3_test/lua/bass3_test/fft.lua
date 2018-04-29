if not TEST then
	if not Bass3Test then return end

	Bass3Test.Load()
	return
end

local oldfloat = 0
local oldvar = 0

local function getLevelColor( float )
	float = math.Clamp( float or 0, 0, 1 )

	return Color( 510 * float, 510 * ( 1 - float ), 0, 255 )
end

local function PrintFloat( float, len )
	float = math.Clamp( float, 0, 1 )
	if ( float == oldvar and oldfloat == float ) then return end
	local str = ""

	if ( float >= oldfloat ) then
		oldfloat = float
	end

	local bar = math.Round( float * len )
	local space = len - math.Round( float * len )
	local space1 = math.Round( ( oldfloat - float ) * len )
	local space2 = space - space1 - 1
	local levelcol = getLevelColor( float )
	str = string.rep( "#", bar ) .. string.rep( " ", space1 ) .. ( math.Round( oldfloat * len ) < len and " " or "" ) .. string.rep( " ", space2 )
	MsgC( levelcol, str, " ", string.format( "% 7.2f%%\t", float * 100 ) )

	if ( float < oldfloat ) then
		oldfloat = oldfloat - 0.5 * FrameTime( )
	end

	oldvar = float

	return str
end

local function PrintFloatFFT( fft, len, isdb, soundlevel )
	local strout = "\n" .. string.rep( "-", len + 1 ) .. "\n"
	local level = 0
	local str = ""
	local bar = 0
	local space = 0
	local levelcol = getLevelColor( soundlevel )

	if isdb then
		for i = 1, math.Clamp( #fft, 0, 64 ) do
			level = fft[i] or 0
			level = ( level + 60 ) / 60
			level = math.Clamp( level, 0, 1 )
			bar = math.Round( level * len )
			space = len - bar
			str = "|" .. string.rep( "#", bar ) .. string.rep( " ", space )
			strout = strout .. ( str .. "\t" .. string.format( "% 8.2f%%\t", level * 100 ) .. "\n" )

			if ( i % 8 ) == 0 then
				MsgC( levelcol, strout )
				strout = ""
			end
		end
	else
		for i = 1, math.Clamp( #fft, 0, 64 ) do
			level = math.Clamp( fft[i] or 0, 0, 1 )
			bar = math.Round( level * len )
			space = len - bar
			str = "|" .. string.rep( "#", bar ) .. string.rep( " ", space )
			strout = strout .. ( str .. "\t" .. string.format( "% 8.2f%%\t", level * 100 ) .. "\n" )

			if ( i % 8 ) == 0 then
				MsgC( levelcol, strout )
				strout = ""
			end
		end
	end

	strout = strout .. "\n"
	MsgC( levelcol, strout )
end

local function PrintFloatFFTC( fft, len, soundlevel )
	local strout = "\n" .. string.rep( "-", len + 1 ) .. "\n"
	len = len / 2
	local level = 0
	local nlevel = 0
	local str = ""
	local bar = 0
	local space = len - bar
	local nbar = 0
	local nspace = len - nbar
	local levelcol = getLevelColor( soundlevel )

	for i = 1, math.Clamp( #fft, 0, 64 ) do
		level = math.Clamp( ( fft[i] or 0 ), 0, 1 ) ^ 0.5
		nlevel = math.Clamp( ( -fft[i] or 0 ), 0, 1 ) ^ 0.5
		str = ""
		bar = math.Round( level * len )
		space = len - bar
		nbar = math.Round( nlevel * len )
		nspace = len - nbar
		str = string.rep( " ", nspace ) .. string.rep( "#", nbar ) .. "|" .. string.rep( "#", bar ) .. string.rep( " ", space )
		strout = strout .. ( str .. "\t" .. string.format( "% 8.2f%%\t", ( level - nlevel ) * 100 ) .. "\n" )

		if ( i % 8 ) == 0 then
			MsgC( levelcol, strout )
			strout = ""
		end
	end

	strout = strout .. "\n"
	MsgC( levelcol, strout )
end

function TEST.print_fft( _, _, args )
	local channel = Bass3Test.g_channel
	print( channel )
	if not IsValid( channel ) then return end

	local fft = {}
	print( channel:FFT( fft, BASS3.ENUM.FFT_64 ) )
	PrintTable( fft )
end

function TEST.print_fftc( _, _, args )
	local channel = Bass3Test.g_channel
	print( channel )
	if not IsValid( channel ) then return end

	local fftc = {}
	print( channel:FFTComplex( fftc, BASS3.ENUM.FFT_64 ) )
	PrintTable( fftc )
end


local doprint_p = false
local doprint_f = false
local doprint_fc = false

local doprint_fft = nil
local doprint_args = nil

function TEST.p( _, _, args )
	local channel = Bass3Test.g_channel

	doprint_fft = nil
	doprint_args = nil

	print( channel )
	if not IsValid( channel ) then return end

	doprint_f = false
	doprint_fc = false
	doprint_p = args[1] ~= "stop"
	doprint_fft = {}
	doprint_args = args
end

function TEST.f( _, _, args )
	local channel = Bass3Test.g_channel

	doprint_fft = nil
	doprint_args = nil

	print( channel )
	if not IsValid( channel ) then return end

	doprint_p = false
	doprint_fc = false
	doprint_f = args[1] ~= "stop"
	doprint_fft = {}
	doprint_args = args
end

TEST._ = TEST.f

function TEST.fc( _, _, args )
	local channel = Bass3Test.g_channel

	doprint_fft = nil
	doprint_args = nil

	print( channel )
	if not IsValid( channel ) then return end

	doprint_p = false
	doprint_f = false
	doprint_fc = args[1] ~= "stop"
	doprint_fft = {}
	doprint_args = args
end


local oldtime = CurTime( )

local function printthink( )
	local channel = Bass3Test.g_channel

	if not IsValid( channel ) then
		doprint_f = false
		doprint_fc = false
		doprint_p = false

		return
	end

	if channel:GetState() == BASS3.ENUM.CHANNEL_STOPPED then
		doprint_f = false
		doprint_fc = false
		doprint_p = false

		return
	end

	if not doprint_fft then
		doprint_f = false
		doprint_fc = false
		doprint_p = false

		return
	end

	if not doprint_args then
		doprint_f = false
		doprint_fc = false
		doprint_p = false

		return
	end

	local now = CurTime( )

	if doprint_f then
		local RemoveDCBias = ( tonumber( doprint_args[1] ) or 0 ) ~= 0
		local RemoveHannWindow = ( tonumber( doprint_args[2] ) or 0 ) ~= 0
		local bAsDecibel = ( tonumber( doprint_args[3] ) or 0 ) ~= 0
		--if (now - oldtime) < 0.05 then return end
		oldtime = CurTime( )
		local count = channel:FFT( doprint_fft, BASS3.ENUM.FFT_32, RemoveDCBias, RemoveHannWindow, bAsDecibel )
		print( count )
		local l, r = channel:GetLevel( 0.1, true, true )
		PrintFloatFFT( doprint_fft, 100, bAsDecibel, ( ( ( ( l + r ) / 2 ) + 60 ) / 60 ) ^ 2 )

		return
	end

	if doprint_fc then
		local RemoveDCBias = ( tonumber( doprint_args[1] ) or 0 ) ~= 0
		local RemoveHannWindow = ( tonumber( doprint_args[2] ) or 0 ) ~= 0
		--if (now - oldtime) < 0.05 then return end
		oldtime = CurTime( )
		local count = channel:FFTComplex( doprint_fft, BASS3.ENUM.FFT_32, RemoveDCBias, RemoveHannWindow )
		print( count )
		local l, r = channel:GetLevel( 0.1, true, true )
		PrintFloatFFTC( doprint_fft, 100, ( ( ( ( l + r ) / 2 ) + 60 ) / 60 ) ^ 2 )

		return
	end

	if doprint_p then
		local TimeFrame = tonumber( doprint_args[1] ) or 0

		if TimeFrame == 0 then
			TimeFrame = nil
		end

		local bRMS = ( tonumber( doprint_args[2] ) or 0 ) ~= 0
		local bAsDecibel = ( tonumber( doprint_args[3] ) or 0 ) ~= 0
		--if (now - oldtime) < 0.01 then return end
		oldtime = CurTime( )
		local l, r = channel:GetLevel( TimeFrame, bRMS, bAsDecibel )

		if bAsDecibel then
			PrintFloat( ( ( l + 40 ) / 40 ), 50 )
			PrintFloat( ( ( r + 40 ) / 40 ), 50 )
		else
			PrintFloat( l, 50 )
			PrintFloat( r, 50 )
		end

		Msg( "\n" )

		return
	end
end

hook.Remove( "Think", "bass3test_printthink" )
hook.Remove( "PostRenderVGUI", "bass3test_printthink" )

if ( CLIENT ) then
	-- Doesn't get paused in single player.
	hook.Add( "PostRenderVGUI", "bass3test_printthink", printthink )
else
	hook.Add( "Think", "bass3test_printthink", printthink )
end
