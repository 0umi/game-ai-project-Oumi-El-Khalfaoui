#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"


//SEEK
//*******
// TODO: Do the Week01 assignment :^)

SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	UWorld* World = Agent.GetWorld();

	if (!World)
	{
		return Steering;
	}


	Steering.LinearVelocity = Target.Position - Agent.GetPosition();

	// 2D to 3D vector
	//FVector example{ Target.Position, 0.0f };



	// Drawing Debug Shapes
	//FColor green{ 0.f,1.f,0.f,1.f };

	FVector Start = FVector(Agent.GetPosition(), 0.0f);
	FVector End = FVector(Target.Position, 0.0f);

	DrawDebugDirectionalArrow(
		World,
		Start,
		End,
		5.0f,
		FColor::Green,
		false,      
		-1.0f,      
		0,          
		2.0f        
	);

	DrawDebugCircle(
		World,
		End,
		100.f,
		12,
		FColor::Cyan,
		false,
		-1.f,
		0,
		2.f,
		FVector(0, 1, 0),
		FVector(1, 0, 0),
		false
	);

	return Steering;
}

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	
	SteeringOutput Fleeing{};

	UWorld* World = Agent.GetWorld();

	if (!World)
	{
		return Fleeing;
	}

	Fleeing.LinearVelocity = -Target.Position - Agent.GetPosition();

	FVector Start = FVector(Agent.GetPosition(), 0.0f);
	FVector End = FVector(-Target.Position, 0.0f);

	DrawDebugDirectionalArrow(
		World,
		Start,
		End,
		5.0f,
		FColor::Green,
		false,
		-1.0f,
		0,
		2.0f
	);
	
	

	return Fleeing;
}

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Arriving{};
	FVector2D Direction = Target.Position - Agent.GetPosition();

	UWorld* World = Agent.GetWorld();

	if (!World)
	{
		return Arriving;
	}

	FVector Start = FVector(Agent.GetPosition(), 0.0f);
	FVector End = FVector(Target.Position, 0.0f);

	float Distance = Direction.Size();

	float SlowRadius = 500.f;
	float TargetRadius = 100.f;
	
	if (OriginalMaxSpeed < 0.f)
	{
		OriginalMaxSpeed = Agent.GetMaxLinearSpeed();
	}

	if (Distance < TargetRadius)
	{
		Agent.SetMaxLinearSpeed(0.f);
		Arriving.LinearVelocity = FVector2D::ZeroVector;
		return Arriving;
	}

	FVector2D NormalisedDirection = Direction.GetSafeNormal();


	if (Distance < SlowRadius)
	{
		float SpeedScale = (Distance - TargetRadius) / (SlowRadius - TargetRadius);
		Agent.SetMaxLinearSpeed(OriginalMaxSpeed * SpeedScale);
	}
	else {
		//TargetSpeed = Agent.GetMaxLinearSpeed();
		Agent.SetMaxLinearSpeed(OriginalMaxSpeed);
	}

	Arriving.LinearVelocity = NormalisedDirection;

	DrawDebugDirectionalArrow(
		World,
		Start,
		End,
		5.0f,
		FColor::Green,
		false,
		-1.0f,
		0,
		2.0f
	);

	DrawDebugCircle(
		World,
		Start,
		SlowRadius,
		12,
		FColor::Cyan,
		false,
		-1.f,
		0,
		2.f,
		FVector(0, 1, 0),
		FVector(1, 0, 0),
		false
	);

	DrawDebugCircle(
		World,
		Start,
		TargetRadius,
		12,
		FColor::Cyan,
		false,
		-1.f,
		0,
		2.f,
		FVector(0, 1, 0),
		FVector(1, 0, 0),
		false
	);

	DrawDebugCircle(
		World,
		End,
		5.f,
		12,
		FColor::Red,
		false,
		-1.f,
		0,
		2.f,
		FVector(0, 1, 0),
		FVector(1, 0, 0),
		false
	);

	return Arriving;
}

SteeringOutput Face::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Facing{};

	UWorld* World = Agent.GetWorld();

	if (!World)
	{
		return Facing;
	}

	FVector Start = FVector(Agent.GetPosition(), 0.0f);
	FVector End = FVector(Target.Position, 0.0f);



	//Facing.LinearVelocity = Target.Position - Agent.GetPosition();
	FVector2D Direction = Target.Position - Agent.GetPosition();

	//float linearDistance = Direction.Size();

	float targetAngle = FMath::Atan2(Direction.Y, Direction.X);
	float forward = Agent.GetRotation();
	float angularDistance = targetAngle - forward;

	Facing.AngularVelocity = angularDistance * Agent.GetMaxAngularSpeed();


	DrawDebugDirectionalArrow(
		World,
		Start,
		End,
		5.0f,
		FColor::Green,
		false,
		-1.0f,
		0,
		2.0f
	);

	DrawDebugCircle(
		World,
		End,
		5.f,
		12,
		FColor::Red,
		false,
		-1.f,
		0,
		2.f,
		FVector(0, 1, 0),
		FVector(1, 0, 0),
		false
	);



	return Facing;
}
