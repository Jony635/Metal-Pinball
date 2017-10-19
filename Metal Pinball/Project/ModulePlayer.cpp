#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModulePhysics.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{


}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	texture = App->textures->Load("Resources/texture.png");

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

void ModulePlayer::OnCollision(PhysBody* bodyA, PhysBody* bodyB) {


}


