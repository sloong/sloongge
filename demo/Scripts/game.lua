g_Game_GUI_Item = 
{
	{ 'DXFile\\tank2.plg', 
	--math.random(),math.random(),math.random(), -- �S�C���ɵ���������
	0,0,0,
	1,1,1,  -- �s�łS��
	0,0,0 -- ���D�S��
	},
	
	{ 'DXFile\\tank3.plg',
	--math.random(),math.random(),math.random(),	--�S�C���ɵ���������
	0,0,0,
	0.75,0.75,0.75,	-- ģ�Ϳs�łS��
	0,0,0,		-- ģ�����D�S��
	},
};

camPos = {0,40,0, -- ���C��������
0,0,0,	--���C����
0,0,0,	--���CĿ��
};

CreateCamera(camPos[1], camPos[2], camPos[3], camPos[4],camPos[5], camPos[6], camPos[7], camPos[8], camPos[9],
200,12000,120
)

for k,v in pairs(g_Game_GUI_Item) do
	Load3DModule( k, v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10]  ) --�������Π����w�����ID,ģ���ļ�,��������,�s�Ŵ�С,���D�S��
end

RegisterKeyboard = 
{
	Keyboard['DIK_LEFT'],
	Keyboard['DIK_RIGHT'],
	Keyboard['DIK_UP'],
	Keyboard['DIK_DOWN'],
	Keyboard['DIK_W'],
	Keyboard['DIK_A'],
	Keyboard['DIK_S'],
	Keyboard['DIK_D'],
}
RegisterKeyboardEvent(RegisterKeyboard);
RegisterEvent("GameEventHandler")
function GameEventHandler( id, event )
	if event == 4 then
		if id == tonumber(RegisterKeyboard[1]) then -- left key down
			camPos[1] = camPos[1] + 5;
			camPos[7] = camPos[7] + 5;
		elseif id == tonumber(RegisterKeyboard[2]) then -- right
			camPos[1] = camPos[1] - 5;
			camPos[7] = camPos[7] - 5;
		elseif id == tonumber(RegisterKeyboard[3]) or id == tonumber(RegisterKeyboard[5]) then -- up or w
			camPos[3] = camPos[3] - 5;		
		elseif id == tonumber(RegisterKeyboard[4]) or id == tonumber(RegisterKeyboard[7]) then -- down or s
			camPos[3] = camPos[3] + 5;		
		elseif id == tonumber(RegisterKeyboard[6]) then -- a
			camPos[1] = camPos[1] + 5;
			
		elseif id == tonumber(RegisterKeyboard[8]) then -- d
			camPos[1] = camPos[1] - 5;
			
		end;
		MoveCamera(camPos[1], camPos[2], camPos[3], camPos[4],camPos[5], camPos[6], camPos[7], camPos[8], camPos[9]);
	end;
end;
