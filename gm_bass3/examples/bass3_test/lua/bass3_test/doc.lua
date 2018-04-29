if not TEST then
	if not Bass3Test then return end

	Bass3Test.Load()
	return
end

function TEST._( _, _, args )
	local BASS3_ENUM = BASS3.ENUM or {}
	local ENUM_TYPES = {}
	local BASS3_TYPES = {}
	local IBASS3Channel = _R.IBASS3Channel or {}

	for k, v in pairs( BASS3_ENUM ) do
		local split = string.Split( k, "_" )
		local t = split[1]
		ENUM_TYPES[t] = ENUM_TYPES[t] or {}

		if t == "ERROR" then
			ENUM_TYPES[t][k] = {
				sortby = v,
				value = v,
				desc = BASS3.DecodeError( v )
			}
		elseif t == "FFT" then
			local levels = tonumber( split[2] ) or 0

			ENUM_TYPES[t][k] = {
				sortby = levels,
				value = v,
				desc = string.format( "%i levels for normal, %i levels for complex.", levels / 2, levels * 2 )
			}
		elseif t == "TAG" then
			local tag = split[2]

			ENUM_TYPES[t][k] = {
				sortby = v,
				value = v,
				desc = string.format( 'Receives the "%s" tag information.', tag )
			}
		else
			ENUM_TYPES[t][k] = {
				sortby = v,
				value = v,
				desc = "..."
			}
		end
	end

	for k, v in pairs( BASS3 ) do
		if istable( v ) then continue end
		local t = isfunction( v ) and 1 or 2
		BASS3_TYPES[t] = BASS3_TYPES[t] or {}
		BASS3_TYPES[t][k] = v
	end

	local strout = ""
	strout = strout .. "## Lua references\n\n"
	strout = strout .. "#### Enumerations\n\n"

	for t, list in SortedPairs( ENUM_TYPES ) do
		strout = strout .. string.format( "##### %s_\\*\n", t )
		strout = strout .. "| Name | Value | Description |\n"
		strout = strout .. "| ---- | ----- | ----------- |\n"

		for k, v in SortedPairsByMemberValue( list, "sortby" ) do
			strout = strout .. string.format( "| BASS3.ENUM.%s | %s | %s |\n", k, v.value, v.desc )
		end

		strout = strout .. "\n\n"
	end

	strout = strout .. "\n\n"
	strout = strout .. "#### BASS3\n\n"

	for t, list in SortedPairs( BASS3_TYPES ) do
		if t == 1 then
			strout = strout .. "##### Functions\n"
			strout = strout .. "| Name | Arguments | Returned values | Description |\n"
			strout = strout .. "| ---- | --------- | --------------- | ----------- |\n"

			for k, v in SortedPairs( list ) do
				strout = strout .. string.format( "| BASS3.%s() | ... | ... | ... |\n", k )
			end
		else
			strout = strout .. "##### Variables\n"
			strout = strout .. "| Name | Value | Description |\n"
			strout = strout .. "| ---- | ----- | ----------- |\n"

			for k, v in SortedPairs( list ) do
				strout = strout .. string.format( "| BASS3.%s | %s | ... |\n", k, v )
			end
		end

		strout = strout .. "\n\n"
	end

	strout = strout .. "\n\n"
	strout = strout .. "#### IBASS3Channel object\n\n"
	strout = strout .. "| Name | Additional arguments | Returned values | Description |\n"
	strout = strout .. "| ---- | -------------------- | --------------- | ----------- |\n"

	for k, v in SortedPairs( IBASS3Channel.__index ) do
		if not isfunction( v ) then continue end
		strout = strout .. string.format( "| IBASS3Channel:%s() | ... | ... | ... |\n", k, v )
	end

	strout = strout .. "\n\n"
	local fn = "mt_doc.md.txt"
	local f = file.Open( fn, "w", "DATA" )
	f:Write( strout )
	f:Close( )
	print( "mt_doc to '" .. fn .. "' done!" )
end
