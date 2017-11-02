#include "ModuleMenu.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"

ModuleMenu::ModuleMenu(Application* app, bool start_enabled) : Module(app, start_enabled) {}
ModuleMenu::~ModuleMenu() {}

enum MenuOptions 
{
	NO_OP = -1,
	PLAY,
	SOUND,
	MUSIC,
	MAX_OPT
};

bool ModuleMenu::Start()
{
	menutex = App->textures->Load("Resources/textures/start-screen.png");
	sound_enabled_TEX = App->textures->Load("Resources/textures/sound_enabled.png");
	music_enabled_TEX = App->textures->Load("Resources/textures/music_enabled.png");
	menu_select = App->audio->LoadFx("Resources/audios/fx/menu_select.wav");

	options = new Option[MAX_OPT];

	if (options[MenuOptions::PLAY].rec_body == nullptr)
	{
		options[MenuOptions::PLAY].rec_body = App->physics->CreateRectangle(87 + 241/2, 268 + 11, 241, 22);
		options[MenuOptions::PLAY].rec_body->physics = App->physics;
		options[MenuOptions::SOUND].rec_body = App->physics->CreateRectangle(161 + 50, 322 + 11, 99, 21);
		options[MenuOptions::SOUND].rec_body->physics = App->physics;
		options[MenuOptions::MUSIC].rec_body = App->physics->CreateRectangle(168 + 86/2, 354 + 21/2, 86, 21);
		options[MenuOptions::MUSIC].rec_body->physics = App->physics;
	}

	return true;
}

update_status ModuleMenu::Update()
{
	App->renderer->Blit(menutex, 0, 0);
	CheckInputs();
	OptionEffects();

	return update_status::UPDATE_CONTINUE;
}

bool ModuleMenu::CleanUp()
{
	if( options != nullptr && options[MenuOptions::PLAY].rec_body!=nullptr)
	{
		delete options[MenuOptions::PLAY].rec_body;
		options[MenuOptions::PLAY].rec_body = nullptr;
		
		delete options[MenuOptions::SOUND].rec_body;
		options[MenuOptions::SOUND].rec_body = nullptr;

		delete options[MenuOptions::MUSIC].rec_body;
		options[MenuOptions::MUSIC].rec_body = nullptr;

		delete[] options;
		options = nullptr;
	}
	App->textures->Unload(menutex);
	App->textures->Unload(sound_enabled_TEX);
	App->textures->Unload(music_enabled_TEX);
	
	return true;
}

void ModuleMenu::GoInGame()
{
	App->scene_intro->Enable();
	App->player->Enable();
	this->Disable();
}

void ModuleMenu::CheckInputs()
{
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (options[MenuOptions::PLAY].rec_body->Contains(App->input->GetMouseX(), App->input->GetMouseY()))
		{
			options[MenuOptions::PLAY].chosen = true;
		}
		else if (options[MenuOptions::SOUND].rec_body->Contains(App->input->GetMouseX(), App->input->GetMouseY()))
		{
			options[MenuOptions::SOUND].chosen = !options[MenuOptions::SOUND].chosen;
			App->audio->PlayFx(menu_select);
			fx_enabled = options[MenuOptions::SOUND].chosen;
		}
		else if (options[MenuOptions::MUSIC].rec_body->Contains(App->input->GetMouseX(), App->input->GetMouseY()))
		{
			options[MenuOptions::MUSIC].chosen = !options[MenuOptions::MUSIC].chosen;
		}
	}
}

void ModuleMenu::OptionEffects()
{
	if (options[MenuOptions::SOUND].chosen)
	{
		int x, y;
		options[MenuOptions::SOUND].rec_body->GetPosition(x, y);
		App->renderer->Blit(sound_enabled_TEX, x, y);
	}
	if (options[MenuOptions::MUSIC].chosen)
	{
		int x, y;
		options[MenuOptions::MUSIC].rec_body->GetPosition(x, y);
		App->renderer->Blit(music_enabled_TEX, x, y);
		if (!music_enabled) 
		{
			if (fx_enabled)
				App->audio->PlayFx(menu_select);

			App->audio->PlayMusic("Resources/audios/music/soundtrack.ogg", -1);
			music_enabled = true;
		}
	}
	else
	{
		if (music_enabled)
		{
			App->audio->PauseMusic();
			music_enabled = false;
		}
	}
	if (options[MenuOptions::PLAY].chosen)
	{
		GoInGame();
	}

}