#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput Steering{};

	if (pFlock->GetNrOfNeighbors() == 0)
	{
		return Steering;
	}

	FVector2D AvgPos = pFlock->GetAverageNeighborPos();

	Target.Position = AvgPos;
	Steering = Seek::CalculateSteering(deltaT, pAgent);
	//Steering.LinearVelocity *= 5.f;
	return Steering;
}

//*********************
//SEPARATION (FLOCKING)
SteeringOutput Separation::CalculateSteering(float deltaT, ASteeringAgent& Agent)
{
	return SteeringOutput();
}



//*************************
//VELOCITY MATCH (FLOCKING)

//SteeringOutput VelocityMatch::CalculateSteering(float deltaT, ASteeringAgent& Agent)
//{
//	return SteeringOutput();
//}
