#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "p2Point.h"
#include "math.h"
#include "ModulePlayer.h"
#include "ModuleSceneIntro.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
	debug = false;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);

	// needed to create joints like mouse joint
	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for(b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if(c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if(pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, ItemType type, b2BodyType b_type)
{
	b2BodyDef body;
	body.type = b_type;
	body.position.Set(PIXEL_TO_METERS(x) + PIXEL_TO_METERS(radius), PIXEL_TO_METERS(y) + PIXEL_TO_METERS(radius));
	if(type==ItemType::BALL)
		body.bullet = true;
	b2Body* b = world->CreateBody(&body);
	
	b2CircleShape circle;
	circle.m_radius = PIXEL_TO_METERS(radius);

	b2FixtureDef fixture;
	fixture.shape = &circle;
	fixture.density = 1.0f;
	if (type == ItemType::BALL)
		fixture.restitution = 0.1f;
	else
		fixture.restitution = 0.5f;
	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->type = type;
	pbody->listener = (Module*)App->scene_intro;
	pbody->physics = this;
	pbody->width = radius;
	pbody->height = radius;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, float32 angle)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = DEGTORAD * angle;

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;


	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;
	pbody->physics = this;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;
	pbody->physics = this;

	return pbody;
}

PhysBody* ModulePhysics::CreateCircleSensor(int x, int y, int radius, ItemType type)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x) + PIXEL_TO_METERS(radius), PIXEL_TO_METERS(y) + PIXEL_TO_METERS(radius));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape circle;
	circle.m_radius = PIXEL_TO_METERS(radius);

	b2FixtureDef fixture;
	fixture.shape = &circle;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->type = type;
	pbody->listener = (Module*)App->scene_intro;
	pbody->physics = this;
	

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size, b2BodyType type)
{
	b2BodyDef body;
	body.type = type;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;
	pbody->physics = this;

	return pbody;
}


update_status ModulePhysics::PostUpdate()
{

	DeleteBodies();

	CreateBodies();

	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	DrawObjects();

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

void PhysBody::GetPosition(int& x, int &y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

PhysBody::~PhysBody()
{
	if (physics != nullptr && this->body != nullptr)
	{
		physics->world->DestroyBody(body);
	}
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx*fx) + (fy*fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if(physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if(physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);

}

PhysBody* ModulePhysics::CreateFlipper(int x, int y, int* points, int size, PhysBody* anchor)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}


	box.Set(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 10.0f;
	

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);


	b2RevoluteJointDef joint;
	joint.bodyA = pbody->body;
	joint.bodyB = anchor->body;
	
//	joint.collideConnected = false;

	joint.enableLimit = true;

	if (anchor == App->player->ChainDL->data) 
	{
		joint.localAnchorA.Set(PIXEL_TO_METERS(134), PIXEL_TO_METERS(672));
		joint.localAnchorB.Set(PIXEL_TO_METERS(134), PIXEL_TO_METERS(672));
		joint.lowerAngle = 0;
		joint.upperAngle = DEGTORAD * 70;
	}

	else if (anchor == App->player->ChainDR->data)
	{
		joint.localAnchorA.Set(PIXEL_TO_METERS(282), PIXEL_TO_METERS(672) + PIXEL_TO_METERS(-3));
		joint.localAnchorB.Set(PIXEL_TO_METERS(282), PIXEL_TO_METERS(672) + PIXEL_TO_METERS(-6));
		joint.lowerAngle = DEGTORAD * -70;
		joint.upperAngle = DEGTORAD * 0;
	}

	else if (anchor == App->player->ChainUL->data)
	{
		joint.localAnchorA.Set(PIXEL_TO_METERS(119), PIXEL_TO_METERS(446));
		joint.localAnchorB.Set(PIXEL_TO_METERS(119), PIXEL_TO_METERS(446));
		joint.lowerAngle = 0;
		joint.upperAngle = DEGTORAD * 70;
	}

	else
	{
		joint.localAnchorA.Set(PIXEL_TO_METERS(327), PIXEL_TO_METERS(250));
		joint.localAnchorB.Set(PIXEL_TO_METERS(325), PIXEL_TO_METERS(247));
		joint.lowerAngle = DEGTORAD * -70;
		joint.upperAngle = DEGTORAD * 0;
	}



	world->CreateJoint(&joint);




	return pbody;

}

void ModulePhysics::DrawObjects()
{
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch (f->GetType())
			{
				// Draw circles ------------------------------------------------
			case b2Shape::e_circle:
			{
				b2CircleShape* shape = (b2CircleShape*)f->GetShape();
				b2Vec2 pos = f->GetBody()->GetPosition();
				App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
			}
			break;

			// Draw polygons ------------------------------------------------
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
				int32 count = polygonShape->GetVertexCount();
				b2Vec2 prev, v;

				for (int32 i = 0; i < count; ++i)
				{
					v = b->GetWorldPoint(polygonShape->GetVertex(i));
					if (i > 0)
						App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

					prev = v;
				}

				v = b->GetWorldPoint(polygonShape->GetVertex(0));
				App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
			}
			break;

			// Draw chains contour -------------------------------------------
			case b2Shape::e_chain:
			{
				b2ChainShape* shape = (b2ChainShape*)f->GetShape();
				b2Vec2 prev, v;

				for (int32 i = 0; i < shape->m_count; ++i)
				{
					v = b->GetWorldPoint(shape->m_vertices[i]);
					if (i > 0)
						App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
					prev = v;
				}

				v = b->GetWorldPoint(shape->m_vertices[0]);
				App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
			}
			break;

			// Draw a single segment(edge) ----------------------------------
			case b2Shape::e_edge:
			{
				b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
				b2Vec2 v1, v2;

				v1 = b->GetWorldPoint(shape->m_vertex0);
				v1 = b->GetWorldPoint(shape->m_vertex1);
				App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
			}
			break;
			}

			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && body_clicked == nullptr && mouse_joint == nullptr) {
				b2Vec2 mouse_position(PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()));
				if (f->TestPoint(mouse_position)) {
					body_clicked = f->GetBody();

					b2MouseJointDef def;
					def.bodyA = ground;
					def.bodyB = body_clicked;
					def.target = mouse_position;
					def.dampingRatio = 0.5f;
					def.frequencyHz = 2.0f;
					def.maxForce = 100.0f * body_clicked->GetMass();

					mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);

					//	body_clicked->SetAwake(true);

				}
			}
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && body_clicked != nullptr)
			{

				b2Vec2 mouse_position(PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()));
				mouse_joint->SetTarget(mouse_position);
				b2Vec2	bodypos = body_clicked->GetPosition();
				App->renderer->DrawLine(METERS_TO_PIXELS(bodypos.x), METERS_TO_PIXELS(bodypos.y), App->input->GetMouseX(), App->input->GetMouseY(), 255, 0, 0);
			}
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP&& body_clicked != nullptr)
			{
				world->DestroyJoint(mouse_joint);
				mouse_joint = nullptr;
				body_clicked = nullptr;

			}
		}
	}
}

void ModulePhysics::DeleteBodies()
{
	p2List_item<PhysBody*>* pbody = deletes.getFirst();
	while (pbody != nullptr)
	{
		if (this->body_clicked == pbody->data->body && body_clicked != nullptr)
		{
			if (mouse_joint != nullptr)
				world->DestroyJoint(mouse_joint);
			mouse_joint = nullptr;
			body_clicked = nullptr;
		}
		if (pbody->data == App->scene_intro->wall)
			App->scene_intro->wall = nullptr;
		pbody->data->~PhysBody();
		pbody = pbody->next;
	}
	deletes.clear();
}

void ModulePhysics::CreateBodies()
{
	for (p2List_item<ItemType>* it_type = creates.getFirst(); it_type != nullptr; it_type = it_type->next)
	{
		switch (it_type->data)
		{
		case ItemType::WALL:
			if (App->scene_intro->wall == nullptr)
				App->scene_intro->wall = CreateRectangle(306, 50, 7, 40, 30);
			break;
		}
	}
	creates.clear();
}