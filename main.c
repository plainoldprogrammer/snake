#include <stdio.h>
#include <allegro5/allegro.h>

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

enum SNAKE_DIRECTION
{
	UP, DOWN, LEFT, RIGHT
};

int initResources(ALLEGRO_TIMER * *, ALLEGRO_DISPLAY * *);

int isSnakeCollisionWithWall(int, int, int, int, int, int, int, int);

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
	bool key[4] = { false, false, false, false };
	bool snake_direction[4] = { false, false, false, false };
	bool redraw = true;
	bool doexit = false;

	initResources(&timer, &display);

	snake = al_create_bitmap(SNAKE_SIZE, SNAKE_SIZE);
	if (!snake)
	{
		fprintf(stderr, "failed to create snake bitmap\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	brick_top = al_create_bitmap(BRICK_HORIZONTAL_WIDTH_SIZE, BRICK_HORIZONTAL_HEIGHT_SIZE);
	if (!brick_top)
	{
		fprintf(stderr, "failed to create a brick_top bitmap\n");
		al_destroy_bitmap(snake);
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	brick_bottom = al_create_bitmap(BRICK_HORIZONTAL_WIDTH_SIZE, BRICK_HORIZONTAL_HEIGHT_SIZE);
	if (!brick_bottom)
	{
		fprintf(stderr, "failed to create a brick_bottom bitmap\n");
		al_destroy_bitmap(brick_top);
		al_destroy_bitmap(snake);
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	brick_left = al_create_bitmap(BRICK_VERTICAL_WIDTH_SIZE, BRICK_VERTICAL_HEIGHT_SIZE);
	if (!brick_left)
	{
		fprintf(stderr, "failed to create a brick_left bitmap\n");
		al_destroy_bitmap(brick_bottom);
		al_destroy_bitmap(brick_top);
		al_destroy_bitmap(snake);
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	brick_right = al_create_bitmap(BRICK_VERTICAL_WIDTH_SIZE, BRICK_VERTICAL_HEIGHT_SIZE);
	if (!brick_right)
	{
		fprintf(stderr, "failed to create a brick_right bitmap\n");
		al_destroy_bitmap(brick_left);
		al_destroy_bitmap(brick_bottom);
		al_destroy_bitmap(brick_top);
		al_destroy_bitmap(snake);
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	al_set_target_bitmap(snake);
	al_clear_to_color(al_map_rgb(102, 255, 51));

	al_set_target_bitmap(brick_top);
	al_clear_to_color(al_map_rgb(194, 194, 214));

	al_set_target_bitmap(brick_bottom);
	al_clear_to_color(al_map_rgb(194, 194, 214));

	al_set_target_bitmap(brick_left);
	al_clear_to_color(al_map_rgb(133, 133, 173));

	al_set_target_bitmap(brick_right);
	al_clear_to_color(al_map_rgb(133, 133, 173));

	al_set_target_bitmap(al_get_backbuffer(display));

	event_queue = al_create_event_queue();
	if (!event_queue)
	{
		fprintf(stderr, "failed to create event_queue");
		al_destroy_bitmap(brick_right);
		al_destroy_bitmap(brick_left);
		al_destroy_bitmap(brick_bottom);
		al_destroy_bitmap(brick_top);
		al_destroy_bitmap(snake);
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));

	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_flip_display();

	al_start_timer(timer);

	snake_direction[RIGHT] = true;

	while (!doexit)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (snake_direction[UP] == true)
		{
			snake_y -= 1.0;
		}
		else if (snake_direction[DOWN] == true)
		{
			snake_y += 1.0;
		}
		else if (snake_direction[LEFT] == true)
		{
			snake_x -= 1.0;
		}
		else if (snake_direction[RIGHT] == true)
		{
			snake_x += 1.0;
		}

		if (ev.type == ALLEGRO_EVENT_TIMER)
		{
			if (key[KEY_UP])
			{
				snake_direction[UP] = true;
				snake_direction[DOWN] = false;
				snake_direction[LEFT] = false;
				snake_direction[RIGHT] = false;
			}

			if (key[KEY_DOWN])
			{
				snake_direction[UP] = false;
				snake_direction[DOWN] = true;
				snake_direction[LEFT] = false;
				snake_direction[RIGHT] = false;
			}

			if (key[KEY_LEFT])
			{
				snake_direction[UP] = false;
				snake_direction[DOWN] = false;
				snake_direction[LEFT] = true;
				snake_direction[RIGHT] = false;
			}

			if (key[KEY_RIGHT])
			{
				snake_direction[UP] = false;
				snake_direction[DOWN] = false;
				snake_direction[LEFT] = false;
				snake_direction[RIGHT] = true;
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
					key[KEY_UP] = true;
					break;
				case ALLEGRO_KEY_DOWN:
					key[KEY_DOWN] = true;
					break;
				case ALLEGRO_KEY_LEFT:
					key[KEY_LEFT] = true;
					break;
				case ALLEGRO_KEY_RIGHT:
					key[KEY_RIGHT] = true;
					break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP)
		{
		        switch (ev.keyboard.keycode)
		        {
		                case ALLEGRO_KEY_UP:
		                        key[KEY_UP] = false;
		                        break;
		                case ALLEGRO_KEY_DOWN:
		                        key[KEY_DOWN] = false;
		                        break;
		                case ALLEGRO_KEY_LEFT:
		                        key[KEY_LEFT] = false;
		                        break;
		                case ALLEGRO_KEY_RIGHT:
		                        key[KEY_RIGHT] = false;
		                        break;
		                case ALLEGRO_KEY_ESCAPE:
		                        doexit = true;
		                        break;
		        }
		}

		if (redraw && al_is_event_queue_empty(event_queue))
		{
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

	al_destroy_bitmap(snake);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}


int initResources(ALLEGRO_TIMER * *timer, ALLEGRO_DISPLAY * *display)
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

	return 0;
}


int isSnakeCollisionWithWall(int snakePosX1, int snakePosY1, int snakeWidth, int snakeHeight, int wallPosX1, int wallPosY1, int wallWidth, int wallHeight)
{
	return 0;
}

