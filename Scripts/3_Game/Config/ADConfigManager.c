class CarePackagesConfigManager
{
	ref array<ref CPLocation> Locations;
	ref array<ref CPPackage> Packages;
	ref TStringArray InfectedTypes;
	
	int MinutesBetweenPackages, DropHeight, DropTime, PackageCallDelay, MinutesAway;
	int MinimumPlayers, PackagesBeingRan, ZombiesToSpawn, LocationHistoryCheckRange, LootSpawnType;
	
	bool LockPackages;
	
	string Title, DroppedMessage, StartMessage, MinutesAwayPrefix, MinutesAwaySuffix;
	
	
	void CarePackagesConfigManager() 
	{
		Locations = new array<ref CPLocation>; 
		Packages = new array<ref CPPackage>;   
	};
	
	CPLocation ReturnLocation(int index)
	{
		return Locations[index];
	}
	
	CPPackage ReturnPackage(int index)
	{
		return Packages[index];
	}
	
	CPItems ReturnRandomItem(int index)
	{
		return Packages[index].ReturnRandomItem();
	}

	void LoadDefaultSettings() 
    {
		TStringArray TempAttachments;
		TIntArray TempIDs;
		TIntArray TempCoords;
		
		array<ref CPItems> RandomItems1;
		array<ref CPWeapons> RandomWeapons1;
		array<ref CPItems> RandomItems2;
		array<ref CPWeapons> RandomWeapons2;
		RandomItems1 = new array<ref CPItems>; 
		RandomWeapons1 = new array<ref CPWeapons>; 
		RandomItems2 = new array<ref CPItems>; 
		RandomWeapons2 = new array<ref CPWeapons>; 
		
		MinutesBetweenPackages = 31;
		DropHeight = 200;
		DropTime = 15;
		MinimumPlayers = -1;
		PackagesBeingRan = 2;
		PackageCallDelay = 15;
		MinutesAway = 2;
		ZombiesToSpawn = 15;
		LootSpawnType = 0;
		LocationHistoryCheckRange = 3;
		LockPackages = true;
		
		Title = "Care Package"; 
		StartMessage = "A care package is on its way to ";
		MinutesAwayPrefix = "The care package is ";
		MinutesAwaySuffix = " minutes away from ";
		DroppedMessage = "The care package has been deployed";
		
		//Infected types								
		InfectedTypes = {"ZmbM_CitizenASkinny_Blue","ZmbM_CitizenASkinny_Brown","ZmbM_CitizenASkinny_Grey",
						 "ZmbM_MotobikerFat_Beige","ZmbM_MotobikerFat_Black","ZmbM_MotobikerFat_Blue",
						 "ZmbM_JoggerSkinny_Red","ZmbM_SkaterYoung_Blue","ZmbM_SkaterYoung_Brown",
						 "ZmbM_SkaterYoung_Green","ZmbM_SkaterYoung_Grey","ZmbM_ClerkFat_Brown","ZmbM_ClerkFat_Khaki",
						 "ZmbF_JournalistNormal_White","ZmbF_Clerk_Normal_White","ZmbF_CitizenANormal_Blue",
						 "ZmbF_CitizenBSkinny","ZmbF_HikerSkinny_Grey","ZmbF_SurvivorNormal_Orange","ZmbF_HikerSkinny_Green",
						 "ZmbF_JoggerSkinny_Green","ZmbF_SkaterYoung_Striped","ZmbF_BlueCollarFat_Red","ZmbF_MechanicNormal_Beige",
						 "ZmbF_PatientOld","ZmbF_ShortSkirt_beige","ZmbF_VillagerOld_Red","ZmbF_JoggerSkinny_Red",
						 "ZmbF_MilkMaidOld_Beige","ZmbF_VillagerOld_Green","ZmbF_ShortSkirt_yellow","ZmbF_NurseFat","ZmbF_PoliceWomanNormal",
						 "ZmbF_HikerSkinny_Blue","ZmbF_ParamedicNormal_Green","ZmbF_JournalistNormal_Red","ZmbF_SurvivorNormal_White",
						 "ZmbF_JoggerSkinny_Brown","ZmbF_MechanicNormal_Grey","ZmbF_BlueCollarFat_Green","ZmbF_DoctorSkinny"};
		
		TempIDs = {1};
		
		TempCoords = {11930, 12490};
		Locations.Insert(new CPLocation("Krasnostav Airstrip", TempCoords, 50, TempIDs));
		TempCoords = {4500, 10350};
		Locations.Insert(new CPLocation("Northwest Airfield", TempCoords, 50, TempIDs));
		TempCoords = {3448, 13039};
		Locations.Insert(new CPLocation("Novaya Petrovka", TempCoords, 50, TempIDs));
		
		TempIDs = {2};
		TempCoords = {8030, 12646};
		Locations.Insert(new CPLocation("Severograd", TempCoords, 50, TempIDs));
		TempCoords = {6600, 7800};
		Locations.Insert(new CPLocation("Stary/Novy", TempCoords, 50, TempIDs));
		TempCoords = {4555, 8160};
		Locations.Insert(new CPLocation("VMC", TempCoords, 50, TempIDs));
		
		TempAttachments = {"Mag_FAL_20Rnd","Fal_FoldingBttstck","ACOGOptic"};
		RandomWeapons1.Insert(new CPWeapons("FAL", TempAttachments));		
		
		TempAttachments = {"Mag_AKM_Drum75rnd","AK_WoodHndgrd","AK_woodBttStck","KobraOptic"};
		RandomWeapons1.Insert(new CPWeapons("AKM",  TempAttachments));		
		
		TempAttachments = {"Mag_SVD_10rnd","PSO1Optic"};
		RandomWeapons1.Insert(new CPWeapons("SVD", TempAttachments));	
		
		TempAttachments = {};
		RandomItems1.Insert(new CPItems("Mag_AKM_Drum75rnd", 1, 75, TempAttachments));	
		RandomItems1.Insert(new CPItems("Mag_SVD_10rnd", 1, 10, TempAttachments));		
		
		TempIDs = {1};
		
		Packages.Insert(new CPPackage("Military","CarePackage_typhon","ArmyParachute_typhon",RandomItems1, RandomWeapons1, 1, 4, 5, 10, TempIDs));
		
		TempIDs = {2};
		
		TempAttachments = {};
		RandomItems2.Insert(new CPItems("Nail", 17, 69, TempAttachments));
		RandomItems2.Insert(new CPItems("TetracyclineAntibiotics", 4, 10, TempAttachments));
		RandomItems2.Insert(new CPItems("VitaminBottle", 10, 25, TempAttachments));
		
		Packages.Insert(new CPPackage("Medical","CarePackage_red","ArmyParachute_black_digital",RandomItems2,RandomWeapons2, 0, 0, 5, 10, TempIDs));
		
		SaveConfig();
	};
	
	//this saves the config to the json file.
	protected void SaveConfig() 
    {
		if (!FileExist(m_CarePackagesProfileDir + m_CarePackagesProfileFolder + "/"))
			MakeDirectory(m_CarePackagesProfileDir + m_CarePackagesProfileFolder + "/");
		
            JsonFileLoader<CarePackagesConfigManager>.JsonSaveFile(m_CarePackagesConfigPath, this);
	};

	//Dont use that to load the config!
	static CarePackagesConfigManager LoadConfig() 
    {
		CarePackagesConfigManager settings = new CarePackagesConfigManager();

		if(!FileExist(m_CarePackagesProfileFolder))
			MakeDirectory(m_CarePackagesProfileFolder);
		if(FileExist(m_CarePackagesConfigPath))
		{
			JsonFileLoader<CarePackagesConfigManager>.JsonLoadFile(m_CarePackagesConfigPath, settings);
		}
		else
		{
			settings.LoadDefaultSettings();
		}
		return settings;
    }
};

/* Global Getter for config */
static ref CarePackagesConfigManager g_CarePackagesConfig;
static ref CarePackagesConfigManager g_CPClientConfig;

static CarePackagesConfigManager GetCarePackagesConfig()
{
      if (g_Game.IsServer() && !g_CarePackagesConfig) 
      {
            g_CarePackagesConfig = CarePackagesConfigManager.LoadConfig();
      }
      return g_CarePackagesConfig;
};

/* config classes */
class CPLocation
{
	string Name;
	ref TIntArray Location;
	int Accuracy;
	ref TIntArray AllowedPackageIDs;
	
	void CPLocation(string name, TIntArray coordinates, int dropAccuracy, TIntArray AllowedIDs )
	{
		Name = name;
		Location = coordinates;
		Accuracy = dropAccuracy;
		AllowedPackageIDs = AllowedIDs;
	};
	
	vector ReturnCoords()
	{
		return Vector(Location[0],0,Location[1]);
	}
};

class CPPackage
{
	string Package_name;
	string object_type;
	string parachute_type;
	int MinWeapons;
	int MaxWeapons;
	int MinMiscItems;
	int MaxMiscItems;
	ref TIntArray AllowedPackageIDs;
	ref array<ref CPItems> Items;
	ref array<ref CPWeapons> Weapons;

	void CPPackage(string Packagename, string objecttype, string parachutetype, array<ref CPItems> tempitems, array<ref CPWeapons> tempweapons, int minWeapons,	int maxWeapons,	int minMiscItems, int maxMiscItems, TIntArray allowedPackageIDs)
	{
		Items = new array<ref CPItems>;
		Items = tempitems;
		Weapons = new array<ref CPWeapons>;
		Weapons = tempweapons;
		Package_name = Packagename;
		object_type = objecttype;
		parachute_type = parachutetype;
		MinWeapons = minWeapons;
		MaxWeapons = maxWeapons;
		MinMiscItems = minMiscItems;
		MaxMiscItems = maxMiscItems;
		AllowedPackageIDs = allowedPackageIDs;
	}
	
	CPWeapons ReturnRandomWeapon()
	{
		return Weapons.GetRandomElement();
	}
	
	CPItems ReturnRandomItem()
	{
		return Items.GetRandomElement();
	}
};

class CPItems
{
	string Item;
	int MinQty;
	int MaxQty;
	ref TStringArray Attachments;
	
	void CPItems(string name, int minQuantity, int maxQuantity, TStringArray attach)
	{
		Item = name;
		MinQty = minQuantity;
		MaxQty = maxQuantity;
		Attachments = attach;
	};
};

class CPWeapons
{
	string Item;
	ref TStringArray Attachments;
	
	void CPWeapons(string name, TStringArray attach)
	{
		Item = name;
		Attachments = attach;
	};
};