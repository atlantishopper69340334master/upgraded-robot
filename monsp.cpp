---------------------- Varables ----------------------
local msversion = "v1"
local cloneref = cloneref or function(o) return o end
local maxremotes = 350
local remotesInLogs = 0

---------------------- Utility Functions ----------------------
local function Create(instance, properties, children)
	local obj = Instance.new(instance)
	for i, v in next, properties or {} do
		obj[i] = v
	end
	for _, child in next, children or {} do
		child.Parent = obj
	end
	return obj
end

local function copyText(text)
	setclipboard(text)
end

local function createClickSound(parent)
	local sound = Instance.new("Sound")
	sound.SoundId = "rbxassetid://6042053626"
	sound.Volume = 1
	sound.Parent = parent
	return sound
end

function Notif(msg)
	local StarterGui = game:GetService("StarterGui")
	pcall(function()
		StarterGui:SetCore("SendNotification", {
			Title = "Moon Spy",
			Text = msg,
			Duration = 7
		})
	end)
end

local function tableToString(t, visited)
	if visited[t] then
		return "--[[Circular Reference]]"
	end
	visited[t] = true

	local items = {}
	for k, v in pairs(t) do
		local keyStr
		if type(k) == "string" then
			keyStr = string.format("[\"%s\"]", k)
		else
			keyStr = string.format("[%s]", tostring(k))
		end
		table.insert(items, string.format("%s = %s", keyStr, valueToString(v, visited)))
	end

	return "{ " .. table.concat(items, ", ") .. " }"
end

function valueToString(v, visited)
	local t = type(v)
	if t == "string" then
		return string.format("%q", v)  -- e.g. "Hello"
	elseif t == "number" or t == "boolean" then
		return tostring(v)             -- e.g. 123 or true
	elseif t == "table" then
		return tableToString(v, visited)
	elseif typeof(v) == "Instance" then
		return "game." .. v:GetFullName()
	else
		return string.format("--[[%s]]", tostring(v))
	end
end

---------------------- Services ----------------------
local CoreGui = cloneref(game:GetService("CoreGui"))
local RunService = cloneref(game:GetService("RunService"))
local UserInputService = cloneref(game:GetService("UserInputService"))
local TweenService = cloneref(game:GetService("TweenService"))

local SimpleSpy3 = Create("ScreenGui", {
	ResetOnSpawn = false,
	Parent = CoreGui,
	Name = "MoonSpy "..msversion
})

local Background = Create("Frame", {
	Parent = SimpleSpy3,
	BackgroundColor3 = Color3.new(1, 1, 1),
	BackgroundTransparency = 1,
	Position = UDim2.new(0, 500, 0, 200),
	Size = UDim2.new(0, 450, 0, 280),
	Name = "Background"
})
Background.Active = true
Background.Draggable = true

local TopBar = Create("Frame", {
	Parent = Background,
	BackgroundColor3 = Color3.fromRGB(37, 35, 38),
	BorderSizePixel = 0,
	Size = UDim2.new(1, 0, 0, 19),
	Name = "TopBar"
})

local Title = Create("TextButton", {
	Parent = TopBar,
	BackgroundColor3 = Color3.new(1, 1, 1),
	AutoButtonColor = false,
	BackgroundTransparency = 1,
	Position = UDim2.new(0, 5, 0, 0),
	Size = UDim2.new(0, 100, 1, 0),
	Font = Enum.Font.SourceSansBold,
	Text = "Moon Spy",
	TextColor3 = Color3.new(1, 1, 1),
	TextSize = 14,
	TextXAlignment = Enum.TextXAlignment.Left
})

local TweenService = game:GetService("TweenService")

local CloseButton = Create("TextButton", {
	Parent = TopBar,
	Position = UDim2.new(1, -19, 0, 0),  -- positioned 20 pixels from the right edge
	Size = UDim2.new(0, 20, 1, 0),        -- 20 pixels wide, full height of TopBar
	BackgroundColor3 = Color3.fromRGB(36, 35, 38),
	BorderSizePixel = 0,
	Text = ""
})

local CloseImage = Create("ImageLabel", {
	Parent = CloseButton,
	BackgroundTransparency = 1,
	Size = UDim2.new(1, 0, 1, 0),
	Image = "http://www.roblox.com/asset/?id=5597086202",
	ScaleType = Enum.ScaleType.Stretch
})

CloseButton.MouseButton1Click:Connect(function()
	for _, v in pairs(SimpleSpy3:GetDescendants()) do
		if v:IsA("Frame") and not v:IsA("Sound") then
			TweenService:Create(v, TweenInfo.new(0.5), {BackgroundTransparency = 1}):Play()
		end

		if v:IsA("TextBox") or v:IsA("TextButton") or v:IsA("TextLabel") and not v:IsA("Sound") then
			TweenService:Create(v, TweenInfo.new(0.5), {TextTransparency = 1}):Play()
		end

		if v:IsA("ImageButton") or v:IsA("ImageLabel") and not v:IsA("Sound") then
			TweenService:Create(v, TweenInfo.new(0.5), {ImageTransparency = 1}):Play()
		end
	end

	delay(0.5, function()
		if SimpleSpy3 then
			SimpleSpy3:Destroy()
		end
	end)
end)

---------------------- Left Panel (Logs) ----------------------
local ClickSound = createClickSound(SimpleSpy3)
local LeftPanel = Create("Frame", {
	Parent = Background,
	BackgroundColor3 = Color3.fromRGB(53, 52, 55),
	BorderSizePixel = 0,
	Position = UDim2.new(0, 0, 0, 19),
	Size = UDim2.new(0, 131, 1, -19),
	Name = "LeftPanel"
})
local LogList = Create("ScrollingFrame", {
	Parent = LeftPanel,
	Active = true,
	BackgroundColor3 = Color3.new(1, 1, 1),
	BackgroundTransparency = 1,
	BorderSizePixel = 0,
	Position = UDim2.new(0, 0, 0, 0),
	Size = UDim2.new(1, 0, 1, 0),
	CanvasSize = UDim2.new(0, 0, 0, 0),
	ScrollBarThickness = 4,
	Name = "LogList"
})

local UIListLayout = Create("UIListLayout", {
	Parent = LogList,
	HorizontalAlignment = Enum.HorizontalAlignment.Center,
	SortOrder = Enum.SortOrder.LayoutOrder,
	Padding = UDim.new(0, 4)
})

local function updateLogListCanvas()
	LogList.CanvasSize = UDim2.new(0, 0, 0, UIListLayout.AbsoluteContentSize.Y + 10)
end
UIListLayout:GetPropertyChangedSignal("AbsoluteContentSize"):Connect(updateLogListCanvas)

---------------------- Right Panel (Code & Buttons) ----------------------
local RightPanel = Create("Frame", {
	Parent = Background,
	BackgroundColor3 = Color3.fromRGB(37, 36, 38),
	BorderSizePixel = 0,
	Position = UDim2.new(0, 131, 0, 19),
	Size = UDim2.new(0, 319, 1, -19),
	Name = "RightPanel"
})

local CodeBox = Create("TextBox", {
	Parent = RightPanel,
	BackgroundColor3 = Color3.fromRGB(20, 18, 20),
	TextColor3 = Color3.new(1, 1, 1),
	ClearTextOnFocus = false,
	MultiLine = true,
	TextWrapped = true,
	TextEditable = false,
	Size = UDim2.new(1, -10, 0, 120),
	Position = UDim2.new(0, 5, 0, 5),
	Font = Enum.Font.Code,
	TextSize = 14,
	Text = 'local args = {[1] = "dsc.gg/getmoonexec"}\nworkspace.MoonOnTop:FireServer(unpack(args))',
	Name = "CodeBox"
})

local ButtonFrame = Create("Frame", {
	Parent = RightPanel,
	BackgroundTransparency = 1,
	BackgroundColor3 = Color3.fromRGB(37, 36, 38),
	Position = UDim2.new(0, 5, 0, 130),
	Size = UDim2.new(1, -10, 0, 140),
	Name = "ButtonFrame"
})

local UIGridLayout = Create("UIGridLayout",{
	Parent = ButtonFrame,
	HorizontalAlignment = Enum.HorizontalAlignment.Center,
	SortOrder = Enum.SortOrder.LayoutOrder,
	CellPadding = UDim2.new(0, 0, 0, 0),
	CellSize = UDim2.new(0, 94, 0, 27)
})

local ToolTip = Create("TextLabel", {
	Parent = SimpleSpy3,
	BackgroundColor3 = Color3.fromRGB(60, 60, 60),
	TextColor3 = Color3.new(1, 1, 1),
	BorderSizePixel = 0,
	Visible = false,
	Font = Enum.Font.SourceSans,
	TextSize = 14,
	TextWrapped = true,
	ZIndex = 9999
})

local function showToolTip(btn, text)
	ToolTip.Text = text
	ToolTip.Visible = true
	ToolTip.Size = UDim2.new(0, ToolTip.TextBounds.X + 10, 0, ToolTip.TextBounds.Y + 6)

	local connection
	connection = RunService.RenderStepped:Connect(function()
		local MousePos = UserInputService:GetMouseLocation()
		ToolTip.Position = UDim2.new(0, MousePos.X + 15, 0, MousePos.Y - 5)
	end)

	btn.MouseLeave:Connect(function()
		ToolTip.Visible = false
		connection:Disconnect()
	end)
end

local function createActionButton(text, tooltipText, callback)
	local btn = Create("TextButton", {
		Parent = ButtonFrame,
		Text = "",
		Size = UDim2.new(0, 69, 0, 18),
		BackgroundColor3 = Color3.new(0, 0, 0),
		BackgroundTransparency = 0.69999998807907,
		TextColor3 = Color3.new(1, 1, 1),
		Font = Enum.Font.SourceSansBold,
		TextSize = 14
	})

	local Bar = Create("Frame", {
		Parent = btn,
		BackgroundColor3 = Color3.new(1, 1, 1),
		BorderSizePixel = 0,
		Position = UDim2.new(0, 0, 0, 0),
		Size = UDim2.new(0, 4, 1, 0),
		Name = "WhiteBar"
	})

	local Label = Create("TextLabel", {
		Parent = btn,
		BackgroundTransparency = 1,
		Text = text,
		TextColor3 = Color3.new(1, 1, 1),
		Font = Enum.Font.SourceSansBold,
		TextSize = 14,
		Position = UDim2.new(0, 8, 0, 0),
		Size = UDim2.new(1, -8, 1, 0),
		Name = "ActionLabel",
		TextXAlignment = Enum.TextXAlignment.Left
	})

	btn.MouseEnter:Connect(function()
		showToolTip(btn, tooltipText)
	end)

	btn.MouseButton1Click:Connect(function()
		ClickSound:Play()
		callback()
	end)
	return btn
end
---------------------- Data & State ----------------------
local logs = {}                -- All remote logs
local excludedRemotes = {}     -- Excluded from logging
local selectedLog = nil        -- Currently selected log
local selectedRemote = nil     -- Currently selected remote

---------------------- Script Generation ----------------------
local function genScript(remote, args)
	local lines = {}
	for i, v in ipairs(args) do
		lines[#lines + 1] = string.format("[%.0f] = %s", i, valueToString(v, {}))
	end

	local finalArgs = table.concat(lines, ", ")
	local scriptStr = string.format(
		"local args = {%s}\n%s:FireServer(unpack(args))",
		finalArgs,
		"game."..remote:GetFullName()
	)
	return scriptStr
end

---------------------- Selecting a Log ----------------------
local function selectLog(log)
	selectedLog = log
	selectedRemote = log.Remote
	log.GenScript = genScript(log.Remote, log.Args or {})
	CodeBox.Text = log.GenScript
end

---------------------- Creating a Log Entry ----------------------
local function newRemote(data)
	if excludedRemotes[data.remote] then return end
	if remotesInLogs >= 350 then return end
	
	local remote = data.remote
	local args = data.args or {}

	local logEntry = {
		Remote = remote,
		Args = args,
		FunctionInfo = data.funcInfo or string.format("Class: %s\nCalled from: %s", remote.ClassName, data.callingScript or "Unknown"),
		GenScript = "-- Generating script..."
	}
	table.insert(logs, logEntry)

	local colorBarColor = Color3.fromRGB(255, 242, 0)
	if remote:IsA("RemoteFunction") or remote:IsA("BindableFunction") then
		colorBarColor = Color3.fromRGB(99, 86, 245)
	end

	local RemoteBtn = Create("TextButton", {
		Parent = LogList,
		Text = "",
		Size = UDim2.new(1, -4, 0, 25),
		BackgroundColor3 = Color3.new(0, 0, 0),
		TextColor3 = Color3.new(1, 1, 1),
		Font = Enum.Font.SourceSans,
		TextSize = 14,
		LayoutOrder = #logs
	})

	local ColorBar = Create("Frame", {
		Parent = RemoteBtn,
		BackgroundColor3 = colorBarColor,
		BorderSizePixel = 0,
		Position = UDim2.new(0, 0, 0, 0),
		Size = UDim2.new(0, 4, 1, 0),
		Name = "ColorBar"
	})

	local RemoteLabel = Create("TextLabel", {
		Parent = RemoteBtn,
		BackgroundTransparency = 1,
		Text = remote.Name,
		TextColor3 = Color3.new(1, 1, 1),
		Font = Enum.Font.SourceSans,
		TextSize = 14,
		Position = UDim2.new(0, 8, 0, 0),
		Size = UDim2.new(1, -8, 1, 0),
		Name = "RemoteLabel",
		TextXAlignment = Enum.TextXAlignment.Left
	})

	RemoteBtn.MouseEnter:Connect(function()
		showToolTip(RemoteBtn, remote.ClassName)
	end)

	RemoteBtn.MouseButton1Click:Connect(function()
		ClickSound:Play()
		selectLog(logEntry)
	end)

	logEntry.UI = RemoteBtn
	remotesInLogs += 1
	updateLogListCanvas()
end

---------------------- Action Buttons ----------------------
createActionButton("Copy Code", "Copies the generated code", function()
	if CodeBox.Text and #CodeBox.Text > 0 then
		copyText(CodeBox.Text)
	else
		Notif("No event selected or no code detected.")
	end
end)

createActionButton("Copy Path", "Copies the remote's full path", function()
	if selectedRemote then
		copyText(selectedRemote:GetFullName())
	else
		Notif("No event selected.")
	end
end)

createActionButton("Run Code", "Executes the generated code", function()
	if CodeBox.Text and #CodeBox.Text > 0 then
		local func = loadstring(CodeBox.Text)()
		if func then
			spawn(func)
		else
			Notif("Error running code.")
		end
	end
end)

createActionButton("Func Info", "Copies event function info", function()
	if selectedLog then
		local info = selectedLog.FunctionInfo or "No additional info."
		copyText(info)
	else
		Notif("No event selected.")
	end
end)

createActionButton("Clr Logs", "Clears logs", function()
	for _, child in ipairs(LogList:GetChildren()) do
		if child:IsA("TextButton") then
			child:Destroy()
		end
	end
	logs = {}
	remotesInLogs = 0
	updateLogListCanvas()
end)

createActionButton("Clr Excludes", "Clears Excludes logs", function()
	table.clear(excludedRemotes)
end)

createActionButton("Exclude", "Prevents from logging", function()
	if selectedRemote then
		excludedRemotes[selectedRemote] = true
		if selectedLog and selectedLog.UI then
			selectedLog.UI:Destroy()
		end
	else
		Notif("No event selected.")
	end
end)

createActionButton("Remove", "Removes the event", function()
	if selectedRemote then
		selectedRemote:GetFullName():Destroy()
	else
		Notif("No event selected.")
	end
end)

---------------------- Hooking Logic ----------------------
local function hookObject(obj)
	if obj:IsA("RemoteEvent") then
		obj.OnClientEvent:Connect(function(...)
			newRemote({
				remote = obj,
				args = {...},
				callingScript = "Unknown"
			})
		end)

	elseif obj:IsA("BindableEvent") then
		obj.Event:Connect(function(...)
			newRemote({
				remote = obj,
				args = {...},
				callingScript = "Unknown"
			})
		end)

	elseif obj:IsA("RemoteFunction") then
		pcall(function()
			local old = obj.OnClientInvoke
			obj.OnClientInvoke = function(...)
				newRemote({
					remote = obj,
					args = {...},
					callingScript = "Unknown"
				})
				if old then
					return old(...)
				end
			end
		end)

	elseif obj:IsA("BindableFunction") then
		pcall(function()
			local old = obj.OnInvoke
			obj.OnInvoke = function(...)
				newRemote({
					remote = obj,
					args = {...},
					callingScript = "Unknown"
				})
				if old then
					return old(...)
				end
			end
		end)
	end
end

local ServicesToScan = {
	game:GetService("ReplicatedStorage"),
	game:GetService("Workspace"),
	game:GetService("StarterGui"),
	game:GetService("Players")
}
for _, service in ipairs(ServicesToScan) do
	for _, desc in ipairs(service:GetDescendants()) do
		hookObject(desc)
	end
	
	service.DescendantAdded:Connect(function(child)
		hookObject(child)
	end)
end

print("[Moon Spy]: Loaded!")
Notif("[Moon Spy]: Loaded!")
Notif("Made by RamingCactus! dsc.gg/getmoonexec")
Notif("server sided remote spy")
Notif()
