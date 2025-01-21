#include "Common.h"
/*#include "cimgui_impl_sdl3.h"*/

static int Init(Context *context) {
  int status = CommonInit(context, SDL_WINDOW_RESIZABLE);
  return status;
}

static int Update(Context *context) {
  /*// Start the Dear ImGui frame*/
  /*ImGui_ImplSDLGPU3_NewFrame();*/
  /**/
  /*// Create a window*/
  /*igBegin("Hello, world!", NULL, 0);*/
  /*igText("This is some useful text.");*/
  /**/
  /*// Add a button*/
  /*if (igButton("Click me!", (ImVec2){120, 30})) {*/
  /*  // Button was clicked*/
  /*}*/
  /**/
  /*igEnd();*/
  /*igRender();*/
  /**/
  return 0;
}

static int Draw(Context *context) {
  SDL_GPUCommandBuffer *cmdbuf = SDL_AcquireGPUCommandBuffer(context->Device);
  if (cmdbuf == NULL) {
    SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
    return -1;
  }

  SDL_GPUTexture *swapchainTexture;
  if (!SDL_AcquireGPUSwapchainTexture(cmdbuf, context->Window,
                                      &swapchainTexture, NULL, NULL)) {
    SDL_Log("AcquireGPUSwapchainTexture failed: %s", SDL_GetError());
    return -1;
  }

  if (swapchainTexture != NULL) {
    SDL_GPUColorTargetInfo colorTargetInfo = {0};
    colorTargetInfo.texture = swapchainTexture;
    colorTargetInfo.clear_color = (SDL_FColor){0.45f, 0.55f, 0.60f, 1.00f};
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

    /*// Prepare ImGui draw data*/
    /*Imgui_ImplSDLGPU3_PrepareDrawData(igGetDrawData(), cmdbuf);*/

    SDL_GPURenderPass *renderPass =
        SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, NULL);

    /*// Render ImGui draw data*/
    /*ImGui_ImplSDLGPU3_RenderDrawData(igGetDrawData(), cmdbuf, renderPass, NULL);*/

    SDL_EndGPURenderPass(renderPass);
  }

  SDL_SubmitGPUCommandBuffer(cmdbuf);

  return 0;
}
static void Quit(Context *context) { CommonQuit(context); }

Example ClearScreen_Example = {"ClearScreen", Init, Update, Draw, Quit};
