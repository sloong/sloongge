g_Menu_GUI_Item = 
{
	[10] = { g_str_Sprite_Name, 0,0,800,600, "Textures\\Test.bmp",['Hanlder']=nil},
	[11] = { g_str_Button_Name,500,200,293,67,"Textures\\UIB_BACK_DN.BMP", 
		"Textures\\UIB_BACK_HV.BMP", "Textures\\UIB_BACK_UP.BMP", "Textures\\UIB_BACK_UP.BMP",
		['Hanlder'] = 	function(event)
					if event == 1 then 
						RunGUI("game.lua");
					end
				end	
		},
	[12] = { g_str_Button_Name,500,300,293,67,"Textures\\UIB_EXIT_DN.BMP", 
		"Textures\\UIB_EXIT_HV.BMP", "Textures\\UIB_EXIT_UP.BMP", "Textures\\UIB_EXIT_UP.BMP",
		['Hanlder'] = 	function( event )
					if event == 1 then 
						Exit(0);
					end
				end
		},
};

for k,v in pairs(g_Menu_GUI_Item) do
	CreateGUIItem( k, v[1], v[6], v[7], v[8], v[9] );
	MoveGUIItem( k, v[2],v[3],v[4],v[5]);
end

RegisterEvent("MenuEventHandler")
function MenuEventHandler( id, event )
	if id ~= 0 then
		local fun = g_Menu_GUI_Item[id]['Hanlder'];
		if nil ~= fun then fun(event) end;
	end
end
