#include <Windows.h>
#include <iostream>
#include <jvmti.h>
#include <jni.h>
#include <string>
#include <winuser.h>

#include "functions.h"
#include "dllmain.h"

using namespace std;

static string PREFIX = "§8[§aEssentialPatcher§8] §7";

jmethodID findclass_md;
jobject classloader_obj;

jint JNICALL heapCallback(
	jlong class_tag,
	jlong size,
	jlong *tag_ptr,
	jint length,
	void *user_data)
{
	*tag_ptr = 1;
	return JVMTI_VISIT_OBJECTS;
}

jobject get_essential_classloader()
{
	jclass relaunchLoader = get_env()->FindClass("gg/essential/loader/stage2/relaunch/RelaunchClassLoader");

	if (relaunchLoader == NULL)
	{
		printf("Couldn't find RelaunchClassLoader.\n");
		return NULL;
	}

	jvmtiHeapCallbacks callbacks = {};
	callbacks.heap_iteration_callback = heapCallback;

	get_jvmti()->IterateThroughHeap(
		0,
		relaunchLoader,
		&callbacks,
		NULL);

	jlong tags = 1;
	jint count;
	jobject *instances;

	get_jvmti()->GetObjectsWithTags(
		1,
		&tags,
		&count,
		&instances,
		NULL);

	jobject target = instances[0];

	get_jvmti()->Deallocate((unsigned char *)instances);

	jmethodID rl_findclass_md = get_env()->GetMethodID(relaunchLoader, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");

	jstring launchwrapper = get_env()->NewStringUTF("net.minecraft.launchwrapper.LaunchClassLoader");
	jclass launchwrapper_cls = reinterpret_cast<jclass>(get_env()->CallObjectMethod(target, rl_findclass_md, launchwrapper));

	jstring launch = get_env()->NewStringUTF("net.minecraft.launchwrapper.Launch");
	jclass launch_cls = reinterpret_cast<jclass>(get_env()->CallObjectMethod(target, rl_findclass_md, launch));

	jfieldID classloader_fid = get_env()->GetStaticFieldID(launch_cls, "classLoader", "Lnet/minecraft/launchwrapper/LaunchClassLoader;");

	jobject classloader_obj = get_env()->NewGlobalRef(get_env()->GetStaticObjectField(launch_cls, classloader_fid));

	get_env()->DeleteLocalRef(launchwrapper_cls);
	get_env()->DeleteLocalRef(launch_cls);
	get_env()->DeleteLocalRef(launchwrapper);
	get_env()->DeleteLocalRef(launch);

	return classloader_obj;
}

void override_normal_with_essential()
{
	jobject essentialClassloader = get_essential_classloader();

	if (essentialClassloader == NULL)
	{
		printf("Couldn't find Essential classloader.\n");
		return;
	}

	jclass launch_cls = get_env()->FindClass("net/minecraft/launchwrapper/Launch");
	jfieldID classloader_fid = get_env()->GetStaticFieldID(launch_cls, "classLoader", "Lnet/minecraft/launchwrapper/LaunchClassLoader;");

	get_env()->SetStaticObjectField(launch_cls, classloader_fid, essentialClassloader);

	get_env()->DeleteLocalRef(launch_cls);
}

bool using_essential()
{
	return get_env()->FindClass("gg/essential/loader/stage2/relaunch/RelaunchClassLoader") != NULL;
}

/* Minecraft Functions */

inline jclass find_class(const char *name)
{
	jstring jname = get_env()->NewStringUTF(name);
	jclass cls = reinterpret_cast<jclass>(get_env()->CallObjectMethod(classloader_obj, findclass_md, jname));

	get_env()->DeleteLocalRef(jname);

	return cls;
}

void set_launchwrapper_essential()
{
	jclass relaunchLoader = get_env()->FindClass("gg/essential/loader/stage2/relaunch/RelaunchClassLoader");

	if (relaunchLoader == NULL)
	{
		printf("Couldn't find loader\n");
		return;
	}

	jvmtiHeapCallbacks callbacks = {};
	callbacks.heap_iteration_callback = heapCallback;

	get_jvmti()->IterateThroughHeap(
		0,
		relaunchLoader,
		&callbacks,
		NULL);

	jlong tags = 1;
	jint count;
	jobject *instances;

	get_jvmti()->GetObjectsWithTags(
		1,
		&tags,
		&count,
		&instances,
		NULL);

	jobject target = instances[0];

	get_jvmti()->Deallocate((unsigned char *)instances);

	jmethodID rl_findclass_md = get_env()->GetMethodID(relaunchLoader, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");

	jstring launchwrapper = get_env()->NewStringUTF("net.minecraft.launchwrapper.LaunchClassLoader");
	jclass launchwrapper_cls = reinterpret_cast<jclass>(get_env()->CallObjectMethod(target, rl_findclass_md, launchwrapper));

	jstring launch = get_env()->NewStringUTF("net.minecraft.launchwrapper.Launch");
	jclass launch_cls = reinterpret_cast<jclass>(get_env()->CallObjectMethod(target, rl_findclass_md, launch));

	auto classloader_fid = get_env()->GetStaticFieldID(launch_cls, "classLoader", "Lnet/minecraft/launchwrapper/LaunchClassLoader;");

	findclass_md = get_env()->GetMethodID(launchwrapper_cls, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	classloader_obj = get_env()->NewGlobalRef(get_env()->GetStaticObjectField(launch_cls, classloader_fid));

	get_env()->DeleteLocalRef(launchwrapper_cls);
	get_env()->DeleteLocalRef(launch_cls);
	get_env()->DeleteLocalRef(launchwrapper);
	get_env()->DeleteLocalRef(launch);
}

void set_launchwrapper_normal()
{
	jclass launchwrapper_cls = get_env()->FindClass("net/minecraft/launchwrapper/LaunchClassLoader");
	jclass launch_cls = get_env()->FindClass("net/minecraft/launchwrapper/Launch");

	auto classloader_fid = get_env()->GetStaticFieldID(launch_cls, "classLoader", "Lnet/minecraft/launchwrapper/LaunchClassLoader;");

	findclass_md = get_env()->GetMethodID(launchwrapper_cls, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	classloader_obj = get_env()->NewGlobalRef(get_env()->GetStaticObjectField(launch_cls, classloader_fid));

	get_env()->DeleteLocalRef(launchwrapper_cls);
	get_env()->DeleteLocalRef(launch_cls);
}

void get_launchwrapper()
{
	if (using_essential())
		set_launchwrapper_essential();
	else
		set_launchwrapper_normal();
}

jobject get_minecraft()
{
	jobject res;

	auto cls = find_class("net.minecraft.client.Minecraft");
	auto mc_fid = get_env()->GetStaticMethodID(cls, "func_71410_x", "()Lnet/minecraft/client/Minecraft;");

	res = get_env()->CallStaticObjectMethod(cls, mc_fid);

	get_env()->DeleteLocalRef(cls);

	return res;
}

jobject get_player(jobject mc)
{
	jobject res;

	auto cls = find_class("net.minecraft.client.Minecraft");
	auto player_fid = get_env()->GetFieldID(cls, "field_71439_g", "Lnet/minecraft/client/entity/EntityPlayerSP;");

	res = get_env()->GetObjectField(mc, player_fid);

	get_env()->DeleteLocalRef(cls);

	return res;
}

void send_chat(jobject player, const char *msg, bool prefix)
{
	auto epsp_cls = find_class("net.minecraft.client.entity.EntityPlayerSP");
	auto send_chat_fid = get_env()->GetMethodID(epsp_cls, "func_145747_a", "(Lnet/minecraft/util/IChatComponent;)V");
	auto msg_cls = find_class("net.minecraft.util.ChatComponentText");
	auto msg_fid = get_env()->GetMethodID(msg_cls, "<init>", "(Ljava/lang/String;)V");

	jstring message = get_env()->NewStringUTF(msg);
	if (prefix)
		message = get_env()->NewStringUTF((PREFIX + msg).c_str());
	auto msg_obj = get_env()->NewObject(msg_cls, msg_fid, message);

	get_env()->CallVoidMethod(player, send_chat_fid, msg_obj);

	get_env()->DeleteLocalRef(epsp_cls);
	get_env()->DeleteLocalRef(msg_cls);
	get_env()->DeleteLocalRef(msg_obj);
}
