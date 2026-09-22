#include <assert.h>
#include <stdio.h>
enum {SCE_CTRL_START=1,SCE_CTRL_UP=2,SCE_CTRL_DOWN=4,SCE_CTRL_LEFT=8,SCE_CTRL_RIGHT=16,SCE_CTRL_CROSS=32,SCE_CTRL_CIRCLE=64};
#include "launcher_ui.h"
int main(void){
 config_defaults(&vita_config);LauncherUI u={0};
 for(int i=1;i<=6;i++){ui_input(&u,SCE_CTRL_DOWN);assert(u.category==i%6);}
 ui_input(&u,SCE_CTRL_CROSS);ui_input(&u,SCE_CTRL_DOWN);ui_input(&u,SCE_CTRL_CROSS);assert(vita_config.language==1);
 ui_input(&u,SCE_CTRL_CIRCLE);assert(!u.focus);
 ui_input(&u,SCE_CTRL_DOWN);ui_input(&u,SCE_CTRL_CROSS);int old=vita_config.resolution;ui_input(&u,SCE_CTRL_RIGHT);assert(vita_config.resolution==(old+1)%3);
 u=(LauncherUI){.category=3,.focus=1,.row=1};ui_input(&u,SCE_CTRL_CROSS);assert(vita_config.profile==1);
 u.row=3;ui_input(&u,SCE_CTRL_CROSS);assert(u.editor==2&&vita_config.profile==1);
 int key=vita_config.keys[0];ui_input(&u,SCE_CTRL_RIGHT);assert(vita_config.keys[0]==(key+1)%KEY_COUNT&&vita_config.profile==1);
 u.row=12;ui_input(&u,SCE_CTRL_CROSS);assert(vita_config.profile==2);
 ui_input(&u,SCE_CTRL_CIRCLE);assert(!u.editor&&u.row==2);ui_input(&u,SCE_CTRL_CROSS);assert(u.editor==1&&vita_config.profile==2);
 u.row=12;ui_input(&u,SCE_CTRL_CROSS);assert(vita_config.profile==0);
 ui_input(&u,SCE_CTRL_CIRCLE);ui_input(&u,SCE_CTRL_CIRCLE);assert(!u.focus);
 u=(LauncherUI){.category=3,.focus=1,.editor=1};ui_input(&u,SCE_CTRL_UP);assert(u.row==13);ui_input(&u,SCE_CTRL_CROSS);assert(!u.editor);
 ui_input(&u,SCE_CTRL_START);assert(u.save);
 u=(LauncherUI){.category=5};ui_input(&u,SCE_CTRL_CROSS);assert(u.save);
 puts("Launcher navigation and explicit profile activation PASS");return 0;
}
