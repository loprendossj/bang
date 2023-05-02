y() {
        GetRPCManager().AddRPC( "CarePackage_Gameplay", "CarePackageLocation_Gameplay", this, SingleplayerExecutionType.Client );
    }

    void CarePackageLocation_Gameplay( CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target ) 
	{
        if (type == CallType.Client) {
            Param1<array<vector>> data;
            if ( !ctx.Read( data ) ) return;
			
			g_Game.ClearCarePackageMarkers();
			array<vector> Positions = data.param1;
					
			for( int i=0; i < Positions.Count(); i++ )
			{ 
				if ( Positions.Get(i) )
					g_Game.SetClientCarePackageMarker(Positions.Get(i));
				else return;
			}
        }
    }
}modded class MissionServer
{	
	//int ChosenPackageLocat