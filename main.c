#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <pwd.h>

#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include "canvas.h"
#include <cucul.h>
#include <stdbool.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <gcrypt.h>
#include <inttypes.h>

static unsigned int head_color = CUCUL_LIGHTGREEN;
static unsigned int default_color = CUCUL_GREEN;
static unsigned int change_color = CUCUL_LIGHTGREEN;


void switch_color(enum state_color color){
  switch(color) {
  case DEFAULT:
    head_color = CUCUL_LIGHTGREEN;
    default_color = CUCUL_GREEN;
    change_color = CUCUL_GREEN;
    break;
  case WARNNING:
    head_color = CUCUL_YELLOW;
    default_color = CUCUL_BROWN;
    change_color = CUCUL_YELLOW;
    break;
  case ALARM:
    head_color = CUCUL_YELLOW;
    default_color = CUCUL_RED;
    change_color = CUCUL_LIGHTRED;
    break;
  default:
    head_color = CUCUL_LIGHTGREEN;
    default_color = CUCUL_GREEN;
    change_color = CUCUL_LIGHTRED;
    break;
  }
}


int main(int argc, char *const argv[])
{
  static caca_display_t *dp;
  static cucul_canvas_t *frontcv;
  frontcv = cucul_create_canvas(0, 0);
  (void) setenv("CACA_DRIVER", "ncurses", 1);
  dp = caca_create_display(frontcv);
  bool is_passwd = true;
  if(!dp)
    return 1;
  caca_set_display_time(dp, 17666);
  passwd_breaker(PREPARE, frontcv);
  passwd_breaker(INIT, frontcv);
  struct timespec sleep;
  sleep.tv_sec = 0;
  sleep.tv_nsec = 17000000;
  
  for(;;)
  {
    /* Handle events */
    caca_event_t ev;
    while(caca_get_event(dp, CACA_EVENT_KEY_PRESS
                         | CACA_EVENT_QUIT, &ev, 0))
    {
      if(caca_get_event_type(&ev) == CACA_EVENT_QUIT)
        goto end;
      
      switch(caca_get_event_key_ch(&ev))
      {
      case CACA_KEY_UP:
	switch_color(DEFAULT);
	break;
      case CACA_KEY_DOWN:
	switch_color(WARNNING);
	break;
      case CACA_KEY_LEFT:
	switch_color(ALARM);
	break;
      case CACA_KEY_ESCAPE:
      case CACA_KEY_CTRL_C:
      case CACA_KEY_CTRL_Z:
	goto end;
      default:
	passwd_breaker(PREPARE, frontcv);
	passwd_breaker(INIT, frontcv);
	break;
          
      }
    }
    if (abort_requested)
      goto end;
    if(is_passwd){
      passwd_breaker(UPDATE, frontcv);
      passwd_breaker(RENDER, frontcv);
    }else{
      /* Update demo's data */
      matrix(UPDATE, frontcv);
      /* Render main demo's canvas */
      matrix(RENDER, frontcv);
    }
    cucul_set_color_ansi(frontcv, CUCUL_BLACK, CUCUL_BLACK);
    caca_refresh_display(dp);
    nanosleep(&sleep,NULL);

    caca_refresh_display(dp);
  }
end:
  
  caca_free_display(dp);
  cucul_free_canvas(frontcv);
  
  return 0;
}

/* Lock the current terminal until proper authentication is received. */
/*
  int main(int argc, char *const argv[])
  {
  char *username;
  
  vlock_debug = (getenv("VLOCK_DEBUG") != NULL);
  
  block_signals();
  
  username = get_username();
  
  if (username == NULL)
  fatal_perror("vlock: could not get username");
    
  ensure_atexit(display_auth_tries);
  
  #ifdef USE_PLUGINS
  for (int i = 1; i < argc; i++)
  if (!load_plugin(argv[i]))
  fatal_error("vlock: loading plugin '%s' failed: %s", argv[i], STRERROR);
      
  ensure_atexit(unload_plugins);
  
  if (!resolve_dependencies()) {
  if (errno == 0)
  exit(EXIT_FAILURE);
  else
  fatal_error("vlock: error resolving plugin dependencies: %s", STRERROR);
  }
  
  plugin_hook("vlock_start");
  ensure_atexit(call_end_hook);
  #else /* !USE_PLUGINS */
/* Emulate pseudo plugin "all". *//*
   if (argc == 2 && (strcmp(argv[1], "all") == 0)) {
   if (!lock_console_switch()) {
   if (errno)
   perror("vlock: could not disable console switching");
        
   exit(EXIT_FAILURE);
   }
    
   ensure_atexit((void (*)(void))unlock_console_switch);
   } else if (argc > 1) {
   fatal_error("vlock: plugin support disabled");
   }
   #endif
  
   if (!isatty(STDIN_FILENO))
   fatal_error("vlock: stdin is not a terminal");
  
   setup_terminal();
   ensure_atexit(restore_terminal);
  
   auth_loop(username);
  
   free(username);
  
   exit(0);
   }

                                  */
