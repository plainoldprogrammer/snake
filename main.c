#include <stdio.h>
#include <allegro5/allegro.h>

const float FPS = 60;
const int SCREEN_W = 1024;
const int SCREEN_H = 768;
const int BOUNCER_SIZE = 32;

enum MKEYS
{
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
};

int main(int argc, char **argv)
{
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_BITMAP *snake = NULL;

	float snake_x = SCREEN_W / 2.0 - BOUNCER_SIZE / 2.0;
	float snake_y = SCREEN_H / 2.0 - BOUNCER_SIZE / 2.0;
	bool key[4] = { false, false, false, false };
	bool redraw = true;
	bool doexit = false;

	if (!al_init())
	{
		fprintf(stderr, "failed to initialize allegro\n");
	}

	if (!al_install_keyboard())
	{
		fprintf(stderr, "failed to initialize the keyboard\n");
	}

	timer = al_create_timer(1.0 / FPS);
	if (!timer)
	{
		fprintf(stderr, "failed to create timer");
		return -1;
	}

	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display)
	{
		fprintf(stderr, "failed to create display\n");
		al_destroy_timer(timer);
		return -1;
	}

	snake = al_create_bitmap(BOUNCER_SIZE, BOUNCER_SIZE);
	if (!snake)
	{
		fprintf(stderr, "failed to create snake bitmap\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	al_set_target_bitmap(snake);
	
	al_clear_to_color(al_map_rgb(102, 255, 51));

	al_set_target_bitmap(al_get_backbuffer(display));

	event_queue = al_create_event_queue();
	if (!event_queue)
	{
		fprintf(stderr, "failed to create event_queue");
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

	while (!doexit)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER)
		{
			if (key[KEY_UP] && snake_y >= 4.0)
			{
				snake_y -= 4.0;
			}

			if (key[KEY_DOWN] && snake_y <= SCREEN_H - BOUNCER_SIZE -4.0)
			{
				snake_y += 4.0;
			}

			if (key[KEY_LEFT] && snake_x >= 4.0)
			{
				snake_x -= 4.0;
			}

			if (key[KEY_RIGHT] && snake_x <= SCREEN_W - BOUNCER_SIZE - 4.0)
			{
				snake_x += 4.0;
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

			al_flip_display();
		}
	}

	al_destroy_bitmap(snake);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}

