class ArmyParachute_base : Inventory_Base
{    
	ref EffectSound m_soundeffect;
	
	void ArmyParachute_base()
	{
		if ( GetGame().IsClient()  ||  !GetGame().IsMultiplayer() )
        {        
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(InitSound, 15, false);
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(StopSound, 1000, false, "Parachute_loop_SoundSet");
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SwitchSound, 1500, false, "Parachute_loop_SoundSet");
        }
	}
	
	void ~ArmyParachute_base()
	{
		StopSound();
	}
	
	void InitSound()
    {
		if (GetGame().IsClient() || !GetGame().IsMultiplayer())
        {
			//--- Your SoundSet Here, check Config.cpp
			PlaySoundSetLoop( m_soundeffect, "Parachute_Start_SoundSet", 0, 0 );
			m_soundeffect.SetSoundLoop( false );
			m_soundeffect.SetSoundAutodestroy( true );
		}
    }

	void SwitchSound( string soundset ) 
    {
        //--- Stop Current Sound
        StopSound();
		if (GetGame().IsClient() || !GetGame().IsMultiplayer())
        {
			PlaySoundSetLoop( m_soundeffect, soundset, 0, 0 );
			m_soundeffect.SetSoundLoop( true );
			m_soundeffect.SetSoundAutodestroy( true );
		}
    }
    
    void StopSound()
    {
        if (GetGame().IsClient() || !GetGame().IsMultiplayer())
        {
            m_soundeffect.SoundStop();
            m_soundeffect.SetSoundLoop(false);
        }
    }

	override void SetActions()
	{
		super.SetActions();
		
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

class ArmyParachute_green: ArmyParachute_base{};
class ArmyParachute_cadpat: ArmyParachute_base{};
class ArmyParachute_typhon: ArmyParachute_base{};
class ArmyParachute_black_digital: ArmyParachute_base{};