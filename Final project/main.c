// [main.c]
// this template is provided for the 2D shooter game.

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>
//#include <time.h>

// If defined, logs will be shown on console and written to file.
// If commented out, logs will not be shown nor be saved.
#define LOG_ENABLED
#define pi 3.1415926
/* Constants. */

// Frame rate (frame per second)
const int FPS = 60;
// Display (screen) width.
const int SCREEN_W = 1500;
// Display (screen) height.
const int SCREEN_H = 900;
// Display (screen + profile) width.
const int FULL_SCREEN_W = 1700;
// Display (screen + profile) height.
const int FULL_SCREEN_H = 1000;
// Display HP bar height.
const int HP_BAR = 20;
const int HP_BAR_LEN = 50;
// At most 4 audios can be played at a time.
const int RESERVE_SAMPLES = 4;
// Same as:
// const int SCENE_MENU = 1;
// const int SCENE_START = 2;
enum {
	SCENE_MENU = 1,
	SCENE_START = 2,
	// [HACKATHON 3-7]
	// TODO: Declare a new scene id.
	SCENE_SETTINGS = 3,
	SCENE_STOP = 4,
	SCENE_GAME_END = 5,
	SCENE_EXIT = 6,
	SCENE_BOSS = 7
};

/* Input states */

// The active scene id.
int active_scene;
// Keyboard state, whether the key is down or not.
bool key_state[ALLEGRO_KEY_MAX];
// Mouse state, whether the key is down or not.
// 1 is for left, 2 is for right, 3 is for middle.
bool *mouse_state;
// Mouse position.
int mouse_x, mouse_y;
// TODO: More variables to store input states such as joysticks, ...

/* Variables for allegro basic routines. */

ALLEGRO_DISPLAY* game_display;
ALLEGRO_EVENT_QUEUE* game_event_queue;
ALLEGRO_TIMER* game_update_timer;

/* Shared resources*/

ALLEGRO_FONT* font_pirulen_32;
ALLEGRO_FONT* font_pirulen_24;
ALLEGRO_FONT* font_pirulen_20;
// TODO: More shared resources or data that needed to be accessed
// across different scenes.

/* Menu Scene resources*/
ALLEGRO_BITMAP* main_img_background;
// [HACKATHON 3-1]
// TODO: Declare 2 variables for storing settings images.
// Uncomment and fill in the code below.
ALLEGRO_BITMAP* img_settings;
ALLEGRO_BITMAP* img_settings2;
ALLEGRO_BITMAP* img_settings_bg;
ALLEGRO_SAMPLE* main_bgm;
ALLEGRO_SAMPLE_ID main_bgm_id;
ALLEGRO_BITMAP* img_profile;
ALLEGRO_BITMAP* img_profile_vertical;
ALLEGRO_BITMAP* img_game_end;
ALLEGRO_BITMAP* img_skill_Q;
ALLEGRO_BITMAP* img_skill_W;
ALLEGRO_BITMAP* img_skill_E;
ALLEGRO_BITMAP* img_stop;
ALLEGRO_BITMAP* img_pause;
ALLEGRO_BITMAP* img_resume;
ALLEGRO_BITMAP* img_exit;
ALLEGRO_BITMAP* img_pause2;
ALLEGRO_BITMAP* img_resume2;
ALLEGRO_BITMAP* img_exit2;

/* Start Scene resources*/
ALLEGRO_BITMAP* start_img_background;
ALLEGRO_BITMAP* start_img_plane;
ALLEGRO_BITMAP* start_img_enemy;
ALLEGRO_BITMAP* img_defeat;
ALLEGRO_SAMPLE* start_bgm;
ALLEGRO_SAMPLE_ID start_bgm_id;
ALLEGRO_SAMPLE* stop_bgm;
ALLEGRO_SAMPLE_ID stop_bgm_id;
ALLEGRO_SAMPLE* game_end_bgm;
ALLEGRO_SAMPLE_ID game_end_bgm_id;
ALLEGRO_SAMPLE* defeat_sound_bgm;
ALLEGRO_SAMPLE_ID defeat_sound_bgm_id;
// [HACKATHON 2-1]
// TODO: Declare a variable to store your bullet's image.
// Uncomment and fill in the code below.
ALLEGRO_BITMAP* img_bullet;
ALLEGRO_BITMAP* img_bullet_Q;
ALLEGRO_BITMAP* img_bullet_W;
ALLEGRO_BITMAP* img_bullet_W_ult;
ALLEGRO_BITMAP* img_bullet_clone;
ALLEGRO_SAMPLE_ID W_sound_id;
ALLEGRO_SAMPLE* W_sound;
ALLEGRO_SAMPLE_ID E_sound_id;
ALLEGRO_SAMPLE* E_sound;
ALLEGRO_BITMAP* img_E_ANIMATION;
ALLEGRO_BITMAP* img_E_SMOKE;
ALLEGRO_BITMAP* img_enemies_bullet;
ALLEGRO_SAMPLE* boss_bgm;
ALLEGRO_SAMPLE_ID boss_bgm_id;
ALLEGRO_BITMAP* boss_bg;
ALLEGRO_BITMAP* img_boss;
ALLEGRO_BITMAP* img_boss_weapon;

typedef struct {
	//?The center coordinate of the image.
	float x, y;
	//?The width and height of the object.
	float w, h;
	//?The velocity in x, y axes.
	float vx, vy;
	// The total score the object has.
	double score;
	// The level the object has.
	float level;
	// The damage the object gives.
	float damage;
	// The q_skill time and the using time.
	float Q_skill, Q_skill_remain;
	// The q_skill time and the using time.
	float W_skill, W_skill_remain;
	// The q_skill time and the using time.
	float E_skill, E_skill_remain;
	// The HP the object has.
	float hp, max_hp;
	// The MP the object has.
	float mp, max_mp;
	// The EXP can get when the object is killed.
	float exp, max_exp;
	//?Should we draw this object on the screen.
	bool hidden;
	//?The pointer to the object?™s image.
	ALLEGRO_BITMAP* img;
} MovableObject;
void draw_movable_object(MovableObject obj);
#define MAX_ENEMY 10
// [HACKATHON 2-2]
// TODO: Declare the max bullet count that will show on screen.
// You can try max 4 bullets here and see if you needed more.
// Uncomment and fill in the code below.
#define MAX_BULLET 10
#define MAX_PLAYER_BULLET 15
#define MAX_PLAYER_BULLET_Q 3
#define MAX_CLONE 4
#define MAX_CLONE_BULLET 5
#define MAX_BULLET_BOSS 30
#define MAX_PLAYER_BULLET_BOSS_MODE 15
MovableObject plane;
MovableObject plane_clone[MAX_CLONE];
MovableObject enemies[MAX_ENEMY];
MovableObject plane_boss_mode;
MovableObject bullets_boss_mode[MAX_PLAYER_BULLET_BOSS_MODE];
MovableObject big_boss;
MovableObject bullets_boss[MAX_BULLET_BOSS];
// [HACKATHON 2-3]
// TODO: Declare an array to store bullets with size of max bullet count.
// Uncomment and fill in the code below.
MovableObject bullets[MAX_PLAYER_BULLET];
MovableObject bullets_Q[MAX_PLAYER_BULLET_Q];
MovableObject bullets_W;
MovableObject bullets_clone[MAX_CLONE][MAX_CLONE_BULLET];
MovableObject bullets_enemy[MAX_ENEMY][MAX_BULLET];
// [HACKATHON 2-4]
// TODO: Set up bullet shooting cool-down variables.
// 1) Declare your shooting cool-down time as constant. (0.2f will be nice)
// 2) Declare your last shoot timestamp.
// Uncomment and fill in the code below.
const float MAX_COOLDOWN = 0.12;
const float MAX_COOLDOWN_E1 = 0.24;
const float MAX_COOLDOWN_DEFEAT = 2;
const float MAX_COOLDOWN_Q = 5;
const float MAX_COOLDOWN_W = 8;
const float MAX_COOLDOWN_E = 10;
const float MAX_COOLDOWN_STOP_CONVERT_START = 2;
const float MAX_COOLDOWN_W_BIGGER_DURATION = 3;
const float MAX_COOLDOWN_W_ULT_DURATION = 0.75;
const float MAX_COOLDOWN_E_SMOKE_DURATION = 1.25;
const float MAX_COOLDOWN_E_ANIMATION_DURATION = 1;

bool isconverted_stop_start = false;
double change_scene_delay;
double last_shoot_timestamp;
double last_enemy_shoot_timestamp[MAX_ENEMY];
double last_boss_shoot_timestamp;
double last_shoot_Eskill_timestamp;
double defeat_show;
int defeat_flag = 1;
int W_skill_stage = 1;
int W_image_add = 0;
int E_skill_stage = 1;
int E_count = 0;
float W_skill_time;
float W_ult_time;
float E_animation_time;
float E_smoke_time;
float degree_W = 0;
float scale_W = 0;
bool Q_in_CD = false;
bool W_in_CD = false;
bool E_in_CD = false;
bool is_win = false;
bool stop_came_back_Q = false;
bool stop_came_back_W = false;
bool stop_came_back_E = false;
double temp_time;
// Declare enemy_revive_cool-down
const float MAX_COOLDOWN_ENEMY_DEATH_CD = 0.8;
double enemy_death_time[MAX_ENEMY];

// Declare enemy_velocity
const float MAX = 8.5;
const float MIN = 3.5;
/* Declare function prototypes. */

// Initialize allegro5 library
void allegro5_init(void);
// Initialize variables and resources.
// Allows the game to perform any initialization it needs before
// starting to run.
void game_init(void);
// Process events inside the event queue using an infinity loop.
void game_start_event_loop(void);
// Run game logic such as updating the world, checking for collision,
// switching scenes and so on.
// This is called when the game should update its logic.
void game_update(void);
// Draw to display.
// This is called when the game should draw itself.
void game_draw(void);
// Release resources.
// Free the pointers we allocated.
void game_destroy(void);
// Function to change from one scene to another.
void game_change_scene(int next_scene);
// Load resized bitmap and check if failed.
ALLEGRO_BITMAP *load_bitmap_resized(const char *filename, int w, int h);
// [HACKATHON 3-2]
// TODO: Declare a function.
// Determines whether the point (px, py) is in rect (x, y, w, h).
// Uncomment the code below.
bool pnt_in_rect(int px, int py, int x, int y, int w, int h);
bool done = false;
/* Event callbacks. */
void on_key_down(int keycode);
void on_mouse_down(int btn, int x, int y);
void level_up(int level);
/* Declare function prototypes for debugging. */

// Display error message and exit the program, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// If the program crashes unexpectedly, you can inspect "log.txt" for
// further information.
void game_abort(const char* format, ...);
// Log events for later debugging, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// You can inspect "log.txt" for logs in the last run.
void game_log(const char* format, ...);
// Log using va_list.
void game_vlog(const char* format, va_list arg);

int main(int argc, char** argv) {
	// Set random seed for better random outcome.
	srand(time(NULL));
	allegro5_init();
	game_log("Allegro5 initialized");
	game_log("Game begin");
	// Initialize game variables.
	game_init();
	game_log("Game initialized");
	// Draw the first frame.
	game_draw();
	game_log("Game start event loop");
	// This call blocks until the game is finished.
	game_start_event_loop();
	game_log("Game end");
	game_destroy();
	game_log("all the allocations are destroyed");
	return 0;
}

void allegro5_init(void) {
	if (!al_init())
		game_abort("failed to initialize allegro");

	// Initialize add-ons.
	if (!al_init_primitives_addon())
		game_abort("failed to initialize primitives add-on");
	if (!al_init_font_addon())
		game_abort("failed to initialize font add-on");
	if (!al_init_ttf_addon())
		game_abort("failed to initialize ttf add-on");
	if (!al_init_image_addon())
		game_abort("failed to initialize image add-on");
	if (!al_install_audio())
		game_abort("failed to initialize audio add-on");
	if (!al_init_acodec_addon())
		game_abort("failed to initialize audio codec add-on");
	if (!al_reserve_samples(RESERVE_SAMPLES))
		game_abort("failed to reserve samples");
	if (!al_install_keyboard())
		game_abort("failed to install keyboard");
	if (!al_install_mouse())
		game_abort("failed to install mouse");
	// TODO: Initialize other addons such as video, ...

	// Setup game display.
	game_display = al_create_display(FULL_SCREEN_W, FULL_SCREEN_H);
	if (!game_display)
		game_abort("failed to create display");
	al_set_window_title(game_display, "I2P(I)_2018_Yang Final Project <107062107>");

	// Setup update timer.
	game_update_timer = al_create_timer(1.0f / FPS);
	if (!game_update_timer)
		game_abort("failed to create timer");

	// Setup event queue.
	game_event_queue = al_create_event_queue();
	if (!game_event_queue)
		game_abort("failed to create event queue");

	// Malloc mouse buttons state according to button counts.
	const unsigned m_buttons = al_get_mouse_num_buttons();
	game_log("There are total %u supported mouse buttons", m_buttons);
	// mouse_state[0] will not be used.
	mouse_state = malloc((m_buttons + 1) * sizeof(bool));
	memset(mouse_state, false, (m_buttons + 1) * sizeof(bool));

	// Register display, timer, keyboard, mouse events to the event queue.
	al_register_event_source(game_event_queue, al_get_display_event_source(game_display));
	al_register_event_source(game_event_queue, al_get_timer_event_source(game_update_timer));
	al_register_event_source(game_event_queue, al_get_keyboard_event_source());
	al_register_event_source(game_event_queue, al_get_mouse_event_source());
	// TODO: Register other event sources such as timer, video, ...

	// Start the timer to update and draw the game.
	al_start_timer(game_update_timer);
}

void game_init(void) {
	/* Shared resources*/
	font_pirulen_32 = al_load_font("font/pirulen.ttf", 32, 0);
	if (!font_pirulen_32)
		game_abort("failed to load font: font/pirulen.ttf with size 32");

	font_pirulen_24 = al_load_font("font/pirulen.ttf", 24, 0);
	if (!font_pirulen_24)
		game_abort("failed to load font: font/pirulen.ttf with size 24");

    font_pirulen_20 = al_load_font("font/pirulen.ttf", 20, 0);
	if (!font_pirulen_20)
		game_abort("failed to load font: font/pirulen.ttf with size 20");
	/* Menu Scene resources*/
	main_img_background = load_bitmap_resized("image/main_background.jpg", FULL_SCREEN_W, FULL_SCREEN_H);
	if (!main_img_background)
        game_abort("failed to load image: image/main_background.jpg");

	main_bgm = al_load_sample("audio/main_bgm.wav");
	if (!main_bgm)
		game_abort("failed to load audio: audio/main_bgm.wav");

	// [HACKATHON 3-4]
	// TODO: Load settings images.
	// Don't forget to check their return values.
	// Uncomment and fill in the code below.
	img_settings = al_load_bitmap("image/settings.png");
	if (!img_settings)
		game_abort("failed to load image: image/settings.png");
	img_settings2 = al_load_bitmap("image/settings2.png");
	if (!img_settings2)
		game_abort("failed to load image: image/settings2.png");
	img_settings_bg = load_bitmap_resized("image/setting_bg.png", FULL_SCREEN_W, FULL_SCREEN_H);
	if (!img_settings_bg)
		game_abort("failed to load image: image/setting_bg.png");

	img_boss = al_load_bitmap("image/boss.png");
	if (!img_boss)
		game_abort("failed to load image: image/boss.png");

    img_boss_weapon = load_bitmap_resized("image/boss_weapon.png", 200, 200);
    if (!img_boss_weapon)
		game_abort("failed to load image: image/boss_weapon.png");

    boss_bg = load_bitmap_resized("image/boss_bg.png", FULL_SCREEN_W, FULL_SCREEN_H);
    if (!boss_bg)
		game_abort("failed to load image: image/boss_bg.png");

    boss_bgm = al_load_sample("audio/boss_bgm.wav");
    if (!boss_bgm)
		game_abort("failed to load audio: audio/boss_bgm.wav");

	/* Start Scene resources*/
	start_img_background = load_bitmap_resized("image/background.jpeg", SCREEN_W, SCREEN_H);

    start_img_plane = al_load_bitmap("image/pika.png");
	if (!start_img_plane)
		game_abort("failed to load image: image/pika.png");

	start_img_enemy = al_load_bitmap("image/smallfighter0006.png");
	if (!start_img_enemy)
		game_abort("failed to load image: image/smallfighter0006.png");

	start_bgm = al_load_sample("audio/start_bgm.wav");
	if (!start_bgm)
		game_abort("failed to load audio: audio/start_bgm.wav");

    // create img of the profile_background.
    img_profile = load_bitmap_resized("image/profile.jpg", SCREEN_W, FULL_SCREEN_H-SCREEN_H);
    if (!img_profile)
        game_abort("failed to load image: image/profile.jpg");

    img_profile_vertical = load_bitmap_resized("image/profile.jpg", FULL_SCREEN_W-SCREEN_W, FULL_SCREEN_H);
    if (!img_profile_vertical)
        game_abort("failed to load image: image/profile.jpg");

    // create img of the end of the game.
    img_game_end = load_bitmap_resized("image/game_end.jpg", FULL_SCREEN_W, FULL_SCREEN_H);
    if (!img_game_end)
        game_abort("failed to load image: image/game_end.jpg");

    img_defeat = al_load_bitmap("image/defeat.png");
    if (!img_defeat)
        game_abort("failed to load image: image/defeat.png");

    game_end_bgm = al_load_sample("audio/game_end_bgm.wav");
    if (!game_end_bgm)
		game_abort("failed to load audio: audio/game_end_bgm.wav");

    defeat_sound_bgm = al_load_sample("audio/defeat_sound.wav");
    if (!defeat_sound_bgm)
		game_abort("failed to load audio: audio/defeat_sound.wav");

	img_skill_Q = load_bitmap_resized("image/Q.jpg", 65, 65);
    if (!img_skill_Q)
		game_abort("failed to load image: image/Q.jpg");

    img_skill_W = load_bitmap_resized("image/W.png", 70, 65);
    if (!img_skill_W)
		game_abort("failed to load image: image/W.png");

    img_skill_E = load_bitmap_resized("image/E.jpg", 65, 65);
    if (!img_skill_E)
		game_abort("failed to load image: image/E.jpg");

    img_stop = load_bitmap_resized("image/stop.jpg", SCREEN_W, SCREEN_H);
    if (!img_stop)
		game_abort("failed to load image: image/stop.jpg");

    stop_bgm = al_load_sample("audio/stop_bgm.wav");
    if (!stop_bgm) {
        game_abort("failed to load audio: audio/stop_bgm.wav");
    }

    img_pause = load_bitmap_resized("image/pause.png", 60, 60);
    if (!img_pause)
        game_abort("failed to load image: image/pause.png");

    img_resume = load_bitmap_resized("image/resume.png", 60, 60);
    if (!img_resume)
        game_abort("failed to load image: image/resume.png");

    img_exit = load_bitmap_resized("image/exit.png", 60, 60);
    if (!img_exit)
        game_abort("failed to load image: image/exit.png");

    img_pause2 = load_bitmap_resized("image/pause2.png", 60, 60);
    if (!img_pause2)
        game_abort("failed to load image: image/pause2.png");

    img_resume2 = load_bitmap_resized("image/resume2.png", 60, 60);
    if (!img_resume2)
        game_abort("failed to load image: image/resume2.png");

    img_exit2 = load_bitmap_resized("image/exit2.png", 60, 60);
    if (!img_exit2)
        game_abort("failed to load image: image/exit2.png");

	// [HACKATHON 2-5-1]
	// TODO: Initialize bullets.
	// 1) Search for a bullet image online and put it in your project.
	//    You can use the image we provided.
	// 2) Load it in by 'al_load_bitmap' or 'load_bitmap_resized'.
	// 3) If you use 'al_load_bitmap', don't forget to check its return value.
	// Uncomment and fill in the code below.
	img_bullet = al_load_bitmap("image/weapon_1.png");
	if (!img_bullet)
		game_abort("failed to load image: image/weapon_1.png");

	img_bullet_Q = load_bitmap_resized("image/weapon_2.png", 150, 150);
	if (!img_bullet_Q)
        game_abort("failed to load image: image/weapon_2.png");

    img_bullet_W = load_bitmap_resized("image/weapon_3.png", 230, 230);
    if (!img_bullet_W)
        game_abort("failed to load image: image/weapon_3.png");

    img_bullet_W_ult = load_bitmap_resized("image/weapon_3_ultimate.png", 400, 350);
    if (!img_bullet_W_ult)
        game_abort("failed to load image: image/weapon_3_ultimate.png");

    img_bullet_clone = load_bitmap_resized("image/weapon_clone.png", 150, 75);
    if (!img_bullet_clone)
        game_abort("failed to load image: image/weapon_clone.png");

    img_E_ANIMATION = al_load_bitmap("image/E_animation.png");
    if (!img_E_ANIMATION)
        game_abort("failed to load image: image/E_animation.png");

    img_E_SMOKE = load_bitmap_resized("image/E_smoke.png", 200, 150);
    if (!img_E_SMOKE)
        game_abort("failed to load image: image/E_smoke.png");

    W_sound = al_load_sample("audio/W_skill_sound.wav");
    if (!W_sound)
        game_abort("failed to load audio: audio/W_skill_sound.wav");

    E_sound = al_load_sample("audio/E_skill_sound.wav");
    if (!E_sound)
        game_abort("failed to load audio: audio/E_skill_sound.wav");

    img_enemies_bullet = al_load_bitmap("image/image12.png");
    if(!img_enemies_bullet)
        game_abort("failed to load image: image/image12.png");

	// Change to first scene.
	game_change_scene(SCENE_MENU);
}

void game_start_event_loop(void) {

	ALLEGRO_EVENT event;
	int redraws = 0;
	while (!done) {
		al_wait_for_event(game_event_queue, &event);
		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			// Event for clicking the window close button.
			game_log("Window close button clicked");
			done = true;
		} else if (event.type == ALLEGRO_EVENT_TIMER) {
			// Event for redrawing the display.
			if (event.timer.source == game_update_timer)
				// The redraw timer has ticked.
				redraws++;
		} else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			// Event for keyboard key down.
			game_log("Key with keycode %d down", event.keyboard.keycode);
			game_log("%d", ALLEGRO_KEY_SPACE);
			key_state[event.keyboard.keycode] = true;
			on_key_down(event.keyboard.keycode);
		} else if (event.type == ALLEGRO_EVENT_KEY_UP) {
			// Event for keyboard key up.
			game_log("Key with keycode %d up", event.keyboard.keycode);
			key_state[event.keyboard.keycode] = false;
		} else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			// Event for mouse key down.
			game_log("Mouse button %d down at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
			mouse_state[event.mouse.button] = true;
			on_mouse_down(event.mouse.button, event.mouse.x, event.mouse.y);
		} else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			// Event for mouse key up.
			game_log("Mouse button %d up at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
			mouse_state[event.mouse.button] = false;
		} else if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
			if (event.mouse.dx != 0 || event.mouse.dy != 0) {
				// Event for mouse move.
				game_log("Mouse move to (%d, %d)", event.mouse.x, event.mouse.y);
				mouse_x = event.mouse.x;
				mouse_y = event.mouse.y;
			} else if (event.mouse.dz != 0) {
				// Event for mouse scroll.
				game_log("Mouse scroll at (%d, %d) with delta %d", event.mouse.x, event.mouse.y, event.mouse.dz);
			}
		}
		// TODO: Process more events and call callbacks by adding more
		// entries inside Scene.

		// Redraw
		if (redraws > 0 && al_is_event_queue_empty(game_event_queue)) {
			if (redraws > 1)
				game_log("%d frame(s) dropped", redraws - 1);
			// Update and draw the next frame.
			game_update();
			game_draw();
			redraws = 0;
		}
	}
}

void game_update(void) {
	if (active_scene == SCENE_START) {
        //set a timer_now
		double now = al_get_time();

		// check if the scene is changed from stop to start,
		// the image stop for 2 sec and then resume playing.
		if (isconverted_stop_start) {
            al_stop_sample(&start_bgm_id);
            if (now - change_scene_delay >= MAX_COOLDOWN_STOP_CONVERT_START) {
                if (!al_play_sample(start_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &start_bgm_id))
                    game_abort("failed to play audio (bgm)");
                isconverted_stop_start = false;
            }
            return ;
		}

        if (defeat_flag == 0) {
            if (now - defeat_show >= MAX_COOLDOWN_DEFEAT) {
                defeat_flag = 1;
                is_win = false;
                game_change_scene(SCENE_GAME_END);
            }
            return ;
		}

        int i;
		plane.vx = plane.vy = 0;
		if (key_state[ALLEGRO_KEY_UP])
			plane.vy -= 3;
		if (key_state[ALLEGRO_KEY_DOWN])
			plane.vy += 3;
		if (key_state[ALLEGRO_KEY_LEFT])
			plane.vx -= 3;
		if (key_state[ALLEGRO_KEY_RIGHT])
			plane.vx += 3;
		// 0.71 is (1/sqrt(2)).
		plane.y += plane.vy * 4 * (plane.vx ? 0.71f : 1);
		plane.x += plane.vx * 4 * (plane.vy ? 0.71f : 1);

		// skill keyboard --- Q
		if (key_state[ALLEGRO_KEY_Q] && !Q_in_CD && plane.mp >= 15) {
            plane.mp -= 15;
            plane.Q_skill = now;
            Q_in_CD = true;
            for (i=0; i<MAX_PLAYER_BULLET_Q; i++) {
                bullets_Q[i].x = plane.x;
                bullets_Q[i].y = plane.y - plane.h/2;
                bullets_Q[i].hidden = false;
            }
		}
		if (Q_in_CD) {
            if (stop_came_back_Q)
                plane.Q_skill += (now - temp_time);
            plane.Q_skill_remain = now - plane.Q_skill;
            if (plane.Q_skill_remain >= MAX_COOLDOWN_Q) {
                plane.Q_skill_remain = MAX_COOLDOWN_Q;
                Q_in_CD = false;
            }

		}
        stop_came_back_Q = false;
        // skill keyboard --- W
		if (key_state[ALLEGRO_KEY_W] && !W_in_CD && plane.mp >= 35) {
            plane.mp -= 35;
            plane.W_skill = now;
            W_in_CD = true;
            bullets_W.img = img_bullet_W;
            bullets_W.w = al_get_bitmap_width(img_bullet_W);
            bullets_W.h = al_get_bitmap_height(img_bullet_W);
            bullets_W.x = plane.x;
            bullets_W.y = plane.y - plane.h/2;
            bullets_W.hidden = false;
            W_skill_stage = 1;
		}
		if (W_in_CD) {
            if (stop_came_back_W)
                plane.W_skill += (now - temp_time);
            plane.W_skill_remain = now - plane.W_skill;
            if (plane.W_skill_remain >= MAX_COOLDOWN_W) {
                plane.W_skill_remain = MAX_COOLDOWN_W;
                W_in_CD = false;
            }

		}
        stop_came_back_W = false;
        // skill keyboard --- E
		if (key_state[ALLEGRO_KEY_E] && !E_in_CD && plane.mp >= 30) {
            plane.mp -= 30;
            plane.E_skill = now;
            E_in_CD = true;
            E_skill_stage = 1;
            E_count = 0;
            E_animation_time = now;
            for (i=0; i<MAX_CLONE; i++) {
                plane_clone[i].w = plane.w;
                plane_clone[i].h = plane.h;
                plane_clone[i].img = img_E_SMOKE;
                plane_clone[i].x = (float)(SCREEN_W*0.8 - SCREEN_W/6) * rand() / (RAND_MAX + 1.0) + SCREEN_W/6;
                plane_clone[i].y = (float)(SCREEN_W/2 - SCREEN_W/5) * rand() / (RAND_MAX + 1.0) + SCREEN_W/5;
                plane_clone[i].vx = plane_clone[i].vy = 0;
                plane_clone[i].hidden = false;
            }

		}
		if (E_in_CD) {
            if (stop_came_back_E)
                plane.E_skill += (now - temp_time);
            plane.E_skill_remain = now - plane.E_skill;
            if (plane.E_skill_remain >= MAX_COOLDOWN_E) {
                plane.E_skill_remain = MAX_COOLDOWN_E;
                E_in_CD = false;
            }

		}
        stop_came_back_E = false;
        // Update every skill_weapons coordinates
        // 1) Q_Skill weapon
		for (i=0; i<MAX_PLAYER_BULLET_Q; i++) {
            if (!bullets_Q[i].hidden) {
                bullets_Q[i].x += bullets_Q[i].vx;
                bullets_Q[i].y += bullets_Q[i].vy;
            }
            // check if the object is out of the boundary
            if (bullets_Q[i].x + bullets_Q[i].w/2 > SCREEN_W ||
                bullets_Q[i].y - bullets_Q[i].h/2 < 0 ||
                bullets_Q[i].x - bullets_Q[i].w/2 > SCREEN_W
            ) {
                bullets_Q[i].x = bullets_Q[i].y = -100;
                bullets_Q[i].hidden = true;
            }
		}
		// 2) W_Skill weapon
		if (!bullets_W.hidden) {
            switch (W_skill_stage) {
            case 1: // go straight and rotate (not hit by enemy)
                bullets_W.y += bullets_W.vy;
                degree_W += 0.45;
                scale_W = 0;
                W_image_add = 0;
                break;
            case 2: // rotate and getting big(hit by enemy)
                W_image_add += 1;
                scale_W += 0.01;
                degree_W += 0.7;
                break;
            case 3:
                bullets_W.img = img_bullet_W_ult;
                bullets_W.w = al_get_bitmap_width(img_bullet_W_ult);
                bullets_W.h = al_get_bitmap_height(img_bullet_W_ult);
                break;

            }
		}
		// 3) E_Skill weapon
		if (!plane_clone[0].hidden || !plane_clone[1].hidden || !plane_clone[2].hidden || !plane_clone[3].hidden) {
            switch (E_skill_stage) {
            case 1:
                if (now - E_animation_time >= MAX_COOLDOWN_E_ANIMATION_DURATION) {
                    E_skill_stage = 2;
                    E_smoke_time = now;
                }
                break;
            case 2:
                if (now - E_smoke_time >= MAX_COOLDOWN_E_SMOKE_DURATION) {
                    E_skill_stage = 3;
                    for (i=0; i<MAX_CLONE; i++) {
                        plane_clone[i].img = start_img_plane;
                        bullets_clone[i][E_count].x = plane_clone[i].x;
                        bullets_clone[i][E_count].y = plane_clone[i].y - plane_clone[i].h/2;
                        bullets_clone[i][E_count].vy = -2.5;
                        bullets_clone[i][E_count].hidden = false;
                        last_shoot_Eskill_timestamp = now;
                    }
                    E_count++;
                }
                break;
            case 3:
                if (now - last_shoot_Eskill_timestamp >= 0.5) {
                    for (i=0; i<MAX_CLONE; i++) {
                        plane_clone[i].img = start_img_plane;
                        bullets_clone[i][E_count].x = plane_clone[i].x;
                        bullets_clone[i][E_count].y = plane_clone[i].y - plane_clone[i].h/2;
                        bullets_clone[i][E_count].vy = -2.5;
                        bullets_clone[i][E_count].hidden = false;
                        last_shoot_Eskill_timestamp = now;
                    }
                    E_count++;
                }

                if (E_count == 5) {
                    for (i=0; i<MAX_CLONE; i++) {
                        plane_clone[i].x = plane_clone[i].y = -100;
                        plane_clone[i].hidden = true;
                    }
                }
                break;
            }
		}
        for (i=0; i<MAX_CLONE; i++) {
            for (int j=0; j<MAX_CLONE_BULLET; j++) {
                if (!bullets_clone[i][j].hidden) {
                    bullets_clone[i][j].y += bullets_clone[i][j].vy;
                    // check if the object is out of the boundary
                    if (bullets_clone[i][j].y - bullets_clone[i][j].h/2 < 0) {
                        bullets_clone[i][j].x = bullets_clone[i][j].y = -100;
                        bullets_clone[i][j].hidden = true;
                    }
                }
            }
        }
		// [HACKATHON 1-1]
		// TODO: Limit the plane's collision box inside the frame.
		//       (x, y axes can be separated.)
		// Uncomment and fill in the code below.
		if ((plane.x-plane.w/2) < 0)
			plane.x = plane.w/2;
		else if ((plane.x+plane.w/2) > SCREEN_W)
			plane.x = SCREEN_W-plane.w/2;
		if ((plane.y-plane.h/2) < 0)
			plane.y = plane.h/2;
		else if ((plane.y+plane.h/2) > SCREEN_H)
			plane.y = SCREEN_H-plane.h/2;

        // Update every enemy_bullet coordinate.
        int flag=1;
		for (i=0; i<MAX_ENEMY; i++) {
            for (int j=0; j<MAX_BULLET; j++) {
                if (!bullets_enemy[i][j].hidden) {
                    bullets_enemy[i][j].x += bullets_enemy[i][j].vx;
                    bullets_enemy[i][j].y += bullets_enemy[i][j].vy;

                    //check if the bullets_enemy is out of boundary.
                    if (bullets_enemy[i][j].y + bullets_enemy[i][j].h/2 > SCREEN_H) {
                        bullets_enemy[i][j].x = bullets_enemy[i][j].y = -100;
                        bullets_enemy[i][j].hidden = true;
                    }
                }
                else {
                    if (!enemies[i].hidden) {
                        if (now - last_enemy_shoot_timestamp[i] >= MAX_COOLDOWN_E1 && flag) {
                            bullets_enemy[i][j].x = enemies[i].x;
                            bullets_enemy[i][j].y = enemies[i].y + enemies[i].h/2;
                            bullets_enemy[i][j].hidden = false;
                            bullets_enemy[i][j].damage = 15;
                            flag=0;
                            last_enemy_shoot_timestamp[i] = now;
                        }
                    }
                }
            }
		}

		// [HACKATHON 2-6]
		// TODO: Update bullet coordinates.
		// 1) For each bullets, if it's not hidden, update x, y
		// according to vx, vy.
		// 2) If the bullet is out of the screen, hide it.
		// Uncomment and fill in the code below.

		for (i=0;i<MAX_PLAYER_BULLET;i++) {
			if (bullets[i].hidden)
				continue;
			bullets[i].x += bullets[i].vx*2;
			bullets[i].y += bullets[i].vy*2;
			if (bullets[i].y - bullets[i].h/2< 0) {
                bullets[i].hidden = true;
                bullets[i].x = bullets[i].y = -100;
			}

		}

        // Update every enemy position.
        for (i=0; i<MAX_ENEMY; i++) {
            enemies[i].x += enemies[i].vx;
            enemies[i].y += enemies[i].vy;

            //detect if the enemies is out of bound.
            if (enemies[i].x + enemies[i].w/2 > SCREEN_W) {
                enemies[i].x = SCREEN_W - enemies[i].w/2;
                enemies[i].vx *= -1;
            }
            else if (enemies[i].x - enemies[i].w/2 < 0) {
                enemies[i].x = enemies[i].w/2;
                enemies[i].vx *= -1;
            }

            /*if (enemies[i].y + enemies[i].h > SCREEN_H) {
                enemies[i].y = SCREEN_H - enemies[i].h/2;
                enemies[i].vy *= (-1);
            }
            else if (enemies[i].x - enemies[i].h < 0) {
                enemies[i].y = enemies[i].h/2;
                enemies[i].vy *= (-1);
            }*/
        }

        // implement all the contact.
        // 1) Q_bullet and enemy.
        for (i=0; i<MAX_PLAYER_BULLET_Q; i++) {
            if(bullets_Q[i].hidden)
                continue;
            for (int j=0; j<MAX_ENEMY; j++) {
                if(enemies[j].hidden)
                    continue;
                if (pnt_in_rect(enemies[j].x, enemies[j].y, bullets_Q[i].x, bullets_Q[i].y, bullets_Q[i].w, bullets_Q[i].h)) {
                    enemies[j].hp -= bullets_Q[i].damage;
                    if (enemies[j].hp<=0) {
                        enemies[j].x = enemies[j].y = -100;
                        plane.exp += enemies[j].exp;
                        plane.score += enemies[j].exp;
                        // check if need to upgrade the level.
                        if (plane.exp >= plane.max_exp) {
                            plane.exp -= plane.max_exp;
                            plane.hp = plane.max_hp;
                            plane.level++;
                            level_up(plane.level);
                        }

                        enemies[j].hidden = true;
                        enemy_death_time[j] = now;
                    }
                }
            }
            // 2) Q_bullet and enemies_bullets
            for (int j=0; j<MAX_ENEMY; j++) {
                for (int k=0; k<MAX_BULLET; k++) {
                    if (bullets_enemy[j][k].hidden) continue;
                    if (pnt_in_rect(bullets_enemy[j][k].x, bullets_enemy[j][k].y, bullets_Q[i].x, bullets_Q[i].y, bullets_Q[i].w, bullets_Q[i].h)) {
                        bullets_enemy[j][k].x = bullets_enemy[j][k].y = -100;
                        bullets_enemy[j][k].hidden = true;
                    }
                }
            }
        }
        // 3) W_bullet and enemy and enemies_bullet.
        if(!bullets_W.hidden) {
            // W_skill_stage == 1;
            // stop until hit an enemy.
            for (int j=0; j<MAX_ENEMY && W_skill_stage == 1; j++) {
                if (enemies[j].hidden) continue;
                if (pnt_in_rect(enemies[j].x, enemies[j].y, bullets_W.x, bullets_W.y, bullets_W.w, bullets_W.h)) {
                        W_skill_stage = 2;
                        W_skill_time = now;
                        enemies[j].vx = enemies[j].vy = 0;
                        break;
                }
            }
            // W_skill_stage == 2;
            // destroy all enemy and enemies_bullet.
            for (int j=0; j<MAX_ENEMY && W_skill_stage == 2; j++) {
                if (pnt_in_rect(enemies[j].x, enemies[j].y, bullets_W.x, bullets_W.y, bullets_W.w + W_image_add, bullets_W.h + W_image_add)) {
                        enemies[j].x = enemies[j].y = -100;
                        plane.exp += enemies[j].exp;
                        plane.score += enemies[j].exp;
                        if (plane.exp >= plane.max_exp) {
                            plane.exp -= plane.max_exp;
                            plane.hp = plane.max_hp;
                            plane.level++;
                            level_up(plane.level);
                        }
                        enemies[j].hidden = true;
                        enemy_death_time[j] = now;
                }
                for (int k=0; k<MAX_BULLET; k++) {
                    if (pnt_in_rect(bullets_enemy[j][k].x, bullets_enemy[j][k].y, bullets_W.x, bullets_W.y, bullets_W.w + W_image_add, bullets_W.h + W_image_add)) {
                            bullets_enemy[j][k].x = bullets_enemy[j][k].y = -100;
                            bullets_enemy[j][k].hidden = true;
                    }
                }
                if (now - W_skill_time >= MAX_COOLDOWN_W_BIGGER_DURATION) {
                    W_skill_stage = 3;
                    W_ult_time = now;
                }
            }
            for (int j=0; j<MAX_PLAYER_BULLET && W_skill_stage == 2; j++) {
                if (pnt_in_rect(bullets[j].x, bullets[j].y, bullets_W.x, bullets_W.y, bullets_W.w + W_image_add, bullets_W.h + W_image_add)) {
                    bullets[j].x = bullets[j].y = -100;
                    bullets[j].hidden = true;
                }
            }
            for (int j=0; j<MAX_PLAYER_BULLET_Q && W_skill_stage == 2; j++) {
                if (pnt_in_rect(bullets_Q[j].x, bullets_Q[j].y, bullets_W.x, bullets_W.y, bullets_W.w + W_image_add, bullets_W.h + W_image_add)) {
                    bullets_Q[j].x = bullets[j].y = -100;
                    bullets_Q[j].hidden = true;
                }
            }
            // W_skill_stage == 3;
            if (W_skill_stage == 3 && now - W_ult_time >= MAX_COOLDOWN_W_ULT_DURATION) {
                bullets_W.x = bullets_W.y = -100;
                bullets_W.hidden = true;
            }
        }
        // 4) E_bullet and enemy and enemies_bullet.
        for (i=0; i<MAX_CLONE; i++) {
            for (int j=0; j<MAX_CLONE_BULLET; j++) {
                if (bullets_clone[i][j].hidden)
                    continue;
                for (int k=0; k<MAX_ENEMY; k++) {
                    if (pnt_in_rect(enemies[k].x, enemies[k].y, bullets_clone[i][j].x, bullets_clone[i][j].y, bullets_clone[i][j].w, bullets_clone[i][j].h)) {
                        enemies[k].hp -= bullets_clone[i][j].damage;
                        if (enemies[k].hp<=0) {
                            enemies[k].x = enemies[k].y = -100;
                            plane.exp += enemies[k].exp;
                            plane.score += enemies[k].exp;
                            // check if need to upgrade the level.
                            if (plane.exp >= plane.max_exp) {
                                plane.exp -= plane.max_exp;
                                plane.hp = plane.max_hp;
                                plane.level++;
                                level_up(plane.level);
                            }

                            enemies[k].hidden = true;
                            enemy_death_time[k] = now;
                        }
                    }
                    for (int l=0; l<MAX_BULLET; l++) {
                        if (pnt_in_rect(bullets_enemy[k][l].x, bullets_enemy[k][l].y, bullets_clone[i][j].x, bullets_clone[i][j].y, bullets_clone[i][j].w, bullets_clone[i][j].h)) {
                            bullets_enemy[k][l].x = bullets_enemy[k][l].y = -100;
                            bullets_enemy[k][l].hidden = true;
                        }
                    }
                }
            }
        }

        /*// if the all five bullet_clone is hidden,
        // the clone will be hidden.
        int check_clone;
        for (i=0; i<MAX_CLONE; i++) {
            check_clone = 0;
            for (int j=0; j<MAX_CLONE_BULLET; j++) {
                if (bullets_clone[i][j].hidden)
                    check_clone++;
            }
            if (check_clone == 5) {
                plane_clone[i].x = plane_clone[i].y = -100;
                plane_clone[i].hidden = true;
            }
        }*/

        // 1) bullet and enemy.
        for (i=0; i<MAX_PLAYER_BULLET; i++) {
            if (bullets[i].hidden)
				continue;
            for (int j=0; j<MAX_ENEMY; j++) {
                if (pnt_in_rect(bullets[i].x, bullets[i].y, enemies[j].x, enemies[j].y, enemies[j].w, enemies[j].h)) {
                    bullets[i].x = bullets[i].y = -100;
                    bullets[i].hidden = true;
                    enemies[j].hp -= bullets[i].damage;
                    if (enemies[j].hp<=0) {
                        enemies[j].x = enemies[j].y = -100;
                        plane.exp += enemies[j].exp;
                        plane.score += enemies[j].exp;
                        // check if need to upgrade the level.
                        if (plane.exp >= plane.max_exp) {
                            plane.exp -= plane.max_exp;
                            plane.hp = plane.max_hp;
                            plane.level++;
                            level_up(plane.level);
                        }

                        enemies[j].hidden = true;
                        enemy_death_time[j] = now;
                    }
                }

			}
            // 2) bullet and enemy_bullet.
			// if the enemy_bullet hit the player_bullet,
			// they will both offset by another damage.
			float c;
			for (int j=0; j<MAX_ENEMY; j++) {
                for (int k=0; k<MAX_BULLET; k++) {
                    if (pnt_in_rect(bullets_enemy[j][k].x, bullets_enemy[j][k].y, bullets[i].x, bullets[i].y, bullets[i].w, bullets[i].h)) {
                        c = bullets[i].damage;
                        bullets[i].damage -= bullets_enemy[j][k].damage;
                        bullets_enemy[j][k].damage -= c;
                        if (bullets_enemy[j][k].damage <= 0) {
                            bullets_enemy[j][k].x = bullets_enemy[j][k].y = -100;
                            bullets_enemy[j][k].hidden = true;
                        }
                        if (bullets[i].damage <= 0) {
                            bullets[i].x = bullets[i].y = -100;
                            bullets[i].hidden = true;
                        }
                    }
                }
			}
        }
        // 3) enemy_bullet and player.
        for (i=0; i<MAX_ENEMY; i++) {
            for (int j=0; j<MAX_BULLET; j++) {
                if (pnt_in_rect(bullets_enemy[i][j].x, bullets_enemy[i][j].y, plane.x, plane.y, plane.w, plane.h)) {
                    plane.hp -= bullets_enemy[i][j].damage;
                    bullets_enemy[i][j].x = bullets_enemy[i][j].y = -100;
                    bullets_enemy[i][j].hidden = true;

                    if (plane.hp <= 0) {
                        plane.hp = 0;
                        defeat_show = now;
                        defeat_flag = 0;
                        return ;
                    }
                }
            }

        }


        // Create new enemy if the cooldown is over.
        for (i=0; i<MAX_ENEMY; i++) {
            if (enemies[i].hidden && now - enemy_death_time[i] >= MAX_COOLDOWN_ENEMY_DEATH_CD) {
                enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
                //enemies[i].y = enemies[i].h / 2 + (float)rand() / RAND_MAX * (SCREEN_H - enemies[i].h);
                enemies[i].y = 80;
                enemies[i].vx = (float)(MAX - MIN) * rand() / (RAND_MAX + 1.0) + MIN;
                enemies[i].hidden = false;
                enemies[i].hp = enemies[i].max_hp;
                enemies[i].exp = 30;
            }
        }

		// [HACKATHON 2-7]
		// TODO: Shoot if key is down and cool-down is over.
		// 1) Get the time now using 'al_get_time'.
		// 2) If Space key is down in 'key_state' and the time
		//    between now and last shoot is not less that cool
		//    down time.
		// 3) Loop through the bullet array and find one that is hidden.
		//    (This part can be optimized.)
		// 4) The bullet will be found if your array is large enough.
		// 5) Set the last shoot time to now.
		// 6) Set hidden to false (recycle the bullet) and set its x, y to the
		//    front part of your plane.
		// Uncomment and fill in the code below.

		if (key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN) {
			for (i=0; i<MAX_PLAYER_BULLET; i++) {
				if (bullets[i].hidden)
					break;
			}
			if (i == MAX_PLAYER_BULLET)
				return;
			last_shoot_timestamp = now;
			bullets[i].hidden = false;
			bullets[i].x = plane.x;
			bullets[i].y = plane.y-plane.h/2;
			bullets[i].damage = 45;
		}

	}
	else if (active_scene == SCENE_BOSS) {
        int i;
        double now = al_get_time();
        // update plane movement
		plane_boss_mode.vx = plane_boss_mode.vy = 0;
		if (key_state[ALLEGRO_KEY_UP])
			plane_boss_mode.vy -= 3;
		if (key_state[ALLEGRO_KEY_DOWN])
			plane_boss_mode.vy += 3;
		if (key_state[ALLEGRO_KEY_LEFT])
			plane_boss_mode.vx -= 3;
		if (key_state[ALLEGRO_KEY_RIGHT])
			plane_boss_mode.vx += 3;
		// 0.71 is (1/sqrt(2)).
		plane_boss_mode.y += plane_boss_mode.vy * 4 * (plane_boss_mode.vx ? 0.71f : 1);
		plane_boss_mode.x += plane_boss_mode.vx * 4 * (plane_boss_mode.vy ? 0.71f : 1);

		if ((plane_boss_mode.x-plane_boss_mode.w/2) < 0)
			plane_boss_mode.x = plane_boss_mode.w/2;
		else if ((plane_boss_mode.x+plane_boss_mode.w/2) > FULL_SCREEN_W)
			plane_boss_mode.x = FULL_SCREEN_W-plane_boss_mode.w/2;
		if ((plane_boss_mode.y-plane_boss_mode.h/2) < 0)
			plane_boss_mode.y = plane_boss_mode.h/2;
		else if ((plane_boss_mode.y+plane_boss_mode.h/2) > FULL_SCREEN_H)
			plane_boss_mode.y = FULL_SCREEN_H-plane_boss_mode.h/2;

		big_boss.x += big_boss.vx;
        big_boss.y += big_boss.vy;

        //detect if the boss is out of bound.
        if (big_boss.x + big_boss.w/2 > FULL_SCREEN_W) {
            big_boss.x = FULL_SCREEN_W - big_boss.w/2;
            big_boss.vx = (float)(MAX - 5) * rand() / (RAND_MAX + 1.0) + 5;
            big_boss.vx *= -1;
        }
        else if (big_boss.x - big_boss.w/2 < 0) {
            big_boss.x = big_boss.w/2;
            big_boss.vx = (float)(MAX - 5) * rand() / (RAND_MAX + 1.0) + 5;
        }

        // update the boss_bullet coordinates.
        int flag=1;
            for (int j=0; j<MAX_BULLET_BOSS; j++) {
                if (!bullets_boss[j].hidden) {
                    bullets_boss[j].x += bullets_boss[j].vx;
                    bullets_boss[j].y += bullets_boss[j].vy;

                    //check if the bullets_enemy is out of boundary.
                    if (bullets_boss[j].y + bullets_boss[j].h/2 > FULL_SCREEN_H) {
                        bullets_boss[j].x = bullets_boss[j].y = -100;
                        bullets_boss[j].hidden = true;
                    }
                }
                else {
                    if (now - last_boss_shoot_timestamp >= 0.1 && flag) {
                            bullets_boss[j].x = big_boss.x;
                            bullets_boss[j].y = big_boss.y + big_boss.h/2;
                            bullets_boss[j].hidden = false;
                            bullets_boss[j].damage = 150;
                            flag=0;
                            last_boss_shoot_timestamp = now;

                    }
                }
            }

        // update boss_bullet coordinates
        for (i=0;i<MAX_PLAYER_BULLET_BOSS_MODE;i++) {
			if (bullets_boss_mode[i].hidden)
				continue;
			bullets_boss_mode[i].x += bullets_boss_mode[i].vx*2;
			bullets_boss_mode[i].y += bullets_boss_mode[i].vy*2;
			if (bullets_boss_mode[i].y - bullets_boss_mode[i].h/2< 0) {
                bullets_boss_mode[i].hidden = true;
                bullets_boss_mode[i].x = bullets_boss_mode[i].y = -100;
			}

		}

        // player bullet and enemy.
        for (int j=0; j<MAX_PLAYER_BULLET_BOSS_MODE; j++) {
            if (bullets_boss_mode[j].hidden)
                continue;
            if (pnt_in_rect(bullets_boss_mode[j].x, bullets_boss_mode[j].y, big_boss.x, big_boss.y, big_boss.w, big_boss.h)) {
                bullets_boss_mode[j].x = bullets_boss_mode[j].y = -100;
                bullets_boss_mode[j].hidden = true;
                big_boss.hp -= bullets_boss_mode[j].damage;
                if (big_boss.hp<=0) {
                    is_win = true;
                    game_change_scene(SCENE_GAME_END);
                    return ;
                }
            }

        }

        // enemy_bullet and player.

        for (int j=0; j<MAX_BULLET_BOSS; j++) {
            if (bullets_boss[j].hidden)
                continue;
            if (pnt_in_rect(bullets_boss[j].x, bullets_boss[j].y, plane_boss_mode.x, plane_boss_mode.y, plane_boss_mode.w, plane_boss_mode.h)) {
                plane_boss_mode.hp -= bullets_boss[j].damage;
                bullets_boss[j].x = bullets_boss[j].y = -100;
                bullets_boss[j].hidden = true;
                if (plane_boss_mode.hp <= 0) {
                    is_win = false;
                    game_change_scene(SCENE_GAME_END);
                    return ;
                }
            }
        }

		for (int j=0; j<MAX_PLAYER_BULLET_BOSS_MODE; j++) {
            if (bullets_boss_mode[j].hidden) continue;
            for (int k=0; k<MAX_BULLET_BOSS; k++) {
                if (bullets_boss[k].hidden) continue;
                if (pnt_in_rect(bullets_boss[k].x, bullets_boss[k].y, bullets_boss_mode[j].x, bullets_boss_mode[j].y, bullets_boss_mode[j].w, bullets_boss_mode[j].h)) {
                        bullets_boss[k].x = bullets_boss[k].y = -100;
                        bullets_boss[k].hidden = true;
                        bullets_boss_mode[j].x = bullets_boss_mode[j].y = -100;
                        bullets_boss_mode[j].hidden = true;
                }
            }
        }

		if (key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN) {
			for (i=0; i<MAX_PLAYER_BULLET_BOSS_MODE; i++) {
				if (bullets_boss_mode[i].hidden)
					break;
			}
			if (i == MAX_PLAYER_BULLET_BOSS_MODE)
				return;
			last_shoot_timestamp = now;
			bullets_boss_mode[i].hidden = false;
			bullets_boss_mode[i].x = plane_boss_mode.x;
			bullets_boss_mode[i].y = plane_boss_mode.y-plane_boss_mode.h/2;
			bullets_boss_mode[i].damage = 150;
		}

	}
}

void game_draw(void) {
	if (active_scene == SCENE_MENU) {
		al_draw_bitmap(main_img_background, 0, 0, 0);
		al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 77), FULL_SCREEN_W / 2-50, 30, ALLEGRO_ALIGN_CENTER, "PIKA PIKA");
		al_draw_text(font_pirulen_24, al_map_rgb(0, 0, 0), FULL_SCREEN_W / 2 - 50, 300, ALLEGRO_ALIGN_CENTER, "Press enter key to start");
		al_draw_text(font_pirulen_24, al_map_rgb(0, 0, 0), FULL_SCREEN_W / 2 - 50, 350, ALLEGRO_ALIGN_CENTER, "Press B to Boss Mode");
		al_draw_text(font_pirulen_24, al_map_rgb(0, 0, 0), FULL_SCREEN_W / 2 - 50, 400, ALLEGRO_ALIGN_CENTER, "Press E to exit");
		// [HACKATHON 3-5]
		// TODO: Draw settings images.
		// The settings icon should be located at (x, y, w, h) =
		// (SCREEN_W - 48, 10, 38, 38).
		// Change its image according to your mouse position.
		// Uncomment and fill in the code below.
		if (pnt_in_rect(mouse_x, mouse_y, FULL_SCREEN_W - 29, 28, 38, 38))
			al_draw_bitmap(img_settings2, FULL_SCREEN_W - 48, 10, 0);
		else
			al_draw_bitmap(img_settings, FULL_SCREEN_W - 48, 10, 0);
	} else if (active_scene == SCENE_START) {
		int i;
		al_draw_bitmap(start_img_background, 0, 0, 0);
		al_draw_bitmap(img_profile, 0, SCREEN_H, 0);
		al_draw_bitmap(img_profile_vertical, SCREEN_W, 0, 0);

		// create HP "TEXT".
		al_draw_text(font_pirulen_24, al_map_rgb(0, 0, 0), 20, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3 , 0, "HP");

        // create MP "TEXT".
		al_draw_text(font_pirulen_24, al_map_rgb(0, 0, 0), 515, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3 , 0, "MP");

        // create EXP "TEXT".
		al_draw_text(font_pirulen_24, al_map_rgb(0, 0, 0), 1005, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3 , 0, "EXP");

		// create LEVEL "TEXT"
		al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 204), SCREEN_W + 25, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3 -45 , 0, "LV");

		// create SCORE "TEXT"
		al_draw_text(font_pirulen_24, al_map_rgb(51, 51, 51), SCREEN_W + 25, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3 -5 , 0, "SCORE");

        // create Q "TEXT" and image
		al_draw_text(font_pirulen_32, al_map_rgb(0, 0, 0), SCREEN_W + 30, SCREEN_H*0.8-70, 0, "Q");
        al_draw_bitmap(img_skill_Q, SCREEN_W + 100, SCREEN_H*0.8-75, 0);

        // create W "TEXT" and image
		al_draw_text(font_pirulen_32, al_map_rgb(0, 0, 0), SCREEN_W + 25, SCREEN_H*0.8+10, 0, "W");
		al_draw_bitmap(img_skill_W, SCREEN_W + 95, SCREEN_H*0.8+5, 0);

        // create E "TEXT" and image
		al_draw_text(font_pirulen_32, al_map_rgb(0, 0, 0), SCREEN_W + 30, SCREEN_H*0.8+90, 0, "E");
		al_draw_bitmap(img_skill_E, SCREEN_W + 100, SCREEN_H*0.8+85, 0);

		// create pause and exit image
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W + 80, 50, 60, 60))
            al_draw_bitmap(img_pause2, SCREEN_W + 50, 20, 0);
        else
            al_draw_bitmap(img_pause, SCREEN_W + 50, 20, 0);
        if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W + 160, 50, 60, 60))
            al_draw_bitmap(img_exit2, SCREEN_W + 130, 20, 0);
        else
            al_draw_bitmap(img_exit, SCREEN_W + 130, 20, 0);

		// [HACKATHON 2-8]
		// TODO: Draw all bullets in your bullet array.
		// Uncomment and fill in the code below.
		for (i=0;i<MAX_PLAYER_BULLET;i++)
			draw_movable_object(bullets[i]);
		draw_movable_object(plane);

		// draw skill_Q bullets.
		for (i=0; i<MAX_PLAYER_BULLET_Q; i++)
            draw_movable_object(bullets_Q[i]);

        // draw skill_W bullet.
        if (!bullets_W.hidden) {
            switch (W_skill_stage) {
            case 1:
                al_draw_rotated_bitmap(bullets_W.img, bullets_W.w/2, bullets_W.h/2, bullets_W.x, bullets_W.y, degree_W, 0);
                break;
            case 2:
                al_draw_scaled_rotated_bitmap(bullets_W.img, bullets_W.w/2, bullets_W.h/2, bullets_W.x, bullets_W.y, 1+scale_W, 1+scale_W, degree_W, 0);
                //al_play_sample(W_sound, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &W_sound_id);
                break;
            /*case 3:
                al_draw_bitmap(bullets_W.img, bullets_W.x - bullets_W.w/2, bullets_W.y - bullets_W.h/2, 0);
                break;*/
            }
        }
        // draw skill_E bullet and image.
        if(!plane_clone[0].hidden || !plane_clone[1].hidden || !plane_clone[2].hidden || !plane_clone[3].hidden) {
            switch (E_skill_stage) {
            case 1:
                al_draw_bitmap(img_E_ANIMATION, SCREEN_W/5, SCREEN_H/5, 0);
                al_play_sample(E_sound, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &E_sound_id);
                break;
            case 2:
                for (i=0; i<MAX_CLONE; i++) {
                    draw_movable_object(plane_clone[i]);
                }
                //al_draw_bitmap(img_E_ANIMATION, SCREEN_W/2, SCREEN_H/2, 0);
                break;
            case 3:
                for (i=0; i<MAX_CLONE; i++) {
                    draw_movable_object(plane_clone[i]);
                }
                break;
            }
        }
        for (i=0; i<MAX_CLONE; i++) {
            for (int j=0; j<MAX_CLONE_BULLET; j++) {
                draw_movable_object(bullets_clone[i][j]);
            }
        }

		// draw information BAR.
		// 1) HP_BAR and HP info.
		al_draw_rectangle(70, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3, 70+SCREEN_W/4, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)*2/3, al_map_rgb(0,0,0),2.5);
		al_draw_filled_rectangle(70, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3, 70 + (plane.hp/plane.max_hp) * SCREEN_W/4, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)*2/3, al_map_rgb(244,80,66));
		al_draw_textf(font_pirulen_20, al_map_rgb(0, 0, 0), 70+SCREEN_W/8, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3 +5 , 0, "%.0f/%.0f", plane.hp, plane.max_hp);

		// 2) MP_BAR and MP info.
		al_draw_rectangle(200+SCREEN_W/4, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3, 200 + 2*SCREEN_W/4, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)*2/3, al_map_rgb(0,0,0),2.5);
		al_draw_filled_rectangle(200 + SCREEN_W/4, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3, 200+SCREEN_W/4 + (plane.mp/plane.max_mp) * SCREEN_W/4, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)*2/3, al_map_rgb(128,170,255));
		al_draw_textf(font_pirulen_20, al_map_rgb(0, 0, 0), 200+SCREEN_W/4+SCREEN_W/8, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3 +5 , 0, "%.0f/%.0f", plane.mp, plane.max_mp);

		// 3) EXR_BAR and EXP info.
		al_draw_rectangle(330+2*SCREEN_W/4, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3, 330+3*SCREEN_W/4, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)*2/3, al_map_rgb(0,0,0),2.5);
		al_draw_filled_rectangle(330+2*SCREEN_W/4, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3, 330+SCREEN_W/4*2 + (plane.exp/plane.max_exp) * SCREEN_W/4, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)*2/3, al_map_rgb(255,255,77));
		al_draw_textf(font_pirulen_20, al_map_rgb(0, 0, 0), 330+2*SCREEN_W/4+SCREEN_W/8, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3 +5 , 0, "%.0f/%.0f", plane.exp, plane.max_exp);

		// 4) LEVEL info
		al_draw_textf(font_pirulen_24, al_map_rgb(255, 255, 204), SCREEN_W + 80, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3 -45 , 0, "%.0f", plane.level);

		// 5) SCORE info
		al_draw_textf(font_pirulen_24, al_map_rgb(51, 51, 51), SCREEN_W + 25, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3 +25 , 0, "%.0f", plane.score);

        // 6) Q CD info
		al_draw_circle(SCREEN_W + 45, SCREEN_H*0.8-50, 30, al_map_rgb(255, 255, 204), 8.5);
		al_draw_arc(SCREEN_W + 45, SCREEN_H*0.8-50, 30, 0, (plane.Q_skill_remain/MAX_COOLDOWN_Q)*2*pi, al_map_rgb(77, 148, 255), 8.5);

		// 7) W CD info
		al_draw_circle(SCREEN_W + 45, SCREEN_H*0.8+27, 30, al_map_rgb(255, 255, 204), 8.5);
		al_draw_arc(SCREEN_W + 45, SCREEN_H*0.8+27, 30, 0, (plane.W_skill_remain/MAX_COOLDOWN_W)*2*pi, al_map_rgb(255, 209, 26), 8.5);

		// 8) E CD info
		al_draw_circle(SCREEN_W + 45, SCREEN_H*0.8+110, 30, al_map_rgb(255, 255, 204), 8.5);
		al_draw_arc(SCREEN_W + 45, SCREEN_H*0.8+110, 30, 0, (plane.E_skill_remain/MAX_COOLDOWN_E)*2*pi, al_map_rgb(121, 255, 77), 8.5);

		for (i = 0; i < MAX_ENEMY; i++){
            draw_movable_object(enemies[i]);
            al_draw_rectangle(enemies[i].x-HP_BAR_LEN, enemies[i].y-enemies[i].h/2-HP_BAR, enemies[i].x+HP_BAR_LEN , enemies[i].y-enemies[i].h/2, al_map_rgb(0,0,0),2);
            al_draw_filled_rectangle(enemies[i].x-HP_BAR_LEN, enemies[i].y-enemies[i].h/2-HP_BAR, (enemies[i].x-HP_BAR_LEN) + (enemies[i].hp/enemies[i].max_hp) * HP_BAR_LEN*2, enemies[i].y-enemies[i].h/2, al_map_rgb(197,244,66));
            for (int j=0; j<MAX_BULLET; j++)
                draw_movable_object(bullets_enemy[i][j]);
		}

        // if the defeat_flag==0, the game is over,
	    // only need to draw the defeat img and remain the firm game backgroud.
	    if (defeat_flag == 0) {
            al_draw_bitmap(img_defeat, SCREEN_W/5+25, SCREEN_H/5, 0);
           // al_stop_sample(&start_bgm_id);
            al_play_sample(defeat_sound_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &defeat_sound_bgm_id);
	    }

	    if (isconverted_stop_start) {
            if (al_get_time() - change_scene_delay >= 1.1) {
                al_draw_text(font_pirulen_32, al_map_rgb(255, 0, 0), SCREEN_W/2, SCREEN_H/2, 0, "1");
            }
            else if (al_get_time() - change_scene_delay >= 0.1)
                al_draw_text(font_pirulen_32, al_map_rgb(255, 0, 0), SCREEN_W/2, SCREEN_H/2, 0, "2");
	    }

	}
	// [HACKATHON 3-9]
	// TODO: If active_scene is SCENE_SETTINGS.
	// Draw anything you want, or simply clear the display.
	else if (active_scene == SCENE_SETTINGS) {
		al_draw_bitmap(img_settings_bg, 0, 0, 0);
		if (pnt_in_rect(mouse_x, mouse_y, FULL_SCREEN_W - 29, 28, 38, 38))
			al_draw_bitmap(img_settings2, FULL_SCREEN_W - 48, 10, 0);
		else
			al_draw_bitmap(img_settings, FULL_SCREEN_W - 48, 10, 0);
	}
	else if (active_scene == SCENE_STOP) {
        al_draw_bitmap(img_stop, 0, 0, 0);

        // create resume and exit image.
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W + 80, 50, 60, 60))
            al_draw_bitmap(img_resume2, SCREEN_W + 50, 20, 0);
        else
            al_draw_bitmap(img_resume, SCREEN_W + 50, 20, 0);
        if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W + 160, 50, 60, 60))
            al_draw_bitmap(img_exit2, SCREEN_W + 130, 20, 0);
        else
            al_draw_bitmap(img_exit, SCREEN_W + 130, 20, 0);
	}
	else if (active_scene == SCENE_BOSS) {
        al_draw_bitmap(boss_bg, 0, 0, 0);

        // draw boss hp info.
        al_draw_rectangle(0, 0, FULL_SCREEN_W, 100, al_map_rgb(0,0,0),2.5);
		al_draw_filled_rectangle(0, 0, (big_boss.hp/big_boss.max_hp) * FULL_SCREEN_W, 100, al_map_rgb(244,80,66));
		al_draw_textf(font_pirulen_20, al_map_rgb(0, 0, 0), FULL_SCREEN_W/2-50, 100, 0, "%.0f/%.0f", big_boss.hp, big_boss.max_hp);

        // draw player hp info.
        al_draw_rectangle(55, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3, 55+SCREEN_W/4, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)*2/3, al_map_rgb(0,0,0),2.5);
		al_draw_filled_rectangle(55, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3, 55 + (plane_boss_mode.hp/plane_boss_mode.max_hp) * SCREEN_W/4, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)*2/3, al_map_rgb(244,80,66));
		al_draw_textf(font_pirulen_20, al_map_rgb(0, 0, 0), 70+SCREEN_W/8, SCREEN_H + (FULL_SCREEN_H-SCREEN_H)/3 +5 , 0, "%.0f/%.0f", plane_boss_mode.hp, plane_boss_mode.max_hp);

        draw_movable_object(plane_boss_mode);
        draw_movable_object(big_boss);
        for (int i=0; i<MAX_PLAYER_BULLET_BOSS_MODE; i++)
            draw_movable_object(bullets_boss_mode[i]);
        for (int i=0; i<MAX_BULLET_BOSS; i++)
            draw_movable_object(bullets_boss[i]);
	}
	else if (active_scene == SCENE_GAME_END) {
        al_draw_bitmap(img_game_end, 0, 0, 0);
        if (is_win) {
            al_draw_textf(font_pirulen_32, al_map_rgb(0, 0, 0), FULL_SCREEN_W/2 -50, FULL_SCREEN_H/2 -100, ALLEGRO_ALIGN_CENTER, "YOUR SCORE: %.0f", plane.score+10000);
            al_draw_text(font_pirulen_32, al_map_rgb(0, 0, 0), FULL_SCREEN_W/2 -50, FULL_SCREEN_H/2-250 , ALLEGRO_ALIGN_CENTER, "Congratulations!");
        }
        else {
            al_draw_textf(font_pirulen_32, al_map_rgb(0, 0, 0), FULL_SCREEN_W/2 -50, FULL_SCREEN_H/2 -100, ALLEGRO_ALIGN_CENTER, "YOUR SCORE: %.0f", plane.score+big_boss.max_hp-big_boss.hp);
            al_draw_text(font_pirulen_32, al_map_rgb(0, 0, 0), FULL_SCREEN_W/2 -50, FULL_SCREEN_H/2-250 , ALLEGRO_ALIGN_CENTER, "Better luck next time~");
        }
		plane.score = 0;
		al_draw_text(font_pirulen_32, al_map_rgb(0, 0, 0), FULL_SCREEN_W / 1.25 -350 , FULL_SCREEN_H*5/6 +60, 0, "Press E to EXIT");
        al_draw_text(font_pirulen_32, al_map_rgb(0, 0, 0), FULL_SCREEN_W / 1.25 -350 , FULL_SCREEN_H*5/6 +120, 0, "Press R to Restart");
	}
	al_flip_display();
}

void game_destroy(void) {
	// Destroy everything you have created.
	// Free the memories allocated by malloc or allegro functions.
	// Destroy shared resources.
	al_destroy_font(font_pirulen_32);
	al_destroy_font(font_pirulen_24);
	al_destroy_font(font_pirulen_20);

	/* Menu Scene resources*/
	al_destroy_bitmap(main_img_background);
	al_destroy_sample(main_bgm);
	// [HACKATHON 3-6]
	// TODO: Destroy the 2 settings images.
	// Uncomment and fill in the code below.
	al_destroy_bitmap(img_settings);
	al_destroy_bitmap(img_settings2);
	al_destroy_bitmap(img_settings_bg);
	al_destroy_bitmap(img_skill_Q);
	al_destroy_bitmap(img_skill_W);
    al_destroy_bitmap(img_skill_E);

    al_destroy_bitmap(img_stop);
    al_destroy_sample(stop_bgm);

    al_destroy_bitmap(img_exit);
    al_destroy_bitmap(img_resume);
    al_destroy_bitmap(img_pause);
    al_destroy_bitmap(img_exit2);
    al_destroy_bitmap(img_resume2);
    al_destroy_bitmap(img_pause2);

	/* Start Scene resources*/
	al_destroy_bitmap(start_img_background);
	al_destroy_bitmap(start_img_plane);
	al_destroy_bitmap(start_img_enemy);
	al_destroy_sample(start_bgm);

	al_destroy_bitmap(img_profile);
	al_destroy_bitmap(img_profile_vertical);

	al_destroy_bitmap(img_game_end);
	al_destroy_sample(game_end_bgm);

	al_destroy_bitmap(img_defeat);
	al_destroy_sample(defeat_sound_bgm);
	// [HACKATHON 2-9]
	// TODO: Destroy your bullet image.
	// Uncomment and fill in the code below.
	al_destroy_bitmap(img_bullet);
	al_destroy_bitmap(img_bullet_Q);
	al_destroy_bitmap(img_bullet_W);
	al_destroy_bitmap(img_bullet_W_ult);
	al_destroy_bitmap(img_bullet_clone);
	al_destroy_bitmap(img_E_ANIMATION);
	al_destroy_bitmap(img_E_SMOKE);
	al_destroy_sample(W_sound);
	al_destroy_sample(E_sound);
	al_destroy_bitmap(img_enemies_bullet);
	al_destroy_bitmap(img_boss);
	al_destroy_bitmap(img_boss_weapon);
	al_destroy_bitmap(boss_bg);
	al_destroy_sample(boss_bgm);

	al_destroy_timer(game_update_timer);
	al_destroy_event_queue(game_event_queue);
	al_destroy_display(game_display);
	free(mouse_state);
}

void game_change_scene(int next_scene) {
    int temp;
	game_log("Change scene from %d to %d", active_scene, next_scene);
	// TODO: Destroy resources initialized when creating scene.
	if (active_scene == SCENE_MENU) {
		al_stop_sample(&main_bgm_id);
		game_log("stop audio (bgm)");
	} else if (active_scene == SCENE_START) {
		al_stop_sample(&start_bgm_id);
		//al_stop_sample(&W_sound_id); al_stop_sample(&E_sound_id);
		game_log("stop audio (bgm)");
	} else if (active_scene == SCENE_GAME_END) {
        al_stop_sample(&game_end_bgm_id);
        game_log("stop audio (bgm)");
	} else if (active_scene == SCENE_STOP) {
        al_stop_sample(&stop_bgm_id);
        game_log("stop audio (bgm)");
	} else if (active_scene == SCENE_BOSS) {
        al_stop_sample(&boss_bgm_id);
        game_log("stop audio (bgm)");
	}
	temp = active_scene;
	active_scene = next_scene;
	// TODO: Allocate resources before entering scene.
	if (active_scene == SCENE_MENU) {
		if (!al_play_sample(main_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &main_bgm_id))
			game_abort("failed to play audio (bgm)");
	} else if (active_scene == SCENE_START && temp != SCENE_STOP) {
		int i;
		plane.img = start_img_plane;
		plane.x = 400;
		plane.y = 500;
		plane.w = 150;
		plane.h = 125;
		plane.Q_skill = plane.W_skill = plane.E_skill = 0;
		plane.Q_skill_remain = MAX_COOLDOWN_Q;
		plane.W_skill_remain = MAX_COOLDOWN_W;
		plane.E_skill_remain = MAX_COOLDOWN_E;
		plane.score = 0;
		plane.level = 1;
		plane.hp = plane.max_hp = 200;
		plane.mp = plane.max_mp = 100;
		plane.exp = 0;
		plane.max_exp = 250;

		// load the skill_Q equipment.
		for (i=0; i<MAX_PLAYER_BULLET_Q; i++) {
            bullets_Q[i].img = img_bullet_Q;
            bullets_Q[i].w = al_get_bitmap_width(img_bullet_Q);
            bullets_Q[i].h = al_get_bitmap_height(img_bullet_Q);
            bullets_Q[i].x = plane.x;
            bullets_Q[i].y = plane.y - plane.h/2;
            bullets_Q[i].damage = 100;
            bullets_Q[i].hidden = true;
            bullets_Q[i].vy = -3.5;
		}
		bullets_Q[0].vx = -3.5;
		bullets_Q[1].vx = 0;
		bullets_Q[2].vx = 3.5;

		// load the skill_W equipment
		bullets_W.img = img_bullet_W;
		bullets_W.w = al_get_bitmap_width(img_bullet_W);
        bullets_W.h = al_get_bitmap_height(img_bullet_W);
        bullets_W.x = -100;
        bullets_W.y = -100;
        bullets_W.damage = 0;
        bullets_W.hidden = true;
        bullets_W.vy = -3.5;

        // load the skill_E equipment
        for (i=0; i<MAX_CLONE; i++) {
            plane_clone[i].hidden = true;
            plane_clone[i].img = img_E_SMOKE;
            for (int j=0; j<MAX_CLONE_BULLET; j++) {
                bullets_clone[i][j].img = img_bullet_clone;
                bullets_clone[i][j].w = al_get_bitmap_width(img_bullet_clone);
                bullets_clone[i][j].h = al_get_bitmap_height(img_bullet_clone);
                bullets_clone[i][j].vy = -2.5;
                bullets_clone[i][j].vx = 0;
                bullets_clone[i][j].y = -100;
                bullets_clone[i][j].x = -100;
                bullets_clone[i][j].damage = 60;
                bullets_clone[i][j].hidden = true;
            }
        }

		for (i = 0; i < MAX_ENEMY; i++) {
			enemies[i].w = 28;
			enemies[i].h = 68;
			enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
			//enemies[i].y = enemies[i].h / 2 + (float)rand() / RAND_MAX * (SCREEN_H - enemies[i].h);
			enemies[i].y = 80;
			enemies[i].hp = enemies[i].max_hp = 100;
			enemies[i].exp = 30;
			enemies[i].vx = (float)(MAX - MIN) * rand() / (RAND_MAX + 1.0) + MIN;
			enemies[i].img = start_img_enemy;
			for (int j=0;j<MAX_BULLET;j++) {
                bullets_enemy[i][j].w = al_get_bitmap_width(img_enemies_bullet);
                bullets_enemy[i][j].h = al_get_bitmap_height(img_enemies_bullet);
                bullets_enemy[i][j].img = img_enemies_bullet;
                bullets_enemy[i][j].vx = 0;
                bullets_enemy[i][j].vy = 4.5;
                bullets_enemy[i][j].x = bullets_enemy[i][j].y = -100;
                bullets_enemy[i][j].damage = 15;
                bullets_enemy[i][j].hidden = true;
			}

		}
        /*
        al_draw_arc(SCREEN_W + 45, SCREEN_H*0.8-50, 30, 0, 0.5, al_map_rgb(77, 148, 255), 8.5);
        al_draw_arc(SCREEN_W + 45, SCREEN_H*0.8+27, 30, 0, 0.5, al_map_rgb(255, 209, 26), 8.5);
		al_draw_arc(SCREEN_W + 45, SCREEN_H*0.8+110, 30, 0, 0.5, al_map_rgb(121, 255, 77), 8.5);
        */

		// [HACKATHON 2-5-2]
		// TODO: Initialize bullets.
		// For each bullets in array, set their w and h to the size of
		// the image, and set their img to bullet image, hidden to true,
		// (vx, vy) to (0, -3).
		// Uncomment and fill in the code below.
		for (int i=0;i<MAX_PLAYER_BULLET;i++) {
			bullets[i].w = al_get_bitmap_width(img_bullet);
			bullets[i].h = al_get_bitmap_height(img_bullet);
			bullets[i].img = img_bullet;
			bullets[i].vx = 0;
			bullets[i].vy = -4;
			bullets[i].damage = 45;
			bullets[i].hidden = true;
		}
		if (!al_play_sample(start_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &start_bgm_id))
			game_abort("failed to play audio (bgm)");
	}
	// the scene change from stop to start;
	else if (active_scene == SCENE_START && temp == SCENE_STOP) {
        isconverted_stop_start = true;
        if (!al_play_sample(start_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &start_bgm_id))
			game_abort("failed to play audio (bgm)");
	}
	else if (active_scene == SCENE_GAME_END) {
        if (!al_play_sample(game_end_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &game_end_bgm_id))
			game_abort("failed to play audio (bgm)");
	}
	else if (active_scene == SCENE_STOP) {
        if (!al_play_sample(stop_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &stop_bgm_id))
			game_abort("failed to play audio (bgm)");
	}
	else if (active_scene == SCENE_BOSS) {
        plane_boss_mode.img = start_img_plane;
        plane_boss_mode.x = FULL_SCREEN_W/3 + 250;
        plane_boss_mode.y = FULL_SCREEN_H - 75;
        plane_boss_mode.w = 150;
        plane_boss_mode.h = 125;
        plane_boss_mode.hp = plane_boss_mode.max_hp = 2000;

        for (int i=0; i<MAX_PLAYER_BULLET_BOSS_MODE; i++) {
            bullets_boss_mode[i].w = al_get_bitmap_width(img_bullet);
            bullets_boss_mode[i].h = al_get_bitmap_height(img_bullet);
            bullets_boss_mode[i].img = img_bullet;
            bullets_boss_mode[i].vx = 0;
            bullets_boss_mode[i].vy = -4;
            bullets_boss_mode[i].x = bullets_boss_mode[i].y = -100;
            bullets_boss_mode[i].damage = 125;
            bullets_boss_mode[i].hidden = true;
        }

        big_boss.img = img_boss;
        big_boss.w = al_get_bitmap_width(img_boss);
        big_boss.h = al_get_bitmap_height(img_boss);
        big_boss.y = 190;
        big_boss.x = FULL_SCREEN_W/3 +250;
        big_boss.hp = big_boss.max_hp = 10000;
        big_boss.vx = (float)(MAX - MIN) * rand() / (RAND_MAX + 1.0) + MIN;

        for (int i=0; i<MAX_BULLET_BOSS; i++) {
            bullets_boss[i].img = img_enemies_bullet;
            bullets_boss[i].w = al_get_bitmap_width(img_enemies_bullet);
            bullets_boss[i].h = al_get_bitmap_height(img_enemies_bullet);
            bullets_boss[i].vx = 0;
            bullets_boss[i].vy = 4;
            bullets_boss[i].x = bullets_boss[i].y = -100;
            bullets_boss[i].damage = 200;
            bullets_boss[i].hidden = true;

        }
        if (!al_play_sample(boss_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &boss_bgm_id))
			game_abort("failed to play audio (bgm)");
	}
	else if (active_scene == SCENE_EXIT) {
        done = true;
	}
}

void on_key_down(int keycode) {
	if (active_scene == SCENE_MENU) {
		if (keycode == ALLEGRO_KEY_ENTER)
			game_change_scene(SCENE_START);
        else if (keycode == ALLEGRO_KEY_E)
            game_change_scene(SCENE_EXIT);
        else if (keycode == ALLEGRO_KEY_B)
            game_change_scene(SCENE_BOSS);
	}
	else if (active_scene == SCENE_GAME_END) {
        if (keycode == ALLEGRO_KEY_E)
            game_change_scene(SCENE_EXIT);
        else if (keycode == ALLEGRO_KEY_R)
            game_change_scene(SCENE_MENU);
	}
	else if (active_scene == SCENE_START) {
        if (keycode == ALLEGRO_KEY_P) {
            temp_time = al_get_time();
            stop_came_back_E = true;
            stop_came_back_Q = true;
            stop_came_back_W = true;
            game_change_scene(SCENE_STOP);
        }
	}
	else if (active_scene == SCENE_BOSS) {
        if (keycode == ALLEGRO_KEY_E)
            game_change_scene(SCENE_GAME_END);
	}
}

void on_mouse_down(int btn, int x, int y) {
	// [HACKATHON 3-8]
	// TODO: When settings clicked, switch to settings scene.
	// Uncomment and fill in the code below.
	if (active_scene == SCENE_MENU) {
		if (btn == 1) {
			if (pnt_in_rect(mouse_x, mouse_y, FULL_SCREEN_W - 29, 28, 38, 38))
				game_change_scene(SCENE_SETTINGS);
		}
	}
	else if (active_scene == SCENE_START) {
        if (btn == 1) {
            // check if click the pause.
            if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W + 80, 50, 60, 60)) {
                temp_time = al_get_time();
                stop_came_back_E = true;
                stop_came_back_Q = true;
                stop_came_back_W = true;
                game_change_scene(SCENE_STOP);
            }
            // check if click the exit.
            else if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W + 160, 50, 60, 60)) {
                game_change_scene(SCENE_MENU);
            }
        }
	}
	else if (active_scene == SCENE_STOP) {
        if (btn == 1) {
            // check if click the resume.
            if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W + 80, 50, 60, 60)) {
                change_scene_delay = al_get_time();
                game_change_scene(SCENE_START);
            }
            // check if click the exit.
            else if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W + 160, 50, 60, 60)) {
                game_change_scene(SCENE_MENU);
            }
        }
	}
	else if (active_scene == SCENE_SETTINGS) {
        if (btn == 1) {
            if (pnt_in_rect(mouse_x, mouse_y, FULL_SCREEN_W - 29, 28, 38, 38))
				game_change_scene(SCENE_MENU);
        }
	}
}

void level_up(int level){
    plane.hp += 30;
    plane.max_hp += 30;
    plane.mp += 25;
    plane.max_mp += 25;
    plane.mp = plane.max_mp;
    plane.hp = plane.max_hp;
    plane.max_exp += 100;
    for (int i=0; i<MAX_PLAYER_BULLET; i++)
        bullets[i].damage += 20;
    for (int i=0; i<MAX_PLAYER_BULLET_Q; i++)
        bullets_Q[i].damage += 5;
    for (int i=0; i<MAX_CLONE; i++) {
        for (int j=0; j<MAX_CLONE_BULLET; j++) {
            bullets_clone[i][j].damage += 5;
        }
    }

    for (int i=0; i<MAX_ENEMY; i++){
        //enemies[i].hp += 15;
        enemies[i].max_hp += 15;
        for (int j=0; j<MAX_BULLET; j++)
            bullets_enemy[i][j].damage += 5;
    }
    if (level == 5) {
        game_change_scene(SCENE_BOSS);
    }

}

void draw_movable_object(MovableObject obj) {
	if (obj.hidden)
		return;
	al_draw_bitmap(obj.img, round(obj.x - obj.w / 2), round(obj.y - obj.h / 2), 0);
}

ALLEGRO_BITMAP *load_bitmap_resized(const char *filename, int w, int h) {
	ALLEGRO_BITMAP* loaded_bmp = al_load_bitmap(filename);
	if (!loaded_bmp)
		game_abort("failed to load image: %s", filename);
	ALLEGRO_BITMAP *resized_bmp = al_create_bitmap(w, h);
	ALLEGRO_BITMAP *prev_target = al_get_target_bitmap();

	if (!resized_bmp)
		game_abort("failed to create bitmap when creating resized image: %s", filename);
	al_set_target_bitmap(resized_bmp);
	al_draw_scaled_bitmap(loaded_bmp, 0, 0,
		al_get_bitmap_width(loaded_bmp),
		al_get_bitmap_height(loaded_bmp),
		0, 0, w, h, 0);
	al_set_target_bitmap(prev_target);
	al_destroy_bitmap(loaded_bmp);

	game_log("resized image: %s", filename);

	return resized_bmp;
}

// [HACKATHON 3-3]
// TODO: Define bool pnt_in_rect(int px, int py, int x, int y, int w, int h)
// Uncomment and fill in the code below.
bool pnt_in_rect(int px, int py, int x, int y, int w, int h) {
    if(px>=x-w/2 && px<=x+w/2 && py>=y-h/2 && py<=y+h/2)
        return true;
    else
        return false;
}


// +=================================================================+
// | Code below is for debugging purpose, it's fine to remove it.    |
// | Deleting the code below and removing all calls to the functions |
// | doesn't affect the game.                                        |
// +=================================================================+

void game_abort(const char* format, ...) {
	va_list arg;
	va_start(arg, format);
	game_vlog(format, arg);
	va_end(arg);
	fprintf(stderr, "error occured, exiting after 2 secs");
	// Wait 2 secs before exiting.
	al_rest(2);
	// Force exit program.
	exit(1);
}

void game_log(const char* format, ...) {
#ifdef LOG_ENABLED
	va_list arg;
	va_start(arg, format);
	game_vlog(format, arg);
	va_end(arg);
#endif
}

void game_vlog(const char* format, va_list arg) {
#ifdef LOG_ENABLED
	static bool clear_file = true;
	vprintf(format, arg);
	printf("\n");
	// Write log to file for later debugging.
	FILE* pFile = fopen("log.txt", clear_file ? "w" : "a");
	if (pFile) {
		vfprintf(pFile, format, arg);
		fprintf(pFile, "\n");
		fclose(pFile);
	}
	clear_file = false;
#endif
}

