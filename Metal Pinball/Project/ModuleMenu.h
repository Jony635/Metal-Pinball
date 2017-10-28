#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody; 
struct SDL_Texture;

class Option
{
public:
	
	bool chosen;
	PhysBody* rec_body = nullptr;




	Option() : chosen(false){}
};



class ModuleMenu : public Module
{
public:
	ModuleMenu(Application* app, bool start_enabled = true);
	~ModuleMenu();

	bool Start();
	update_status Update();
	bool CleanUp();

	void GoInGame();
	void CheckInputs();
	void OptionEffects();
public:
	SDL_Texture* menutex = nullptr;
	SDL_Texture* sound_enabled = nullptr;
	SDL_Texture* music_enabled = nullptr;

	Option* options = nullptr;


};
