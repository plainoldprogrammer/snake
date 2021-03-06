#include <stdio.h>
#include <allegro5/allegro.h>

#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

#define FPS 60
#define SNAKE_SIZE 16
#define SCREEN_H 600
#define SCREEN_W 800
#define BRICK_HORIZONTAL_WIDTH_SIZE  SCREEN_W
#define BRICK_HORIZONTAL_HEIGHT_SIZE 32
#define BRICK_VERTICAL_WIDTH_SIZE 32
#define BRICK_VERTICAL_HEIGHT_SIZE SCREEN_H - (BRICK_HORIZONTAL_HEIGHT_SIZE * 2)

enum MKEYS
{
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
};

enum Direction
{
	UP, DOWN, LEFT, RIGHT
};

int init_allegro_resources(ALLEGRO_TIMER * *, ALLEGRO_DISPLAY * *);

int init_game_graphics(ALLEGRO_BITMAP **, ALLEGRO_BITMAP **, ALLEGRO_BITMAP **, ALLEGRO_BITMAP **, ALLEGRO_BITMAP **, ALLEGRO_DISPLAY **, ALLEGRO_TIMER **);

void set_graphics_color(ALLEGRO_BITMAP *, ALLEGRO_BITMAP *, ALLEGRO_BITMAP *, ALLEGRO_BITMAP *, ALLEGRO_BITMAP *);

int create_event_queue(ALLEGRO_EVENT_QUEUE **, ALLEGRO_BITMAP **, ALLEGRO_BITMAP **, ALLEGRO_BITMAP **, ALLEGRO_BITMAP **, ALLEGRO_BITMAP **, ALLEGRO_DISPLAY **, ALLEGRO_TIMER **);

void destroy_resources(ALLEGRO_TIMER *, ALLEGRO_DISPLAY *, ALLEGRO_BITMAP *, ALLEGRO_BITMAP *, ALLEGRO_BITMAP *, ALLEGRO_BITMAP *, ALLEGRO_BITMAP *, ALLEGRO_EVENT_QUEUE *);

int is_snake_collision_with_brick(int, int);

int main(int argc, char **argv)
{
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_BITMAP *snake = NULL;
	ALLEGRO_BITMAP *brick_top = NULL;
	ALLEGRO_BITMAP *brick_bottom = NULL;
	ALLEGRO_BITMAP *brick_left = NULL;
	ALLEGRO_BITMAP *brick_right = NULL;

	float snake_x = SCREEN_W / 2.0 - SNAKE_SIZE / 2.0;
	float snake_y = SCREEN_H / 2.0 - SNAKE_SIZE / 2.0;

	Direction snake_direction = RIGHT;
	Direction key_pressed = RIGHT;
	
	bool redraw = true;
	bool doexit = false;

	init_allegro_resources(&timer, &display);
	init_game_graphics(&snake, &brick_top, &brick_bottom, &brick_left, &brick_right, &display, &timer);
	set_graphics_color(snake, brick_top, brick_bottom, brick_left, brick_right);

	al_set_target_bitmap(al_get_backbuffer(display));

	create_event_queue(&event_queue, &brick_right, &brick_left, &brick_bottom, &brick_top, &snake, &display, &timer);

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();
	al_start_timer(timer);


	/* * * * * * * * * * * * * * * * *
	 *			Game loop start		 *
	 * * * * * * * * * * * * * * * * */
	while (!doexit)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (snake_direction == UP)
		{
			snake_y -= 1.0;
		}
		else if (snake_direction == DOWN)
		{
			snake_y += 1.0;
		}
		else if (snake_direction == LEFT)
		{
			snake_x -= 1.0;
		}
		else if (snake_direction == RIGHT)
		{
			snake_x += 1.0;
		}

		if (ev.type == ALLEGRO_EVENT_TIMER)
		{
			if (key_pressed == UP)
			{
				snake_direction = UP;
			}

			if (key_pressed == DOWN)
			{
				snake_direction = DOWN;
			}

			if (key_pressed == LEFT)
			{
				snake_direction = LEFT;
			}

			if (key_pressed == RIGHT)
			{
				snake_direction = RIGHT;
			}

			redraw = true;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			break;
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_UP:
					key_pressed = UP;
					break;
				case ALLEGRO_KEY_DOWN:
					key_pressed = DOWN;
					break;
				case ALLEGRO_KEY_LEFT:
					key_pressed = LEFT;
					break;
				case ALLEGRO_KEY_RIGHT:
					key_pressed = RIGHT;
					break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch (ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_ESCAPE:
					doexit = true;
					break;
			}
		}


		if (redraw && al_is_event_queue_empty(event_queue))
		{
			is_snake_collision_with_brick(snake_x, snake_y);

			redraw = false;

			al_clear_to_color(al_map_rgb(0, 0, 0));

			al_draw_bitmap(snake, snake_x, snake_y, 0);

			al_draw_bitmap(brick_top, 0, 0, 0);
			al_draw_bitmap(brick_bottom, 0, SCREEN_H - BRICK_HORIZONTAL_HEIGHT_SIZE, 0);
			al_draw_bitmap(brick_left, 0, BRICK_HORIZONTAL_HEIGHT_SIZE, 0);
			al_draw_bitmap(brick_right, SCREEN_W - BRICK_VERTICAL_WIDTH_SIZE, BRICK_HORIZONTAL_HEIGHT_SIZE, 0);

			al_flip_display();
		}
	}
	/* * * * * * * * * * * * * * * * *
	 *			Game loop end		 *
	 * * * * * * * * * * * * * * * * */


	destroy_resources(timer, display, snake, brick_top, brick_bottom, brick_left, brick_right, event_queue);
	LOG(INFO) << "End of the program";

	return 0;
}


int init_allegro_resources(ALLEGRO_TIMER * *timer, ALLEGRO_DISPLAY * *display)
{
	if (!al_init())
	{
		fprintf(stderr, "failed to initialize allegro\n");
	}

	if (!al_install_keyboard())
	{
		fprintf(stderr, "failed to initialize the keyboard\n");
	}

	*timer = al_create_timer(1.0 / FPS);
	if (!timer)
	{
		fprintf(stderr, "failed to create timer");
		return -1;
	}

	*display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display)
	{
		fprintf(stderr, "failed to create display\n");
		// al_destroy_timer(timer);
		return -1;
	}

	LOG(INFO) << "Allegro resources has been initialized correctly";
	return 0;
}


int init_game_graphics(ALLEGRO_BITMAP ** snake, ALLEGRO_BITMAP ** brick_top, ALLEGRO_BITMAP ** brick_bottom, ALLEGRO_BITMAP ** brick_left, ALLEGRO_BITMAP ** brick_right, ALLEGRO_DISPLAY ** display, ALLEGRO_TIMER ** timer)
{
	*snake = al_create_bitmap(SNAKE_SIZE, SNAKE_SIZE);
	if (!(*snake))
	{
		fprintf(stderr, "failed to create snake bitmap\n");
		al_destroy_display(*display);
		al_destroy_timer(*timer);
		return -1;
	}

	*brick_top = al_create_bitmap(BRICK_HORIZONTAL_WIDTH_SIZE, BRICK_HORIZONTAL_HEIGHT_SIZE);
	if (!(*brick_top))
	{
		fprintf(stderr, "failed to create a brick_top bitmap\n");
		al_destroy_bitmap(*snake);
		al_destroy_display(*display);
		al_destroy_timer(*timer);
		return -1;
	}

	*brick_bottom = al_create_bitmap(BRICK_HORIZONTAL_WIDTH_SIZE, BRICK_HORIZONTAL_HEIGHT_SIZE);
	if (!(*brick_bottom))
	{
		fprintf(stderr, "failed to create a brick_bottom bitmap\n");
		al_destroy_bitmap(*brick_top);
		al_destroy_bitmap(*snake);
		al_destroy_display(*display);
		al_destroy_timer(*timer);
		return -1;
	}

	*brick_left = al_create_bitmap(BRICK_VERTICAL_WIDTH_SIZE, BRICK_VERTICAL_HEIGHT_SIZE);
	if (!(*brick_left))
	{
		fprintf(stderr, "failed to create a brick_left bitmap\n");
		al_destroy_bitmap(*brick_bottom);
		al_destroy_bitmap(*brick_top);
		al_destroy_bitmap(*snake);
		al_destroy_display(*display);
		al_destroy_timer(*timer);
		return -1;
	}

	*brick_right = al_create_bitmap(BRICK_VERTICAL_WIDTH_SIZE, BRICK_VERTICAL_HEIGHT_SIZE);
	if (!(*brick_right))
	{
		fprintf(stderr, "failed to create a brick_right bitmap\n");
		al_destroy_bitmap(*brick_left);
		al_destroy_bitmap(*brick_bottom);
		al_destroy_bitmap(*brick_top);
		al_destroy_bitmap(*snake);
		al_destroy_display(*display);
		al_destroy_timer(*timer);
		return -1;
	}

	LOG(INFO) << "Game graphics has been initialized correctly";
}


void set_graphics_color(ALLEGRO_BITMAP *snake, ALLEGRO_BITMAP * brick_top, ALLEGRO_BITMAP * brick_bottom, ALLEGRO_BITMAP * brick_left, ALLEGRO_BITMAP * brick_right)
{
	al_set_target_bitmap(snake);
	al_clear_to_color(al_map_rgb(255, 255, 0));

	al_set_target_bitmap(brick_top);
	al_clear_to_color(al_map_rgb(194, 194, 214));

	al_set_target_bitmap(brick_bottom);
	al_clear_to_color(al_map_rgb(194, 194, 214));

	al_set_target_bitmap(brick_left);
	al_clear_to_color(al_map_rgb(133, 133, 173));

	al_set_target_bitmap(brick_right);
	al_clear_to_color(al_map_rgb(133, 133, 173));
}


int create_event_queue(ALLEGRO_EVENT_QUEUE ** event_queue, ALLEGRO_BITMAP ** brick_right, ALLEGRO_BITMAP ** brick_left, ALLEGRO_BITMAP ** brick_bottom, ALLEGRO_BITMAP ** brick_top, ALLEGRO_BITMAP ** snake, ALLEGRO_DISPLAY ** display , ALLEGRO_TIMER ** timer)
{
	*event_queue = al_create_event_queue();
	if (!event_queue)
	{
		fprintf(stderr, "failed to create event_queue");
		al_destroy_bitmap(*brick_right);
		al_destroy_bitmap(*brick_left);
		al_destroy_bitmap(*brick_bottom);
		al_destroy_bitmap(*brick_top);
		al_destroy_bitmap(*snake);
		al_destroy_display(*display);
		al_destroy_timer(*timer);
		return -1;
	}

	LOG(INFO) << "Event queue initialized correctly";
}


void destroy_resources(ALLEGRO_TIMER * timer, ALLEGRO_DISPLAY * display, ALLEGRO_BITMAP * snake, ALLEGRO_BITMAP * brick_top, ALLEGRO_BITMAP * brick_bottom, ALLEGRO_BITMAP * brick_left, ALLEGRO_BITMAP * brick_right, ALLEGRO_EVENT_QUEUE * event_queue)
{
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_bitmap(snake);
	al_destroy_bitmap(brick_top);
	al_destroy_bitmap(brick_bottom);
	al_destroy_bitmap(brick_left);
	al_destroy_bitmap(brick_right);
	al_destroy_event_queue(event_queue);
	
	LOG(INFO) << "Resources has been destroyed";
}


int is_snake_collision_with_brick(int snake_pos_x1, int snake_pos_y1)
{
	if (snake_pos_y1 <= BRICK_HORIZONTAL_HEIGHT_SIZE)
	{
		LOG(INFO) << "Collision with top brick";
	}
	else if (snake_pos_y1 + SNAKE_SIZE >= SCREEN_H - BRICK_HORIZONTAL_HEIGHT_SIZE)
	{
		LOG(INFO) << "Collision with bottom brick";
	}
	else if (snake_pos_x1 <= BRICK_VERTICAL_WIDTH_SIZE)
	{
		LOG(INFO) << "Collision with left brick";
	}
	else if (snake_pos_x1 + SNAKE_SIZE >= SCREEN_W - BRICK_VERTICAL_WIDTH_SIZE)
	{
		LOG(INFO) << "Collision with right brick";
	}

	LOG(INFO) << "x: " << snake_pos_x1 << "\ty: " << snake_pos_y1;
	return 0;
}

