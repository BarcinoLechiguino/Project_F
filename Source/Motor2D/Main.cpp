#include <stdlib.h>

#include "Definitions.h"
#include "Log.h"
#include "Application.h"

// This is needed here because SDL redefines main function
// do not add any other libraries here, instead put them in their modules
#include "SDL/include/SDL.h"
#include "Brofiler/Brofiler.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )

#pragma comment( lib, "Brofiler/ProfilerCore32.lib")

enum class MainState
{
	CREATE = 1,
	AWAKE,
	START,
	LOOP,
	CLEAN,
	FAIL,
	EXIT
};

Application* App = NULL;

int main(int argc, char* args[])
{
	LOG("Engine starting ... %d");

	MainState state = MainState::CREATE; 
	int result = EXIT_FAILURE;

	while(state != MainState::EXIT)
	{
		BROFILER_FRAME("PROJECT F Brofiler Test");
		switch(state)
		{

			// Allocate the engine --------------------------------------------
			case MainState::CREATE:
			LOG("CREATION PHASE ===============================");

			App = new Application(argc, args);

			if(App != NULL)
				state = MainState::AWAKE;
			else
				state = MainState::FAIL;

			break;

			// Awake all modules -----------------------------------------------
			case MainState::AWAKE:
			LOG("AWAKE PHASE ===============================");
			if(App->Awake() == true)
				state = MainState::START;
			else
			{
				LOG("ERROR: Awake failed");
				state = MainState::FAIL;
			}

			break;

			// Call all modules before first frame  ----------------------------
			case MainState::START:
			LOG("START PHASE ===============================");
			if(App->Start() == true)
			{
				state = MainState::LOOP;
				LOG("UPDATE PHASE ===============================");
			}
			else
			{
				state = MainState::FAIL;
				LOG("ERROR: Start failed");
			}
			break;

			// Loop all modules until we are asked to leave ---------------------
			case MainState::LOOP:
			{
				
				if (App->Update() == false)
				{
					LOG("Update is false :(");
					state = MainState::CLEAN;
				}
			}
			
			break;

			// Cleanup allocated memory -----------------------------------------
			case MainState::CLEAN:
			LOG("CLEANUP PHASE ===============================");
			if(App->CleanUp() == true)
			{
				RELEASE(App);
				result = EXIT_SUCCESS;
				state = MainState::EXIT;
			}
			else
				state = MainState::FAIL;

			break;

			// Exit with errors and shame ---------------------------------------
			case MainState::FAIL:
			LOG("Exiting with errors :(");
			result = EXIT_FAILURE;
			state = MainState::EXIT;
			break;
		}
	}

	LOG("... Bye! :)\n");

	// Dump memory leaks
	return result;
}