//
// Created by Vasily Shorin on 2023-01-10.
//

#include "util.h"

const char *get_prompt(const struct dc_env *env, struct dc_error *err);

const char *get_prompt(const struct dc_env *env, struct dc_error *err) {
    const char *prompt = dc_getenv(env, "PS1");
    if (prompt == NULL) {
        prompt = "$ ";
    }
    return prompt;
}
