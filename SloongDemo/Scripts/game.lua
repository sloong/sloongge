g_Game_GUI_Item = 
{
	{ 'DXFile\\tank2.plg', 
	math.random(),math.random(),math.random(), -- 隨機生成的世界座標
	0.75,0.75,0.75,  -- 縮放係數
	0,0,0 -- 旋轉係數
	},
	
	{ 'DXFile\\tank3.plg',
	math.random(),math.random(),math.random(),	--隨機生成的世界座標
	0.75,0.75,0.75,	-- 模型縮放係數
	0,0,0,		-- 模型旋轉係數
	},
};

CreateCamera(0,40,0, -- 相機世界座標
0,0,0,	--相機方向
0,0,0,	--相機目標
200,12000,120
)

for k,v in pairs(g_Game_GUI_Item) do
	Load3DModule( k, v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10]  ) --參數依次爲物體保存的ID,模型文件,世界座標,縮放大小,旋轉係數
end

RegisterKeyboard = 
{
	Keyboard['DIK_LEFT'],
	Keyboard['DIK_RIGHT'],
	Keyboard['DIK_UP'],
	Keyboard['DIK_DOWN'],
}
RegisterKeyboardEvent(RegisterKeyboard);
RegisterEvent("GameEventHandler")
function GameEventHandler( id, event )
	if id ~= 0 then
		local fun = g_Game_GUI_Item[id]['Hanlder'];
		if nil ~= fun then fun(event) end;
	end
end
