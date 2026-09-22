#pragma once
#define BUTTON_COUNT 12
#define KEY_COUNT 62
typedef struct { int language,resolution,stretch,performance,vsync,show,analog; int keys[BUTTON_COUNT]; int fps,profile; int actions[BUTTON_COUNT]; } VitaConfig;
extern VitaConfig vita_config;
void config_defaults(VitaConfig *c);
int config_load(VitaConfig *c,const char *path);
int config_save(const VitaConfig *c,const char *path);
void vita_launcher(void);
void vita_apply_config(void);
void vita_draw_count(unsigned triangles);

int config_commit(const char *temporary,const char *path);

#define ACTION_COUNT 13
void config_preset(VitaConfig *c,int profile);
void vita_input_snapshot(void);

void config_activate_editor(VitaConfig *c,int pc_keys);
