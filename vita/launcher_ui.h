#pragma once
#include "config.h"
typedef struct {int category,focus,row,editor,save,notice;} LauncherUI;
static int ui_count(const LauncherUI *u){const int counts[]={6,2,3,6,1,1};return u->category==3&&u->editor?14:counts[u->category];}
static void ui_input(LauncherUI *u,unsigned pressed){
 if(pressed&SCE_CTRL_START){u->save=1;return;}
 if(!u->focus){
  if(pressed&SCE_CTRL_UP)u->category=(u->category+5)%6;
  if(pressed&SCE_CTRL_DOWN)u->category=(u->category+1)%6;
  u->row=0;u->editor=0;
  if(pressed&(SCE_CTRL_CROSS|SCE_CTRL_RIGHT)){if(u->category==5)u->save=1;else u->focus=1;}
  return;
 }
 if(pressed&SCE_CTRL_CIRCLE){if(u->editor){u->editor=0;u->row=2;}else {u->focus=0;u->row=0;}return;}
 int n=ui_count(u);
 if(pressed&SCE_CTRL_UP)u->row=(u->row+n-1)%n;
 if(pressed&SCE_CTRL_DOWN)u->row=(u->row+1)%n;
 int delta=(pressed&SCE_CTRL_RIGHT)?1:((pressed&SCE_CTRL_LEFT)?-1:0);
 if(u->category==3){
  if(u->editor){
   if(u->row<12&&delta){int *v=u->editor==1?&vita_config.actions[u->row]:&vita_config.keys[u->row];int limit=u->editor==1?ACTION_COUNT:KEY_COUNT;*v=(*v+limit+delta)%limit;}
   if(pressed&SCE_CTRL_CROSS){
    if(u->row==12){config_activate_editor(&vita_config,u->editor==2);u->notice=u->editor==2?3:4;}
    if(u->row==13){u->editor=0;u->row=2;}
   }
  }else if(pressed&SCE_CTRL_CROSS){
   if(u->row<2){config_preset(&vita_config,u->row);u->notice=u->row+1;}
   else if(u->row==2||u->row==3){u->editor=u->row-1;u->row=0;}
   else if(u->row==4){config_preset(&vita_config,vita_config.profile==1?1:0);u->notice=5;}
   else {u->focus=0;u->row=0;}
  }
 }else if(u->category==0){if(pressed&SCE_CTRL_CROSS)vita_config.language=u->row;
 }else{
  if(pressed&SCE_CTRL_CROSS)delta=1;
  if(delta){int *value=0,limit=2;
   if(u->category==1){value=u->row?&vita_config.stretch:&vita_config.resolution;if(!u->row)limit=3;}
   else if(u->category==2){value=u->row==0?&vita_config.performance:(u->row==1?&vita_config.vsync:&vita_config.fps);}
   else if(u->category==4)value=&vita_config.show;
   if(value)*value=(*value+limit+delta)%limit;
  }
 }
}
