if SERVER then return end

if not TEST then
	if not Bass3Test then return end

	Bass3Test.Load()
	return
end

if IsValid(Bass3Test.g_mainpanel) then
	Bass3Test.g_mainpanel:Remove()
	Bass3Test.g_mainpanel = nil
end

local function AddFxCheckbox(fxpanel, cfgpanel, funcaddfx, funcremovefx)
	local tickbox = vgui.Create("DCheckBoxLabel", fxpanel)
	tickbox:SetSize(20, 100)
	tickbox:SetPos(20, 20)
	tickbox:SetText("Enable Effect")
	tickbox:SetDark(true)

	funcaddfx = funcaddfx or (function(ch, panel)
	end)

	funcremovefx = funcremovefx or (function(fx, ch, panel)
		if not IsValid(fx) then return end
		fx:Remove()
	end)

	tickbox.OnChange = function(pnl, val)
		if not IsValid(Bass3Test.g_channel) then
			if val then
				tickbox:SetValue(false)
			end

			cfgpanel:SetDisabled(true)
			return
		end

		if not IsValid(cfgpanel) then
			if val then
				tickbox:SetValue(false)
			end

			cfgpanel:SetDisabled(true)
			return
		end

		if val then
			local fx = nil

			funcremovefx(cfgpanel.fx, Bass3Test.g_channel, cfgpanel)
			cfgpanel.fx = funcaddfx(Bass3Test.g_channel, cfgpanel)

			if cfgpanel.fx then
				cfgpanel:SetDisabled(false)
			else
				tickbox:SetValue(false)
			end
		else
			funcremovefx(cfgpanel.fx, cfgpanel)
			cfgpanel.fx = nil
			cfgpanel:SetDisabled(true)
		end
	end

	local oldthink = tickbox.Think

	tickbox.Think = function(pnl)
		if oldthink then
			oldthink(pnl)
		end

		if not IsValid(Bass3Test.g_channel) then
			if tickbox:GetValue() then
				tickbox:SetValue(false)
			end
		end
	end


	tickbox:SetValue(false)
	return tickbox
end


local function BuildChannelPanel(mainpanel, sheet)
	local panel = vgui.Create("DPanel", sheet)
	panel:SetPaintBackground( false )

	local textdesc = vgui.Create("DLabel", panel)
	textdesc:SetSize(300, 20)
	textdesc:SetPos(20, 50)
	textdesc:SetText("Channel URL")
	textdesc:SetDark(true)

	local textinput = vgui.Create("DTextEntry", panel)
	textinput:SetSize(300, 20)
	textinput:SetPos(20, 70)
	textinput:SetText(Bass3Test.DefaultURL)

	local stopbutton = vgui.Create("DButton", panel)
	stopbutton:SetSize(140, 20)
	stopbutton:SetPos(20, 100)
	stopbutton:SetText("Stop")

	stopbutton.DoClick = function()
		if IsValid(Bass3Test.g_channel) then
			Bass3Test.g_channel:Remove()
			Bass3Test.g_channel = nil
		end
	end

	local playbutton = vgui.Create("DButton", panel)
	playbutton:SetSize(140, 20)
	playbutton:SetPos(180, 100)
	playbutton:SetText("Play")

	playbutton.DoClick = function()
		if IsValid(Bass3Test.g_channel) then
			Bass3Test.g_channel:Remove()
			Bass3Test.g_channel = nil
		end

		url = textinput:GetText()

		BASS3.PlayFile( url, 0, function( ch, err, ... )
			print("BASS3.PlayFile", url, ch, err, ... )
			print("ch type", type(ch), TypeID(ch))

			if IsValid(Bass3Test.g_channel) then
				Bass3Test.g_channel:Remove()
				Bass3Test.g_channel = nil
			end

			if not IsValid(ch) then
				BASS3.PlayURL( url, "", function( ch, err, ... )
					print("BASS3.PlayURL", url, ch, err, ... )
					print("ch type", type(ch), TypeID(ch))

					if IsValid(Bass3Test.g_channel) then
						Bass3Test.g_channel:Remove()
						Bass3Test.g_channel = nil
					end

					if not IsValid(ch) then
						return
					end

					Bass3Test.g_channel = ch
					Bass3Test.g_channel:Play()

				end)

				return
			end

			Bass3Test.g_channel = ch
			Bass3Test.g_channel:Play()
		end)
	end

	local fftpanel = vgui.Create("DPanel", panel)
	fftpanel:SetPaintBackground( false )

	panel.PerformLayout = function(pnl)
		local W, H = panel:GetSize()
		fftpanel:SetSize(W - 40, H - 160)
		fftpanel:SetPos(20, 140)
	end

	local fft = {}

	fftpanel.Paint = function(pnl, pw, ph)
		local channel = Bass3Test.g_channel
		if not IsValid(channel) then return end

		local count = channel:FFT(fft, BASS3.ENUM.FFT_64, false, false, true)

		local x, y = 0, 0
		local w, h = pw, ph

		local inner_w = w / 1.333

		local groundz = inner_w / count * 0.707 * 2
		local wbars = inner_w - groundz
		local wground = w - groundz

		local barz = groundz / 2

		local totalbarw_inner = wbars / count
		local totalbarw_outer = (wground - (totalbarw_inner / 2)) / count

		local barzhalf = barz / 2
		local inner_h =	h - barzhalf

		for j=1, groundz do
			surface.SetDrawColor(color_black)
			surface.DrawRect(x + groundz - j, h - j, wground, 1)
		end

		barx = w - barz

		for i = count, 1, -1 do
			level = fft[i] or 0
			level = level * -0.01667
			level = 1 - math.Clamp(level, 0, 1)

			local barh = level * inner_h - barz
			local barw = totalbarw_inner

			if i == count then
				barx = barx - totalbarw_inner
			else
				barx = barx - totalbarw_outer
			end

			local bary = y + (inner_h - barh)
			local col = 120 * (1 - level)

			for j=1, barz do
				surface.SetDrawColor(HSVToColor(col, 1, 0.75))
				surface.DrawRect(barx - j, bary - j, barw, barh)
			end

			surface.SetDrawColor(HSVToColor(col, 1, 1))
			surface.DrawRect(barx, bary, barw, barh)
		end

		surface.SetTextColor(color_white)

		surface.SetTextPos(x + 10, y + 10)
		surface.DrawText(tostring(channel))
	end

	return panel
end

local pnlcreaters = {
	["float"] = function(parentpnl, valuedata)
		local pnl = vgui.Create("DNumSlider", parentpnl)
		if not IsValid(pnl) then return end

		local unit = valuedata.unit or ""
		if unit ~= "" then
			unit = " (" .. unit .. ")"
		end

		pnl:SetText((valuedata.name or "(unknown)") .. unit)
		pnl:SetMin(valuedata.min or 0)
		pnl:SetMax(valuedata.max or 0)
		pnl:SetDecimals(valuedata.decimals or 3)
		pnl:SetDark(true)
		pnl:SetSize(10, 30)

		pnl.mt_SetValue = function(this, ...)
			if not IsValid(this) then return end
			return this:SetValue(...)
		end

		pnl.mt_GetValue = function(this, ...)
			if not IsValid(this) then return end
			return this:GetValue(...)
		end

		pnl.OnValueChanged = function(this, value)
			if not IsValid(this) then return end
			if not this.mt_OnChange then return end
			this:mt_OnChange(value)
		end

		pnl.mt_valuedata = valuedata
		return pnl
	end,

	["bool"] = function(parentpnl, valuedata)
		local pnl = vgui.Create("DCheckBoxLabel", parentpnl)
		if not IsValid(pnl) then return end

		pnl:SetText(valuedata.name or "(unknown)")
		pnl:SetDark(true)
		pnl:SetSize(10, 20)

		pnl.mt_SetValue = function(this, ...)
			if not IsValid(this) then return end
			return this:SetValue(...)
		end

		pnl.mt_GetValue = function(this, ...)
			if not IsValid(this) then return end
			return this:GetValue(...)
		end

		pnl.OnChange = function(this, value)
			if not IsValid(this) then return end
			if not this.mt_OnChange then return end
			this:mt_OnChange(value)
		end

		pnl.mt_valuedata = valuedata
		return pnl
	end,

	["flag"] = function(parentpnl, valuedata)
		local pnl = vgui.Create("DComboBox", parentpnl)
		if not IsValid(pnl) then return end

		pnl:SetSize(10, 25)

		local lookup = {
			name = {},
			index = {}
		}

		local index = 1
		for k, v in pairs(valuedata.options) do
			k = (valuedata.name or "") .. ": " .. k

			pnl:AddChoice(k, v)

			lookup.name[v] = k
			lookup.index[v] = index
			index = index + 1
		end

		pnl.mt_SetValue = function(this, value)
			if not IsValid(this) then return end
			return this:ChooseOption(lookup.name[value] or "unknown", lookup.index[value] or -1)
		end

		pnl.mt_GetValue = function(this)
			if not IsValid(this) then return end
			local _, value = this:GetSelected()
			return value
		end

		pnl.OnSelect = function(this, index, name, value)
			if not IsValid(this) then return end
			if not this.mt_OnChange then return end
			this:mt_OnChange(value)
		end

		pnl.mt_valuedata = valuedata
		return pnl
	end,
}

local function BuildValuePanels(parentpnl, values)
	local panels = {}

	for i, v in ipairs(values) do
		local func = pnlcreaters[v.paneltype or ""]
		if not func then continue end

		local pnl = func(parentpnl, v)
		if not IsValid(pnl) then continue end

		panels[i] = pnl
	end

	return panels
end

local function BuildGenericFxPanel(mainpanel, sheet, fxtype, values)
	local panel = vgui.Create("DPanel", sheet)
	panel:SetPaintBackground( false )

	local fxpanel = vgui.Create("DScrollPanel", panel)
	fxpanel:SetPaintBackground( true )

	local panels = BuildValuePanels(fxpanel, values)

	panel.PerformLayout = function(pnl)
		local W, H = panel:GetSize()
		fxpanel:SetSize(W - 40, H - 80)
		fxpanel:SetPos(20, 60)

		local fx_x, fx_y = 15, 15
		local fx_w, fx_h = W - 70, H - 110

		local lastypos = fx_y
		for i, pnl in ipairs(panels) do
			if not IsValid(pnl) then continue end

			pnl:SetPos(fx_x, lastypos)
			local h = pnl:GetTall()

			pnl:SetWidth(fx_w)
			lastypos = lastypos + h + 10
		end
	end

	AddFxCheckbox(panel, fxpanel, function(ch, panel)
		local fx = ch:AddFx(fxtype)
		print(fx)
		print("fx type", type(fx), TypeID(fx))

		if not fx then return nil end
		local err = fx:GetError()

		print("fx:GetError()", err, "=>", BASS3.DecodeError(err))
		print("fx:GetTypeEnum()", fx:GetTypeEnum())

		if not IsValid(fx) then return nil end

		for i, pnl in ipairs(panels) do
			if not IsValid(pnl) then continue end

			local valuedata = pnl.mt_valuedata
			if not valuedata then
				pnl:Remove()
				continue
			end

			local name = valuedata.name

			if not name then
				pnl:Remove()
				continue
			end

			local fxGetter = fx["Get" .. name]
			if not fxGetter then
				pnl:Remove()
				continue
			end

			local fxSetter = fx["Set" .. name]
			if not fxSetter then
				pnl:Remove()
				continue
			end

			pnl:mt_SetValue(fxGetter(fx))
			pnl.mt_OnChange = function(this, value)
				if not IsValid(fx) then return end
				fxSetter(fx, value)
			end
		end

		return fx
	end, function(fx, ch, panel)
		if not IsValid(fx) then return end
		fx:Remove()
	end)

	return panel
end


local function BuildEqualiserPanel(fxpanel, hz)
	local panel = vgui.Create("DPanel", fxpanel)
	panel:SetPaintBackground( false )
	panel:SetSize(10, 90)

	local hzstr = ""

	if hz < 1000 then
		hzstr = hz .. " Hz"
	else
		hzstr = math.Round(hz / 1000, 2) .. " kHz"
	end

	local label = vgui.Create("DLabel", panel)
	label:SetText(hzstr)
	label:SetDark(true)
	label:Dock(TOP)

	local pnl_gain = vgui.Create("DNumSlider", panel)

	pnl_gain:SetText("Gain (dB)")
	pnl_gain:SetMin(-15)
	pnl_gain:SetMax(15)
	pnl_gain:SetDecimals(3)
	pnl_gain:SetDark(true)
	pnl_gain:SetSize(10, 30)
	pnl_gain:Dock(TOP)

	pnl_gain.mt_SetValue = function(this, ...)
		if not IsValid(this) then return end
		return this:SetValue(...)
	end

	pnl_gain.mt_GetValue = function(this, ...)
		if not IsValid(this) then return end
		return this:GetValue(...)
	end

	pnl_gain.OnValueChanged = function(this, value)
		if not IsValid(this) then return end
		if not this.mt_OnChange then return end
		this:mt_OnChange(value)
	end

	local pnl_bandwidth = vgui.Create("DNumSlider", panel)

	pnl_bandwidth:SetText("Bandwidth (semitones)")
	pnl_bandwidth:SetMin(1)
	pnl_bandwidth:SetMax(36)
	pnl_bandwidth:SetDecimals(3)
	pnl_bandwidth:SetDark(true)
	pnl_bandwidth:SetSize(10, 30)
	pnl_bandwidth:Dock(TOP)

	pnl_bandwidth.mt_SetValue = function(this, ...)
		if not IsValid(this) then return end
		return this:SetValue(...)
	end

	pnl_bandwidth.mt_GetValue = function(this, ...)
		if not IsValid(this) then return end
		return this:GetValue(...)
	end

	pnl_bandwidth.OnValueChanged = function(this, value)
		if not IsValid(this) then return end
		if not this.mt_OnChange then return end
		this:mt_OnChange(value)
	end

	panel.mt_pnl_gain = pnl_gain
	panel.mt_pnl_bandwidth = pnl_bandwidth
	panel.mt_hz = hz

	return panel
end

local function BuildEqualiserPanels(fxpanel)
	local hztab = {
		80,
		100,
		125,
		160,
		200,
		250,
		320,
		400,
		500,
		640,
		800,
		1000,
		1280,
		1600,
		2000,
		2560,
		3200,
		4000,
		5120,
		6400,
		8000,
		10240,
		12800,
		16000,
	}

	local panels = {}

	for i, v in ipairs(hztab) do
		panels[i] = BuildEqualiserPanel(fxpanel, v)
	end

	return panels
end

local function BuildParamEQFxPanel(mainpanel, sheet)
	local panel = vgui.Create("DPanel", sheet)
	panel:SetPaintBackground( false )

	local fxpanel = vgui.Create("DScrollPanel", panel)
	fxpanel:SetPaintBackground( true )

	local panels = BuildEqualiserPanels(fxpanel)

	panel.PerformLayout = function(pnl)
		local W, H = panel:GetSize()
		fxpanel:SetSize(W - 40, H - 80)
		fxpanel:SetPos(20, 60)

		local fx_x, fx_y = 15, 15
		local fx_w, fx_h = W - 70, H - 110

		local lastypos = fx_y
		for i, pnl in ipairs(panels) do
			if not IsValid(pnl) then continue end

			pnl:SetPos(fx_x, lastypos)
			local h = pnl:GetTall()

			pnl:SetWidth(fx_w)
			lastypos = lastypos + h + 10
		end
	end

	AddFxCheckbox(panel, fxpanel, function(ch, panel)
		local fxtab = {}

		for i, pnl in ipairs(panels) do
			if not IsValid(pnl) then continue end

			local pnl_gain = pnl.mt_pnl_gain
			if not IsValid(pnl_gain) then
				pnl:Remove()
				continue
			end

			local pnl_bandwidth = pnl.mt_pnl_bandwidth
			if not IsValid(pnl_bandwidth) then
				pnl:Remove()
				continue
			end

			local hz = pnl.mt_hz
			local fx = ch:AddFx(BASS3.ENUM.FX_PARAMEQ)

			print(i, hz, fx)
			print("fx type", type(fx), TypeID(fx))

			if not fx then
				pnl:Remove()
				continue
			end

			local err = fx:GetError()

			fxtab[#fxtab + 1] = fx

			print("fx:GetError()", err, "=>", BASS3.DecodeError(err))
			print("fx:GetTypeEnum()", fx:GetTypeEnum())

			if not IsValid(fx) then
				pnl:Remove()
				continue
			end

			if not fx.GetGain then
				pnl:Remove()
				continue
			end

			if not fx.SetGain then
				pnl:Remove()
				continue
			end

			if not fx.GetBandwidth then
				pnl:Remove()
				continue
			end

			if not fx.SetBandwidth then
				pnl:Remove()
				continue
			end

			if not fx.GetCenter then
				pnl:Remove()
				continue
			end

			if not fx.SetCenter then
				pnl:Remove()
				continue
			end

			fx:SetCenter(hz)

			pnl_gain:mt_SetValue(fx:GetGain())
			pnl_gain.mt_OnChange = function(this, value)
				if not IsValid(fx) then return end
				fx:SetGain(value)
			end

			pnl_bandwidth:mt_SetValue(fx:GetBandwidth())
			pnl_bandwidth.mt_OnChange = function(this, value)
				if not IsValid(fx) then return end
				fx:SetBandwidth(value)
			end
		end

		return fxtab
	end, function(fxtab, ch, panel)
		if not fxtab then return end

		for i, fx in ipairs(fxtab) do
			if IsValid(fx) then
				fx:Remove()
			end

			fxtab[i] = nil
		end
	end)

	return panel
end

local function BuildChorusPanel(mainpanel, sheet)
	local values = {
		{
			paneltype = "float",
			name = "WetDryMix",
			min = 0,
			max = 100,
			unit = "%",
		},
		{
			paneltype = "float",
			name = "Depth",
			min = 0,
			max = 100,
			unit = "%",
		},
		{
			paneltype = "float",
			name = "Feedback",
			min = -99,
			max = 99,
		},
		{
			paneltype = "float",
			name = "Frequency",
			min = 0,
			max = 10,
			unit = "Hz",
		},
		{
			paneltype = "flag",
			name = "Waveform",
			options = {
				["BASS3.ENUM.FXPARAMETER_WAVEFORM_TRIANGLE"] = BASS3.ENUM.FXPARAMETER_WAVEFORM_TRIANGLE,
				["BASS3.ENUM.FXPARAMETER_WAVEFORM_SINE"] = BASS3.ENUM.FXPARAMETER_WAVEFORM_SINE,
			},
		},
		{
			paneltype = "float",
			name = "Delay",
			min = 0,
			max = 20,
			unit = "ms",
		},
		{
			paneltype = "flag",
			name = "Phase",
			options = {
				["BASS3.ENUM.FXPARAMETER_PHASE_N180 (-180°)"] = BASS3.ENUM.FXPARAMETER_PHASE_N180,
				["BASS3.ENUM.FXPARAMETER_PHASE_N90 (-90°)"] = BASS3.ENUM.FXPARAMETER_PHASE_N90,
				["BASS3.ENUM.FXPARAMETER_PHASE_ZERO (0°)"] = BASS3.ENUM.FXPARAMETER_PHASE_ZERO,
				["BASS3.ENUM.FXPARAMETER_PHASE_P90 (+90°)"] = BASS3.ENUM.FXPARAMETER_PHASE_P90,
				["BASS3.ENUM.FXPARAMETER_PHASE_P180 (+180°)"] = BASS3.ENUM.FXPARAMETER_PHASE_P180,
			},
		},
	}

	local panel = BuildGenericFxPanel(mainpanel, sheet, BASS3.ENUM.FX_CHORUS, values)
	return panel
end

local function BuildCompressorPanel(mainpanel, sheet)
	local values = {
		{
			paneltype = "float",
			name = "Gain",
			min = -60,
			max = 60,
			unit = "dB",
		},
		{
			paneltype = "float",
			name = "Attack",
			min = 0.01,
			max = 500,
			unit = "ms",
		},
		{
			paneltype = "float",
			name = "Release",
			min = 50,
			max = 3000,
			unit = "ms",
		},
		{
			paneltype = "float",
			name = "Threshold",
			min = -60,
			max = 0,
			unit = "dB",
		},
		{
			paneltype = "float",
			name = "Ratio",
			min = 1,
			max = 10,
		},
		{
			paneltype = "float",
			name = "Predelay",
			min = 0,
			max = 4,
			unit = "ms",
		},
	}

	local panel = BuildGenericFxPanel(mainpanel, sheet, BASS3.ENUM.FX_COMPRESSOR, values)
	return panel
end

local function BuildDistortionPanel(mainpanel, sheet)
	local values = {
		{
			paneltype = "float",
			name = "Gain",
			min = -60,
			max = 0,
			unit = "dB",
		},
		{
			paneltype = "float",
			name = "Edge",
			min = 0,
			max = 100,
			unit = "%",
		},
		{
			paneltype = "float",
			name = "PostEQCenterFrequency",
			min = 100,
			max = 8000,
			unit = "Hz",
		},
		{
			paneltype = "float",
			name = "PostEQBandwidth",
			min = 100,
			max = 8000,
			unit = "Hz",
		},
		{
			paneltype = "float",
			name = "PreLowpassCutoff",
			min = 100,
			max = 8000,
			unit = "Hz",
		},
	}

	local panel = BuildGenericFxPanel(mainpanel, sheet, BASS3.ENUM.FX_DISTORTION, values)
	return panel
end

local function BuildEchoPanel(mainpanel, sheet)
	local values = {
		{
			paneltype = "float",
			name = "WetDryMix",
			min = 0,
			max = 100,
			unit = "%",
		},
		{
			paneltype = "float",
			name = "Feedback",
			min = 0,
			max = 100,
			unit = "%",
		},
		{
			paneltype = "float",
			name = "LeftDelay",
			min = 1,
			max = 2000,
			unit = "ms",
		},
		{
			paneltype = "float",
			name = "RightDelay",
			min = 1,
			max = 2000,
			unit = "ms",
		},
		{
			paneltype = "bool",
			name = "PanDelay",
		},
	}

	local panel = BuildGenericFxPanel(mainpanel, sheet, BASS3.ENUM.FX_ECHO, values)
	return panel
end


local function BuildFlangerPanel(mainpanel, sheet)
	local values = {
		{
			paneltype = "float",
			name = "WetDryMix",
			min = 0,
			max = 100,
			unit = "%",
		},
		{
			paneltype = "float",
			name = "Depth",
			min = 0,
			max = 100,
			unit = "%",
		},
		{
			paneltype = "float",
			name = "Feedback",
			min = -99,
			max = 99,
		},
		{
			paneltype = "float",
			name = "Frequency",
			min = 0,
			max = 10,
			unit = "Hz",
		},
		{
			paneltype = "flag",
			name = "Waveform",
			options = {
				["BASS3.ENUM.FXPARAMETER_WAVEFORM_TRIANGLE"] = BASS3.ENUM.FXPARAMETER_WAVEFORM_TRIANGLE,
				["BASS3.ENUM.FXPARAMETER_WAVEFORM_SINE"] = BASS3.ENUM.FXPARAMETER_WAVEFORM_SINE,
			},
		},
		{
			paneltype = "float",
			name = "Delay",
			min = 0,
			max = 4,
			unit = "ms",
		},
		{
			paneltype = "flag",
			name = "Phase",
			options = {
				["BASS3.ENUM.FXPARAMETER_PHASE_N180 (-180°)"] = BASS3.ENUM.FXPARAMETER_PHASE_N180,
				["BASS3.ENUM.FXPARAMETER_PHASE_N90 (-90°)"] = BASS3.ENUM.FXPARAMETER_PHASE_N90,
				["BASS3.ENUM.FXPARAMETER_PHASE_ZERO (0°)"] = BASS3.ENUM.FXPARAMETER_PHASE_ZERO,
				["BASS3.ENUM.FXPARAMETER_PHASE_P90 (+90°)"] = BASS3.ENUM.FXPARAMETER_PHASE_P90,
				["BASS3.ENUM.FXPARAMETER_PHASE_P180 (+180°)"] = BASS3.ENUM.FXPARAMETER_PHASE_P180,
			},
		},
	}

	local panel = BuildGenericFxPanel(mainpanel, sheet, BASS3.ENUM.FX_FLANGER, values)
	return panel
end

local function BuildGarglePanel(mainpanel, sheet)
	local values = {
		{
			paneltype = "float",
			name = "Rate",
			min = 1,
			max = 1000,
			decimals = 0,
			unit = "Hz",
		},
		{
			paneltype = "flag",
			name = "Waveform",
			options = {
				["BASS3.ENUM.FXPARAMETER_WAVEFORM_TRIANGLE"] = BASS3.ENUM.FXPARAMETER_WAVEFORM_TRIANGLE,
				["BASS3.ENUM.FXPARAMETER_WAVEFORM_SQUARE"] = BASS3.ENUM.FXPARAMETER_WAVEFORM_SQUARE,
			},
		},
	}

	local panel = BuildGenericFxPanel(mainpanel, sheet, BASS3.ENUM.FX_GARGLE, values)
	return panel
end

local function BuildI3DL2ReverbPanel(mainpanel, sheet)
	local values = {
		{
			paneltype = "float",
			name = "Room",
			min = -10000,
			max = 0,
			decimals = 0,
			unit = "mB",
		},
		{
			paneltype = "float",
			name = "RoomHF",
			min = -10000,
			max = 0,
			decimals = 0,
			unit = "mB",
		},
		{
			paneltype = "float",
			name = "RoomRolloffFactor",
			min = 0,
			max = 10,
		},
		{
			paneltype = "float",
			name = "DecayTime",
			min = 0.1,
			max = 20,
			unit = "s",
		},


		{
			paneltype = "float",
			name = "DecayHFRatio",
			min = 0.1,
			max = 2,
		},
		{
			paneltype = "float",
			name = "Reflections",
			min = -10000,
			max = 1000,
			decimals = 0,
			unit = "mB",
		},
		{
			paneltype = "float",
			name = "ReflectionsDelay",
			min = 0,
			max = 0.3,
			decimals = 4,
			unit = "s",
		},
		{
			paneltype = "float",
			name = "Reverb",
			min = -10000,
			max = 2000,
			decimals = 0,
			unit = "mB",
		},


		{
			paneltype = "float",
			name = "ReverbDelay",
			min = 0,
			max = 0.1,
			decimals = 4,
			unit = "s",
		},
		{
			paneltype = "float",
			name = "Diffusion",
			min = 0,
			max = 100,
			unit = "%",
		},
		{
			paneltype = "float",
			name = "Density",
			min = 0,
			max = 100,
			unit = "%",
		},
		{
			paneltype = "float",
			name = "HFReference",
			min = 20,
			max = 20000,
			unit = "Hz",
		},
	}

	local panel = BuildGenericFxPanel(mainpanel, sheet, BASS3.ENUM.FX_I3DL2REVERB, values)
	return panel
end

local function BuildReverbPanel(mainpanel, sheet)
	local values = {
		{
			paneltype = "float",
			name = "InGain",
			min = -96,
			max = 0,
			unit = "dB",
		},
		{
			paneltype = "float",
			name = "ReverbMix",
			min = -96,
			max = 0,
			unit = "dB",
		},
		{
			paneltype = "float",
			name = "ReverbTime",
			min = 0.0010,
			max = 3000,
			decimals = 4,
			unit = "ms",
		},
		{
			paneltype = "float",
			name = "HighFreqRTRatio",
			min = 0.0010,
			max = 0.9990,
			decimals = 4,
		},
	}

	local panel = BuildGenericFxPanel(mainpanel, sheet, BASS3.ENUM.FX_REVERB, values)
	return panel
end

local function BuildMainPanel()
	local g_mainpanel = Bass3Test.g_mainpanel

	if IsValid(g_mainpanel) then
		g_mainpanel:SetVisible(true)
		g_mainpanel:MakePopup()
		g_mainpanel:InvalidateLayout(true)
		return
	end

	Bass3Test.g_mainpanel = vgui.Create( "DFrame" )
	local g_mainpanel = Bass3Test.g_mainpanel

	g_mainpanel:SetPos(25, 25)

	local W = math.Clamp(ScrW() - 50, 750, 1200)
	local H = math.Clamp(ScrH() - 50, 400, 800)
	g_mainpanel:SetSize(W, H)

	g_mainpanel:SetMinWidth(750)
	g_mainpanel:SetMinHeight(400)
	g_mainpanel:SetSizable(true)
	g_mainpanel:SetDeleteOnClose(false)
	g_mainpanel:SetTitle("BASS3 FX Test")
	g_mainpanel:SetVisible(false)
	g_mainpanel:GetParent():SetWorldClicker(true)

	local sheet = vgui.Create("DPropertySheet", g_mainpanel)
	sheet:Dock(FILL)
	sheet:AddSheet("Channel", BuildChannelPanel(g_mainpanel, sheet), "icon16/sound.png")

	sheet:AddSheet("Chorus", BuildChorusPanel(g_mainpanel, sheet), "icon16/pill.png")
	sheet:AddSheet("Compressor", BuildCompressorPanel(g_mainpanel, sheet), "icon16/compress.png")
	sheet:AddSheet("Distortion", BuildDistortionPanel(g_mainpanel, sheet), "icon16/telephone.png")
	sheet:AddSheet("Echo", BuildEchoPanel(g_mainpanel, sheet), "icon16/arrow_redo.png")
	sheet:AddSheet("Flanger", BuildFlangerPanel(g_mainpanel, sheet), "icon16/arrow_rotate_anticlockwise.png")
	sheet:AddSheet("Gargle", BuildGarglePanel(g_mainpanel, sheet), "icon16/shading.png")
	sheet:AddSheet("I3DL2Reverb", BuildI3DL2ReverbPanel(g_mainpanel, sheet), "icon16/asterisk_orange.png")
	sheet:AddSheet("Reverb", BuildReverbPanel(g_mainpanel, sheet), "icon16/asterisk_yellow.png")
	sheet:AddSheet("ParamEQ", BuildParamEQFxPanel(g_mainpanel, sheet), "icon16/control_equalizer.png")

	if IsValid(g_mainpanel) then
		BuildMainPanel()
	end
end

TEST._ = BuildMainPanel
