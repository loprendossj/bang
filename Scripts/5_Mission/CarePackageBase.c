class CarePackageBase
{	
	private bool m_CarePackageActive;
	private bool CallLaterMode = false;
	private int m_CurrentDrop;
	private string m_DropLocationName;
	private string m_DropObjectType;
	private string m_ParachuteObjectType;
	private vector m_DropLocation;
	private int m_DropFinalHeight;
	private int m_Accuracy;
	
	ref CarePackagesConfigManager cp_currentcfg;
	
	CPLocation LocationEntry;
	CPPackage DropTypeEntry;
	
	ref array<DayZInfected> Infected;
	
	ref Timer m_Timer;	
	
	string StartMessage, MinutesAwayMessage;

	EntityAI DropObject;
	EntityAI ParachuteObject;
	
	void CarePackageBase()
	{	//Grab the config for CarePackages
		cp_currentcfg = GetCarePackagesConfig();
		
		//Inits the system variables
		Infected = new array<DayZInfected>;
		m_Timer = new Timer();
		
		m_DropLocation = "0 0 0";
	}
	
	void ProcessCarePackage(int ChosenDropLocation, int ChosenDropType )
	{	//Clears the current CarePackage if it exists
		ClearCarePackage();
		
		//Grabs all the config info for the CarePackage that was chosen
		LocationEntry = cp_currentcfg.ReturnLocation(ChosenDropLocation);
		DropTypeEntry = cp_currentcfg.ReturnPackage(ChosenDropType);
		
		m_DropLocationName = LocationEntry.Name; 
		m_DropObjectType = DropTypeEntry.object_type; 
		m_ParachuteObjectType = DropTypeEntry.parachute_type; 
		m_DropLocation = LocationEntry.ReturnCoords(); 
		m_Accuracy = LocationEntry.Accuracy; 
		
		float X = m_DropLocation[0] + Math.RandomFloatInclusive(-m_Accuracy, m_Accuracy);
		float Z = m_DropLocation[2] + Math.RandomFloatInclusive(-m_Accuracy, m_Accuracy);
		float Y = GetGame().SurfaceY(X,Z);
		
		m_DropLocation = Vector(X,Y,Z);
		
		StartMessage = cp_currentcfg.StartMessage + m_DropLocationName;
		MinutesAwayMessage = cp_currentcfg.MinutesAwayPrefix + cp_currentcfg.MinutesAway + cp_currentcfg.MinutesAwaySuffix  + m_DropLocationName + ".";
		
		//Sets off the timings for the different messages as well as for the drop setup
		SetTimings();
		
		//Send the CarePackage location to clients
		GetRPCManager().SendRPC( "CarePackage_Gameplay", "CarePackageLocation_Gameplay", new Param1<vector>(m_DropLocation), true, NULL);
		
		//Tells the system that an CarePackage is active
		m_CarePackageActive = true;
	}
	
	void UpdateCallLaterMode( bool enable )
	{
		CallLaterMode = enable;
	}
	
	void SpawnZombies( int ZombiesToSpawn )
	{
		// Makes sure the distance is at least 5m
		float MaxSpawnDistance = 10;
		
		//Spawns in zombies around the drop location
		for ( int i = 0; i < ZombiesToSpawn; i++)
		{
			string RandomInfected = cp_currentcfg.InfectedTypes.GetRandomElement();
			float X = m_DropLocation[0] + Math.RandomFloatInclusive(-MaxSpawnDistance, MaxSpawnDistance);
			float Z = m_DropLocation[2] + Math.RandomFloatInclusive(-MaxSpawnDistance, MaxSpawnDistance);
			float Y = GetGame().SurfaceY(X,Z);
			
			vector InfectedPos1 = (Vector(X,Y,Z));
			DayZInfected InfectedSurvivor = DayZInfected.Cast(GetGame().CreateObject(RandomInfected, InfectedPos1, false, true));
			Infected.Insert(InfectedSurvivor);
		}
	}
	
	void SetupAndDropCarePackage()
	{	//Creates, sets and drops the CarePackage box
		//Drop it like its hot
		Print("[CarePackage] Dropping Package at: " + m_DropLocation);
		m_DropFinalHeight = GetGame().SurfaceY(m_DropLocation[0],m_DropLocation[2]) + 0.2;
		
		//Sets the height of the CarePackage start location
		vector StartLocation = Vector(m_DropLocation[0],
									  m_DropFinalHeight + cp_currentcfg.DropHeight, // Find the ground location at X/Y and add DropHeight to it;
									  m_DropLocation[2] );
		
		//Creates the box and chute
		vector ParachuteStartLocation = StartLocation + Vector(0, 1.3, 0);
		DropObject = ItemBase.Cast( GetGame().CreateObject( m_DropObjectType, StartLocation ));
		//Disable Sim
		DropObject.ClearEventMask(EntityEvent.POSTSIMULATE);
		DropObject.dBodyActive(DropObject, ActiveState.INACTIVE);
		
		ParachuteObject = ItemBase.Cast(GetGame().CreateObject( m_ParachuteObjectType, ParachuteStartLocation ));
		
		if (!DropObject)
		{	Print("[CarePackage] Package was not created, possible error in config");	}
		
		if (!ParachuteObject)
		{	Print("[CarePackage] Parachute was not created, possible error in config");	}
		
		//Set the box location to the start position of the CarePackage (otherwise it just spawns on the ground)
		DropObject.SetPosition( StartLocation );
		ParachuteObject.SetPosition( ParachuteStartLocation );
		
		//Spawn Zombies around the drop location
		SpawnZombies( cp_currentcfg.ZombiesToSpawn );
		
		//Spawn shit in that bitch		
		if (DropObject)
		{
			EntityAI Item;
			CPItems CPItem;
			CPWeapons CPWeapon;
			ItemBase ItemB;
			Magazine_Base ItemM;
			
			int WeaponsToSpawn = Math.RandomIntInclusive(DropTypeEntry.MinWeapons, DropTypeEntry.MaxWeapons);
			int MiscItemsToSpawn = Math.RandomIntInclusive(DropTypeEntry.MinMiscItems, DropTypeEntry.MaxMiscItems);
			
			if (cp_currentcfg.LootSpawnType == 0)
			{
				for (int i1 = 0; i1 < WeaponsToSpawn; i1++)
				{	//Select a random entry from the configuration
					CPWeapon = DropTypeEntry.ReturnRandomWeapon();
					Item = DropObject.GetInventory().CreateInInventory( CPWeapon.Item );
					if(Item) //Just incase the item isnt spawned
					{ 	
						for (int i2 = 0; i2 < CPWeapon.Attachments.Count(); i2++)
						{	//Spawns attachments on the item if applicable
							Item.GetInventory().CreateAttachment(CPWeapon.Attachments[i2]);
						}
					} else
					{ Print( "[CarePackage] "+ CPWeapon.Item + " not spawned, possible typo in config"); }
				}
				for (int k1 = 0; k1 < MiscItemsToSpawn; k1++)
				{	//Select a random entry from the configuration
					CPItem = DropTypeEntry.ReturnRandomItem();
					Item = DropObject.GetInventory().CreateInInventory( CPItem.Item );
					if(Item) //Just incase the item isnt spawned
					{ 	
						for (int k2 = 0; k2 < CPItem.Attachments.Count(); k2++)
						{	//Spawns attachments on the item if applicable
							Item.GetInventory().CreateAttachment(CPItem.Attachments[k2]);
						}
					} else
					{ Print( "[CarePackage] "+ CPItem.Item + " not spawned, possible typo in config"); }
					//Casts the item to item base (so we can set quantity)
					ItemB = ItemBase.Cast(Item);
					ItemM = Magazine_Base.Cast(Item);
					if(ItemB)
					{	ItemB.SetQuantity( Math.RandomIntInclusive( CPItem.MinQty, CPItem.MaxQty )); }
					if(ItemM)
					{	ItemM.ServerSetAmmoCount( Math.RandomIntInclusive( CPItem.MinQty, CPItem.MaxQty )); }
				}
			} else if (cp_currentcfg.LootSpawnType == 1)
			{
				for (int a1 = 0; a1 < DropTypeEntry.Items.Count(); a1++)
				{	//Select a each entry from the configuration
					CPItem = DropTypeEntry.Items[a1];
					Item = DropObject.GetInventory().CreateInInventory( CPItem.Item );
					if(Item) //Just incase the item isnt spawned
					{ 	
						for (int a2 = 0; a2 < CPItem.Attachments.Count(); a2++)
						{	//Spawns attachments on the item if applicable
							Item.GetInventory().CreateAttachment(CPItem.Attachments[a2]);
						}
					} else
					{ Print( "[CarePackage] "+ CPItem.Item + " not spawned, possible typo in config"); }
					//Casts the item to item base (so we can set quantity)
					ItemB = ItemBase.Cast(Item);
					ItemM = Magazine_Base.Cast(Item);
					if(ItemB && !ItemM )
					{	ItemB.SetQuantity( Math.RandomIntInclusive( CPItem.MinQty, CPItem.MaxQty )); }
					if(ItemM && CPItem.MaxQty != 1	)
					{	ItemM.ServerSetAmmoCount( Math.RandomIntInclusive( CPItem.MinQty, CPItem.MaxQty )); }
				}
			} else Print("[CarePackage] LootSpawnType not set Correctly");
		}
		
		if ( cp_currentcfg.LockPackages )
		{
			CarePackage_base Package = CarePackage_base.Cast(DropObject);
			if ( Package )
				Package.LockPackage();
		}
		
		Roadflare MissionFlare = Roadflare.Cast( GetGame().CreateObject( "RoadFlare", Vector(m_DropLocation[0] + 1, m_DropFinalHeight, m_DropLocation[2] + 1 )));
		MissionFlare.GetCompEM().SwitchOn();
		
		//Runs a timer that continuously lowers the drop at ~30FPS
		m_Timer.Run(0.03, this, "LowerDrop", NULL, true);
	}	
	
	void LowerDrop()
	{
		if (DropObject) //If we didnt fuck it up earlier
		{	//Get the objects current location
			vector DropObjectVector = DropObject.GetPosition();
			float DropY = DropObjectVector[1];
			if (DropY > m_DropFinalHeight + 25)
			{ 	//If its still higher than the target location drop it by 0.1m
				DropObject.SetPosition( DropObject.GetPosition() + Vector(0, -0.1, 0) ); 
				ParachuteObject.SetPosition( ParachuteObject.GetPosition() + Vector(0, -0.1, 0) ); 
			}
			else //Otherwise stop the timer and enable physics
			{					
				GetGame().ObjectDelete( ParachuteObject );
				DropObject.CreateDynamicPhysics(PhxInteractionLayers.DYNAMICITEM);
				DropObject.EnableDynamicCCD(true);
				DropObject.SetVelocity(DropObject, Vector(0, -0.01, 0));
				m_Timer.Stop();
				
				GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( FixPackage, 20000 , false ); 
			}
		}
		else //If I buggered it somewhere stop the timer
		{	
			m_Timer.Stop();
		}
	}
	
	void FixPackage()
	{
		vector ObjectPosition = DropObject.GetPosition();
		float LandHeight = GetGame().SurfaceY(ObjectPosition[0],ObjectPosition[2]);
		
		Print("[CarePackage] Checking if Package yeeted: " + ObjectPosition[1] + " < " + LandHeight + ".");
		if (ObjectPosition[1] < LandHeight)
		{
			DropObject.SetPosition(m_DropLocation);
			DropObject.SetOrientation("0 0 0");
			Print("[CarePackage] Set Package to " + m_DropLocation + ".");
		}
	}
	
	void SetTimings()
	{   int MA = ((cp_currentcfg.DropTime - cp_currentcfg.MinutesAway) * 60 ) * 1000;
		int DM = (cp_currentcfg.DropTime * 60 ) * 1000;
	
		// Called when the CarePackage starts its timer
		SendNotifcation( StartMessage ); 
		if ( CallLaterMode == true )
		{
			// Called when the CarePackage is X MinutesAway
			GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( SendNotifcation, MA , false, MinutesAwayMessage); 
			//Called when the drop, drops
			GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( SendNotifcation, DM, false, cp_currentcfg.DroppedMessage); 
			//Called when the drop, drops
			GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( SetupAndDropCarePackage, DM, false); 
		}
	}
	
	void SendNotifcation(string message)
	{	//Sends the requested notifcation
		NotificationSystem.SendNotificationToPlayerIdentityExtended( null, 25.0, cp_currentcfg.Title, message, "CarePackage\\Icon.paa" );
	}
	
	void ClearCarePackage()
	{	//If there is an active CarePackage
		if ( m_CarePackageActive )
		{	//Delete the box
			GetGame().ObjectDelete(DropObject);
			
			//DeleteZombies
			for ( int i = 0; i < Infected.Count(); i++)
			{
				if(Infected[i])
				{	GetGame().ObjectDelete( Infected[i] ); }
			}
			
			//Clear the array
			Infected.Clear();
			
			//No CarePackage active any more
			m_CarePackageActive = false;
		} 
	}
	
	vector ReturnPackageLocation()
	{
		return m_DropLocation;
	}	
	
	bool IsCarePackageActive()
	{
		return m_CarePackageActive;
	}
}#ifdef LBGROUP_SYSTEM
modded class LBGroupUI : UIScripte