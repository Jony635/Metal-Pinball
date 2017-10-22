#pragma once
#include "Module.h"
#include "p2List.h"
#include "Globals.h"
#include "p2Point.h"

struct SDL_Texture;

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

	void SpawnBall();
	void CheckInputs();
	void CheckDeath();

public:

	p2List<PhysBody*> balls;
	int lives;
	uint score;
	bool start = false;

	float RespawnCounter = 0.0f;
};