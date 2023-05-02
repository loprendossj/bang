ion = -1;
	//int CurrentPackageToProcess = 0;
	
	static ref array<ref CarePackageBase> CarePackageSystemClasses;
	static ref array<vector> UsedCarePackageLocations;
	static ref array<int> CarePackageProcessTracker;
	static ref array<float> CarePackageTickTimer;
	
	//Reference to the Configuration manager
	protected ref CarePackagesConfigManager cp_currentcfg;

	override void OnInit()
	{
		super.OnInit();
		
		if(!cp_currentcfg)
			Print("[CarePackage] Loading Config File");
			cp_currentcfg = GetCarePackagesConfig();	//Server creates default config for the mod on the startup!
		if(cp_currentcfg)
			Print("[CarePackage] Config Sucessfully Loaded!");
		else
			Print("[CarePackage] Internal Serverconfig Load Failed!");
		
		if (cp_currentcfg)
		{	//Creates the required classes and arrays for this mod
	
			UsedCarePackageLocations = new array<vector>;
			CarePackageSystemClasses = new array<ref CarePackageBase>;
			CarePackageProcessTracker = new array<int>;
			CarePackageTickTimer = new array<float>;
			
			for( int i=0; i < cp_currentcfg.PackagesBeingRan; i++ )
			{	
				CarePackageSystemClasses.Insert(new CarePackageBase());
				CarePackageProcessTracker.Insert(0);
				CarePackageTickTimer.Insert(0);
			}
		}
	}
	
	void MissionServer()
	{	
		Print("[CarePackage] Initializing CarePackage MOD...");
		
		//Creates the RPC that sends the CarePackage location back to the client
		GetRPCManager().AddRPC("CarePackage_Server", "CarePackageLocation_Server", this, SingleplayerExecutionType.Server);
		
		#ifdef BASICMAP
		GetRPCManager().AddRPC("CarePackage_Server", "SendBasicMapMarker_Server", this, SingleplayerExecutionType.Server);
		#endif
				
		//if ( GetGame().ServerConfigGetInt("UseOverrideCarepackageSystem") == 1 )
		//{
		//	GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( Jumper, 120000 , false);
		//}
	}
	
	override void TickScheduler( float timeslice )
	{
		super.TickScheduler( timeslice );
		
		for( int i=0; i < cp_currentcfg.PackagesBeingRan; i++ )
		{
			CarePackageTickTimer[i] = CarePackageTickTimer[i] + timeslice;
			
			if ( (CarePackageTickTimer[i] >= ((cp_currentcfg.DropTime - cp_currentcfg.MinutesAway) * 60 )) && ( CarePackageProcessTracker[i] == 1 ))
			{	Print("[CarePackage] Sending reminder for Package number "+ (i + 1) + "!");
				//Send notifcation for a drop in x minutes
				CarePackageSystemClasses[i].SendNotifcation(CarePackageSystemClasses[i].MinutesAwayMessage);
				CarePackageTickTimer[i] = 0;
				CarePackageProcessTracker[i] = 2;
			}
			if ( ( CarePackageTickTimer[i] >=  cp_currentcfg.MinutesAway * 60 ) && ( CarePackageProcessTracker[i] == 2 ) )
			{	Print("[CarePackage] Dropping Package number "+ (i + 1) + "!");
				//Send a drop notifcation and then actually drop the bugger
				CarePackageSystemClasses[i].SendNotifcation(cp_currentcfg.DroppedMessage);
				CarePackageSystemClasses[i].SetupAndDropCarePackage();
				CarePackageTickTimer[i] = 0;
				CarePackageProcessTracker[i] = 0;
			}
			if ( CarePackageTickTimer[i] >= ( cp_currentcfg.MinutesBetweenPackages * 60 + ( i * (cp_currentcfg.PackageCallDelay * 60 ) )) && ( CarePackageProcessTracker[i] == 0 ))
			{	
				array<Man> PlayersOnline = new array<Man>;
				GetGame().GetPlayers(PlayersOnline);
				int PlayerCount = PlayersOnline.Count();
				
				if ( PlayerCount > cp_currentcfg.MinimumPlayers || cp_currentcfg.MinimumPlayers == -1 ) 
				{
					Print("[CarePackage] Calling Package number "+ (i + 1) + "!");
					// Find a location that hasnt been used in the last 5 CarePackages
					int ChosenPackageLocation = ChoosePackageLocation( i );
					int ChosenPackageType = ChoosePackageType( i );
					
					if ( ChosenPackageLocation != -1 && ChosenPackageType != -1 ) 
					{
						//Trigger the CarePackage system to start processing a new CarePackage call
						CarePackageSystemClasses[i].ProcessCarePackage( ChosenPackageLocation , ChosenPackageType ); 
					
					
						//Sends the location of the CarePackages back to the clients
						array<vector> positions = new array<vector>;
						for( int j=0; j < cp_currentcfg.PackagesBeingRan; j++ )
						{	
							if ( CarePackageSystemClasses[j].ReturnPackageLocation() != "0 0 0")
							{
								positions.Insert(CarePackageSystemClasses[j].ReturnPackageLocation());
							}
						}
						GetRPCManager().SendRPC( "CarePackage_Gameplay", "CarePackageLocation_Gameplay", new Param1<array<vector>>(positions), true, NULL);
						
						//Add the called CarePackage location to the array of recently called CarePackages
						UsedCarePackageLocations.Insert(cp_currentcfg.Locations[ChosenPackageLocation].ReturnCoords());
						
						//Reset the schedule ticker
						CarePackageTickTimer[i] = 0;
						CarePackageProcessTracker[i] = 1;
						
						#ifdef BASICMAP
							vector droppoint = cp_currentcfg.Locations.Get(ChosenPackageLocation).ReturnCoords();
							int Accuracy = cp_currentcfg.Locations.Get(ChosenPackageLocation).Accuracy * 1.4; //Adding 40% since your math doesn't do a random circle just a random square         
							
							//Create Marker
							BasicMapCircleMarker marker = new BasicMapCircleMarker( "", droppoint, "CarePackage\\Icon.paa", {237,59,110}, 248, false);
							
							//Prevent Editing of the marker
							marker.SetCanEdit(false);
							//Making it so the icon shows in the center
							marker.SetShowCenterMarker(true);
							//Setting hide Intersects to true this way if you have other circle markers this will show in full
							marker.SetHideIntersects(false);
							
							//Set Radius
							if ( GetGame().ServerConfigGetInt("CPMarkerRadius") > 0 )
							{	marker.SetRadius( GetGame().ServerConfigGetInt("CPMarkerRadius") );	}
							else
							{ marker.SetRadius(Accuracy); }
							
							
							//Add Marker
							array<ref BasicMapMarker> markers = new array<ref BasicMapMarker>;
							markers.Insert(marker);

							//Set the Marker
							BasicMap().SetMarkers("CarePackages", markers);
							
							//Update all the clients
							BasicMap().UpdateGroupRemote("CarePackages");
						#endif
					} else 
					{
						//int run = i + 1
						Print("[CarePackage] [ERROR] FAILED TO PROCESS PACKAGE WITH RUN ID: " + (i + 1) );
						CarePackageTickTimer[i] = 0;
					}
				} else {	CarePackageTickTimer[i] = 0;	}
			}
		}
	}

	
	//void Jumper()
	//{
	//	for( int i=0; i < cp_currentcfg.PackagesBeingRan; i++ )
	//	{	
	//		CarePackageSystemClasses[i].UpdateCallLaterMode(true);
	//	}		
	//	int T = ( cp_currentcfg.PackageCallDelay * 60) * 1000;
	//	Print("[CarePackage] Calling CarePackageServerExecution with interval: " + T);
	//	GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( CarePackageServerExecution, T , true);
	//}
	//
	//void CarePackageServerExecution()
	//{	// Find a location that hasnt been used in the last 5 CarePackages
	//	ChosenPackageLocation = ChoosePackageLocation();
	//	
	//	//Trigger the CarePackage system to start processing a new CarePackage call
	//	CarePackageSystemClasses[CurrentPackageToProcess].ProcessCarePackage( ChosenPackageLocation , ChoosePackageType() ); 
	//	
	//	if (CurrentPackageToProcess == cp_currentcfg.PackagesBeingRan)
	//		CurrentPackageToProcess = 0;
	//	else
	//		CurrentPackageToProcess++;
	//	
	//	//Add the called CarePackage location to the array of recently called CarePackages
	//	UsedCarePackageLocations.Insert(cp_currentcfg.Locations[ChosenPackageLocation].ReturnCoords());
	//	
	//	#ifdef BASICMAP
	//		vector droppoint = cp_currentcfg.Locations.Get(ChosenPackageLocation).ReturnCoords();
	//		int Accuracy = cp_currentcfg.Locations.Get(ChosenPackageLocation).Accuracy * 1.4; //Adding 40% since your math doesn't do a random circle just a random square         
	//		
	//		//Create Marker
	//		BasicMapCircleMarker marker = new BasicMapCircleMarker( "", droppoint, "CarePackage\\Icon.paa", {237,59,110}, 248, false);
	//		
	//		//Prevent Editing of the marker
	//		marker.SetCanEdit(false);
	//		//Making it so the icon shows in the center
	//		marker.SetShowCenterMarker(true);
	//		//Setting hide Intersects to true this way if you have other circle markers this will show in full
	//		marker.SetHideIntersects(false);
	//		
	//		//Set Radius
	//		if ( GetGame().ServerConfigGetInt("CPMarkerRadius") > 0 )
	//		{	marker.SetRadius( GetGame().ServerConfigGetInt("CPMarkerRadius") );	}
	//		else
	//		{ marker.SetRadius(Accuracy); }
	//		
	//		
	//		//Add Marker
	//		array<ref BasicMapMarker> markers = new array<ref BasicMapMarker>;
	//		markers.Insert(marker);
	//
	//		//Set the Marker
	//		BasicMap().SetMarkers("CarePackages", markers);
	//		
	//		//Update all the clients
	//		BasicMap().UpdateGroupRemote("CarePackages");
	//	#endif
	//}
	
	int ChoosePackageLocation( int RunID )
	{	RunID++; // Adjustment so that the 0 initialized run array matches the 1 initiallized config run ID's
		int F = 0; // Simple Counter to avoid an infinitely running while loop
		
		//Choose a random location from all configured CarePackage locations
		int Random = Math.RandomIntInclusive( 0, cp_currentcfg.Locations.Count() - 1 );	 
		vector VectorChosenPackageLocation = cp_currentcfg.Locations[Random].ReturnCoords();
		
		//Checks if the location was already used in the last 5 CarePackages
		while ( (PackageLocationRecentlySelected( VectorChosenPackageLocation ) || cp_currentcfg.Locations[Random].AllowedPackageIDs.Find(RunID) == -1) && F < 15 ) 
		{
			//If the CarePackage location had been used pick another one
			Random = Math.RandomIntInclusive( 0, cp_currentcfg.Locations.Count() - 1 );	 
			VectorChosenPackageLocation = cp_currentcfg.Locations[Random].ReturnCoords();
			F++;
		}		
		if (F >= 15)
			{	
				Print("[CarePackage] [ERROR] FAILED TO FIND VALID DROP LOCATION TO USE FOR RUN: " + RunID );	
				Random = -1;
			}	
		
		//If the chosen location hadnt been used it will return that chosen location index
		return Random;
	}
	
	int ChoosePackageType( int RunID )
	{	RunID++; // Adjustment so that the 0 initialized run array matches the 1 initiallized config run ID's
		int F = 0; // Simple Counter to avoid an infinitely running while loop
		
		//Choose a random Package from all configured CarePackage types
		int Random = Math.RandomIntInclusive( 0, cp_currentcfg.Packages.Count() - 1 );
		
		//Checks if the type AllowedPackageIDs matches the current package run ID
		while ( cp_currentcfg.Packages[Random].AllowedPackageIDs.Find(RunID) == -1 && F < 15 ) 
		{
			//If the CarePackage AllowedPackageIDs does not match pick another one
			Random = Math.RandomIntInclusive( 0, cp_currentcfg.Packages.Count() - 1 );	 
			F++;
		}		
		if (F >= 15)
			{	
				Print("[CarePackage] [ERROR] FAILED TO FIND PACKAGE TYPE TO USE FOR RUN: " + RunID );
				Random = -1;
			}		
		
		//If the chosen type AllowedPackageIDs matches return its type ID		
		return Random;
	}
	
	bool PackageLocationRecentlySelected( vector PackageLocation )
	{	//Checks the last 5 location or however many locations there had been so far
		int AmountOfLocationsToCheck;
		if ( UsedCarePackageLocations.Count() <= cp_currentcfg.LocationHistoryCheckRange  ) 
			{	AmountOfLocationsToCheck = UsedCarePackageLocations.Count();	}
		else {	AmountOfLocationsToCheck = cp_currentcfg.LocationHistoryCheckRange;	}
	
		//Scrolls through the last 5 CarePackages used checking if they are the same as the chosen one
		for( int i=UsedCarePackageLocations.Count() - 1; i > UsedCarePackageLocations.Count() - AmountOfLocationsToCheck - 1; i-- )
		{	
			vector LocationVector = UsedCarePackageLocations[i];
			if( PackageLocation == LocationVector )
			{	
				return true; //If they match it returns true
			}
		}
		return false;// If there was no matches it will return false
	}
	
	void CarePackageLocation_Server( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{	
		//Sends the location of the CarePackage back to the client
		array<vector> positions = new array<vector>;
		for( int i=0; i < cp_currentcfg.PackagesBeingRan; i++ )
		{	
			if (CarePackageSystemClasses[i].ReturnPackageLocation() != "0 0 0")
			{
				positions.Insert(CarePackageSystemClasses[i].ReturnPackageLocation());
			}
		}
		
		if (type == CallType.Server) 
		{
			GetRPCManager().SendRPC( "CarePackage_Gameplay", "CarePackageLocation_Gameplay", new Param1<array<vector>>(positions), true, sender);			
		}
	}
	
	//#ifdef BASICMAP
	//void SendBasicMapMarker_Server(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target)
	//{
	//	if (ChosenPackageLocation != -1)
	//	{
	//		vector droppoint = cp_currentcfg.Locations.Get(ChosenPackageLocation).ReturnCoords();
	//		int Accuracy = cp_currentcfg.Locations.Get(ChosenPackageLocation).Accuracy * 1.4; //Adding 40% since your math doesn't do a random circle just a random square         
	//		
	//		//Create Marker
	//		BasicMapCircleMarker marker = new BasicMapCircleMarker( "", droppoint, "CarePackage\\Icon.paa", {237,59,110}, 248, false);
	//		
	//		//Prevent Editing of the marker
	//		marker.SetCanEdit(false);
	//		//Making it so the icon shows in the center
	//		marker.SetShowCenterMarker(true);
	//		//Setting hide Intersects to true this way if you have other circle markers this will show in full
	//		marker.SetHideIntersects(false);
	//		
	//		//Set Radius
	//		if ( GetGame().ServerConfigGetInt("CPMarkerRadius") > 0 )
	//		{	marker.SetRadius( GetGame().ServerConfigGetInt("CPMarkerRadius") );	}
	//		else
	//		{ marker.SetRadius(Accuracy); }
	//		
	//		//Add Marker
	//		array<ref BasicMapMarker> markers = new array<ref BasicMapMarker>;
	//		markers.Insert(marker);
	//
	//		//Set the Marker
	//		BasicMap().SetMarkers("CarePackages", markers);
	//		
	//		//Update all the clients
	//		BasicMap().UpdateGroupRemote("CarePackages", sender);
	//	}
	//}
	//#endif
};



	
	
	OggS         »M¨f    ®Ìjpvorbis    D¬      €µ     ¸