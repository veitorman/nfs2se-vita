#include "config.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv){
 assert(argc==2);VitaConfig c,d;config_defaults(&c);assert(c.resolution==2&&c.keys[4]==5);
 config_preset(&c,1);assert(c.actions[4]==7&&c.actions[8]==3&&c.actions[9]==4&&c.actions[10]==11);c.fps=1;
 VitaConfig snapshot=c;
 config_activate_editor(&c,1);assert(c.profile==2&&!memcmp(c.actions,snapshot.actions,sizeof(c.actions)));
 config_activate_editor(&c,0);assert(c.profile==0&&!memcmp(c.actions,snapshot.actions,sizeof(c.actions)));
 config_preset(&c,1);
 c.language=1;c.stretch=1;c.show=0;c.keys[8]=49;
 assert(config_save(&c,argv[1]));assert(config_load(&d,argv[1]));assert(!memcmp(&c,&d,sizeof(c)));
 c.resolution=0;assert(config_save(&c,argv[1]));assert(config_load(&d,argv[1]));assert(d.resolution==0);
 FILE *f=fopen(argv[1],"w");assert(f);fputs("resolution=99\nlanguage=-2\nbutton8=62\nshow=0junk\nbutton0=0\nvsync=0\n",f);fclose(f);
 assert(config_load(&d,argv[1]));assert(d.resolution==2&&d.language==0&&d.keys[8]==1&&d.show==1&&d.keys[0]==0&&d.vsync==0);
 char bak[512];snprintf(bak,sizeof(bak),"%s.bak",argv[1]);remove(bak);assert(rename(argv[1],bak)==0);
 assert(config_load(&d,argv[1]));assert(d.vsync==0);assert(config_save(&c,argv[1]));
 config_preset(&c,0);assert(c.actions[4]==3&&c.actions[6]==4&&c.actions[0]==5&&c.actions[10]==11);
 assert(!config_save(&c,"/does-not-exist/test.cfg"));puts("config persistence and bounds PASS");
}
