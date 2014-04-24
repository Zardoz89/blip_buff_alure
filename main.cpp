#include <cstdio>
#include <cmath>

#include "AL/alure.h"
#include "Blip_Buffer.h"

const static unsigned SR = 44100; //! Sampling rate

alureStream* stream = nullptr;
ALuint src;

// Blip Buffer stuff
Blip_Buffer blipbuf;                        //! Blip Buffer
Blip_Synth<blip_good_quality,20> synth;     //! Synthetizer of Blip Buffer

double offset = 0;      //! Signal phase
double sign = 1;
volatile double freq = 457.338;

volatile int isdone = 0;


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

  // Seeting Blip Buffer

  synth.treble_eq( -18.0f );

  synth.volume (0.80);
  synth.output (&blipbuf);

  // Init Blip Buffer with a buffer of 250ms (second paramter is time in ms)
  if ( blipbuf.set_sample_rate( SR, 1000 / 4 ) ) {
    std::fprintf(stderr, "Failed to create Blip Buffer! Our of Memory\n");
    alureShutdownDevice();
    return 1;
  }
  blipbuf.clock_rate( blipbuf.sample_rate() );
  blipbuf.bass_freq(300); // Equalization like a TV speaker

  stream = alureCreateStreamFromCallback (StreamCB, nullptr, AL_FORMAT_MONO16, SR, SR/2, 0, nullptr);

  if(!stream) {
    std::fprintf(stderr, "Error creating stream! %s\n",  alureGetErrorString());
    alDeleteSources(1, &src);

    alureShutdownDevice();
    return 1;
  }

  if (!alurePlaySourceStream(src, stream, 4, 0, eos_callback, NULL)) {
    std::fprintf(stderr, "Failed to play stream: %s\n", alureGetErrorString());
    isdone = 1;
  }

  alureUpdateInterval(0.005f); // Should be a independint thread  playing the stream

  while(!isdone) {
    freq -= 1;
    if (freq < 1) {
      freq = 600;
    }

    alureSleep(0.02f);
  }
  alureStopSource(src, AL_FALSE);

  alDeleteSources(1, &src);
  alureDestroyStream(stream, 0, NULL);

  alureShutdownDevice();
  return 0;
}


// Callback called when Alure needs more data to ffed a buffer
ALuint StreamCB (void* userdata, ALubyte *data, ALuint bytes) {

  double period = SR / (2* freq);  // How many samples need to do a half cycle.
  size_t lenght = bytes / 2;          // Lenght in "samples"

  unsigned const amplitude = 9;
  while (offset < lenght) {
      sign = -sign;
      synth.update(offset, amplitude * sign);
      offset += period;
  }
  blipbuf.end_frame(lenght);
  offset -= lenght; // adjust time to new frame

  return 2* blipbuf.read_samples ((blip_sample_t*) data, lenght ); // return bytes!

}

// Callback when the strem ends
static void eos_callback(void *unused, ALuint unused2) {
  isdone = 1;
  (void)unused;
  (void)unused2;
}

