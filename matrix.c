#include <wchar.h>
#include <cucul.h>
#include "canvas.h"

#define min(a, b) ((a) < (b) ? (a) : (b))

static unsigned int head_color = CUCUL_LIGHTGREEN;
static unsigned int default_color = CUCUL_GREEN;
static unsigned int change_color = CUCUL_LIGHTGREEN;

/* Matrix effect */
#define MAXDROPS 500
#define MINLEN 10
#define MAXLEN 30
#define MAXMUTANT 10
#define MIN_DROP_SPEED 10
#define MAX_DROP_SPEED 20
#define RESET_CHANCE 20

typedef struct mutant {
  int count;
  int pos[MAXMUTANT],speed[MAXMUTANT],left_times[MAXMUTANT];
} mutant;

typedef struct drop {
  int x, y, speed, len;
  mutant mut;
  wchar_t str[1024];
}drop;

void new_mutant(int len, int speed, mutant *mut){
  int i;  
  mut->count = cucul_rand(0,MAXMUTANT);
  for(i = 0;i < mut->count; i++){
    mut->pos[i] = cucul_rand(1, len);
    mut->speed[i] = cucul_rand(2,10);
    mut->left_times[i] = (((len - mut->pos[i]) * (20 / mut->speed[i])) / speed) * 2;
  }
};


static wchar_t *terfont_chars = L"£0@Pp¢¾ÎÞîþēįŃŕũŽ΅ΩρЉЦияҗ¦ҲӰ↓!1AQaq¤¿ÏßïÿĖİńŖŪžΆάςЊЧйђҘҳӱ←2BRbr¥ÀÐàðĀėıŅŗūƆΈέσЋШлѓҙ¬Ҷẽ→±#3CScs§ÁÑáñāĘĶņŘŮƐΉήτЌЩмєҚҷỹ↑$4DTdt©ÂÒâòĂęķŇřůƒΊίυЍЪнљқҸ…%5EUeuªÃÓãóăĚĸňŚŰƝΌΰφЎЫтњҜҹ‰■—&6FVfv«ÄÔäôĄěĹŉśűȘΎαχЏЬфħҝҺ₧≡7GWgw®ÅÕåõąĞĺŊŞŲșΏβψБЭцќҠ†һ€°8HXhxÆÖæöĆğĻŋşųȚΐδωДЮчѝҡ‡Ə№◆9IYiyÇ×ç÷ćĢļŌŠŴțΓεϊЖЯшўҢ•ə≥*JZjzµÈØèøČģĽōšŵȲΘζϋИбщџң‹ӢΔπ+Kk¶ÉÙéùčĨľŐŢŸȳΛηύЙвъҐҪ›ӣ√<Ll|ºÊÚêúĎĩĿőţŹɔΞθώЛгыґҫϴ∞-=Mm»ËÛëûďĪŀŒŤźɛΣιЂΠдьҒγө∩>N^n~¼ÌÜìüđīŁœťŻɲΦλЃУжэғҰ≈Ӯ≤/?Oo¡½ÍÝíýĒĮłŔŨżΨξЄФзюҖұ⌐ӯ";      

void matrix(enum action action, cucul_canvas_t *cv){
  
  drop drop[MAXDROPS];
  size_t terfont_size = wcslen(terfont_chars);
  int w, h, i, j, k;
  
  switch(action)
  {
    case PREPARE:
      for(i = 0; i < MAXDROPS; i++)
      {
        drop[i].x = cucul_rand(0, 1000);
        drop[i].y = cucul_rand(0, 1000);
        drop[i].speed = cucul_rand(MIN_DROP_SPEED, MAX_DROP_SPEED);
        drop[i].len = MINLEN + cucul_rand(0, MINLEN);
        new_mutant(drop[i].len, drop[i].speed, &drop[i].mut);
        for(j = 0; j < MAXLEN; j++)
          drop[i].str[j] = terfont_chars[cucul_rand(0,terfont_size)];
      }
      break;
      
    case INIT:
      break;
      
    case UPDATE:
      w = cucul_get_canvas_width(cv);
      h = cucul_get_canvas_height(cv);
      
      for(i = 0; i < MAXDROPS && i < (w * h / 32); i++)
      {
        drop[i].y += drop[i].speed;
        if(drop[i].y > 1000)
        {
          drop[i].y -= 1000;
          
          drop[i].x = cucul_rand(0, 1000);
        }
        if(cucul_rand(0,RESET_CHANCE) == 1){
          new_mutant(drop[i].len, drop[i].speed, &drop[i].mut);
        }
        for (j = 0;j < drop[i].mut.count; j++) {
          if(drop[i].mut.left_times[j] > 0) {
            if(drop[i].mut.left_times[j] % drop[i].mut.speed[j] == 0)
              drop[i].str[drop[i].mut.pos[j]] = terfont_chars[cucul_rand(0,terfont_size)];
            drop[i].mut.left_times[j]--;
          }
        }
      }
      break;
      
    case RENDER:
      w = cucul_get_canvas_width(cv);
      h = cucul_get_canvas_height(cv);
      
      cucul_set_color_ansi(cv, CUCUL_BLACK, CUCUL_BLACK);
      cucul_clear_canvas(cv);
      
      for(i = 0; i < MAXDROPS && i < (w * h / 32); i++)
      {
        int x, y;
        
        x = drop[i].x * w / 1000;
        y = drop[i].y * (h + MAXLEN) / 1000;
        
        for(j = 0; j < drop[i].len; j++)
        {
          unsigned int fg;
          if(j < 1){
            fg = head_color;
          } else {
            fg = default_color;
          }
          
          for(k = 0;k < drop[i].mut.count; k++){
            if(drop[i].mut.left_times[k] > 0 && drop[i].mut.pos[k] == (y -j) % drop[i].len){
              fg = change_color;
            }
          }
          cucul_set_color_ansi(cv, fg, CACA_BLACK);
          cucul_put_char(cv, x, y - j,
                         drop[i].str[(y - j) % drop[i].len]);
        }
      }
      
      break;
      
    case FREE:
      break;
  }
}
