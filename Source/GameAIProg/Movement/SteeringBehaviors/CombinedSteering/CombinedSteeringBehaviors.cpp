
#include "CombinedSteeringBehaviors.h"
#include <algorithm>
#include "../SteeringAgent.h"

BlendedSteering::BlendedSteering(const std::vector<WeightedBehavior>& WeightedBehaviors)
	:WeightedBehaviors(WeightedBehaviors)
{};

//****************
//BLENDED STEERING
SteeringOutput BlendedSteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput BlendedSteering = {};
	// TODO: Calculate the weighted average steeringbehavior

	UWorld* World = Agent.GetWorld();
	FVector AgentPos = FVector(Agent.GetPosition(), 0.f);

	//BlendedSteering.LinearVelocity = FVector2D::ZeroVector;
	//BlendedSteering.AngularVelocity = 0.f;

	float totalWeight = 0.f;

	for (const WeightedBehavior& wb : WeightedBehaviors)
	{
		if (!wb.pBehavior || wb.Weight <= KINDA_SMALL_NUMBER)
		{
			continue;
		}

		SteeringOutput steering = wb.pBehavior->CalculateSteering(DeltaT, Agent);
		if (!steering.IsValid)
		{
			continue;
		}

		BlendedSteering.LinearVelocity += steering.LinearVelocity * wb.Weight;
		BlendedSteering.AngularVelocity += steering.AngularVelocity * wb.Weight;

		totalWeight += wb.Weight;

		//normalise
		if (totalWeight > KINDA_SMALL_NUMBER)
		{
			BlendedSteering.LinearVelocity /= totalWeight;
			BlendedSteering.AngularVelocity /= totalWeight;
		}

		if (Agent.GetDebugRenderingEnabled())
		{
			DrawDebugDirectionalArrow(
				Agent.GetWorld(),
				Agent.GetActorLocation(),
				Agent.GetActorLocation() + FVector{ BlendedSteering.LinearVelocity, 0 } *(Agent.GetMaxLinearSpeed() * DeltaT),
				30.f,
				FColor::Red,
				false,
				-1.f,
				0,
				2.f
			);
		}
	}

	return BlendedSteering;
}

float* BlendedSteering::GetWeight(ISteeringBehavior* const SteeringBehavior)
{
	auto it = find_if(WeightedBehaviors.begin(),
		WeightedBehaviors.end(),
		[SteeringBehavior](const WeightedBehavior& Elem)
		{
			return Elem.pBehavior == SteeringBehavior;
		}
	);

	if(it!= WeightedBehaviors.end())
		return &it->Weight;
	
	return nullptr;
}

//*****************
//PRIORITY STEERING
SteeringOutput PrioritySteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering = {};

	for (ISteeringBehavior* const pBehavior : m_PriorityBehaviors)
	{
		Steering = pBehavior->CalculateSteering(DeltaT, Agent);

		if (Steering.IsValid)
			break;
	}

	//If non of the behavior return a valid output, last behavior is returned
	return Steering;
}