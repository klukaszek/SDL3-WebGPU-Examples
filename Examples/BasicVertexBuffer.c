#include "Common.h"
#include <SDL3/SDL_gpu.h>

static SDL_GPUGraphicsPipeline *Pipeline;
static SDL_GPUBuffer *VertexBuffer;

// Added for WebGPU testing purposes
static void readBackFromVB(Context *context) {
  SDL_Log("Reading from the vertex buffer that we just wrote to...");

  // Create a transfer buffer to download the vertex buffer data
  SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(
      context->Device, &(SDL_GPUTransferBufferCreateInfo){
                           .usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD,
                           .size = sizeof(PositionColorVertex) * 3});

  // Acquire a command buffer to handle the download
  SDL_GPUCommandBuffer *downloadCmdBuf =
      SDL_AcquireGPUCommandBuffer(context->Device);

  // Begin a copy pass to download the vertex buffer data using the cmdBuf
  // This effectively creates a commandEncoder to handle the download
  SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(downloadCmdBuf);

  // Copy the first 3 PositionColorVertex structs from
  // the vertex buffer to the transfer buffer using a
  // wgpuCommandEncoderCopyBufferToBuffer call
  SDL_DownloadFromGPUBuffer(
      copyPass,
      &(SDL_GPUBufferRegion){.buffer = VertexBuffer,
                             .offset = 0,
                             .size = sizeof(PositionColorVertex) * 3},
      &(SDL_GPUTransferBufferLocation){.transfer_buffer = transferBuffer,
                                       .offset = 0});

  // End the copy pass and submit the command buffer
  SDL_EndGPUCopyPass(copyPass);
  SDL_SubmitGPUCommandBuffer(downloadCmdBuf);

  // Get a pointer to the mapped data in the transfer buffer.
  // This is a blocking call that waits for the download to complete.
  PositionColorVertex *transferData =
      SDL_MapGPUTransferBuffer(context->Device, transferBuffer, false);

  SDL_Log("Data read from the vertex buffer:");
  SDL_Log("First vertex: (%f, %f, %f) (%d, %d, %d, %d)", transferData[0].x,
          transferData[0].y, transferData[0].z, transferData[0].r,
          transferData[0].g, transferData[0].b, transferData[0].a);
  SDL_Log("Second vertex: (%f, %f, %f) (%d, %d, %d, %d)", transferData[1].x,
          transferData[1].y, transferData[1].z, transferData[1].r,
          transferData[1].g, transferData[1].b, transferData[1].a);
  SDL_Log("Third vertex: (%f, %f, %f) (%d, %d, %d, %d)", transferData[2].x,
          transferData[2].y, transferData[2].z, transferData[2].r,
          transferData[2].g, transferData[2].b, transferData[2].a);

  SDL_UnmapGPUTransferBuffer(context->Device, transferBuffer);
  SDL_ReleaseGPUTransferBuffer(context->Device, transferBuffer);
}

static int Init(Context *context) {
  /*int result = CommonInit(context, 0);*/
  /*if (result < 0)*/
  /*{*/
  /*	return result;*/
  /*}*/

  // Create the shaders
  SDL_GPUShader *vertexShader =
      LoadShader(context->Device, "PositionColor.vert", 0, 0, 0, 0);
  if (vertexShader == NULL) {
    SDL_Log("Failed to create vertex shader!");
    return -1;
  }

  SDL_GPUShader *fragmentShader =
      LoadShader(context->Device, "SolidColor.frag", 0, 0, 0, 0);
  if (fragmentShader == NULL) {
    SDL_Log("Failed to create fragment shader!");
    return -1;
  }

  // Create the pipeline
  SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
      .target_info =
          {
              .num_color_targets = 1,
              .color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
                  .format = SDL_GetGPUSwapchainTextureFormat(context->Device,
                                                             context->Window),
              }},
          },
      // This is set up to match the vertex shader layout!
      .vertex_input_state =
          (SDL_GPUVertexInputState){
              .num_vertex_buffers = 1,
              .vertex_buffer_descriptions =
                  (SDL_GPUVertexBufferDescription[]){
                      {.slot = 0,
                       .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                       .instance_step_rate = 0,
                       .pitch = sizeof(PositionColorVertex)}},
              .num_vertex_attributes = 2,
              .vertex_attributes =
                  (SDL_GPUVertexAttribute[]){
                      {.buffer_slot = 0,
                       .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                       .location = 0,
                       .offset = 0},
                      {.buffer_slot = 0,
                       .format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,
                       .location = 1,
                       .offset = sizeof(float) * 3}}},
      .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
      .vertex_shader = vertexShader,
      .fragment_shader = fragmentShader};

  Pipeline =
      SDL_CreateGPUGraphicsPipeline(context->Device, &pipelineCreateInfo);
  if (Pipeline == NULL) {
    SDL_Log("Failed to create pipeline!");
    return -1;
  }

  SDL_ReleaseGPUShader(context->Device, vertexShader);
  SDL_ReleaseGPUShader(context->Device, fragmentShader);

  SDL_Log("Writing to vertex buffer...");

  // Create the vertex buffer
  VertexBuffer = SDL_CreateGPUBuffer(
      context->Device,
      &(SDL_GPUBufferCreateInfo){.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
                                 .size = sizeof(PositionColorVertex) * 3});

  // To get data into the vertex buffer, we have to use a transfer buffer
  SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(
      context->Device, &(SDL_GPUTransferBufferCreateInfo){
                           .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                           .size = sizeof(PositionColorVertex) * 3});

  PositionColorVertex *transferData =
      SDL_MapGPUTransferBuffer(context->Device, transferBuffer, false);

  transferData[0] = (PositionColorVertex){-1, -1, 0, 255, 0, 0, 255};
  transferData[1] = (PositionColorVertex){1, -1, 0, 0, 255, 0, 255};
  transferData[2] = (PositionColorVertex){0, 1, 0, 0, 0, 255, 255};

  SDL_UnmapGPUTransferBuffer(context->Device, transferBuffer);

  // Upload the transfer data to the vertex buffer
  SDL_GPUCommandBuffer *uploadCmdBuf =
      SDL_AcquireGPUCommandBuffer(context->Device);
  SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

  SDL_UploadToGPUBuffer(
      copyPass,
      &(SDL_GPUTransferBufferLocation){.transfer_buffer = transferBuffer,
                                       .offset = 0},
      &(SDL_GPUBufferRegion){.buffer = VertexBuffer,
                             .offset = 0,
                             .size = sizeof(PositionColorVertex) * 3},
      false);

  SDL_EndGPUCopyPass(copyPass);
  SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
  SDL_ReleaseGPUTransferBuffer(context->Device, transferBuffer);

  // This is just to show that the data was successfully uploaded to the vertex
  // buffer Also showcases readback from a GPU buffer using a transfer buffer
  readBackFromVB(context);

  return 0;
}

static int Update(Context *context) { return 0; }

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
    colorTargetInfo.clear_color = (SDL_FColor){0.0f, 0.0f, 0.0f, 1.0f};
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass *renderPass =
        SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, NULL);

    SDL_BindGPUGraphicsPipeline(renderPass, Pipeline);
    SDL_BindGPUVertexBuffers(
        renderPass, 0,
        &(SDL_GPUBufferBinding){.buffer = VertexBuffer, .offset = 0}, 1);
    SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

    SDL_EndGPURenderPass(renderPass);
  }

  SDL_SubmitGPUCommandBuffer(cmdbuf);

  return 0;
}

static void Quit(Context *context) {
  SDL_ReleaseGPUGraphicsPipeline(context->Device, Pipeline);
  SDL_ReleaseGPUBuffer(context->Device, VertexBuffer);

  CommonQuit(context);
}

Example BasicVertexBuffer_Example = {"BasicVertexBuffer", Init, Update, Draw,
                                     Quit};
