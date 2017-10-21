#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModulePhysics.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{


}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");


	texture = App->textures->Load("Resources/texture/items.png");

	lives = 3;

	SpawnBall();


	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	return UPDATE_CONTINUE;
}

void ModulePlayer::SpawnBall() 
{
	balls.add(App->physics->CreateCircle(380, 650, 15));
	balls.getLast()->data->listener = this;
}

void ModulePlayer::OnCollision(PhysBody* bodyA, PhysBody* bodyB) 
{
	switch (bodyB->type)
	{
	case ItemType::RED:
		App->audio->PlayFx(App->scene_intro->coin_red);
		score+= 100;
		break;
	case ItemType::GREEN:
		App->audio->PlayFx(App->scene_intro->coin_green);
		score += 150;
		break;
	case ItemType::GOLD:
		App->audio->PlayFx(App->scene_intro->coin_gold);
		score += 500;
		break;
	case ItemType::KILLER:
		App->audio->PlayFx(App->scene_intro->lose);
		balls.clear();
		break;
	}

}



