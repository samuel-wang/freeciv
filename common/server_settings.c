/***********************************************************************
 Freeciv - Copyright (C) 2017 - Freeciv Development Team
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

/* common */
#include "fc_interface.h"

#include "server_settings.h"

/***************************************************************************
  Returns the server setting with the specified name.
***************************************************************************/
server_setting_id server_setting_by_name(const char *name)
{
  return fc_funcs->server_setting_by_name(name);
}

/***************************************************************************
  Returns the name of the server setting with the specified id.
***************************************************************************/
const char *server_setting_name_get(server_setting_id id)
{
  return fc_funcs->server_setting_name_get(id);
}

/***************************************************************************
  Returns the type of the server setting with the specified id.
***************************************************************************/
enum sset_type server_setting_type_get(server_setting_id id)
{
  return fc_funcs->server_setting_type_get(id);
}

/***************************************************************************
  Returns TRUE iff a server setting with the specified id exists.
***************************************************************************/
bool server_setting_exists(server_setting_id id)
{
  return sset_type_is_valid(server_setting_type_get(id));
}
