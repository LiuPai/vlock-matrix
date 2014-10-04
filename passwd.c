#include <gcrypt.h>
#include <stdio.h>
#include <string.h>
#include "passwd.h"

static enum passwd_event event = NOTHING;
static char *hash_str = "";

void hash_to_str(unsigned char *hash, char *hashstr){
  int hashlen = gcry_md_get_algo_dlen(GCRY_MD_SHA512);
  char *strp = hashstr;
  int i;
  for (i = 0; i < hashlen; i++) {
    sprintf(strp, "%02X", hash[i]);
    strp += 2;
  }
  *(strp - 1) = '\0';
}

void hash_passwd(char *passwd, unsigned char *hash_result){
  int i;
  
  gcry_md_hd_t digest = NULL;
  gcry_error_t err = 0;
  unsigned char *lochash = NULL;
  int hash_len = gcry_md_get_algo_dlen(GCRY_MD_SHA512);
  
  err = gcry_md_open (&digest, GCRY_MD_SHA512, GCRY_MD_FLAG_SECURE);
  if (err){
    fprintf(stderr, "%s\n", gcry_strerror(err));
    goto done;
    
  }
  for(i = 0; passwd[i] != '\0'; i++){
    gcry_md_putc(digest, passwd[i]);
  }
  gcry_md_final(digest);
  lochash = gcry_md_read(digest, GCRY_MD_SHA512);
  memcpy(hash_result, lochash, hash_len);
done:
  gcry_md_close(digest);
}

enum passwd_event passwd_get_event(){
  return event;
}

void passwd_get_hashstr(char *hashstr){
  memcpy(hashstr,hash_str, 128);
}
