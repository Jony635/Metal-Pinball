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
	lives = 3;
	SpawnBall();
	
	int flipperRightDown[16]{ 233, 715,
		262, 683,
		281, 671,
		287, 676,
		288, 691,
		274, 701,
		237, 720,
		233, 716 };
	int flipperLeftDown[16]{ 138, 673,
		134, 683,
		138, 697,
		187, 722,
		189, 717,
		178, 702,
		158, 682,
		141, 672 };
	int flipperRightUp[16]{ 315, 243,
		298, 255,
		280, 274,
		279, 280,
		287, 279,
		318, 262,
		322, 252,
		317, 243 };
	int flipperLeftUp[16] = { 121, 443,
		116, 451,
		118, 460,
		154, 479,
		159, 478,
		150, 464,
		131, 446,
		124, 443 };



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
	
	CheckDeath();
	CheckInputs();
	
	return UPDATE_CONTINUE;
	
}

void ModulePlayer::SpawnBall() 
{
	balls.add(App->physics->CreateCircle(380, 650, 10));
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

void ModulePlayer::CheckInputs()
{
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		p2List_item<PhysBody*>* c = balls.getFirst();
		SDL_Rect ball = { 0,0,20,21 };
		while (c != NULL)
		{
			c->data->body->ApplyLinearImpulse({ 0,-1 }, { 0,10 }, true);
			c = c->next;
		}
	}
}

void ModulePlayer::CheckDeath()
{
	if (lives > 0 && balls.count() == 0)
	{
		RespawnCounter += 0.1f;

		if (RespawnCounter >= 1.5f)
		{
			lives--;
			RespawnCounter = 0.0f;
			SpawnBall();
		}
	}
}


