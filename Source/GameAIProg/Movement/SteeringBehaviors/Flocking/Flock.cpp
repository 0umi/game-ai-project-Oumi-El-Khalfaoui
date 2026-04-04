#include "Flock.h"
#include "FlockingSteeringBehaviors.h"
#include "Shared/ImGuiHelpers.h"
#include "DrawDebugHelpers.h"


Flock::Flock(UWorld* pWorld, TSubclassOf<ASteeringAgent> AgentClass, int FlockSize, float WorldSize, ASteeringAgent* const pAgentToEvade, bool bTrimWorld)
	: pWorld{pWorld}
	, FlockSize{ FlockSize }
	, pAgentToEvade{pAgentToEvade}
{
	Agents.SetNum(FlockSize);

 // TODO: initialize the flock and the memory pool

	// stering behaviors
	pSeekBehavior = std::make_unique<Seek>();
	pWanderBehavior = std::make_unique<Wander>();
	pEvadeBehavior = std::make_unique<Evade>();
	pCohesionBehavior = std::make_unique<Cohesion>(this);
	pSeparationBehavior = std::make_unique<Separation>(this);
	//pVelMatchBehavior = std::make_unique<VelocityMatch>(this);

	// blended
	//priority

	Neighbors.SetNum(FlockSize);

	// spawn
	Agents.SetNum(FlockSize);
	float HalfWorld = WorldSize / 2.f;

	for (int index = 0; index < FlockSize; index++)
	{
		FVector2D RandPos = FVector2D(FMath::RandRange(-HalfWorld, HalfWorld), FMath::RandRange(-HalfWorld, HalfWorld));

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ASteeringAgent* Agent = pWorld->SpawnActor<ASteeringAgent>(AgentClass, FVector(RandPos, 90.f), FRotator::ZeroRotator, SpawnParams);

		if (Agent)
		{
			Agent->SetActorTickEnabled(false);
			Agent->SetSteeringBehavior(pSeparationBehavior.get());
		}

		Agents[index] = Agent;
	}

}

Flock::~Flock()
{
 // TODO: Cleanup any additional data
	for (ASteeringAgent* Agent : Agents)
	{
		if (IsValid(Agent))
		{
			Agent->Destroy();
		}
	}
	Agents.Empty();

}

void Flock::Tick(float DeltaTime)
{
 // TODO: update the flock
	if (IsValid(pAgentToEvade))
	{
		FTargetData EvadeTarget;
		EvadeTarget.Position = pAgentToEvade->GetPosition();
		EvadeTarget.LinearVelocity = pAgentToEvade->GetLinearVelocity();
		pEvadeBehavior->SetTarget(EvadeTarget);
	}

	// TODO: for every agent:
	for (ASteeringAgent* Agent : Agents)
	{
		if (!IsValid(Agent))
		{
			continue;
		}


		// TODO: register the neighbors for this agent (-> fill the memory pool with the neighbors for the currently evaluated agent)
#ifdef GAMEAI_USE_SPACE_PARTITIONING
		FVector2D OldPos = Agent->GetPosition();
		pPartitionedSpace->UpdateAgentCell(*Agent, OldPos);
		pPartitionedSpace->RegisterNeighbors(*Agent, NeighborhoodRadius);

#else
		RegisterNeighbors(Agent);
#endif // !GAMEAI_USE_SPACE_PARTITIONING


		// TODO: update the agent (-> the steeringbehaviors use the neighbors in the memory pool)
	/*	SteeringOutput Cohesion = pCohesionBehavior->CalculateSteering(DeltaTime, *Agent);
		SteeringOutput Separation = pSeparationBehavior->CalculateSteering(DeltaTime, *Agent);
		SteeringOutput Wander = pWanderBehavior->CalculateSteering(DeltaTime, *Agent);

		SteeringOutput final;
		final.LinearVelocity =
			Separation.LinearVelocity * 2.0f +
			Cohesion.LinearVelocity * 1.0f +
			Wander.LinearVelocity * 0.5f;*/

		SteeringOutput Separation = pSeparationBehavior->CalculateSteering(DeltaTime, *Agent);

		SteeringOutput final;
		final.LinearVelocity = Separation.LinearVelocity * 5.0f;

		Agent->AddMovementInput(FVector(final.LinearVelocity, 0.f));
	}
  // TODO: trim the agent to the world
}

void Flock::RenderDebug()
{
 // TODO: Render all the agents in the flock
}

void Flock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize)
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		bool bWindowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Gameplay Programming", &bWindowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Flocking");
		ImGui::Spacing();

  // TODO: implement ImGUI checkboxes for debug rendering here

		ImGui::Text("Behavior Weights");
		ImGui::Spacing();

  // TODO: implement ImGUI sliders for steering behavior weights here
		//End
		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::RenderNeighborhood()
{
 // TODO: Debugrender the neighbors for the first agent in the flock
}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
void Flock::RegisterNeighbors(ASteeringAgent* const pAgent)
{
 // TODO: Implement
	NrOfNeighbors = 0;

	FVector2D AgentPos = pAgent->GetPosition();

	for (ASteeringAgent* Other : Agents)
	{
		if (!IsValid(Other) || Other == pAgent)
		{
			continue;
		}

		float Dist = FVector2D::Distance(AgentPos, Other->GetPosition());

		if (Dist <= NeighborhoodRadius)
		{
			Neighbors[NrOfNeighbors] = Other;
			++NrOfNeighbors;
		}
	}
}
#endif

FVector2D Flock::GetAverageNeighborPos() const
{
	FVector2D avgPosition = FVector2D::ZeroVector;

 // TODO: Implement
#ifdef GAMEAI_USE_SPACE_PARTITIONING
	int NrN = pPartitionedSpace->GetNrOfNeighbors();
	auto const& N = pPartitionedSpace->GetNeighbors();
#else
	int NrN = NrOfNeighbors;
	auto const& N = Neighbors;

#endif // !GAMEAI_USE_SPACE_PARTITIONING

	if (NrN == 0)
	{
		return avgPosition;
	}

	for (int index = 0; index < NrN; index++)
	{
		if (IsValid(N[index]))
		{
			avgPosition += N[index]->GetPosition();
		}
	}


	return avgPosition / static_cast<float>(NrN);
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	FVector2D avgVelocity = FVector2D::ZeroVector;

 // TODO: Implement

	return avgVelocity;
}

void Flock::SetTarget_Seek(FSteeringParams const& Target)
{
 // TODO: Implement
}

