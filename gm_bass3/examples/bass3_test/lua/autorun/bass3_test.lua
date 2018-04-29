AddCSLuaFile()

local col_err = Color(255, 128, 0)

if not BASS3 then
	local run = pcall( require, "bass3" )
	if not run then
		MsgC(col_err, "BASS3 was could not be loaded!\n")
		return
	end
end

if not BASS3 then
	MsgC(col_err, "_G.BASS3 does not exist!\n")
	return
end

if VERSION < 180126 and VERSION > 5 then
	MsgC(col_err, "Your GMod (Version: " .. VERSION .. ") is too old!\n")
	return
end

if BASS3.ModuleVersion < 13 then
	MsgC(col_err, "Your BASS3 (Version: " .. BASS3.ModuleVersion .. ") is too old!\n")
	return
end

if BASS3.Version < 2041308 then
	MsgC(col_err, "Your BASS3 (Version: " .. BASS3.Version .. ") is too old!\n")
	return
end

local BASS3 = BASS3 or {}
local _R = debug.getregistry( )

local TmpR = {}
local HasR = false

for k, v in pairs(_R) do
	if not isstring(k) then continue end
	if not istable(v) then continue end
	if not string.match(k, "^IBASS3") then continue end

	TmpR[k] = v
	HasR = true
end

if not HasR then
	MsgC(col_err, "_R.IBASS3* is not populated!\n")
	return
end

PrintTable( {
	BASS3 = BASS3 or "nil",
	_R = TmpR
} )

TmpR = nil

local col1 = Color( 255, 255, 0 )
local col2 = Color( 200, 200, 200 )
local col3 = Color( 0, 255, 0 )

local function addcmd( cmd, func, ... )
	cmd = string.Trim(tostring(cmd or ""))
	if cmd == "" then return end

	cmd = string.lower(cmd)

	if SERVER then
		cmd = "sv_" .. cmd
	else
		cmd = "cl_" .. cmd
	end

	local function run(...)
		local funcinfo = debug.getinfo(func, "lS")

		MsgC( col1, "[BASS3 Test]" )

		MsgC( col2, " Running command: " )
		MsgC( col3, cmd )
		MsgC( col2, " -> " )
		MsgC( col3, funcinfo.short_src .. ":" .. funcinfo.linedefined, "\n" )

		return func(...)
	end

	concommand.Add( cmd, run, ... )

	MsgC( col1, "[BASS3 Test]" )
	MsgC( col2, " Added command: " )
	MsgC( col3, cmd, "\n" )
end

Bass3Test = Bass3Test or {}
Bass3Test.cmd = {}

if IsValid(Bass3Test.g_channel) then
	Bass3Test.g_channel:Remove()
	Bass3Test.g_channel = nil
end

Bass3Test.DefaultURL = "http://sv.ggradio.net:8000/stream"
Bass3Test.DefaultFile = "sound/music/hl1_song10.mp3"

local luapath = "bass3_test"

local function AddTestCase(script)
	script = script or ""
	if script == "" then return false end

	local scriptfile = luapath .. "/" .. script
	scriptfile = string.lower(scriptfile)

	local scriptname = string.StripExtension(script)

	if not file.Exists(scriptfile, "LUA") then return false end

	TEST = nil
	TEST = {}

	AddCSLuaFile(scriptfile)
	include(scriptfile)

	if not istable(TEST) then
		TEST = nil
		return false
	end

	local found = false
	for k, func in pairs(TEST) do
		local casename = string.Trim(tostring(k or ""))
		if not isfunction(func) then continue end
		if casename == "" then continue end

		if casename == "_" then
			casename = scriptname
		else
			casename = scriptname .. "_" .. casename
		end

		Bass3Test.g_cmd[#Bass3Test.g_cmd + 1] = {
			name = "b3t_" .. casename,
			func = func,
		}

		Bass3Test.g_cmd[#Bass3Test.g_cmd + 1] = {
			name = "bass3test_" .. casename,
			func = func,
		}

		found = true
	end

	if not found then
		TEST = nil
		return false
	end

	Bass3Test.g_tests[scriptname] = TEST
	TEST = nil

	return true
end

function Bass3Test.Load()
	local files = file.Find( luapath .. "/*", "LUA" )

	Bass3Test.g_tests = {}
	Bass3Test.g_cmd = {}

	for _, f in pairs( files or {} ) do
		AddTestCase(f)
	end

	table.SortByMember(Bass3Test.g_cmd, "name", true)

	for i, v in ipairs(Bass3Test.g_cmd) do
		addcmd(v.name, v.func)
	end

	collectgarbage( "collect" )
end

function Bass3Test.SetChannal(ch)
	if ch and Bass3Test.g_channel == ch then
		return
	end

	if IsValid( Bass3Test.g_channel ) then
		Bass3Test.g_channel:Remove()
	end

	Bass3Test.g_channel = ch
end

function Bass3Test.GetChannal(ch)
	return Bass3Test.g_channel
end

Bass3Test.Load()
