#include <stdbool.h>
#include <stdio.h>
#include <gcrypt.h>
#include "passwd.h"
#include "canvas.h"


static unsigned int head_color = CUCUL_LIGHTGREEN;
static unsigned int default_color = CUCUL_GREEN;
static unsigned int change_color = CUCUL_LIGHTGREEN;

char *cell_chars = "0123456789ABCDEF";

typedef struct matrix_cell {
  char val;
  int active_count;
}cell;

typedef struct hash_line {
  int updates;
  int update_pos[HASH_STR_LEN];
  char hashstr[HASH_STR_LEN];
  char stack[MAX_PASSWD_LEN][HASH_STR_LEN];
}hash_line;

static  hash_line hashline;
static cell cells[500][250];
void passwd_breaker(enum action action, cucul_canvas_t *cv){
  int i,j,k;
  int w = cucul_get_canvas_width(cv);
  int h = cucul_get_canvas_height(cv);
  int cell_chars_len = strlen(cell_chars);
  // don't know why errors on right side
  
  switch(action)
  {
    case PREPARE:
      // reset canvas
      cucul_set_color_ansi(cv, CUCUL_BLACK, CUCUL_BLACK);
      cucul_clear_canvas(cv);
      break;
      
    case INIT:
      // init hashline
      hashline.updates = 0;
      for(i = 0;i < HASH_STR_LEN;i++){
        hashline.hashstr[i] = '0';
        hashline.update_pos[i] = -1;
        for(j = 0; j < HASH_STR_LEN; j++){
          hashline.stack[i][j] = '0';
        }
      }
      // init cells
      for(i = 0;i < w; i++){
        for(j = 0;j < h - 2;j++){
          cells[i][j].val = cell_chars[cucul_rand(0, cell_chars_len - 1)];
          cells[i][j].active_count = -1;
        }
      }
      break;
      
    case UPDATE:
      // update hashline
      switch(passwd_get_event()){
        case NOTHING:
          break;
        case INSERT:
          hashline.updates++;
          passwd_get_hashstr(hashline.stack[hashline.updates]);
          hashline.update_pos[hashline.updates] = 0;
          break;
        case CLEAR:
          hashline.updates = 0;
          for(i = 0;i < HASH_STR_LEN;i++){
            hashline.hashstr[i] = '0';
            hashline.update_pos[i] = -1;
            for(j = 0; j < HASH_STR_LEN; j++){
              hashline.stack[i][j] = '0';
            }
          }
          break;
        case OVERFLOW:
          break;
      }
      // update hashline str
      for(i = 0;i < hashline.updates;i++){
        // update pos
        j = hashline.update_pos[i];
        hashline.hashstr[j] = hashline.stack[i][j];
        hashline.stack[i][j] = '0';
        if(++hashline.update_pos[i] > HASH_STR_LEN){
          hashline.update_pos[i] = -1;
        }
      }
      
      //update cells
      for(i = 0;i < w; i++){
        for(j = 0;j < h - 2;j++){
          if(cells[i][j].active_count != 0){
            cells[i][j].val = cell_chars[cucul_rand(0, 16)];
            if(cells[i][j].active_count > 0)
              cells[i][j].active_count--;
            if(cells[i][j].active_count == -1 && cucul_rand(0, 16) == 0){
              cells[i][j].active_count = 16;
            }
          }
        }         
      }
      
      break;
      
    case RENDER:
      cucul_set_color_ansi(cv, CUCUL_BLACK, CUCUL_BLACK);
      cucul_clear_canvas(cv);
      unsigned int fg;
      for (i = 0;i < HASH_STR_LEN; i++){
        if(hashline.update_pos[i] != -1){
          fg = CUCUL_WHITE;
        }else {
          fg = CUCUL_GREEN;
        }
        cucul_set_color_ansi(cv, fg, CACA_BLACK);
        cucul_put_char(cv, i, 0,
                       hashline.hashstr[i]);
      }
      
      
      for(i = 0; i < w; i++){
        for(j = 0;j < h - 2; j++){
          
          if(cells[i][j].active_count <= 0){
            fg = default_color;
          } else {
            fg = change_color;
          }
          
          cucul_set_color_ansi(cv, fg, CACA_BLACK);
          cucul_put_char(cv, i, j + 2,
                         cells[i][j].val);
          
        }
      }
      break;
      
    case FREE:
      break;
  }
  
  
}
