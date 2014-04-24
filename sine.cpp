#include <cstdio>
#include <cmath>

#include "AL/alure.h"

const static unsigned SR = 44100;               //! Sampling rate
const static unsigned PI2 = 2.0f * 3.14159679f; //! 2 * PI

const double dt = 1.0f / SR; // Time in seconds of a single sample
const unsigned NUM_BUFS = 4;

alureStream* stream = nullptr;
ALuint src;

double offset = 0;      //! Signal phase
volatile int isdone = 0;
volatile double freq = 457.338;


ALuint StreamCB (void* userdata, ALubyte *data, ALuint bytes);
static void eos_callback(void *unused, ALuint unused2);

int main () {
  if (!alureInitDevice(NULL, NULL)) {
    std::fprintf(stderr, "Failed to open OpenAL device: %s\n", alureGetErrorString());
    return 1;
  }

  alGenSources(1, &src);
  if (alGetError() != AL_NO_ERROR) {
    std::fprintf(stderr, "Failed to create OpenAL source!\n");
    alureShutdownDevice();
    return 1;
  }

  stream = alureCreateStreamFromCallback (StreamCB, nullptr, AL_FORMAT_MONO8, SR, SR/4, 0, nullptr);

  if(!stream) {
    std::fprintf(stderr, "Error creating stream! %s\n",  alureGetErrorString());
    alDeleteSources(1, &src);

    alureShutdownDevice();
    return 1;
  }

  if (!alurePlaySourceStream(src, stream, NUM_BUFS, 0, eos_callback, NULL)) {
    std::fprintf(stderr, "Failed to play stream: %s\n", alureGetErrorString());
    isdone = 1;
  }

  alureUpdateInterval(0.005f); // Should be a independint thread  playing the stream

  while(!isdone) {
    freq -= 1;
    if (freq < 1) {
      freq = 600;
    }

    alureSleep(0.05);
  }
  alureStopSource(src, AL_FALSE);

  alDeleteSources(1, &src);
  alureDestroyStream(stream, 0, NULL);

  alureShutdownDevice();
  return 0;
}


// Callback called when Alure needs more data to ffed a buffer
ALuint StreamCB (void* userdata, ALubyte *data, ALuint bytes) {
  const double w = freq * PI2; // Convert to angular freq.
  double x;
  unsigned i;
  for( i = 0; i < bytes; i++) {
      x = w*(i*dt) + offset; // x = wt
      data[i] = (unsigned char)(128.0f + 128.f * std::sin(x) );
  }
  offset = x;

  return i;
}

// Callback when the strem ends
static void eos_callback(void *unused, ALuint unused2) {
  isdone = 1;
  (void)unused;
  (void)unused2;
}

