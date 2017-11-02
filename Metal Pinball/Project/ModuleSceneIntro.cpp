#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"
#include "ModuleFonts.h"
#include "ModuleMenu.h"

#include<stdio.h>


ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ray_on = false;
	sensed = false;

	//Ship sprite
	ship_sprite = {23, 0, 32, 77};

	// Gold coin animation
	int x = 57, y = 50, w = 23, h = 27;
	int margin = 1;
	for (int i = 0; i < 8; i++) {
		x = (w + margin) * i + 57;
		gold_coin_ANIM.PushBack({ x, y, w, h });
	}
	gold_coin_ANIM.speed = 0.15f;

	//Red coin animation
	red_coin_ANIM.PushBack({170, 38, 14, 11});
	red_coin_ANIM.PushBack({191, 0, 36, 36});
	red_coin_ANIM.speed = 0.07f;

	//Green coin animation
	green_coin_ANIM.PushBack({ 170, 38, 14, 11 });
	green_coin_ANIM.PushBack({ 161, 0, 28, 29 });
	green_coin_ANIM.speed = 0.07f;
	
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	// Camera reset
	App->renderer->camera.x = App->renderer->camera.y = 0;

	// FX
	coin_gold_FX = App->audio->LoadFx("Resources/audios/fx/coin_gold.wav");
	coin_green_FX = App->audio->LoadFx("Resources/audios/fx/coin_green.wav");
	coin_red_FX = App->audio->LoadFx("Resources/audios/fx/coin_red.wav");
	launch_FX = App->audio->LoadFx("Resources/audios/fx/launch.wav");
	lose_FX = App->audio->LoadFx("Resources/audios/fx/lose.wav");
	if (App->menu->music_enabled)
	{
		App->audio->PlayMusic("Resources/audios/music/soundtrack.ogg",-1);
	}
	
	// TEXTURES
	items_tex = App->textures->Load("Resources/textures/items.png");
	in_Game = App->textures->Load("Resources/textures/in-game.png");
	UI_Tex = App->textures->Load("Resources/textures/UI.png");
	

	// SENSORS

		// Killer
		sensors.add(App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT + 50, SCREEN_WIDTH, 50))->data->type=ItemType::KILLER;

		// Greens
		sensors.add(App->physics->CreateCircleSensor(203, 397, 14.5, GREEN));
		sensors.add(App->physics->CreateCircleSensor(106, 310, 14.5, GREEN));
		sensors.add(App->physics->CreateCircleSensor(292, 310, 14.5, GREEN));
		sensors.add(App->physics->CreateCircleSensor(203, 220, 14.5, GREEN));
		sensors.add(App->physics->CreateCircleSensor(140, 25, 14.5, GREEN));
		sensors.add(App->physics->CreateCircleSensor(189, 17, 14.5, GREEN));
		sensors.add(App->physics->CreateCircleSensor(242, 24, 14.5, GREEN));

		// Reds
		sensors.add(App->physics->CreateCircle(121, 136, 18, RED));
		sensors.add(App->physics->CreateCircle(197, 99, 18, RED));
		sensors.add(App->physics->CreateCircle(265, 136, 18, RED));


		// Gold
		sensors.add(App->physics->CreateCircleSensor(60, 227, 12, GOLD));
		sensors.add(App->physics->CreateCircleSensor(327, 149, 12, GOLD));

	// CHAINS:
	int in_game1[126] = {
		375, 741,
		415, 741,
		415, 182,
		406, 151,
		386, 102,
		355, 62,
		308, 27,
		252, 5,
		207, 2,
		155, 8,
		101, 31,
		63, 63,
		34, 99,
		12, 150,
		6, 200,
		10, 253,
		25, 300,
		45, 356,
		62, 411,
		78, 458,
		68, 460,
		60, 464,
		53, 470,
		49, 476,
		46, 482,
		44, 489,
		35, 489,
		22, 493,
		14, 500,
		7, 512,
		5, 692,
		171, 742,
		171, 747,
		169, 960,
		250, 957,
		250, 741,
		369, 705,
		369, 662,
		285, 689,
		280, 686,
		277, 679,
		278, 670,
		367, 643,
		368, 397,
		321, 379,
		355, 278,
		366, 237,
		367, 200,
		365, 174,
		352, 140,
		328, 103,
		304, 80,
		290, 72,
		291, 65,
		305, 73,
		317, 82,
		329, 94,
		343, 110,
		355, 129,
		366, 156,
		374, 182,
		375, 517,
		375, 740
	};
	int in_game2[76] = {
		124, 68,
		104, 82,
		86, 101,
		75, 115,
		61, 141,
		55, 155,
		50, 172,
		47, 187,
		46, 203,
		47, 229,
		49, 241,
		55, 263,
		63, 288,
		70, 309,
		78, 331,
		88, 362,
		99, 396,
		111, 432,
		115, 443,
		118, 445,
		121, 444,
		119, 438,
		114, 422,
		103, 390,
		87, 341,
		69, 285,
		57, 247,
		53, 231,
		53, 219,
		53, 190,
		58, 165,
		65, 145,
		71, 133,
		79, 120,
		90, 106,
		101, 94,
		113, 83,
		127, 73
	};
	int in_game3[96] = {
		100, 167,
		109, 150,
		119, 135,
		133, 119,
		147, 108,
		171, 97,
		193, 91,
		222, 91,
		244, 96,
		265, 104,
		280, 113,
		294, 125,
		306, 139,
		315, 153,
		321, 168,
		325, 181,
		327, 194,
		328, 216,
		327, 225,
		325, 235,
		322, 245,
		317, 244,
		320, 233,
		322, 224,
		322, 212,
		322, 199,
		321, 188,
		317, 174,
		312, 161,
		306, 150,
		299, 139,
		289, 127,
		281, 121,
		269, 113,
		258, 107,
		245, 102,
		231, 98,
		217, 96,
		200, 96,
		181, 99,
		168, 104,
		157, 109,
		148, 114,
		135, 125,
		123, 138,
		114, 151,
		107, 164,
		105, 169
	};
	int in_game4[26] = {
		93, 542,
		93, 607,
		94, 613,
		98, 617,
		103, 619,
		109, 621,
		119, 624,
		126, 625,
		127, 622,
		126, 619,
		118, 599,
		98, 549,
		94, 542
	};
	int in_game5[20] = {
		327, 543,
		317, 568,
		302, 603,
		295, 619,
		294, 622,
		296, 625,
		305, 624,
		320, 619,
		328, 611,
		329, 543
	};
	int in_game6[18] = {
		46, 548,
		45, 662,
		129, 686,
		137, 686,
		138, 682,
		140, 682,
		140, 672,
		53, 644,
		53, 547
	};
	int in_game7[8] = {
		374, 663,
		415, 663,
		415, 677,
		375, 677
	};

	App->player->ChainDR = chains.add(App->physics->CreateChain(0, 0, in_game1, 126, b2_staticBody));
	App->player->ChainUL = chains.add(App->physics->CreateChain(0, 0, in_game2, 76, b2_staticBody));
	App->player->ChainUR = chains.add(App->physics->CreateChain(0, 0, in_game3, 96, b2_staticBody));
	chains.add(App->physics->CreateChain(0, 0, in_game4, 26, b2_staticBody));
	chains.add(App->physics->CreateChain(0, 0, in_game5, 20, b2_staticBody));
	App->player->ChainDL = chains.add(App->physics->CreateChain(0, 0, in_game6, 18, b2_staticBody));
	initialchain= chains.add(App->physics->CreateChain(0, 0, in_game7, 8, b2_staticBody));
	initialchain->data->type = INITIAL_CHAIN;
  
	// UI FONTS
	score_font = App->fonts->Load("Resources/fonts/score.png", "1234567890", 1);
	lifes_font = App->fonts->Load("Resources/fonts/lives.png", "0123", 1);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	
	//UnLoading Textures
	App->textures->Unload(items_tex);
	App->textures->Unload(in_Game);
	App->textures->Unload(UI_Tex);

	//Freeing Lists (Not physbodys, those will be deleted when App->physics->world does
	sensors.clear();
	chains.clear();

	//Freeing fonts
	App->fonts->UnLoad(score_font);
	App->fonts->UnLoad(lifes_font);

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	CheckInputs();

	//BLITS----------------------------------------------------------------------

		//Blit screem
	App->renderer->Blit(in_Game, 0, 0);

	//Blit ship
	App->renderer->Blit(items_tex, 379, 663, &ship_sprite, 1.0f);

	//Blit gold coins
	SDL_Rect gold_coin_currFrame = gold_coin_ANIM.GetCurrentFrame();
	App->renderer->Blit(items_tex, 60, 225, &gold_coin_currFrame);
	App->renderer->Blit(items_tex, 327, 147, &gold_coin_currFrame);

	//Blit red coins
	SDL_Rect red_coin_currFrame = red_coin_ANIM.GetCurrentFrame();
	App->renderer->Blit(items_tex, 121, 135, &red_coin_currFrame);
	App->renderer->Blit(items_tex, 197, 98, &red_coin_currFrame);
	App->renderer->Blit(items_tex, 265, 135, &red_coin_currFrame);

	//Blit green coins
	SDL_Rect green_coin_currFrame = green_coin_ANIM.GetCurrentFrame();
	App->renderer->Blit(items_tex, 140, 25, &green_coin_currFrame);
	App->renderer->Blit(items_tex, 190, 16, &green_coin_currFrame);
	App->renderer->Blit(items_tex, 243, 23, &green_coin_currFrame);
	App->renderer->Blit(items_tex, 203, 219, &green_coin_currFrame);
	App->renderer->Blit(items_tex, 107, 309, &green_coin_currFrame);
	App->renderer->Blit(items_tex, 292, 309, &green_coin_currFrame);
	App->renderer->Blit(items_tex, 203, 396, &green_coin_currFrame);

	//Blit UI
	App->renderer->Blit(UI_Tex, 140, 330);

	//Blit wall
	if (wall)
	{
		int x, y;
		wall->GetPosition(x, y);
		SDL_Rect wall_r;
		wall_r.x = 229;
		wall_r.y = 0;
		wall_r.w = 27;
		wall_r.h = 38;
		App->renderer->Blit(items_tex, x-10, y+2, &wall_r);
	}
			
	// Prepare for raycast ------------------------------------------------------
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c;

	c = App->player->balls.getFirst();

	SDL_Rect ball = { 0,0,20,21 };
	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(items_tex, x, y, &ball, 1.0f);
		c = c->next;
	}


	// ray -----------------
	if (ray_on == true)
	{
		fVector destination(mouse.x - ray.x, mouse.y - ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if (normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	if (App->player->score > 99999)
		App->player->score = 99999;

	sprintf_s(score_text, 10, "%7d", App->player->score);
	sprintf_s(lifes_text, 10, "%7d", App->player->lives + 1);

	App->fonts->BlitText(249, 340, score_font, score_text);
	App->fonts->BlitText(266, 365, lifes_font, lifes_text);




	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
}


void ModuleSceneIntro::CheckInputs()
{
}

