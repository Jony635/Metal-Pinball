#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModulePhysics.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRender.h"
#include "ModuleMenu.h"

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
	dead = false;
	SpawnBall();

	out_of_balls = App->textures->Load("Resources/textures/out_of_balls.png");
	restart = { 38,279,345,188 };
	
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

	
		
	Flippers.add(App->physics->CreateFlipper(0, 0, flipperLeftDown,16, ChainDL->data));
	Flippers.add(App->physics->CreateFlipper(0, 0, flipperRightDown, 16, ChainDR->data));
	Flippers.add(App->physics->CreateFlipper(0, 0, flipperRightUp, 16, ChainUR->data));
	Flippers.add(App->physics->CreateFlipper(0, 0, flipperLeftUp, 16, ChainUL->data));

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
	
	
	CheckInputs();

	p2List_item<PhysBody*>* iterator = Flippers.getFirst();
	for (int i =0; iterator != nullptr; iterator = iterator->next, ++i) 
	{
		int x, y;
		iterator->data->GetPosition(x, y);
		SDL_Rect rect;
		switch (i)
		{
		case 0:
			rect = { 58,0,57,48 };
			App->renderer->Blit(App->scene_intro->items_tex, 134, 672, &rect,1,iterator->data->GetRotation(), PIXEL_TO_METERS(134), PIXEL_TO_METERS(-672)+9);
			break;
		case 1:
			rect = { 58,0,57,48 };
			App->renderer->Blit(App->scene_intro->items_tex, 282-50, 672, &rect, 1, iterator->data->GetRotation(), PIXEL_TO_METERS(282+50), PIXEL_TO_METERS(-672) + 10, SDL_FLIP_HORIZONTAL);
			break;
		case 2:
			rect = { 117,0,42,37 };
			App->renderer->Blit(App->scene_intro->items_tex, 315-40, 240, &rect, 1, iterator->data->GetRotation(), PIXEL_TO_METERS(315+40), PIXEL_TO_METERS(-247) + 9, SDL_FLIP_HORIZONTAL);
			break;
		case 3:
			rect = { 117,0,42,37 };
			App->renderer->Blit(App->scene_intro->items_tex, 119, 446, &rect, 1, iterator->data->GetRotation(), PIXEL_TO_METERS(119), PIXEL_TO_METERS(-446) + 9);
			break;
		}
	}

	if(!dead)
		CheckDeath();

	if(dead)
		App->renderer->Blit(out_of_balls, 38, 279);

	return UPDATE_CONTINUE;
	
}

void ModulePlayer::SpawnBall() 
{

	balls.add(App->physics->CreateCircle(385, 500, 10, BALL, b2_dynamicBody));
	balls.getLast()->data->listener = this;
	balls.getLast()->data->physics = App->physics;

	//Spawn Wall Sensor
	App->scene_intro->wall_sensor = App->physics->CreateRectangleSensor(285, 40, 7, 40);
	App->scene_intro->wall_sensor->type = ItemType::WALL_SENS;

	start = false;
}

void ModulePlayer::OnCollision(PhysBody* bodyA, PhysBody* bodyB) 
{
	if(bodyA->type==ItemType::BALL)
	{
		switch (bodyB->type)
		{
		case ItemType::RED:
		if (App->menu->fx_enabled)
			App->audio->PlayFx(App->scene_intro->coin_red_FX);
		score+= 100;
		break;
		case ItemType::GREEN:
		if (App->menu->fx_enabled)
			App->audio->PlayFx(App->scene_intro->coin_green_FX);
		score += 50;
		break;
		case ItemType::GOLD:
		if (App->menu->fx_enabled)
			App->audio->PlayFx(App->scene_intro->coin_gold_FX);
		score += 500;
		break;
		case ItemType::KILLER:
			if (App->menu->fx_enabled)
			  App->audio->PlayFx(App->scene_intro->lose_FX);
			App->physics->deletes.add(bodyA);
			App->physics->deletes.add(App->scene_intro->wall);
			balls.clear();
			if (this->lives == 0)
				dead = true;
			break;
		case ItemType::WALL_SENS:
			App->physics->deletes.add(App->scene_intro->wall_sensor);
			App->physics->creates.add(WALL);
			break;
		case ItemType::INITIAL_CHAIN:
			start = true;
			break;
		}
	}
}

void ModulePlayer::CheckInputs()
{
	if (!dead)
	{
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && start)
		{
			p2List_item<PhysBody*>* c = balls.getFirst();
			SDL_Rect ball = { 0,0,20,21 };
			while (c != NULL)
			{
				c->data->body->ApplyLinearImpulse({ 0,-5 }, { 0,10 }, true);
				c = c->next;
				if (App->menu->fx_enabled)
					App->audio->PlayFx(App->scene_intro->launch_FX);
			}

			start = false;
		}

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {

			Flippers.getFirst()->data->body->ApplyLinearImpulse({ 0,10 }, { PIXEL_TO_METERS(12),0 }, true);
			Flippers.getLast()->data->body->ApplyLinearImpulse({ 0,2 }, { PIXEL_TO_METERS(8),0 }, true);
		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {

			Flippers.getFirst()->next->data->body->ApplyLinearImpulse({ 0,-10 }, { PIXEL_TO_METERS(-8),PIXEL_TO_METERS(-2) }, true);
			Flippers.getFirst()->next->next->data->body->ApplyLinearImpulse({ 0,-2 }, { PIXEL_TO_METERS(-8),PIXEL_TO_METERS(-2) }, true);
		}
	}

	if (dead && (App->input->GetMouseX() > restart.x && App->input->GetMouseX() < restart.x + restart.w && App->input->GetMouseY() > restart.y && App->input->GetMouseY() < restart.y + restart.h ) && App->input->GetMouseButton(1)) {
		
		dead = false;
		lives = 3;
		score = 0;
		RespawnCounter = 0.0f;
		SpawnBall();
	}

}

void ModulePlayer::CheckDeath()
{
	if (lives > 0 && balls.count() == 0)
	{
		RespawnCounter += 0.1f;

		if (RespawnCounter >= 1.5f)
		{
			if(!App->GodMode)
				lives--;
			RespawnCounter = 0.0f;
			SpawnBall();
		}
	}
}


