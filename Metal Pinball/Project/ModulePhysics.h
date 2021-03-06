#pragma once
#include "Module.h"
#include "Globals.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum ItemType 
{
	NO_TYPE = -1,

	RED,
	GREEN,
	GOLD,
	BALL,
	KILLER,
	WALL_SENS,
	WALL,

	INITIAL_CHAIN
};

class ModulePhysics;

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{}
	~PhysBody();
	
	void GetPosition(int& x, int &y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
	int width, height;
	b2Body* body;
	Module* listener;
	ModulePhysics* physics = nullptr;
	ItemType type = NO_TYPE;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener // TODO
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	PhysBody* CreateCircle(int x, int y, int radius, ItemType type = ItemType::NO_TYPE, b2BodyType b_type = b2_staticBody);
	PhysBody* CreateRectangle(int x, int y, int width, int height, float32 angle=0);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height);
	PhysBody* CreateCircleSensor(int x, int y, int radius, ItemType type = ItemType::NO_TYPE);
	PhysBody* CreateChain(int x, int y, int* points, int size, b2BodyType type= b2_dynamicBody);
	PhysBody* CreateFlipper(int x, int y, int* points, int size, PhysBody* anchor);
	// b2ContactListener ---
	void BeginContact(b2Contact* contact);
	
	b2World* world;

	p2List<PhysBody*> deletes;
	p2List<ItemType> creates;

private:

	bool debug;
	
	b2MouseJoint* mouse_joint;
	b2Body* ground;
	b2Body* body_clicked = nullptr;

	void DrawObjects();
	void DeleteBodies();
	void CreateBodies();
};