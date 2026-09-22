#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
VitaConfig vita_config;
void config_defaults(VitaConfig *c){
 memset(c,0,sizeof(*c));c->resolution=2;c->stretch=1;c->performance=1;c->vsync=1;c->show=1;
 int keys[BUTTON_COUNT]={1,2,3,4,5,6,7,8,1,2,5,6};memcpy(c->keys,keys,sizeof(keys));config_preset(c,0);
}
void config_preset(VitaConfig *c,int profile){
 const int classic[12]={5,0,1,2,3,6,4,8,9,10,11,0};
 const int modern[12]={5,6,1,2,7,8,10,9,3,4,11,0};
 c->profile=profile;memcpy(c->actions,profile==1?modern:classic,sizeof(c->actions));
}
static int valid(int x,int n){return x>=0 && x<n;}
int config_load(VitaConfig *c,const char *path){
 config_defaults(c);FILE *f=fopen(path,"r");if(!f){char backup[512];snprintf(backup,sizeof(backup),"%s.bak",path);f=fopen(backup,"r");}if(!f)return 0;
 char line[128],key[64],extra;int v;while(fgets(line,sizeof(line),f)){
  if(sscanf(line,"%63[^=]=%d %c",key,&v,&extra)!=2)continue;
  if(!strcmp(key,"language")&&valid(v,6))c->language=v;
  else if(!strcmp(key,"resolution")&&valid(v,3))c->resolution=v;
  else if(!strcmp(key,"stretch")&&valid(v,2))c->stretch=v;
  else if(!strcmp(key,"performance")&&valid(v,2))c->performance=v;
  else if(!strcmp(key,"vsync")&&valid(v,2))c->vsync=v;
  else if(!strcmp(key,"show")&&valid(v,2))c->show=v;
  else if(!strcmp(key,"fps")&&valid(v,2))c->fps=v;
  else if(!strcmp(key,"profile")&&valid(v,3))c->profile=v;
  else if(!strcmp(key,"analog")&&valid(v,2))c->analog=v;
  else for(int i=0;i<BUTTON_COUNT;i++){char k[16];snprintf(k,sizeof(k),"button%d",i);if(!strcmp(key,k)&&valid(v,KEY_COUNT))c->keys[i]=v;snprintf(k,sizeof(k),"action%d",i);if(!strcmp(key,k)&&valid(v,ACTION_COUNT))c->actions[i]=v;}
 }fclose(f);return 1;
}
int config_save(const VitaConfig *c,const char *path){
 char tmp[512];if(snprintf(tmp,sizeof(tmp),"%s.tmp",path)>=(int)sizeof(tmp))return 0;
 FILE *f=fopen(tmp,"w");if(!f)return 0;
 int ok=fprintf(f,"language=%d\nresolution=%d\nstretch=%d\nperformance=%d\nvsync=%d\nshow=%d\nanalog=%d\n",c->language,c->resolution,c->stretch,c->performance,c->vsync,c->show,c->analog)>0;
 if(fprintf(f,"fps=%d\nprofile=%d\n",c->fps,c->profile)<0)ok=0;
 for(int i=0;i<BUTTON_COUNT;i++)if(fprintf(f,"action%d=%d\n",i,c->actions[i])<0)ok=0;
 for(int i=0;i<BUTTON_COUNT;i++)if(fprintf(f,"button%d=%d\n",i,c->keys[i])<0)ok=0;
 if(fclose(f)!=0)ok=0;
 if(!ok)return 0;
 return config_commit(tmp,path);
}

int config_commit(const char *temporary,const char *path){
 char backup[512];if(snprintf(backup,sizeof(backup),"%s.bak",path)>=(int)sizeof(backup))return 0;
 struct stat st;int existed=stat(path,&st)==0;
 if(existed){
  if(remove(backup)!=0&&errno!=ENOENT)return 0;
  if(rename(path,backup)!=0)return 0;
 }
 if(rename(temporary,path)==0)return 1;
 if(existed)rename(backup,path);
 return 0;
}

void config_activate_editor(VitaConfig *c,int pc_keys){
 if(pc_keys)c->profile=2;else if(c->profile==2)c->profile=0;
}
