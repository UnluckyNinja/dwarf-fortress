/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/musicsound.hpp"

#include <logging/logging.hpp>

musicsoundst::musicsoundst() {
  functional = false;
  background_slot = slot(false, -1);
}

musicsoundst::~musicsoundst() {
  deinitsound();
}

void musicsoundst::update() {
}

void musicsoundst::set_song(::std::string& filename, slot slot) {
  if (!functional) {
    return;
  }

  // printf("%s requested in %d-%d\n", filename.c_str(), (int)slot.first, slot.second);
  if (!buffers.count(filename)) {
    // Song not already loaded. Load it.
    SF_INFO sfinfo;
    sfinfo.format = 0;
    SNDFILE *sf = sf_open(filename.c_str(), SFM_READ, &sfinfo);

    if (sf == 0) {
      __error
        << filename << " not found, sound not loaded";
      __al_check_errors;
      return;
    }

    short *buffer = new short[sfinfo.channels * sfinfo.frames];
    sf_count_t frames_read = sf_readf_short(sf, buffer, sfinfo.frames);
    if (frames_read != sfinfo.frames) {
      __warn
        << filename << ": " << sfinfo.frames << " frames requested, " << frames_read << " frames read. Corrupted file?";
    }
    sf_close(sf);

    ALenum format;
    switch (sfinfo.channels) {
      case 1:
        format = AL_FORMAT_MONO16;
        break;
      case 2:
        format = AL_FORMAT_STEREO16;
        break;
      default: {
        __error
          << filename << ": Unexpected number of channels: " << sfinfo.channels;
        __al_check_errors;
        return;
      }
    }

    // Construct openal buffer and load this
    ALuint albuf;
    alGenBuffers(1, &albuf);
    if (!alIsBuffer(albuf)) {
      __error
        << "Constructing OpenAL buffer mysteriously failed!";
      __al_check_errors;
      return;
    }
    buffers[filename] = albuf;

    alBufferData(albuf, format, (ALvoid*) buffer, sfinfo.channels * sfinfo.frames * 2, sfinfo.samplerate);
    __al_check_errors;

    delete[] buffer;

    // Create a source for this song
    ALuint source;
    alGenSources(1, &source);
    if (!alIsSource(source)) {
      __error
        << "Constructing OpenAL source mysteriously failed!";
      __al_check_errors;
      return;
    }
    sources[filename] = source;

    alSourceQueueBuffers(source, 1, &albuf);
  }

  // Store the requested song in the requested slot.
  // Say, should this alter the playing song if that slot is already playing?
  slot_buffer[slot] = buffers[filename];
  slot_source[slot] = sources[filename];

  __al_check_errors;
}

void musicsoundst::playsound(slot slot) {
  if (!functional) {
    return;
  }

  // printf("%d requested\n", slot);
  if (!slot_source.count(slot)) {
    // printf("Slot %d-%d requested, but no song loaded\n", (int)slot.first, slot.second);
    return;
  }

  if (background_slot == slot) {
    __info
      << "playsound called on background song, background song cancelled!?";
    background_slot = ::std::make_pair(false, -1);
  }
  alSourcei(slot_source[slot], AL_LOOPING, AL_FALSE);
  alSourcePlay(slot_source[slot]);

  __al_check_errors;
}

void musicsoundst::playsound(int slot) {
  playsound(::std::make_pair(false, slot));
}

void musicsoundst::startbackgroundmusic(slot slot) {
  if (!functional)
    return;

  if (!slot_source.count(slot)) {
    // printf("Slot %d-%d requested, but no song loaded\n", (int)slot.first, slot.second);
    return;
  }

  if (background_slot == slot) {
    // Verily, it is already playing
    return;
  }

  stop_sound(background_slot);
  background_slot = slot;
  // printf("%d backgrounded\n", slot);

  alSourcei(slot_source[slot], AL_LOOPING, AL_TRUE);
  alSourcePlay(slot_source[slot]);

  __al_check_errors;
}

void musicsoundst::stop_sound() {
  if (!functional) {
    return;
  }

  // Stop all playing sounds. Does this include background music?
  std::map< std::string, ALuint >::iterator it;
  for (it = sources.begin(); it != sources.end(); ++it)
    alSourceStop(it->second);
}

void musicsoundst::stop_sound(slot slot) {
  if (!functional) {
    return;
  }

  if (slot_source.count(slot) == 0) {
    return;
  }

  ALuint source = slot_source[slot];
  alSourceStop(source);
}

void musicsoundst::playsound(int s, int channel) {
  if (!functional) {
    return;
  }

  playsound(s);
}

void musicsoundst::set_sound(::std::string &filename, int slot, int pan, int priority) {
  if (!functional) {
    return;
  }

  set_song(filename, ::std::make_pair(false, slot));
}

void musicsoundst::deinitsound() {
  if (!functional) return;

  ::std::map< std::string, ALuint >::iterator it;
  // Free all sources
  for (it = sources.begin(); it != sources.end(); ++it) {
    ALuint source = it->second;
    alDeleteSources(1, &source);
  }

  // Free all sample memory
  for (it = buffers.begin(); it != buffers.end(); ++it) {
    ALuint buffer = it->second;
    alDeleteBuffers(1, &buffer);
  }

  // Deinit OpenAL
  alcMakeContextCurrent(0);
  alcDestroyContext(context);
  alcCloseDevice(device);

  functional = false;
}
