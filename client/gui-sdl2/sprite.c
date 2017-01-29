/***********************************************************************
 Freeciv - Copyright (C) 2005 The Freeciv Team
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

/* SDL2 */
#ifdef SDL2_PLAIN_INCLUDE
#include <SDL.h>
#include <SDL_image.h>
#else  /* SDL2_PLAIN_INCLUDE */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif /* SDL2_PLAIN_INCLUDE */

/* utility */
#include "fcintl.h"
#include "log.h"
#include "mem.h"

/* client/gui-sdl2 */
#include "colors.h"
#include "graphics.h"

#include "sprite.h"

static struct sprite *ctor_sprite(SDL_Surface *pSurface);

/****************************************************************************
  Return a NULL-terminated, permanently allocated array of possible
  graphics types extensions.  Extensions listed first will be checked
  first.
****************************************************************************/
const char **gfx_fileextensions(void)
{
  static const char *ext[] = {
    "png",
    "xpm",
    NULL
  };

  return ext;
}

/****************************************************************************
  Load the given graphics file into a sprite.  This function loads an
  entire image file, which may later be broken up into individual sprites
  with crop_sprite.
****************************************************************************/
struct sprite *load_gfxfile(const char *filename)
{
  SDL_Surface *pbuf = NULL;

  if ((pbuf = IMG_Load(filename)) == NULL) {
    log_error(_("load_gfxfile: Unable to load graphic file %s!"), filename);
    return NULL; /* Should I use abort() ? */
  }

  if (pbuf->format->palette != NULL) {
    SDL_Surface *pnew = convert_surf(pbuf);

    FREESURFACE(pbuf);
    pbuf = pnew;
  }

  return ctor_sprite(pbuf);
}

/****************************************************************************
  Create a new sprite by cropping and taking only the given portion of
  the image.

  source gives the sprite that is to be cropped.

  x,y, width, height gives the rectangle to be cropped.  The pixel at
  position of the source sprite will be at (0,0) in the new sprite, and
  the new sprite will have dimensions (width, height).

  mask gives an additional mask to be used for clipping the new
  sprite. Only the transparency value of the mask is used in
  crop_sprite. The formula is: dest_trans = src_trans *
  mask_trans. Note that because the transparency is expressed as an
  integer it is common to divide it by 256 afterwards.

  mask_offset_x, mask_offset_y is the offset of the mask relative to the
  origin of the source image.  The pixel at (mask_offset_x,mask_offset_y)
  in the mask image will be used to clip pixel (0,0) in the source image
  which is pixel (-x,-y) in the new image.
****************************************************************************/
struct sprite *crop_sprite(struct sprite *source,
                           int x, int y, int width, int height,
                           struct sprite *mask,
                           int mask_offset_x, int mask_offset_y, float scale)
{
  SDL_Rect src_rect = {(Sint16) x, (Sint16) y, (Uint16) width, (Uint16) height};
  SDL_Surface *pSrc = crop_rect_from_surface(GET_SURF(source), &src_rect);
  SDL_Surface *pDest = NULL;

  if (mask) {
    pDest = mask_surface(pSrc, mask->psurface, x - mask_offset_x, y - mask_offset_y);
    FREESURFACE(pSrc);
    return ctor_sprite(pDest);
  }

  return ctor_sprite(pSrc);
}

/****************************************************************************
  Create a sprite with the given height, width and color.
****************************************************************************/
struct sprite *create_sprite(int width, int height, struct color *pcolor)
{
  SDL_Surface *mypixbuf = NULL;

  fc_assert_ret_val(width > 0, NULL);
  fc_assert_ret_val(height > 0, NULL);
  fc_assert_ret_val(pcolor != NULL, NULL);

  mypixbuf = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32,
#if SDL_BYTEORDER != SDL_LIL_ENDIAN
                0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF
#else
                0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000
#endif
  );

  SDL_FillRect(mypixbuf, NULL,
               SDL_MapRGBA(mypixbuf->format,
                           pcolor->color->r,
                           pcolor->color->g,
                           pcolor->color->b,
                           pcolor->color->a));

  return ctor_sprite(mypixbuf);
}

/****************************************************************************
  Find the dimensions of the sprite.
****************************************************************************/
void get_sprite_dimensions(struct sprite *sprite, int *width, int *height)
{
  *width = GET_SURF(sprite)->w;
  *height = GET_SURF(sprite)->h;
}

/****************************************************************************
  Free a sprite and all associated image data.
****************************************************************************/
void free_sprite(struct sprite *s)
{
  fc_assert_ret(s != NULL);
  FREESURFACE(GET_SURF_REAL(s));
  FC_FREE(s);
}

/**************************************************************************
  Create a sprite struct and fill it with SDL_Surface pointer
**************************************************************************/
static struct sprite *ctor_sprite(SDL_Surface *pSurface)
{
  struct sprite *result = fc_malloc(sizeof(struct sprite));

  result->psurface = pSurface;

  return result;
}
