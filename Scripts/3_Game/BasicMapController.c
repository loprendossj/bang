#ifdef BASICMAP
modded class BasicMapController{

	override void LoadClientMarkers(){
		super.LoadClientMarkers();
		
		GetRPCManager().SendRPC( "CarePackage_Server", "SendBasicMapMarker_Server", NULL);
		//BasicMap().RequestGroupUpdate("CarePackages");
	}
}
#endif