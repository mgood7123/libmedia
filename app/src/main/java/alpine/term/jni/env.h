//
// Created by konek on 7/9/2019.
//

#ifndef MEDIA_PLAYER_PRO_ENV_H
#define MEDIA_PLAYER_PRO_ENV_H

extern char * strcatb(char *dest, const char *src);
typedef char ** env_t;
extern env_t environ_default;
extern int env__size(env_t env);
extern char * env__get(env_t env, const char *envtarget);
extern char * env__get_name(env_t env, const char *envtarget);
extern void env__print(env_t env, const char *envtarget);
extern char * env__return(env_t env, const char *envtarget);
extern int env__getposition(env_t env, const char *envtarget);
extern env_t env__new();
extern void env__free(env_t env);
extern void env__clear(env_t env);
extern void env__list(env_t env);
extern void env__put(env_t env);
extern env_t env__add(env_t env, const char * string);
extern env_t env__add_allow_duplicates(env_t env, const char * string);
extern env_t env__copy(env_t env);
extern env_t env__add_env(env_t env, char * name, env_t * variable);
extern env_t * env__get_env(env_t env, char * s);
extern env_t env__remove(env_t env, const char * string);
extern env_t env__replace(env_t env, const char * string, const char * string2);
extern env_t env__append(env_t env, const char * name, const char * string);
extern env_t env__append_env(env_t env1, env_t env2);
extern env_t env__clean(env_t env1);

#endif //MEDIA_PLAYER_PRO_ENV_H
