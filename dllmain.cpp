#include <Windows.h>
#include <iostream>
#include <jvmti.h>
#include <jni.h>
#include <string>
#include <winuser.h>
#include <jvmti.h>

#include "dllmain.h"
#include "functions.h"

using namespace std;

static boolean DEBUG = false;

HMODULE dll;

static JavaVM *jvm;
static JNIEnv *env;
static jvmtiEnv *jvmti;

void inject()
{
	if (DEBUG)
	{
		AllocConsole();
		SetConsoleCtrlHandler(NULL, true);

		FILE *fIn;
		FILE *fOut;
		freopen_s(&fIn, "conin$", "r", stdin);
		freopen_s(&fOut, "conout$", "w", stdout);
		freopen_s(&fOut, "conout$", "w", stderr);
	}

	printf("Injection Successful!\n");

	jsize count;

	if (JNI_GetCreatedJavaVMs(&jvm, 1, &count) != JNI_OK || count == 0)
	{
		printf("Failed to get the JVM\n");
		return;
	}

	jint res = jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

	if (res == JNI_EDETACHED)
		res = jvm->AttachCurrentThread((void **)&env, nullptr);

	if (res != JNI_OK)
	{
		printf("Failed to attach to thread\n");
		return;
	}

	jint res2 = jvm->GetEnv((void **)&jvmti, JVMTI_VERSION_1_2);

	if (res2 == JNI_EDETACHED)
		res2 = jvm->AttachCurrentThread((void **)&jvmti, nullptr);

	if (res2 != JNI_OK)
	{
		printf("Failed to attach to thread\n");
		return;
	}

	jvmtiCapabilities caps = {};
	caps.can_tag_objects = 1;

	if (jvmti->AddCapabilities(&caps) != JVMTI_ERROR_NONE)
	{
		printf("Failed to add capabilities\n");
		return;
	}

	printf("Attached to JVM.\n");

	bool essential = using_essential();

	get_launchwrapper();
	auto mc = get_minecraft();
	auto player = get_player(mc);

	send_chat(player, "§8<> §7EssentialPatcher - by Debuggings §8<>");
	send_chat(player, "§aInjected successfully!");

	printf("Using Essential? %s\n", essential ? "Yes" : "No");

	if (essential)
	{
		send_chat(player, "§aUsing Essential, overriding classloader.");

		override_normal_with_essential();
	}
	else
	{
		send_chat(player, "§cNot using Essential, aborting...");
	}

	send_chat(player, "Done. Exiting...");
	printf("Done. Exiting...\n");
}

void thread()
{
	inject();

	FreeLibraryAndExitThread(dll, 0);
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	dll = hinstDLL;
	if (fdwReason == DLL_PROCESS_ATTACH)
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)thread, nullptr, 0, nullptr);
	return TRUE;
}

JavaVM *get_jvm()
{
	return jvm;
}

JNIEnv *get_env()
{
	return env;
}

jvmtiEnv *get_jvmti()
{
	return jvmti;
}
