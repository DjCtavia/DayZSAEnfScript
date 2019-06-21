class ActionReadPaper: ActionSingleUseBase
{
	void ActionReadPaper()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_VIEWNOTE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_PRONE;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}

	override string GetText()
	{
		return "#read";
	}

	override void OnExecuteClient( ActionData action_data )
	{
		//display note
		//action_data.m_Player.enterNoteMenuRead = true;
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		Paper paper_item = Paper.Cast(action_data.m_MainItem);
		PaperParams params = new PaperParams(paper_item.m_AdvancedText);
		//WritePaperParams params = new WritePaperParams("", action_data.m_Player.m_penColor,action_data.m_Player.m_handwriting);
		paper_item.RPCSingleParam(ERPCs.RPC_READ_NOTE, params, true);
	}
};