#include "SpacePartitioning.h"
#include "DrawDebugHelpers.h"

// --- Cell ---
// ------------
Cell::Cell(float Left, float Bottom, float Width, float Height)
{
	BoundingBox.Min = { Left, Bottom };
	BoundingBox.Max = { BoundingBox.Min.X + Width, BoundingBox.Min.Y + Height };
}

std::vector<FVector2D> Cell::GetRectPoints() const
{
	const float left = BoundingBox.Min.X;
	const float bottom = BoundingBox.Min.Y;
	const float width = BoundingBox.Max.X - BoundingBox.Min.X;
	const float height = BoundingBox.Max.Y - BoundingBox.Min.Y;

	std::vector<FVector2D> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(UWorld* pWorld, float Width, float Height, int Rows, int Cols, int MaxEntities)
	: pWorld{pWorld}
	, SpaceWidth{Width}
	, SpaceHeight{Height}
	, NrOfRows{Rows}
	, NrOfCols{Cols}
	, NrOfNeighbors{0}
{
	Neighbors.SetNum(MaxEntities);
	
	//calculate bounds of a cell
	CellWidth = Width / Cols;
	CellHeight = Height / Rows;

	// TODO create the cells
	CellOrigin = FVector2D(-SpaceWidth * 0.5f, -SpaceHeight * 0.5f);

	Cells.reserve(NrOfRows * NrOfCols);

	for (int row = 0; row < NrOfRows; row++)
	{
		for (int col = 0; col < NrOfCols; col++)
		{
			float Left = CellOrigin.X + col * CellWidth;
			float Bottom = CellOrigin.Y + row * CellHeight;
			Cells.emplace_back(Left, Bottom, CellWidth, CellHeight);
		}
	}
}

void CellSpace::AddAgent(ASteeringAgent& Agent)
{
	// TODO Add the agent to the correct cell
	int index = PositionToIndex(Agent.GetPosition());
	Cells[index].Agents.push_back(&Agent);
}

void CellSpace::UpdateAgentCell(ASteeringAgent& Agent, const FVector2D& OldPos)
{
	int OldIndex = PositionToIndex(OldPos);
	int NewIndex = PositionToIndex(Agent.GetPosition());

	if (OldIndex != NewIndex)
	{
		Cells[OldIndex].Agents.remove(&Agent);
		Cells[NewIndex].Agents.push_back(&Agent);
	}
}

void CellSpace::RegisterNeighbors(ASteeringAgent& Agent, float QueryRadius)
{
	NrOfNeighbors = 0;
	FVector2D AgentPos = Agent.GetPosition();

	FRect QueryRect;
	QueryRect.Min = { AgentPos.X - QueryRadius, AgentPos.Y - QueryRadius };
	QueryRect.Max = { AgentPos.Y + QueryRadius, AgentPos.Y + QueryRadius };

	for (Cell const& C : Cells)
	{
		if (!DoRectsOverlap(C.BoundingBox, QueryRect))
		{
			continue;
		}

		for (ASteeringAgent* Other : C.Agents)
		{
			if (!IsValid(Other) || Other == &Agent)
			{
				continue;
			}

			float Dist = FVector2D::Distance(AgentPos, Other->GetPosition());

			if (Dist <= QueryRadius)
			{
				Neighbors[NrOfNeighbors] = Other;
				++NrOfNeighbors;
			}
		}
	}
}

void CellSpace::EmptyCells()
{
	for (Cell& c : Cells)
		c.Agents.clear();
}

void CellSpace::RenderCells() const
{
	// TODO Render the cells with the number of agents inside of it
	float Z = 110.f;

	for (Cell const& C : Cells)
	{
		auto Pts = C.GetRectPoints();

		for (int index = 0; index < 4; index++)
		{
			FVector A(Pts[index], Z);
			FVector B(Pts[(index + 1) % 4], Z);
			DrawDebugLine(pWorld, A, B, FColor::Red, false, -1.f, 0, 1.5f);
		}

		int Count = static_cast<int>(C.Agents.size());
		if (Count > 0)
		{
			FVector2D Centre = (C.BoundingBox.Min + C.BoundingBox.Max) * 0.5f;

			DrawDebugString(
				pWorld,
				FVector(Centre, Z + 5.f),
				FString::FromInt(Count),
				nullptr,
				FColor::Yellow,
				0.f,
				false
			);
			
		}
	}
}

int CellSpace::PositionToIndex(FVector2D const & Pos) const
{
	// TODO Calculate the index of the cell based on the position
	int Col = static_cast<int>((Pos.X - CellOrigin.X) / CellWidth);
	int Row = static_cast<int>((Pos.Y - CellOrigin.Y) / CellHeight);

	Col = FMath::Clamp(Col, 0, NrOfCols - 1);
	Row = FMath::Clamp(Row, 0, NrOfRows - 1);

	return Row * NrOfCols + Col;
}

bool CellSpace::DoRectsOverlap(FRect const & RectA, FRect const & RectB)
{
	// Check if the rectangles are separated on either axis
	if (RectA.Max.X < RectB.Min.X || RectA.Min.X > RectB.Max.X) return false;
	if (RectA.Max.Y < RectB.Min.Y || RectA.Min.Y > RectB.Max.Y) return false;
    
	// If they are not separated, they must overlap
	return true;
}