// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCameraShake.h"

UMyCameraShake::UMyCameraShake()
{
	OscillationDuration = 0.2f;
	
	RotOscillation.Pitch.Amplitude = 0.3f;
	RotOscillation.Pitch.Amplitude = 0.3f;
	
	RotOscillation.Yaw.Amplitude = 0.3f;
	RotOscillation.Yaw.Frequency = 12.0f;
	
	RotOscillation.Roll.Frequency = 0.2f;
	RotOscillation.Pitch.Frequency = 3.0f;

	LocOscillation.Z.Amplitude = 0.6f;
	LocOscillation.Z.Frequency = 12.0f;
}
