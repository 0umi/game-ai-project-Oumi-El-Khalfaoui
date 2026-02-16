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

	float targetAngle = FMath::RadiansToDegrees(atan2f(Direction.Y, Direction.X));
	float forward = Agent.GetRotation();
	float angularDistance = targetAngle - forward;

	while (angularDistance > 180.f) {
		angularDistance -= 360.f;

	}
	while (angularDistance < -180.f) {
		angularDistance += 360.f;

	}

	float angularThreshold = 2.f;

	if (fabsf(angularDistance) < angularThreshold)
	{
		Facing.AngularVelocity = 0.f;
	}
	else
	{
		Facing.AngularVelocity = FMath::Sign(angularDistance) * Agent.GetMaxAngularSpeed();

	}

	Facing.LinearVelocity = FVector2D::ZeroVector;

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

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	UWorld* World = Agent.GetWorld();
	if (!World)
	{
		return SteeringOutput{};
	}

	FVector2D ToTarget = Target.Position - Agent.GetPosition();
	float distance = ToTarget.Size();

	float pursuerSpeed = Agent.GetMaxLinearSpeed();

	if (pursuerSpeed <= KINDA_SMALL_NUMBER)
		return SteeringOutput{};

	// t = d/v {Predicted time = distance (between target and agent) / pursuer's speed
	float predictionTime = distance / pursuerSpeed;

	// d = v * t
	FVector2D predictedPosition = Target.Position + (Target.LinearVelocity * predictionTime);

	
	FTargetData originalTarget = Target;
	Target.Position = predictedPosition;

	SteeringOutput result = Seek::CalculateSteering(DeltaT, Agent);

	Target = originalTarget;



	FVector Start = FVector(Agent.GetPosition(), 0.0f);
	FVector CurrentTargetPos = FVector(originalTarget.Position, 0.0f);
	FVector PredictedPos = FVector(predictedPosition, 0.0f);

	DrawDebugDirectionalArrow(
		World,
		Start,
		CurrentTargetPos,
		5.0f,
		FColor::Yellow,
		false,
		-1.0f,
		0,
		2.0f
	);

	DrawDebugDirectionalArrow(
		World,
		Start,
		PredictedPos,
		5.0f,
		FColor::Green,
		false,
		-1.0f,
		0,
		3.0f 
	);

	DrawDebugCircle(
		World,
		PredictedPos,
		5.f,
		12,
		FColor::Magenta,
		false,
		-1.f,
		0,
		3.f,
		FVector(0, 1, 0),
		FVector(1, 0, 0),
		false
	);

	FVector VelocityEnd = CurrentTargetPos + FVector(Target.LinearVelocity * predictionTime, 0.0f);
	DrawDebugDirectionalArrow(
		World,
		CurrentTargetPos,
		VelocityEnd,
		3.0f,
		FColor::Cyan,
		false,
		-1.0f,
		0,
		1.5f
	);

	return result;
}

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	UWorld* World = Agent.GetWorld();
	if (!World)
	{
		return SteeringOutput{};
	}

	FVector2D ToTarget = Target.Position - Agent.GetPosition();
	float distance = ToTarget.Size();

	float evaderSpeed = Agent.GetMaxLinearSpeed();

	if (evaderSpeed <= KINDA_SMALL_NUMBER)
		return SteeringOutput{};

	float threatSpeed = Target.LinearVelocity.Size();
	float predictionTime;

	if (threatSpeed < KINDA_SMALL_NUMBER)
	{
		predictionTime = 0.f;
	}
	else
	{
		predictionTime = distance / evaderSpeed;
		predictionTime = FMath::Min(predictionTime, 1.5f);
	}

	FVector2D predictedPosition = Target.Position + (Target.LinearVelocity * predictionTime);

	FVector2D predictionOffset = predictedPosition - Target.Position;
	float predictionDistance = predictionOffset.Size();
	float maxPredictionDistance = distance * 2.0f;

	if (predictionDistance > maxPredictionDistance)
	{
		predictedPosition = Target.Position + predictionOffset.GetSafeNormal() * maxPredictionDistance;
	}

	FTargetData originalTarget = Target;
	Target.Position = predictedPosition;

	SteeringOutput result = Flee::CalculateSteering(DeltaT, Agent);

	Target = originalTarget;

	// Debug
	FVector Start = FVector(Agent.GetPosition(), 0.0f);
	FVector CurrentThreatPos = FVector(originalTarget.Position, 0.0f);
	FVector PredictedPos = FVector(predictedPosition, 0.0f);

	DrawDebugDirectionalArrow(
		World,
		Start,
		CurrentThreatPos,
		5.0f,
		FColor::Yellow,
		false,
		-1.0f,
		0,
		2.0f
	);

	DrawDebugDirectionalArrow(
		World,
		Start,
		Start + FVector(result.LinearVelocity.GetSafeNormal() * 200.f, 0.f),
		5.0f,
		FColor::Red,
		false,
		-1.0f,
		0,
		3.0f
	);

	DrawDebugCircle(
		World,
		PredictedPos,
		10.f,
		12,
		FColor::Magenta,
		false,
		-1.f,
		0,
		3.f,
		FVector(0, 1, 0),
		FVector(1, 0, 0),
		false
	);

	FVector VelocityEnd = CurrentThreatPos + FVector(Target.LinearVelocity * predictionTime, 0.0f);
	DrawDebugDirectionalArrow(
		World,
		CurrentThreatPos,
		VelocityEnd,
		3.0f,
		FColor::Cyan,
		false,
		-1.0f,
		0,
		1.5f
	);

	return result;
}

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Wandering{};
	return Wandering;
}
