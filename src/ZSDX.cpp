/**
 * This module initializes the game and handles the screen.
 */

#include "ZSDX.h"
#include "FileTools.h"
#include "Color.h"
#include "Music.h"
#include "Sound.h"
#include "Game.h"
#include "GameResource.h"
#include "Savegame.h"
#include "TitleScreen.h"
#include "SelectionMenu.h"
#include "TextDisplayer.h"


/**
 * Number of times the screen is redrawn is a second.
 */
const int FRAMES_PER_SECOND = 50;

/**
 * Delay between two frames in millisecond.
 */
const int FRAME_INTERVAL = 1000 / FRAMES_PER_SECOND;

/**
 * Global variable to get the ZSDX instance
 * from anywhere in the code.
 */
ZSDX *zsdx = NULL;

/**
 * Initializes the game engine.
 */
ZSDX::ZSDX(void) {

  // initialize SDL
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  SDL_WM_SetCaption("Zelda Solarus Deluxe", NULL);

  SDL_ShowCursor(SDL_ENABLE);
  set_fullscreen(false);
 
  color_init();

  // set the timer
  SDL_AddTimer(250, TileAnimationManager::increment_frame_counter, NULL);

  // initialize the audio system
  Music::initialize();

  // initialize the game resource
  game_resource = new GameResource();
  game = NULL;

  text_displayer = new TextDisplayer();
}

/**
 * Cleans everything.
 */
ZSDX::~ZSDX(void) {
  SDL_Quit();
  Music::exit();
  delete game_resource;
  delete text_displayer;
}

/**
 * Sets the full screen mode or the windowed mode.
 * @param fullscreen true for full screen mode, false for the windowed mode
 */
void ZSDX::set_fullscreen(bool fullscreen) {
  if (fullscreen) {
    screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
    SDL_ShowCursor(SDL_DISABLE);
  }
  else {
    screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_ShowCursor(SDL_ENABLE);
  }
  this->fullscreen = fullscreen;
}

/**
 * Returns whether the game is in full screen.
 * @return true if the game is in full screen mode, false otherwise
 */
bool ZSDX::is_fullscreen(void) {
  return fullscreen;
}

/**
 * Switches between full screen mode and windowed mode.
 */
void ZSDX::switch_fullscreen(void) {
  set_fullscreen(!fullscreen);
}

/**
 * Launches the game.
 */
void ZSDX::main(void) {

  bool quit = false;
  while (!quit) {
    
    // title screen
    TitleScreen *title_screen = new TitleScreen();
    quit = title_screen->show();
    delete title_screen;

    if (!quit) {

      // savegame selection menu
      SelectionMenu *menu = new SelectionMenu();
      menu->show();

      // get the savegame selected (if any)
      Savegame *savegame = menu->get_selected_save();

      if (savegame == NULL) {
	quit = true;
	delete menu;
      }
      else {

	// launch the mode selected by the player
	bool is_adventure_mode = menu->is_adventure_mode();
	delete menu;

	if (is_adventure_mode) {
	  launch_adventure_mode(savegame);
	}
	else {
	  launch_solarus_dreams_mode(savegame);
	}

	delete savegame;
      }
    }
  }
}

/**
 * This function handles an SDL event.
 * In any SDL main loop, you should get the event
 * with SDL_PollEvent() and call this function.
 * @return true if the user closed the window
 */
bool ZSDX::handle_event(const SDL_Event &event) {

  bool quit = false;

  switch (event.type) {
    
    // quit if the user closes the window
  case SDL_QUIT:
    quit = true;
    break;
	
    // a key is pressed
  case SDL_KEYDOWN:
    switch (event.key.keysym.sym) {

      // escape: quit
    case SDLK_ESCAPE:
      quit = true;
      break;
      
      // F5: full screen / windowed mode
    case SDLK_F5:
      switch_fullscreen();
      break;

    default:
      break;
    }
    break;
  }

  return quit;
}

/**
 * Launches the adventure mode: shows the game file selection screen
 * and starts the game with the selected file.
 */
void ZSDX::launch_adventure_mode(Savegame *savegame) {

  /*
  // debug: test Link's equipment
  savegame->set_reserved_integer(SAVEGAME_LINK_TUNIC, 0); // green tunic
  savegame->set_reserved_integer(SAVEGAME_LINK_SWORD, 1); // first sword
  savegame->set_reserved_integer(SAVEGAME_LINK_SHIELD, 1); // first shield
  */

  // create the game
  game = new Game(savegame);
  game->play();

  delete game;
  game = NULL;
}

/**
 * Launches the Solarus Dreams mode.
 */
void ZSDX::launch_solarus_dreams_mode(Savegame *savegame) {

  /* TODO: make another class to display the menus of this mode

Lorsqu'il lance ce mode, le jeu affiche les écrans suivants :

1. Le joueur doit saisir son login et son mot de passe du forum.
2. Le jeu se connecte au serveur et en cas de réussite, télécharge les derniers niveaux éventuellement ajoutés.
3. Ecran de sélection des niveaux. La liste des niveaux s'affiche, avec pour chaque niveau : son nom, le nom de son créateur, la difficulté, les médailles obtenues par le joueur.
4. Une fois un niveau choisi, un écran plus détaillé affiche toutes les informations du niveau, avec une description textuelle, un screenshot, et le nombre de joueurs ayant déjà réussi chaque objectif. Le joueur doit choisir un des objectifs et peut ensuite lancer le jeu.
  */
}

/**
 * Entry point of the program.
 */
int main(int argc, char **argv) {

  zsdx = new ZSDX();
  zsdx->main();
  delete zsdx;

  return 0;
}
