#include <config.h>
#include <glib.h>

#include "gsm_su.h"
#include "util.h"

static gboolean (*gksu_run) (const char *, GError **);
static gboolean (*gnomesu_exec) (const char *commandline);

static void
load_su_lib (const char *lib_name, const char *symbol_name, void **symbol, gboolean *init)
{
  if (*init)
    return;

  *init = TRUE;

  load_symbols (lib_name,
                symbol_name, symbol,
                NULL);
}

static void
load_gksu (void)
{
  static gboolean init;
  load_su_lib ("libgksu2.so", "gksu_run", (void **)&gksu_run, &init);
}

static void
load_gnomesu (void)
{
  static gboolean init;
  load_su_lib ("libgnomesu.so.0", "gnomesu_exec", (void **)&gnomesu_exec, &init);
}

gboolean
gsm_gksu_create_root_password_dialog (const char *command)
{
  GError *e = NULL;

  /* Returns FALSE or TRUE on success, depends on version ... */
  gksu_run (command, &e);

  if (e)
    {
      g_critical ("Could not run gksu_run(\"%s\") : %s\n",
                  command, e->message);
      g_error_free (e);
      return FALSE;
    }

  g_message ("gksu_run did fine\n");
  return TRUE;
}

gboolean
procman_has_gksu (void)
{
  load_gksu ();
  return gksu_run != NULL;
}

gboolean
gsm_gnomesu_create_root_password_dialog (const char *command)
{
  return gnomesu_exec (command);
}

gboolean
procman_has_gnomesu (void)
{
  load_gnomesu ();
  return gnomesu_exec != NULL;
}
