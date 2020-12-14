#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class JizzMate : public olc::PixelGameEngine
{
public:
	JizzMate()
	{
		sAppName = "The Dissolution of Jizzmate";
	}

private:
	//Grid variables
	olc::vi2d vBlockSize = { 32, 32 };
	std::array<std::array<int, 11>, 11> roomStructure = { {0} };
	std::unique_ptr<olc::Sprite> sprTile;

	//Unique variables
	float fFriction = -400.0f;

	//Player variables
	olc::vf2d vPlayerPosition = { 32.0f, 160.0f };
	olc::vf2d vPlayerDir;
	olc::vf2d vPlayerDirState;
	olc::vf2d vPlayerSize = { 24.0f, 24.0f };
	float fPlayerRadius = vPlayerSize.x / 2;
	std::unique_ptr<olc::Sprite> sprPlayer;

public:
	bool OnUserCreate() override
	{
		srand(time(NULL));
		//Create room structure
		for (int x = 0; x < 11; x++)
		{
			for (int y = 0; y < 11; y++)
			{
				if ((x == 0 || x == 10) && (y != 5)) roomStructure[x][y] = 1;
				else if ((x == 0 || x == 10) && (y == 5)) roomStructure[x][y] = 2;
				else if (y == 0 || y == 10) roomStructure[x][y] = 1;
			}
		}
		
		//Create random pillars
		for (int j = 0; j < rand() % 10 + 3; j++) 
		{
			retry:
			int xTemp = rand() % 9 + 1;
			int yTemp = rand() % 9 + 1;
			if ((roomStructure[xTemp][yTemp] != 0) || (xTemp == 1 && yTemp == 5) || (xTemp == 9 && yTemp == 5)) goto retry;
			else roomStructure[xTemp][yTemp] = 3;
		}
		
		//Load tile textures
		sprTile = std::make_unique<olc::Sprite>("./spriteTiles.png");

		//Load player textures
		sprPlayer = std::make_unique<olc::Sprite>("./spritePlayer.png");
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		SetPixelMode(olc::Pixel::ALPHA);
		//Draw room
		Clear(olc::BLACK);
		for (int x = 0; x < 11; x++)
		{
			for (int y = 0; y < 11; y++)
			{
				switch (roomStructure[x][y])
				{
				case 0:
					break;
				case 1:
					DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, sprTile.get(), olc::vi2d(0, 0) * vBlockSize, vBlockSize);
					break;
				case 2:
					DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, sprTile.get(), olc::vi2d(1, 0) * vBlockSize, vBlockSize);
					break;
				case 3:
					DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, sprTile.get(), olc::vi2d(2, 0) * vBlockSize, vBlockSize);
					break;
				}
			}
		}

		//Draw player
		//Player movement
		//Check and change velocity for intertia
		if (vPlayerDir.x < 0.0f)
		{
			vPlayerDirState += olc::vf2d(-1.0f, 0.0f);
			vPlayerDir = { vPlayerDir.x * -1.0f, vPlayerDir.y };
		}
		else 
		{
			vPlayerDirState += olc::vf2d(1.0f, 0.0f);
		}
		if (vPlayerDir.y < 0.0f)
		{
			vPlayerDirState += olc::vf2d(0.0f, -1.0f);
			vPlayerDir = { vPlayerDir.x, vPlayerDir.y * -1.0f };
		}
		else
		{
			vPlayerDirState += olc::vf2d(0.0f, 1.0f);
		}

		//Friction (could work with or statement??)
		if ((vPlayerDir.x + fFriction * fElapsedTime) > 0.0f)
			vPlayerDir = { (vPlayerDir.x + fFriction * fElapsedTime) * vPlayerDirState.x, vPlayerDir.y };
		else
			vPlayerDir = { 0.0f, vPlayerDir.y };

		if ((vPlayerDir.y + fFriction * fElapsedTime) > 0.0f)
			vPlayerDir = { vPlayerDir.x, (vPlayerDir.y + fFriction * fElapsedTime) * vPlayerDirState.y };
		else vPlayerDir = { vPlayerDir.x, 0.0f };

		vPlayerDirState = { 0.0f, 0.0f };
		
		//Check for key press
		if (GetKey(olc::Key::W).bHeld)
		{
			vPlayerDir = olc::vf2d(vPlayerDir.x, 0.0f);
			vPlayerDir += olc::vf2d(0.0f, -100.0f);
		}
		if (GetKey(olc::Key::A).bHeld)
		{
			vPlayerDir = olc::vf2d(0.0f, vPlayerDir.y);
			vPlayerDir += olc::vf2d(-100.0f, 0.0f);
		}
		if (GetKey(olc::Key::S).bHeld)
		{
			vPlayerDir = olc::vf2d(vPlayerDir.x, 0.0f);
			vPlayerDir += olc::vf2d(0.0f, 100.0f);
		}
		if (GetKey(olc::Key::D).bHeld)
		{
			vPlayerDir = olc::vf2d(0.0f, vPlayerDir.y);
			vPlayerDir += olc::vf2d(100.0f, 0.0f);
		}

		//Test for collision
		olc::vf2d vPlayerPotentialPos = vPlayerPosition + (vPlayerDir * fElapsedTime);
		olc::vf2d vPlayerPotentialCenter = { vPlayerPotentialPos.x + (vPlayerSize.x / 2), vPlayerPotentialPos.y + (vPlayerSize.y / 2) };
		auto bTestForCollision = [&](const olc::vf2d& point)
		{
			
			olc::vi2d vTestPoint = vPlayerPotentialCenter + point * fPlayerRadius;
			auto& tile = roomStructure[xQuadrant(vTestPoint)][yQuadrant(vTestPoint)];

			if (tile == 0)
			{
				return false;
			}
			else if (tile == 2)
			{
				
				return false;
			}
			else
			{
				return true;
			}
		};

		if (!(bTestForCollision(olc::vf2d(-1, -1)) || bTestForCollision(olc::vf2d(1, -1)) || bTestForCollision(olc::vf2d(-1, 1)) || bTestForCollision(olc::vf2d(1, 1))))
			vPlayerPosition += vPlayerDir * fElapsedTime;
		//Top stick
		if ((bTestForCollision(olc::vf2d(-1, -1)) || bTestForCollision(olc::vf2d(1, -1))) && GetKey(olc::Key::W).bHeld)
		{
			vPlayerPosition += olc::vf2d(vPlayerDir.x * fElapsedTime, 0.0f);
			vPlayerDir = vPlayerDir * olc::vf2d(1.0f, 0.0f);
		}
		//Left stick
		if ((bTestForCollision(olc::vf2d(-1, -1)) || bTestForCollision(olc::vf2d(-1, 1))) && GetKey(olc::Key::A).bHeld)
		{
			vPlayerPosition += olc::vf2d(0.0f, vPlayerDir.y * fElapsedTime);
			vPlayerDir = vPlayerDir * olc::vf2d(0.0f, 1.0f);
		}
		//Right stick
		if ((bTestForCollision(olc::vf2d(-1, 1)) || bTestForCollision(olc::vf2d(1, 1))) && GetKey(olc::Key::S).bHeld)
		{
			vPlayerPosition += olc::vf2d(vPlayerDir.x * fElapsedTime, 0.0f);
			vPlayerDir = vPlayerDir * olc::vf2d(1.0f, 0.0f);
		}
		//Bottom stick
		if ((bTestForCollision(olc::vf2d(1, 1)) || bTestForCollision(olc::vf2d(1, -1))) && GetKey(olc::Key::D).bHeld)
		{
			vPlayerPosition += olc::vf2d(0.0f, vPlayerDir.y * fElapsedTime);
			vPlayerDir = vPlayerDir * olc::vf2d(0.0f, 1.0f);
		}

		DrawPartialSprite(vPlayerPosition, sprPlayer.get(), olc::vi2d(0, 0) * vPlayerSize, vPlayerSize);
		SetPixelMode(olc::Pixel::NORMAL);
		return true;
	}

	int xQuadrant(olc::vf2d input)
	{
		int xTemp = (int)input.x;
		while (xTemp % 32 != 0)
			xTemp--;
		xTemp = xTemp / 32;
		return xTemp;
	}

	int yQuadrant(olc::vf2d input)
	{
		int yTemp = (int)input.y;
		while (yTemp % 32 != 0)
			yTemp--;
		yTemp = yTemp / 32;
		return yTemp;
	}
};

int main()
{
	JizzMate demo;
	if (demo.Construct(352, 352, 2, 2))
		demo.Start();
	return 0;	
}