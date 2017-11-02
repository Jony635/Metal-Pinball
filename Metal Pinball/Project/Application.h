#pragma once

#include "p2List.h"
#include "Globals.h"

class Module;
class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleAudio;
class ModulePlayer;
class ModuleSceneIntro;
class ModuleMenu;
class ModulePhysics;
class ModuleFonts;

class Application
{
public:
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleTextures* textures;
	ModuleInput* input;
	ModuleAudio* audio;
	ModulePlayer* player;
	ModuleSceneIntro* scene_intro;
	ModuleMenu* menu;
	ModulePhysics* physics;
	ModuleFonts* fonts;
	

private:

	p2List<Module*> list_modules;

public:

	Application(int argc, char ** argv);
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	bool GodMode = false;
private:

	void AddModule(Module* mod);
};