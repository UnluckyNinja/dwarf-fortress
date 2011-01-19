/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_TEXTURES_HPP_
#define EXTERN_TEXTURES_HPP_

#include <GL/gl.h>

#include <vector>

#include <string>

struct SDL_Surface;

// GL texture positions
struct gl_texpos {
    GLfloat left, right, top, bottom;
};

// TODO: comment
// TODO: categorize
// Covers every allowed permutation of text
struct ttf_id {
    ::std::string text;
    unsigned char fg, bg, bold;
    unsigned char justification;

    bool operator<(const ttf_id &other) const {
      if (fg != other.fg)
        return fg < other.fg;
      if (bg != other.bg)
        return bg < other.bg;
      if (bold != other.bold)
        return bold < other.bold;
      if (justification != other.justification)
        return justification < other.justification;
      return text < other.text;
    }

    bool operator==(const ttf_id &other) const {
      return fg == other.fg && bg == other.bg && bold == other.bold && justification == other.justification && text == other.text;
    }
};

// TODO: comment
// TODO: categorize
namespace std {
  template< > struct hash< ttf_id > {
      ::std::size_t operator()(ttf_id val) const {
        // Not the ideal hash function, but it'll do. And it's better than GCC's. id? Seriously?
        return hash< ::std::string > ()(val.text) + val.fg + (val.bg << 4) + (val.bold << 8) + (val.justification << 12);
      }
  };
}
;

// Being a texture catalog interface, with opengl, sdl and truetype capability
class textures {
    friend class enablerst;
    friend class renderer_opengl;
  private:
    ::std::vector< SDL_Surface* > raws;
    bool uploaded;
    long add_texture(SDL_Surface*);
  protected:
    GLuint gl_catalog; // texture catalog gennum
    struct gl_texpos* gl_texpos; // Texture positions in the GL catalog, if any
  public:
    // Initialize state variables
    textures();
    int textureCount();
    // Upload in-memory textures to the GPU
    // When textures are uploaded, any alteration to a texture
    // is automatically reflected in the uploaded copy - eg. it's replaced.
    // This is very expensive in opengl mode. Don't do it often.
    void upload_textures();
    // Also, you really should try to remove uploaded textures before
    // deleting a window, in case of driver memory leaks.
    void remove_uploaded_textures();
    /**
     * @brief Returns the most recent texture data.
     * @ingroup extern
     */
    SDL_Surface *get_texture_data(long pos);
    /**
     * @brief Clone a texture
     * @ingroup extern
     */
    long clone_texture(long src);

    /**
     * @brief Remove all color, but not transparency
     * @ingroup extern
     */
    void grayscale_texture(long pos);
    /**
     * @brief Loads dimx * dimy textures from a file, assuming all tiles are equally large and arranged in a grid.
     * @details Texture positions are saved in row-major order to tex_pos. If convert_magenta is true and the file does
     *   not have built-in transparency, any magenta (255,0,255 RGB) is converted to full transparency. The calculated
     *   size of individual tiles is saved to disp_x, disp_y.
     * @ingroup extern
     */
    void load_multi_pdim(const ::std::string &filename, long *tex_pos, long dimx, long dimy, bool convert_magenta, long *disp_x, long *disp_y);

    /**
     * @brief Loads a single texture from a file, returning the handle.
     * @ingroup extern
     */
    long load(const ::std::string &filename, bool convert_magenta);

    /**
     * @brief Delete a texture.
     * @ingroup extern
     */
    void delete_texture(long pos);
};

SDL_Surface *canonicalize_format(SDL_Surface *src, bool convert_magenta);

#endif /* EXTERN_TEXTURES_HPP_ */
