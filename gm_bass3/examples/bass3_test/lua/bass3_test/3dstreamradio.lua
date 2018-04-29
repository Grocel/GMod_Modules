if not StreamRadioLib then return end

if not TEST then
	if not Bass3Test then return end

	Bass3Test.Load()
	return
end

function TEST._( ply, _, args )
	if not StreamRadioLib then return end
	if not StreamRadioLib.Trace then return end

	local tr = StreamRadioLib.Trace(ply)
	if not tr then return end

	local Radio = tr.Entity
	if not IsValid( Radio ) then return end
	if not Radio.__IsRadio then return end
	if not IsValid( Radio.StreamObj ) then return end

	local ch = Radio.StreamObj:GetChannel()
	Bass3Test.SetChannal(ch)

	local col1 = Color( 255, 255, 0 )
	local col2 = Color( 200, 200, 200 )
	local col3 = Color( 0, 255, 0 )

	MsgC( col1, "[BASS3 Test]" )
	MsgC( col2, " Channel set " )
	MsgC( col3, ch )
	MsgC( col2, " from radio " )
	MsgC( col3, Radio, "\n" )
end
