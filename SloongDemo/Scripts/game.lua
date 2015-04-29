g_Game_GUI_Item = 
{
	{ 'DXFile\\tank2.plg', 0.75,0.75,0.75,  -- 縮放係數
	math.random(),math.random(),math.random(), -- 隨機生成的世界座標
	0,0,0 -- 旋轉係數
	},
	
};

for k,v in pairs(g_Game_GUI_Item) do
	Load3DModule( k, v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10]  ) --參數依次爲物體保存的ID,模型文件,縮放大小,後續使用WorldPos進行更改世界座標
end

RegisterEvent("GameEventHandler")
function GameEventHandler( id, event )
	if id ~= 0 then
		local fun = g_Game_GUI_Item[id]['Hanlder'];
		if nil ~= fun then fun(event) end;
	end
end
