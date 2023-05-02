class ActionUnlockPackage: ActionInteractBase
{
	void ActionUnlockPackage()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
	}

	override string GetText()
	{
		return "#open_with_crowbar";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target) return false;

		CarePackage_base Package;
		if (Class.CastTo(Package, target.GetObject()))
		{
			// Ha a láda zárolva van
			if (Package.IsPackageLocked())
			{
			 	// Ha a játékos crowbar vagy lockpick itemmel rendelkezik
				if (player.HasItemInInventory("Crowbar") || player.HasItemInInventory("Lockpick"))
				{
					return true;
				}
			}
		}

		return false;
	}

	override void OnStartServer(ActionData action_data)
	{
		CarePackage_base Package = CarePackage_base.Cast(action_data.m_Target.GetObject());
		if (Package)
		{
			Package.UnlockPackage();
			return;
		}
	}
};
