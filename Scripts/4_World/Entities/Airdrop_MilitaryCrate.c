modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)     
	{         
		super.RegisterActions(actions);   
		
		actions.Insert(ActionUnlockPackage);         
	}
}

class CarePackage_base : Inventory_Base
{   
	protected Particle m_ParticleEfx;	
	ref EffectSound m_soundeffect;
	bool m_IsLocked = false;
	
	void CarePackage_base()
	{
		EnableSmoke();
		
		RegisterNetSyncVariableBool("m_IsLocked");	
	}	
	
	void ~CarePackage_base()
	{
		DisableSmoke();
	}
	
	void EnableSmoke()
	{
		if (GetGame().IsClient() || !GetGame().IsMultiplayer())
        {
			m_ParticleEfx = Particle.PlayOnObject( ParticleList.PackageSmokeRedYellow, this );
		}
	}
	
	void DisableSmoke()
	{
		if ( this )
			if (GetGame().IsClient() || !GetGame().IsMultiplayer())
			{
				m_ParticleEfx.Stop();
			}
	}
	
	void LockPackage()
	{
		m_IsLocked = true;
		GetInventory().LockInventory(HIDE_INV_FROM_SCRIPT);
		this.SetSynchDirty();
	}

	void UnlockPackage()
	{
		m_IsLocked = false;
		GetInventory().UnlockInventory(HIDE_INV_FROM_SCRIPT);
		this.SetSynchDirty();
	}
	
	bool IsPackageLocked()
	{
		return m_IsLocked;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
        AddAction(ActionUnlockPackage);
		RemoveAction(ActionTakeItem);
        RemoveAction(ActionTakeItemToHands);
	}
	
    override bool CanPutInCargo( EntityAI parent )
    {
        return false;
    }
    
    override bool CanPutIntoHands( EntityAI parent )
	{
		return false;
    }
};

class CarePackage_black: CarePackage_base{};
class CarePackage_red: CarePackage_base{};
class CarePackage_green: CarePackage_base{};
class CarePackage_digital_black: CarePackage_base{};
class CarePackage_typhon: CarePackage_base{};
class CarePackage_blood_moon: CarePackage_base{};