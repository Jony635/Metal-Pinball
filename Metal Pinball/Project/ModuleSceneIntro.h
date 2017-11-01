#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"
#include "Animation.h"


class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

	void CheckInputs();
public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> chains;
	p2List<PhysBody*> sensors;
	p2List_item<PhysBody*>* initialchain = nullptr;
	PhysBody* wall_sensor = nullptr;
	PhysBody* wall = nullptr;

	bool sensed;


	SDL_Texture* in_Game=nullptr;
	SDL_Texture* items_tex = nullptr;

	Animation gold_coin_ANIM;
	Animation green_coin_ANIM;
	Animation red_coin_ANIM;
	SDL_Rect ship_sprite;

	uint coin_gold_FX;
	uint coin_red_FX;
	uint coin_green_FX;
	uint launch_FX;
	uint lose_FX;


	p2Point<int> ray;
	bool ray_on;
};
