class FlashbangEffect
{
	protected const float 	ALPHA_MIN = 0;
	protected const float 	ALPHA_MAX = 1;
	
	protected const float 	SOUND_DEFER_TIME = 0.4;			//! SFX will be played 0.5s AFTER VFX 
	protected const float 	ATTENUATION_RESET_TIME_FILTER = 1;	//! reset of attenuation will be called ATTENUATION_RESET_TIME_FILTER seconds before the VFX end
	protected const float 	ATTENUATION_RESET_TIME_TINNITUS = 3;	//! stop of tinnitus will be called ATTENUATION_RESET_TIME_TINNITUS seconds BEFORE the VFX end

	protected float 		m_HitDuration;
	protected float 		m_BreakPoint;
	protected float 		m_TimeActive;
	protected bool			m_Visual;
	
	protected PlayerBase 	m_Player;
	protected EffectSound 	m_FlashbangEffectSound;
	
	protected ref Timer		m_DeferAttenuation;
	
	void FlashbangEffect(PlayerBase player, bool visual = true)
	{
		m_Player = player;
		m_Visual = visual;

		m_HitDuration = 8.0;
		m_BreakPoint = 2.5;
		
		m_FlashbangEffectSound = null;

		//PlaySound();
		m_DeferAttenuation = new Timer;
		m_DeferAttenuation.Run(SOUND_DEFER_TIME, this, "PlaySound", null, false);
	}
	
	void ~FlashbangEffect()
	{
		if( m_Visual )
		{
			ClearVisual();
		}
		
		if( m_Player )
		{
			m_Player.OnPlayerReceiveFlashbangHitEnd();
		}
		
		//StopSound();
		//ResetAttenuationFilter();
		
		if( m_DeferAttenuation.IsRunning() )
		{
			m_DeferAttenuation.Stop();
		}

	}
	
	protected void PlaySound()
	{
		vector pos;

		MiscGameplayFunctions.GetHeadBonePos(m_Player, pos);

		m_FlashbangEffectSound = SEffectManager.CreateSound("Tinnitus_SoundSet", pos);
		m_FlashbangEffectSound.SetParent(m_Player);
		m_FlashbangEffectSound.SetAttachedLocalPos(m_Player.WorldToModel(pos));
		m_FlashbangEffectSound.SetSoundWaveKind(WaveKind.WAVEEFFECTEX);
		m_FlashbangEffectSound.SetSoundFadeIn(4);
		m_FlashbangEffectSound.SetSoundFadeOut(2);
		m_FlashbangEffectSound.SetSoundLoop(true);
		m_FlashbangEffectSound.SoundPlay();
		
		SetAttenuationFilter();
	}

	protected void SetAttenuationFilter()
	{
		if( !m_DeferAttenuation.IsRunning() || m_Player.GetMasterAttenuation() != "FlashbangAttenuation" )
		{
			m_Player.SetMasterAttenuation("FlashbangAttenuation");
		}
	}
	
	protected void ResetAttenuationFilter()
	{
		m_Player.SetMasterAttenuation("");
	}
	
	protected void StopSound()
	{
		m_FlashbangEffectSound.SoundStop();
	}
	
	protected void ClearVisual()
	{
		PPEffects.FlashbangEffect(0);
		PPEffects.UpdateColor();
		PPEffects.SetBlurFlashbang(0);
		//PPEffects.ResetVignette();
		g_Game.SetEVValue(0);
	}
	
	protected void SetVisual(float val)
	{
		PPEffects.FlashbangEffect(val);
		PPEffects.UpdateColor();
		PPEffects.SetBlurFlashbang(val);
		g_Game.SetEVValue(val*5);
		//PPEffects.SetVignette(val, 1, 1, 1);	
	}
	
	void Update(float deltatime)
	{
		if( m_Visual )
		{
			float value;

			if( m_TimeActive <= m_BreakPoint )
			{
				value = 1;
			}
			else
			{
				float tmp_value = Math.InverseLerp(m_HitDuration - m_BreakPoint, m_HitDuration, m_TimeActive);
				value = 1 - tmp_value;
			}

			value = Math.Clamp(value,0,1);
			float val = Math.Lerp(ALPHA_MIN, ALPHA_MAX, value);

			SetVisual(val);
		}

		m_TimeActive += deltatime;

		if(m_TimeActive >= m_HitDuration - ATTENUATION_RESET_TIME_FILTER)
		{
			ResetAttenuationFilter();
		}

		if(m_TimeActive >= m_HitDuration - ATTENUATION_RESET_TIME_TINNITUS)
		{
			StopSound();
		}

		if(m_TimeActive >= m_HitDuration)
		{
			delete this;
		}
	}
}