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
	SteeringOutput Steering{};

	int NrNeighbors = pFlock->GetNrOfNeighbors();
	if (NrNeighbors == 0)
	{
		return Steering;
	}

	FVector2D AgentPos = Agent.GetPosition();
	FVector2D TotalForce = FVector2D::ZeroVector;

	auto const& Neighbors = pFlock->GetNeighbors();

	for (int index = 0; index < NrNeighbors; index++)
	{
		if (!Neighbors[index])
		{
			continue;
		}

		FVector2D ToNeighbor = AgentPos - Neighbors[index]->GetPosition();
		float Dist = ToNeighbor.Size();

		if (Dist > KINDA_SMALL_NUMBER)
		{
			float Strength = 1.f / (Dist * Dist);
			TotalForce += ToNeighbor.GetSafeNormal() * Strength;
		}
	}

	Steering.LinearVelocity = TotalForce.GetSafeNormal() * Agent.GetMaxLinearSpeed();

	return Steering;
}



//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput VelocityMatch::CalculateSteering(float deltaT, ASteeringAgent& Agent)
{

	return SteeringOutput();
}
