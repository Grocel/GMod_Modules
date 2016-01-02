# GM_BASS3
Binary modules for creating GLua bindings to the [BASS sound API](http://www.un4seen.com/).

## About
This module will make you able to stream various formats of sound and play them as 3D world sound.
Getting the FFT spectrum and tag information of a stream is also possible. On the server the sound is always muted.
So far it is available for Windows and Linux.

## Installation
You put the DLL files for your OS in to the garrysmod/lua/bin/ folder.

## Compiling
At the first I run the premake script so the project files are generated.
For windows I compile the code with Visual Studio 2013. For Linux I use 'gcc/g++'' and 'make' on a Linux Mint VirtualBox.
If you want to compile it yourself you could get some trouble on Linux, because I had to install like a ton of libraries and stuff I don't even remember to make the compiler work properly.
You need the Source SDK 2013 to compile too.

## Requirements/Problems
* Make sure your game and your server is up to date.
* You may need to manually replace the bass.dll or libbass.so on the server with the latest versions of them. It is located at <gmodserver>/bin/.
* As it uses c++11 features you may need to install a c++11 runtime library on your operation system.
* For Linux you may need to install ASIO and give the user that is running the server or the client "rwx" access to the sound device/interface.
* For servers (especially Linux ones) that do not have sound hardware, you need to install a dummy driver.
* It could crash on Windows Server while loading, if a sound (dummy) driver or DirectX is missing.
* For dedicated servers you need to install the [Filesystem Exporter Plugin](https://github.com/Grocel/GMod_Modules/tree/master/filesys_export) to access local and mounted sound files.


## Lua references

#### Enumerations
This is a list of all enumerations. They are designed to match these of GMod to make migration easier.

However you should prefer to only use ``BASS3.ENUM.*`` for BASS3 related functions as the GMod ones could be changed anytime.
Also never pass the numbers directly, as they could be changed too.

##### CHANNEL_\*
| Name                       | Value | Description |
| -------------------------- | ----- | ----------- |
| BASS3.ENUM.CHANNEL_STOPPED | 0     | The channel is stopped. |
| BASS3.ENUM.CHANNEL_PLAYING | 1     | The channel is playing. |
| BASS3.ENUM.CHANNEL_PAUSED  | 2     | The channel is paused. |
| BASS3.ENUM.CHANNEL_STALLED | 3     | The channel is buffering. |


##### ERROR_\*
| Name                        | Value | Description |
| --------------------------- | ----- | ----------- |
| BASS3.ENUM.ERROR_UNKNOWN    | -1    | Unknown error! |
| BASS3.ENUM.ERROR_OK         | 0     | OK |
| BASS3.ENUM.ERROR_MEM        | 1     | Memory error! |
| BASS3.ENUM.ERROR_FILEOPEN   | 2     | Can't open the file! |
| BASS3.ENUM.ERROR_DRIVER     | 3     | Can't find a driver! |
| BASS3.ENUM.ERROR_BUFLOST    | 4     | The sample buffer was lost! |
| BASS3.ENUM.ERROR_HANDLE     | 5     | Invalid channel! |
| BASS3.ENUM.ERROR_FORMAT     | 6     | Unsupported sample format! |
| BASS3.ENUM.ERROR_POSITION   | 7     | Invalid position! |
| BASS3.ENUM.ERROR_INIT       | 8     | BASS_Init has not been successfully called! |
| BASS3.ENUM.ERROR_START      | 9     | BASS_Start has not been successfully called! |
| BASS3.ENUM.ERROR_ALREADY    | 14    | Already initialized, paused or used! |
| BASS3.ENUM.ERROR_NOCHAN     | 18    | Can't get a free channel! |
| BASS3.ENUM.ERROR_ILLTYPE    | 19    | An illegal type was specified! |
| BASS3.ENUM.ERROR_ILLPARAM   | 20    | An illegal parameter was specified! (URL wrong?) |
| BASS3.ENUM.ERROR_NO3D       | 21    | No 3D support! |
| BASS3.ENUM.ERROR_NOEAX      | 22    | No EAX support! |
| BASS3.ENUM.ERROR_DEVICE     | 23    | Illegal device! (No sound card, no access?) |
| BASS3.ENUM.ERROR_NOPLAY     | 24    | Not playing! |
| BASS3.ENUM.ERROR_FREQ       | 25    | Illegal sample rate! |
| BASS3.ENUM.ERROR_NOTFILE    | 27    | The stream is not a file stream! |
| BASS3.ENUM.ERROR_NOHW       | 29    | No hardware voices available! |
| BASS3.ENUM.ERROR_EMPTY      | 31    | The MOD music has no sequence data! |
| BASS3.ENUM.ERROR_NONET      | 32    | No internet connection could be opened! |
| BASS3.ENUM.ERROR_CREATE     | 33    | Couldn't create the file! |
| BASS3.ENUM.ERROR_NOFX       | 34    | Effects are not available! |
| BASS3.ENUM.ERROR_NOTAVAIL   | 37    | Requested data is not available! |
| BASS3.ENUM.ERROR_DECODE     | 38    | The channel is a decoding channel! |
| BASS3.ENUM.ERROR_DX         | 39    | A sufficient DirectX version is not installed! |
| BASS3.ENUM.ERROR_TIMEOUT    | 40    | Connection timedout! |
| BASS3.ENUM.ERROR_FILEFORM   | 41    | Unsupported file format! |
| BASS3.ENUM.ERROR_SPEAKER    | 42    | Speaker not found! |
| BASS3.ENUM.ERROR_VERSION    | 43    | Invalid BASS version! |
| BASS3.ENUM.ERROR_CODEC      | 44    | Codec not supported! |
| BASS3.ENUM.ERROR_ENDED      | 45    | The channel has ended! |
| BASS3.ENUM.ERROR_BUSY       | 46    | The device is busy! |
| BASS3.ENUM.ERROR_FILESYSTEM | 102   | Missing Filesystem! |


##### FFT_\*
| Name                 | Value | Description |
| -------------------- | ----- | ----------- |
| BASS3.ENUM.FFT_16    | 10    | 8 levels for normal, 32 levels for complex. |
| BASS3.ENUM.FFT_32    | 11    | 16 levels for normal, 64 levels for complex. |
| BASS3.ENUM.FFT_64    | 12    | 32 levels for normal, 128 levels for complex. |
| BASS3.ENUM.FFT_128   | 13    | 64 levels for normal, 256 levels for complex. |
| BASS3.ENUM.FFT_256   | 0     | 128 levels for normal, 512 levels for complex. |
| BASS3.ENUM.FFT_512   | 1     | 256 levels for normal, 1024 levels for complex. |
| BASS3.ENUM.FFT_1024  | 2     | 512 levels for normal, 2048 levels for complex. |
| BASS3.ENUM.FFT_2048  | 3     | 1024 levels for normal, 4096 levels for complex. |
| BASS3.ENUM.FFT_4096  | 4     | 2048 levels for normal, 8192 levels for complex. |
| BASS3.ENUM.FFT_8192  | 5     | 4096 levels for normal, 16384 levels for complex. |
| BASS3.ENUM.FFT_16384 | 6     | 8192 levels for normal, 32768 levels for complex. |


##### MODE_\*
| Name                    | Value | Description |
| ----------------------- | ----- | ----------- |
| BASS3.ENUM.MODE_NONE    | 0     | No modes, same as "". |
| BASS3.ENUM.MODE_3D      | 1     | Play as 3D world sound, same as "3d". |
| BASS3.ENUM.MODE_LOOP    | 2     | Loop the sound, same as "loop". |
| BASS3.ENUM.MODE_MONO    | 4     | Play in mono, same as "mono". |
| BASS3.ENUM.MODE_NOPLAY  | 8     | Don't play on load, same as "noplay". |
| BASS3.ENUM.MODE_NOBLOCK | 16    | Disable streaming in blocks, same as "noblock". |


##### TAG_\*
| Name                | Value | Description |
| ------------------- | ----- | ----------- |
| BASS3.ENUM.TAG_META | 0     | Receives the "META" tag information. |
| BASS3.ENUM.TAG_HTTP | 1     | Receives the "HTTP" tag information. |
| BASS3.ENUM.TAG_ICY  | 2     | Receives the "ICY" tag information. |
| BASS3.ENUM.TAG_ID3  | 3     | Receives the "ID3" tag information. |
| BASS3.ENUM.TAG_MF   | 4     | Receives the "MF" tag information. |
| BASS3.ENUM.TAG_MP4  | 5     | Receives the "MP4" tag information. |
| BASS3.ENUM.TAG_APE  | 6     | Receives the "APE" tag information. |
| BASS3.ENUM.TAG_OGG  | 7     | Receives the "OGG" tag information. |


#### BASS3

##### Functions
| Name                  | Arguments                                                                                      | Returned values                                                         | Description |
| --------------------- | ---------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------| ----------- |
| BASS3.CreateChannel() | ``nil``                                                                                        | ``IBASS3Channel Channel``                                               | Creates an empty IBASS3Channel object. |
| BASS3.DecodeError()   | ``number Errorcode``                                                                           | ``string Errorstring``                                                  | Returns the description of the given errorcode. |
| BASS3.PlayFile()      | ``string Path``<br><br>``{string or MODE_*-Enum} Flags``<br><br>``function Callback``          | ``bool Success``                                                        | Play a local file at the given path relative to the garrysmod content folder.<br><br>The Flags argument accepts a string as like as sound.PlayFile() does and the *recommend* MODE_\*-Enums.<br><br>The Callback is called after the loading is done.<br>Its aguments are:<br>``IBASS3Channel Channel``<br>``number Errorcode``<br>``string Errorstring``<br><br>Returns true when the loading process was successfully started. |
| BASS3.PlayURL()       | ``string URL``<br><br>``{string or MODE_*-Enum} Flags``<br><br>``function Callback``           | ``bool Success``                                                        | Play an online file or stream at the given URL.<br><br>Basically it works as same as BASS3.PlayFile(). |
| BASS3.GetEAX()        | ``nil``                                                                                        | ``number ENV``<br><br>``number Volume``<br><br>``number Decay``<br><br>``number Damp`` | **Clientside, Broken/WIP**<br><br>Returns the values of the current EAX effect. |
| BASS3.SetEAX()        | ``[number ENV]``<br><br>``[number Volume]``<br><br>``[number Decay]``<br><br>``[number Damp]`` | ``bool Success``                                                        | **Clientside, Broken/WIP**<br><br>Applies an EAX effect to all BASS sounds.<br>Unfilled arguments will not be changed.<br><br>Only has an effect on 3D channels.<br>Returns true on success.<br><br>Also see:<br>[BASS_SetEAXParameters()](http://www.un4seen.com/doc/#bass/BASS_SetEAXParameters.html)<br>[BASS_ATTRIB_EAXMIX](http://www.un4seen.com/doc/#bass/BASS_ATTRIB_EAXMIX.html) |


##### Variables
| Name                | Value   | Description |
| ------------------- | ------- | ----------- |
| BASS3.ModuleVersion | 6       | Version of gm_bass3. |
| BASS3.Version       | 2041100 | Version of the BASS sound engine. |


#### IBASS3Channel object
This is the channel object. With this object you can change and receive information of a sound.
All Functions are designed to match their counter parts of [IGModAudioChannel](http://wiki.garrysmod.com/page/Category:IGModAudioChannel) in their behavior.
The channel can be recycled improve performance. Multiple references can lead to one channel object.

However when the last reference of a channel object gets out of scope the channel will be stopped and removed. So be careful.
Since all channels are muted on the server, all 3D sound, volume and balance related functions are clientside only.

| Name                              | Additional arguments                     | Returned values          | Description |
| --------------------------------- | ---------------------------------------- | ------------------------ | ----------- |
| IBASS3Channel:BalanceFadeTo()     | ``float Balance``<br><br>``float time``  | ``nil``                  | **Clientside**<br><br>Fade the sound balance to the given value for the given time in secounds. |
| IBASS3Channel:BalanceIsFading()   | ``nil``                                  | ``bool Fading``          | **Clientside**<br><br>Returns true if the sound balance is fading. |
| IBASS3Channel:EnableLooping()     | ``nil``                                  | ``bool Loop``            | Enable or disable Looping. |
| IBASS3Channel:FFT()               | ``table FFT``<br><br>``FFT_*-Enum Size``<br><br>``bool RemoveDCBias = false``<br><br>``bool RemoveHannWindow = false``<br><br>``bool AsDecibel = false`` | ``number Count`` | It fills the given table with the first half of the current FFT spectrum.<br><br>Returned values: 0 ... 1<br>OR: -1000 db ... 0 db<br><br>The FFT_*-Enum defines the size of the data. The data size is returned.<br><br>The values are returned as decibel base 10 figures if AsDecibel is true.<br><br>Also see:<br>[IGModAudioChannel:FFT](http://wiki.garrysmod.com/page/IGModAudioChannel/FFT)<br>[BASS_ChannelGetData()](http://www.un4seen.com/doc/#bass/BASS_ChannelGetData.html) |
| IBASS3Channel:FFTComplex()        | ``table FFT``<br><br>``FFT_*-Enum Size``<br><br>``bool RemoveDCBias = false``<br><br>``bool RemoveHannWindow = false`` | ``number Count`` | Similar to BASS3Channel:FFT(), but with different more complete data.<br><br>The given table is filled with complex numbers.<br><br>The real parts are in uneven indexes, while the imaginary parts are in even indexes.<br>So the first index is the real part of the first complex number.<br><br>Returned values: -1 ... 1 |
| IBASS3Channel:Get3DCone()         | ``nil`` | ``float InnerAngle``<br><br>``float OuterAngle``<br><br>``float OuterVolume`` | **Clientside**<br><br>Returns 3D cone of the sound channel. |
| IBASS3Channel:Get3DFadeDistance() | ``nil`` | ``float Min``<br><br>``float Max``                        | **Clientside**<br><br>Returns 3D fade distances of a sound channel. |
| IBASS3Channel:GetBalance()        | ``nil``                                  | ``float Balance``        | Returns the current sound balance. |
| IBASS3Channel:GetBitsPerSample()  | ``nil``                                  | ``number Bits``          | Returns the bit count per sample. |
| IBASS3Channel:GetEAXMix()         | ``nil``                                  | ``float DryWetRatio``    | **Clientside, Broken/WIP**<br><br>Returns the Dry/Wet ratio in the EAX mix. |
| IBASS3Channel:GetFileFormat()     | ``nil``                                  | ``string Fileformat``    | Returns the format of the stream. Ex: "MP3", "WAV" |
| IBASS3Channel:GetFileName()       | ``nil``                                  | ``string Filename``      | Returns the URL or Path of the stream. |
| IBASS3Channel:GetLength()         | ``nil``                                  | ``float Length``         | Returns the length of the streamed file in seconds.<br><br>Results less then zero are indicating an endless stream. |
| IBASS3Channel:GetLevel()          | ``float TimeFrame = 0.02``<br><br>``bool RMS = false``<br><br>``bool AsDecibel = false`` | ``float Left``<br><br>``float Right`` | Returns the sound levels.<br><br>Returned values: 0 ... 1<br>OR: -1000 db ... 0 db<br><br>The time frame value definites how much data to inspect to calculate the level.<br>Max: 1, Min: 0.001<br><br>If RMS is true, it will return the RMS instead of the peak levels.<br><br>The values are returned as decibel base 10 figures if AsDecibel is true. |
| IBASS3Channel:GetPos()            | ``[Vector Position]``<br><br>``[Vector Direction]``<br><br>``[Vector Velocity]`` | ``Vector Position``<br><br>``Vector Direction``<br><br>``Vector Velocity`` | **Clientside**<br><br>Returns position, direction and velocity of the channel.<br><br>If you put vectors into the optional arguments, the function will change and return the given ones instead of creating new ones. Useful for recycling. |
| IBASS3Channel:GetSamplingRate()   | ``nil``                                  | ``number Frequency``     | Returns the sampling rate.<br>Usually it is 44100 Hz. |
| IBASS3Channel:GetState()          | ``nil``                                  | ``CHANNEL_*-Enum State`` | Returns the channel state.<br><br>See the CHANNEL_* table. |
| IBASS3Channel:GetTag()            | ``TAG_*-Enum Type``<br><br>``[table Tags]`` | ``table Tags`` | Returns the channel tags to a table.<br><br>The table is indexed by string or numbers depending on the given type.<br><br>If you pass the optional table, it will change and return this instead of creating a one. Useful for recycling. |
| IBASS3Channel:GetTime()           | ``nil``                                  | ``float TimePos``        | Returns the current time position in seconds. |
| IBASS3Channel:GetVolume()         | ``nil``                                  | ``float Volume``         | Returns the current volume. |
| IBASS3Channel:Is3D()              | ``nil``                                  | ``bool 3D``              | **Clientside**<br><br>Returns if the channel is in 3D mode. |
| IBASS3Channel:IsBlockStreamed()   | ``nil``                                  | ``bool BlockStreamed``   | Returns if the channel is streamed in blocks. |
| IBASS3Channel:IsEndless()         | ``nil``                                  | ``bool Endless``         | Returns if the channel is endless.<br>Usually online radios are endless. |
| IBASS3Channel:IsLooping()         | ``nil``                                  | ``bool Looping``         | Returns if the channel is looping. |
| IBASS3Channel:IsOnline()          | ``nil``                                  | ``bool Online``          | Returns if the channel streaming from an online source. |
| IBASS3Channel:IsSeeking()         | ``nil``                                  | ``bool Seeking``         | Returns if the channel is seeking to new time position. |
| IBASS3Channel:IsValid()           | ``nil``                                  | ``bool Valid``           | Returns if the channel is valid. |
| IBASS3Channel:Pause()             | ``nil``                                  | ``nil``                  | Pauses the channel. |
| IBASS3Channel:Play()              | ``bool Restart = false``                 | ``nil``                  | Plays the channel.<br>Restarts it if the given bool is true. |
| IBASS3Channel:Remove()            | ``nil``                                  | ``nil``                  | Stops the channel and frees it. |
| IBASS3Channel:Set3DCone()         | ``float InnerAngle = -1``<br><br>``float OuterAngle = -1``<br><br>``float OuterVolume = -1`` | ``nil`` | **Clientside**<br><br>Sets 3D cone of the sound channel.<br><br>A value set to nil or -1 will remain unchanged. |
| IBASS3Channel:Set3DFadeDistance() | ``float Min = -1``<br><br>``float Max = -1``       | ``nil``        | **Clientside**<br><br>Sets 3D fade distances of a sound channel.<br><br>A value set to nil or -1 will remain unchanged. |
| IBASS3Channel:SetBalance()        | ``float Balance``                        | ``nil``                  | **Clientside**<br><br>Set the sound balance to the given value.<br><br>Balance values:<br>-1 = Left<br>0 = Center<br>1 = Right |
| IBASS3Channel:SetEAXMix()         | ``[float DryWetRatio]``                  | ``nil``                  | **Clientside, Broken/WIP**<br><br>Sets the Dry/Wet ratio in the EAX mix. A nil or -1 resets it to automatic mixing. |
| IBASS3Channel:SetPos()            | ``Vector Position = Vector(0,0,0)``<br><br>``Vector Direction = Vector(0,0,0)``<br><br>``Vector Velocity = Vector(0,0,0)`` | ``nil`` | **Clientside**<br><br>Sets the position, direction and velocity of sound channel in case the channel has a 3d option set. |
| IBASS3Channel:SetTime()           | ``float TimePos``                        | ``nil``                  | Set the time position to the given value.<br><br>It seeks to the position, so the position not set immediately. |
| IBASS3Channel:SetVolume()         | ``float Volume``                         | ``nil``                  | **Clientside**<br><br>Set the volume to the given value.<br><br>Volume values:<br>0 = Muted<br>1 = Full volume |
| IBASS3Channel:Stop()              | ``nil``                                  | ``nil``                  | Stops the channel. |
| IBASS3Channel:PlayFile()          | ``string Path``<br><br>``{string or MODE_*-Enum} Flags``<br><br>``[function Callback]`` | ``bool Success`` | Same as BASS3.PlayFile(), but with the Callback being optional.<br><br>This changes the channel object instead of creating a new one. Useful for recycling. |
| IBASS3Channel:PlayURL()           | ``string URL``<br><br>``{string or MODE_*-Enum} Flags``<br><br>``[function Callback]``  | ``bool Success`` | Same as BASS3.PlayURL(), but with the Callback being optional.<br><br>This changes the channel object instead of creating a new one. Useful for recycling. |
| IBASS3Channel:VolumeFadeTo()      | ``float Volume``<br><br>``number time``  | ``nil``                  | **Clientside**<br><br>Fade the sound volume to the given value for the given time in secounds. |
| IBASS3Channel:VolumeIsFading()    | ``nil``                                  | ``bool Fading``          | **Clientside**<br>Returns true if the sound volume is fading. |


## Lua examples
These examples should help you to understand how to use gm_bass3.

#### Loading the module
```lua
require("gm_bass3")

-- Let's see what version we have got.
print("BASS3 Version ", BASS3.Version)
print("GM_BASS3 Version ", BASS3.ModuleVersion)
```


#### Creating a channel from a radio stream.
```lua
if g_channel then g_channel = g_channel:Remove() end

local url = "http://pub1.di.fm:80/di_epictrance"

-- String flags
-- BASS3.PlayURL(url, "mono noblock", function(ch, err, errtxt)

-- Enum flags, recommend
BASS3.PlayURL(url, bit.bor(BASS3.ENUM.MODE_MONO, BASS3.ENUM.MODE_NOBLOCK), function(ch, err, errtxt)
	print("BASS3.PlayURL callback", ch, err, errtxt)

	-- Remove the old one.
	if g_channel then g_channel:Remove() end

	-- don't let it go out of scope!
	g_channel = ch

	if g_channel and CLIENT then
		g_channel:SetBalance(-1) -- start on the left speaker.
		print(g_channel:BalanceIsFading()) -- "false"

		g_channel:BalanceFadeTo(0, 10) -- slowly move to the center for 10 secounds.
		print(g_channel:BalanceIsFading()) -- "true" for the next 10 secounds.
	end
end)
```

#### Creating a channel from a local file.
```lua
if g_channel then g_channel = g_channel:Remove() end
local path = "sound/music/hl1_song10.mp3"

-- String flags
-- BASS3.PlayFile(path, "mono noblock noplay", function(ch, err, errtxt)

-- Enum flags, recommend
BASS3.PlayFile(path, bit.bor(BASS3.ENUM.MODE_MONO, BASS3.ENUM.MODE_NOBLOCK, BASS3.ENUM.MODE_NOPLAY), function(ch, err, errtxt)
	print("BASS3.PlayFile callback", ch, err, errtxt)

	-- Remove the old one.
	if g_channel then g_channel:Remove() end

	-- don't let it go out of scope!
	g_channel = ch

	if g_channel then
		if CLIENT then
			g_channel:SetVolume(0) -- start muted
			print(g_channel:VolumeIsFading()) -- "false"

			g_channel:VolumeFadeTo(1, 2) -- slowly increase the volume to 1 for 2 secounds.
			print(g_channel:VolumeIsFading()) -- "true" for the next 2 secounds.
		end

		g_channel:Play()
	end
end)
```

#### Printing the FFT values.
```lua
local fft = {}

local function printfft(ch)
	-- Is 'ch' valid?
	if !IsValid(ch) then return end

	-- Get the spectrum and print the count
	print("count: ", ch:FFT(fft, BASS3.ENUM.FFT_16))

	-- Print the spectrum values
	print("spectrum values: ")
	PrintTable(fft)

	-- Get the complex spectrum and print the count
	print("count: ", ch:FFTComplex(fft, BASS3.ENUM.FFT_16))

	-- Print the complex spectrum values
	print("complex spectrum values: ")
	PrintTable(fft)
end

-- A valid and playing 'g_channel' has to be created before.
printfft(g_channel)
```

#### Setting and Getting the 3D Position
```lua
if g_channel then g_channel = g_channel:Remove() end

local url = "http://pub1.di.fm:80/di_epictrance"
local pos, dir = Vector(), Vector()

-- String flags
-- BASS3.PlayURL(url, "mono 3d", function(ch, err, errtxt)

-- Enum flags, recommend
BASS3.PlayURL(url, bit.bor(BASS3.ENUM.MODE_MONO, BASS3.ENUM.MODE_3D), function(ch, err, errtxt)
	print("BASS3.PlayURL callback", ch, err, errtxt)

	-- Remove the old one.
	if g_channel then g_channel:Remove() end

	-- don't let it go out of scope!
	g_channel = ch

	-- Set the Position and make the sound emit upwardly.
	g_channel:SetPos(Vector(124,241,235), Vector(0,0,1))

	-- Recycled
	local pos1, dir1 = g_channel:GetPos(pos, dir)
	print(pos, dir) -- Prints "124.0000 241.0000 235.0000    0.0000 0.0000 1.0000"

	-- pos1, dir1 are leading to the same vector as pos, dir
	print(pos1, dir2) -- Prints "124.0000 241.0000 235.0000    0.0000 0.0000 1.0000"

	-- Not Recycled
	local pos2, dir2 = g_channel:GetPos()
	print(pos2, dir2) -- Also prints "124.0000 241.0000 235.0000    0.0000 0.0000 1.0000", but these are new vectors.
end)
```
