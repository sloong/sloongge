g_Game_GUI_Item = 
{
	{ 'DXFile\\tank2.plg', 0.75,0.75,0.75, 
	math.random(),math.random(),math.random(), -- 隨機生成的世界座標
	},
	
};

for k,v in pairs(g_Game_GUI_Item) do
	Loading3DModule( k, v[1], v[2], v[3], v[4] ) --參數依次爲物體保存的ID,模型文件,縮放大小,後續使用WorldPos進行更改世界座標
	SetModulePos( k, v[5], v[6], v[7]) --參數依次爲模型ID,座標的x,y,z
end

RegisterEvent("GameEventHandler")
function GameEventHandler( id, event )
	if id ~= 0 then
		local fun = g_Game_GUI_Item[id]['Hanlder'];
		if nil ~= fun then fun(event) end;
	end
end
