#include "Common.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL_gpu_shadercross.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <webgpu/webgpu.h>

// Include all the examples here
static Example *Examples[] = {
    &ClearScreen_Example,        &BasicTriangle_Example,
    &BasicVertexBuffer_Example,  &CullMode_Example,
    &BasicStencil_Example,       &InstancedIndexed_Example,
    &TexturedQuad_Example,       &TexturedAnimatedQuad_Example,
    &Clear3DSlice_Example,       &BasicCompute_Example,
    &ComputeUniforms_Example,    &ToneMapping_Example,
    &CustomSampling_Example,     &DrawIndirect_Example,
    &ComputeSpriteBatch_Example, &CopyAndReadback_Example,
    &CopyConsistency_Example,    &Texture2DArray_Example,
    &TriangleMSAA_Example,       &Cubemap_Example,
    &WindowResize_Example,       &Blit2DArray_Example,
    &BlitCube_Example,           &BlitMirror_Example,
    &GenerateMipmaps_Example,
};

static EM_BOOL emsc_dummy_key_callback(int type,
                                       const EmscriptenKeyboardEvent *ev,
                                       void *userdata) {
  if (ev->ctrlKey || ev->altKey || ev->metaKey) {
    SDL_Log("Key down: %s (ctrl/alt/meta)", ev->key);
  }
  return EM_TRUE;
}

static EM_BOOL emsc_dummy_mousebutton_callback(int type,
                                               const EmscriptenMouseEvent *ev,
                                               void *userdata) {
  return EM_TRUE;
}

static EM_BOOL emsc_dummy_mousewheel_callback(int type,
                                              const EmscriptenWheelEvent *ev,
                                              void *userdata) {
  return EM_TRUE;
}

static EM_BOOL emsc_dummy_touch_callback(int type,
                                         const EmscriptenTouchEvent *ev,
                                         void *userdata) {
  return EM_TRUE;
}

// Global variables for managing the state of the example suite.
// ------------------------------------------------------------
Context ctx = {0};
int exampleIndex = -1;
int gotoExampleIndex = 0;
int quit = 0;
float lastTime = 0;
SDL_Gamepad *gamepad = NULL;
bool canDraw = false;
// ------------------------------------------------------------


// Callback for when the window is resized
static bool emsc_fullscreen(char *id) {
  EMSCRIPTEN_RESULT res;
  if (!id) {
    res = emscripten_request_fullscreen("#canvas", 1);
  } else {
    res = emscripten_request_fullscreen(id, 1);
  }

  // Tell the SDL event loop that the window should be resized
  // since we just can't rely on an event being posted when going
  // fullscreen with browser.
  SDL_PushEvent(&(SDL_Event){
      .window =
          (SDL_WindowEvent){
              .type = SDL_EVENT_WINDOW_RESIZED,
              .windowID = SDL_GetWindowID(ctx.Window),
          },
  });
  return res;
}

bool AppLifecycleWatcher(void *userdata, SDL_Event *event) {
  /* This callback may be on a different thread, so let's
   * push these events as USER events so they appear
   * in the main thread's event loop.
   *
   * That allows us to cancel drawing before/after we finish
   * drawing a frame, rather than mid-draw (which can crash!).
   */
  if (event->type == SDL_EVENT_DID_ENTER_BACKGROUND) {
    SDL_Event evt;
    evt.type = SDL_EVENT_USER;
    evt.user.code = 0;
    SDL_PushEvent(&evt);
  } else if (event->type == SDL_EVENT_WILL_ENTER_FOREGROUND) {
    SDL_Event evt;
    evt.type = SDL_EVENT_USER;
    evt.user.code = 1;
    SDL_PushEvent(&evt);
  }
  return false;
}

// Process events for the example suite
// This has a 6ms limit to prevent the browser from hanging
// This is caused by mouse events flooding the event queue
// so we empty the queue of mouse events at 6ms
void process_events(Context *context) {
  // Keep track of when we started processing events
  uint32_t start = SDL_GetTicks();
  context->LeftPressed = 0;
  context->RightPressed = 0;
  context->DownPressed = 0;
  context->UpPressed = 0;

  // Poll for events on frame update.
  SDL_Event evt;
  while (SDL_PollEvent(&evt)) {
    switch (evt.type) {
    case SDL_EVENT_QUIT:
      if (exampleIndex != -1) {
        Examples[exampleIndex]->Quit(context);
      }
      quit = 1;
      break;
    case SDL_EVENT_GAMEPAD_ADDED:
      if (gamepad == NULL) {
        gamepad = SDL_OpenGamepad(evt.gdevice.which);
      }
      break;
    case SDL_EVENT_GAMEPAD_REMOVED:
      if (evt.gdevice.which == SDL_GetGamepadID(gamepad)) {
        SDL_CloseGamepad(gamepad);
      }
      break;
    case SDL_EVENT_USER:
      if (evt.user.code == 0) {
        SDL_Log("App did enter background");
        canDraw = false;
      } else if (evt.user.code == 1) {
        SDL_Log("App will enter foreground");
        canDraw = true;
      }
      break;
    case SDL_EVENT_KEY_DOWN:
      SDL_Log("Key down: %s", SDL_GetKeyName(evt.key.key));
      if (evt.key.key == (unsigned int)'d') {
        gotoExampleIndex = exampleIndex + 1;
        if (gotoExampleIndex >= SDL_arraysize(Examples)) {
          gotoExampleIndex = 0;
        }
      } else if (evt.key.key == (unsigned int)'a') {
        gotoExampleIndex = exampleIndex - 1;
        if (gotoExampleIndex < 0) {
          gotoExampleIndex = SDL_arraysize(Examples) - 1;
        }
      } else if (evt.key.key == SDLK_F11) {
        emsc_fullscreen("#canvas");
      } else if (evt.key.key == SDLK_F12) {
        emscripten_debugger();
      } else if (evt.key.key == SDLK_LEFT) {
        context->LeftPressed = true;
        SDL_Log("Left pressed");
      } else if (evt.key.key == SDLK_RIGHT) {
        context->RightPressed = true;
        SDL_Log("Right pressed");
      } else if (evt.key.key == SDLK_DOWN) {
        context->DownPressed = true;
        SDL_Log("Down pressed");
      } else if (evt.key.key == SDLK_UP) {
        context->UpPressed = true;
        SDL_Log("Up pressed");
      }
      break;
    case SDL_EVENT_MOUSE_MOTION:
      SDL_Log("Mouse motion detected!");
      break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      if (evt.button.button == SDL_BUTTON_LEFT) {
        SDL_Log("Left mouse button pressed!");
      }
      if (evt.button.button == SDL_BUTTON_RIGHT) {
        SDL_Log("Right mouse button pressed!");
      }
      if (evt.button.button == SDL_BUTTON_MIDDLE) {
        SDL_Log("Middle mouse button pressed!");
      }
      break;
    }

    // If we've been processing events for more than 2ms, empty any mouse events
    // in the event queue as there is an issue with mouse events flooding the
    // event queue on browsers.
    if (SDL_GetTicks() - start > 2) {
      SDL_Log("Emptying mouse events from the queue!");
      while (SDL_PeepEvents(&evt, 1, SDL_GETEVENT, SDL_EVENT_MOUSE_MOTION,
                            SDL_EVENT_MOUSE_MOTION) > 0) {
        //
      }
      break;
    }
  } // SDL_Event Processing End
}

// Load one of the examples into our WebGPU context.
static bool load_example(Context *context) {
  int tmp = exampleIndex;
  int depth = 0;
  canDraw = false;

  // If we're already running an example, quit it first
  if (exampleIndex != -1) {
    SDL_Log("QUITTING EXAMPLE: %s", context->ExampleName);
    Examples[exampleIndex]->Quit(context);
  }

  // Cursed goto label to try to catch failed loads
  // Realistically this should probably be a cond loop
  // but what is a loop if not a goto statement in disguise?
load:
  exampleIndex = gotoExampleIndex;
  context->exampleIndex = exampleIndex;
  context->ExampleName = Examples[exampleIndex]->Name;
  SDL_Log("STARTING EXAMPLE: %s", context->ExampleName);
  if (Examples[exampleIndex]->Init(context) < 0) {
    SDL_Log("%s Init failed!", context->ExampleName);
    SDL_Log("QUITTING EXAMPLE: %s", context->ExampleName);
    SDL_Log("Returning to previous example...");
    gotoExampleIndex = tmp;
    if (depth++ < 2)
      goto load;
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to load fallback example: %s", context->ExampleName);
    return false;
  }

  SDL_Log("LOADED EXAMPLE: %s", context->ExampleName);

  // Once the example is loaded, reset the goto index
  gotoExampleIndex = -1;
  canDraw = true;
  return true;
}

static EM_BOOL emsc_frame(double time, void *userdata) {

  Context *context = (Context *)userdata;

  // Calculate the time since the last frame
  float newTime = SDL_GetTicks() / 1000.0f;
  context->DeltaTime = newTime - lastTime;
  lastTime = newTime;

  // Calculate framerate from DeltaTime
  context->FPS = 1.0f / context->DeltaTime;

  // Process any SDL events that have occurred since the last frame
  process_events(context);

  if (gotoExampleIndex != -1) {
    if (!load_example(context)) {
      return false;
    }
    return true;
  }

  // Update the current example and draw it
  if (Examples[exampleIndex]->Update(context) == 0) {
    if (Examples[exampleIndex]->Draw(context) != 0) {
      SDL_Log("Draw Failed!");
      // if draw returns false, we quit
      CommonQuit(&ctx);
      return false;
    }
    /*emscripten_debugger();*/
    /*SDL_GL_SwapWindow(context->Window);*/
    return true;
  }

  // if update returns false, we quit
  CommonQuit(&ctx);
  return false;
}

int main(int argc, char **argv) {
  for (int i = 1; i < argc; i += 1) {
    if (SDL_strcmp(argv[i], "-name") == 0 && argc > i + 1) {
      const char *exampleName = argv[i + 1];
      int foundExample = 0;

      for (int j = 0; j < SDL_arraysize(Examples); j += 1) {
        if (SDL_strcmp(Examples[j]->Name, exampleName) == 0) {
          gotoExampleIndex = j;
          foundExample = 1;
          break;
        }
      }

      if (!foundExample) {
        SDL_Log("No example named '%s' exists.", exampleName);
        return 1;
      }
    }
  }

  // Initialize SDL and the GPU device
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    return 1;
  }

  // Create a GPU device for the example suite
  ctx.Device =
      SDL_CreateGPUDevice(SDL_ShaderCross_GetSPIRVShaderFormats(), true, NULL);
  if (ctx.Device == NULL) {
    SDL_Log("GPUCreateDevice failed");
    return -1;
  }

  // Create a window for the example suite
  ctx.Window =
      SDL_CreateWindow(ctx.ExampleName, 640, 480, SDL_WINDOW_RESIZABLE);
  if (ctx.Window == NULL) {
    SDL_Log("CreateWindow failed: %s", SDL_GetError());
    return -1;
  }

  // Claim the window for the GPU device
  if (!SDL_ClaimWindowForGPUDevice(ctx.Device, ctx.Window)) {
    SDL_Log("GPUClaimWindow failed");
    return -1;
  }

  InitializeAssetLoader();
  SDL_AddEventWatch(AppLifecycleWatcher, NULL);

  SDL_Log("Welcome to the SDL_GPU example suite!");
  SDL_Log("Press A/D (or LB/RB) to move between examples!");

  /*// Load whatever example is set to be loaded*/
  if (!load_example(&ctx)) {
    return 1;
  }

  /*// Write and read a buffer*/
  /*writeAndReadBuffer((WGPUDevice)0x1);*/

  // Set keyboard callbacks for emscripten
  emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, &ctx, true,
                                  &emsc_dummy_key_callback);

  // Set touch callbacks for emscripten
  emscripten_set_touchstart_callback("#canvas", &ctx, true,
                                     emsc_dummy_touch_callback);
  emscripten_set_touchend_callback("#canvas", &ctx, true,
                                   emsc_dummy_touch_callback);
  emscripten_set_touchmove_callback("#canvas", &ctx, true,
                                    emsc_dummy_touch_callback);

  // Set the emscripten render loop
  emscripten_request_animation_frame_loop(emsc_frame, &ctx);

  return 0;
}
