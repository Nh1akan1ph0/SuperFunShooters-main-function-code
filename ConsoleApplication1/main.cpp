// The main class that tests the functioning of the game
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <list>

#include "GameObject.h"
#include "SpaceShip.h"
#include "Bullet.h"
#include "Enemies.h"
#include "Explosion.h"
#include "Background.h"
#include "Globals.h"

bool keys[] = { false, false, false, false, false };
enum KEYS{ UP, DOWN, LEFT, RIGHT, SPACE, ENTER };

//globals
SpaceShip *ship;
std::list<GameObject *> objects;
std::list<GameObject *>::iterator iter;
std::list<GameObject *>::iterator iter2;
Background *titleScreen;
Background *lostScreen;
ALLEGRO_SAMPLE_INSTANCE *songInstance;
ALLEGRO_SAMPLE_INSTANCE *songInstance2;
ALLEGRO_SAMPLE_INSTANCE *songInstance3;

//prototypes
void __cdecl TakeLife();
// void TakeLife() __attribute__((cdecl));
void __cdecl ScorePoint();
// void ScorePoint() __attribute__((cdecl));
void ChangeState(int &state, int newState);

int main(int argc, char **argv)
{
	//==============================================
	//SHELL VARIABLES
	//==============================================
	bool done = false;
	bool render = false;

	float gameTime = 0;
	int frames = 0;
	int gameFPS = 0;
	int direction = 0;
	//==============================================
	//PROJECT VARIABLES
	//==============================================
	ship = new SpaceShip();

	int state = -1;

	ALLEGRO_BITMAP *shipImage = NULL;
	ALLEGRO_BITMAP *EnemiesImage = NULL;
	ALLEGRO_BITMAP *explImage = NULL;
	ALLEGRO_BITMAP *bgImage = NULL;
	ALLEGRO_BITMAP *mgImage = NULL;
	ALLEGRO_BITMAP *fgImage = NULL;
	ALLEGRO_BITMAP *titleImage = NULL;
	ALLEGRO_BITMAP *lostImage = NULL;
	ALLEGRO_SAMPLE *shot = NULL;
	ALLEGRO_SAMPLE *boom = NULL;
	ALLEGRO_SAMPLE *song = NULL;
	ALLEGRO_SAMPLE *start = NULL;
	ALLEGRO_SAMPLE *over = NULL;
	ALLEGRO_SAMPLE *life = NULL;
	ALLEGRO_SAMPLE *medium = NULL;
	ALLEGRO_SAMPLE *hard = NULL;
	//==============================================
	//ALLEGRO VARIABLES
	//==============================================
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue2 = NULL;
	ALLEGRO_TIMER *timer, *t2;
	ALLEGRO_FONT *font18;

	//==============================================
	//ALLEGRO INIT FUNCTIONS
	//==============================================
	if (!al_init())										//initialize Allegro
		return -1;

	display = al_create_display(WIDTH, HEIGHT);			//create our display object

	if (!display)										//test display object
		return -1;

	//==============================================
	//ADDON INSTALL
	//==============================================
	al_install_keyboard();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();
	al_install_audio();
	al_init_acodec_addon();


	//==============================================
	//PROJECT INIT
	//==============================================
	font18 = al_load_font("arial.ttf", 18, 0);
	al_reserve_samples(15);

	bgImage = al_load_bitmap("starBG.png");
	mgImage = al_load_bitmap("background.png");
	al_convert_mask_to_alpha(mgImage, al_map_rgb(255, 255, 255));
	fgImage = al_load_bitmap("starFG.png");

	Background *bg = new Background(bgImage, 1);
	objects.push_back(bg);

	bg = new Background(mgImage, 3);
	objects.push_back(bg);

	bg = new Background(fgImage, 5);
	objects.push_back(bg);


	shipImage = al_load_bitmap("s2.png");
	al_convert_mask_to_alpha(shipImage, al_map_rgb(255, 255, 255));
	ship->Init(shipImage);

	objects.push_back(ship);

	EnemiesImage = al_load_bitmap("skulls.png");
	al_convert_mask_to_alpha(EnemiesImage, al_map_rgb(255, 255, 255));
	explImage = al_load_bitmap("explosion_3_40_128.png");

	titleImage = al_load_bitmap("welcome.png");
	lostImage = al_load_bitmap("end.png");

	titleScreen = new Background(titleImage, 0);
	lostScreen = new Background(lostImage, 0);

	shot = al_load_sample("shoot.ogg");
	boom = al_load_sample("eexplosion.ogg");
	song = al_load_sample("easy.ogg");
	start = al_load_sample("levelstarting.ogg");
	over = al_load_sample("gameover.ogg");
	life = al_load_sample("fexplosion.ogg");

	int flag = 0;

	songInstance = al_create_sample_instance(song);

	al_set_sample_instance_playmode(songInstance, ALLEGRO_PLAYMODE_LOOP);

	al_attach_sample_instance_to_mixer(songInstance, al_get_default_mixer());


	al_play_sample(start, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
	ChangeState(state, TITLE);

	srand(time(NULL));
	int random = 0;
	int level = 0;
	//==============================================
	//TIMER INIT AND STARTUP
	//==============================================
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / 60);

	event_queue2 = al_create_event_queue();
	t2 = al_create_timer(0.5);

	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue2, al_get_timer_event_source(t2));

	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_start_timer(timer);
	al_start_timer(t2);

	gameTime = al_current_time();
	while (!done)
	{
		ALLEGRO_EVENT ev, ev2;
		al_wait_for_event(event_queue, &ev);

		//==============================================
		//INPUT
		//==============================================
		if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = true;
				break;
			case ALLEGRO_KEY_UP:
				keys[UP] = true;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = true;
				break;
			case ALLEGRO_KEY_SPACE:
				keys[SPACE] = true;
				if (state == PLAYING)
				{
					direction = 1;
					Bullet *bullet = new Bullet(ship->GetX() + 17, ship->GetY(), direction, &ScorePoint);
					objects.push_back(bullet);
					al_play_sample(shot, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
				}
				break;
			case ALLEGRO_KEY_ENTER:
				if (state == TITLE)
					ChangeState(state, PLAYING);
				else if (state == LOST)
					ChangeState(state, PLAYING);
				break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = false;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = false;
				break;
			case ALLEGRO_KEY_UP:
				keys[UP] = false;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = false;
				break;
			case ALLEGRO_KEY_SPACE:
				keys[SPACE] = false;
				break;
			}
		}
		//==============================================
		//GAME UPDATE
		//==============================================
		else if (ev.type == ALLEGRO_EVENT_TIMER)
		{
			render = true;

			//UPDATE FPS===========
			frames++;
			if (al_current_time() - gameTime >= 1)
			{
				gameTime = al_current_time();
				gameFPS = frames;
				frames = 0;
			}
			//=====================

			if (state == PLAYING)
			{
				if (keys[UP])
					ship->MoveUp();
				else if (keys[DOWN])
					ship->MoveDown();
				else
					ship->ResetAnimation(1);
				if (keys[LEFT])
					ship->MoveLeft();
				else if (keys[RIGHT])
					ship->MoveRight();
				else
					ship->ResetAnimation(0);
				if (ship->GetScore() <= 20)    //level 1
				{
					level = 1;
					random = 100;
				}
				else if (ship->GetScore() > 20 && ship->GetScore() <= 50)           //level2
				{
					level = 2;
					random = 40;
				}
				else if (level == 2)
				{
					EnemiesImage = al_load_bitmap("bat.png");
					al_convert_mask_to_alpha(EnemiesImage, al_map_rgb(255, 255, 255));
					level = 0;
				}
				else if (ship->GetScore() > 50 && ship->GetScore() <= 100)           //level3
				{
					level = 3;
					random = 30;
				}
				else if (level == 3)
				{
					EnemiesImage = al_load_bitmap("dragooon.png");
					al_convert_mask_to_alpha(EnemiesImage, al_map_rgb(255, 255, 0));
					level = 0;
				}
				else if (ship->GetScore() > 200)
				{
					random = 15;   //insane difficulty
					level = 4;
				}




				if (rand() % random == 0)
				{
					Enemies *enemies = new Enemies(WIDTH, 30 + rand() % (HEIGHT - 60), EnemiesImage, &TakeLife);
					objects.push_back(enemies);
					if (level > 2)
					{
						al_wait_for_event(event_queue2, &ev2);
						if (ev2.type == ALLEGRO_EVENT_TIMER)
						{
							direction = -1;
							Bullet *bullet = new Bullet(WIDTH - 30, 30 + rand() % (HEIGHT - 60), direction, &ScorePoint);
							objects.push_back(bullet);
							al_play_sample(shot, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
						}
					}
				}


				//update
				for (iter = objects.begin(); iter != objects.end(); ++iter)
					(*iter)->Update();

				//collisions
				for (iter = objects.begin(); iter != objects.end(); ++iter)
				{
					if (!(*iter)->Collidable()) continue;

					for (iter2 = iter; iter2 != objects.end(); ++iter2)
					{
						if (!(*iter2)->Collidable()) continue;
						if ((*iter)->GetID() == (*iter2)->GetID()) continue;

						if ((*iter)->CheckCollisions((*iter2)))
						{
							(*iter)->Collided((*iter2)->GetID());
							(*iter2)->Collided((*iter)->GetID());

							Explosion *explosion = new Explosion(((*iter)->GetX() + (*iter2)->GetX()) / 2,
								((*iter)->GetY() + (*iter2)->GetY()) / 2, explImage);

							objects.push_back(explosion);
							al_play_sample(boom, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
						}
					}
				}

				if (ship->GetLives() <= 0){
					al_play_sample(over, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
					ChangeState(state, LOST);
				}
			}

			//cull the dead
			for (iter = objects.begin(); iter != objects.end();)
			{
				if (!(*iter)->GetAlive())
				{
					delete (*iter);
					iter = objects.erase(iter);
				}
				else
					iter++;
			}

		}

		//==============================================
		//RENDER
		//==============================================
		if (render && al_is_event_queue_empty(event_queue))
		{
			render = false;
			al_draw_textf(font18, al_map_rgb(150, 0, 0), 550, 5, 0, "FPS: %i", gameFPS);	//display FPS on screen

			//BEGIN PROJECT RENDER================
			if (state == TITLE)
			{
				titleScreen->Render();
			}
			else if (state == PLAYING)
			{
				al_draw_textf(font18, al_map_rgb(255, 255, 255), 5, 5, 0, "Lives: %i            Score: %i points            Level: %i",
					ship->GetLives(), ship->GetScore(), level);
				for (iter = objects.begin(); iter != objects.end(); ++iter)
					(*iter)->Render();
			}
			else if (state == LOST)
			{

				lostScreen->Render();
			}

			//FLIP BUFFERS========================
			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
		}

	}

	//==============================================
	//DESTROY PROJECT OBJECTS
	//==============================================
	for (iter = objects.begin(); iter != objects.end();)
	{
		(*iter)->Destroy();
		delete (*iter);
		iter = objects.erase(iter);
	}

	titleScreen->Destroy();
	lostScreen->Destroy();
	delete titleScreen;
	delete lostScreen;

	al_destroy_bitmap(EnemiesImage);
	al_destroy_bitmap(shipImage);
	al_destroy_bitmap(explImage);
	al_destroy_bitmap(bgImage);
	al_destroy_bitmap(mgImage);
	al_destroy_bitmap(fgImage);
	al_destroy_bitmap(titleImage);
	al_destroy_bitmap(lostImage);
	al_destroy_sample(shot);
	al_destroy_sample(boom);
	al_destroy_sample(song);
	al_destroy_sample_instance(songInstance);

	//SHELL OBJECTS=================================
	al_destroy_font(font18);
	al_destroy_timer(timer);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);

	return 0;
}


void __cdecl TakeLife()
{
	ship->LoseLife();
}

void __cdecl ScorePoint()
{
	ship->AddPoint();
}

void ChangeState(int &state, int newState)
{
	if (state == TITLE)
	{
	}
	else if (state == PLAYING)
	{
		for (iter = objects.begin(); iter != objects.end(); ++iter)
		{
			if ((*iter)->GetID() != PLAYER && (*iter)->GetID() != MISC)
				(*iter)->SetAlive(false);
		}

		al_stop_sample_instance(songInstance);
	}
	else if (state == LOST)
	{
	}

	state = newState;

	if (state == TITLE)
	{
	}
	else if (state == PLAYING)
	{
		ship->Init();
		al_play_sample_instance(songInstance);
	}
	else if (state == LOST)
	{
	}
}