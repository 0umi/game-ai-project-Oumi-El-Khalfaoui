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
		2.f,
		12,
		FColor::Cyan,
		false,
		-1.f,
		0,
		2.f
	);

	return Steering;
}

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	
	SteeringOutput Fleeing{};

	Fleeing.LinearVelocity = -Target.Position - Agent.GetPosition();

	

	return Fleeing;
}

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Arriving{};
	Arriving.LinearVelocity = Target.Position - Agent.GetPosition();
	
	/*float SlowRadius = 5.f;
	float TargetRadius = 1.f;*/


	//FVector2D SlowRadius{ 1.f, 1.f };
	//FVector2D TargetRadius{ .5f, .5f };

	//if (Arriving.LinearVelocity < SlowRadius)
	//{
	//	Agent.SetMaxLinearSpeed(Agent.GetMaxLinearSpeed() / 2.f);

	//}
	//if (Arriving.LinearVelocity < TargetRadius)
	//{
	//	Agent.SetMaxLinearSpeed(0);
	//}
	


	return Arriving;
}
