/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_MUSICSOUND_HPP_
#define EXTERN_MUSICSOUND_HPP_

#include <string>
#include <map>

#include "intern/utils/openal.hpp"

#define SOUND_CHANNELNUM 16

// Preferred mixer frequency. Should be the same as what the ogg files
// use, to avoid resampling.
#define SOUND_FREQUENCY 44100

// If the bool is false, a sound; otherwise a song
typedef ::std::pair< bool, int > slot;

/**
 * @ingroup extern
 */
class musicsoundst {
  public:
    /**
     * @brief Returns false if it failed.
     * @ingroup extern
     */
    bool initsound();
    void update();

    /**
     * @ingroup extern
     */
    void set_master_volume(long newvol);

    void set_song(::std::string& filename, slot slot);
    /**
     * @ingroup extern
     */
    void set_song(::std::string& filename, int slot);
    void playsound(slot slot);
    /**
     * @brief Assumes sound
     */
    void playsound(int slot);

    void startbackgroundmusic(slot slot);
    /**
     * @ingroup extern
     */
    void startbackgroundmusic(int slot); // Assumes song
    /**
     * @ingroup extern
     */
    void stopbackgroundmusic();
    void stop_sound();
    void stop_sound(slot slot);
    void playsound(int s, int channel);
    void set_sound(::std::string& filename, int slot, int pan = -1, int priority = 0);
    void deinitsound();

    /**
     * @deprecated
     */
    void forcebackgroundmusic(int slot, unsigned long time);
    void playsound(int s, int min_channel, int max_channel, int force_channel);
    void set_sound_params(int slot, int p1, int vol, int pan, int priority);

    musicsoundst();
    ~musicsoundst();

  private:
    bool functional;
    ALCdevice* device;
    ALCcontext* context;

    /**
     * @brief OpenAL buffers
     */
    ::std::map< ::std::string, ALuint > buffers;
    /**
     * @brief And sources
     */
    ::std::map< ::std::string, ALuint > sources;
    /**
     * @brief Mappings from DF slots to openal
     */
    ::std::map< slot, ALuint > slot_buffer;
    ::std::map< slot, ALuint > slot_source;

    /**
     * @brief Currently playing background music, or -1
     */
    slot background_slot;
};

#endif /* EXTERN_MUSICSOUND_HPP_ */
