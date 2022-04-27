#pragma once

void override_normal_with_essential();
bool using_essential();

jclass find_class(const char *name);
void get_launchwrapper();
jobject get_minecraft();
jobject get_player(jobject mc);
void send_chat(jobject player, const char *msg, bool prefix = true);
