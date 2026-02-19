#include <ba_runtime.h>

static int event_broadcast(ba_thread_t* thread) {
	int   i;
	char* inp = ba_thread_input(thread, "BROADCAST_INPUT");

	if(inp == NULL) return ba_status_next;

	for(i = 0; i < arrlen(thread->runtime->targets); i++) {
		int j;
		for(j = 0; j < arrlen(thread->runtime->targets[i]->tree); j++) {
			char* opt = NULL;

			if(strcmp(thread->runtime->targets[i]->tree[j]->opcode, "event_whenbroadcastreceived") == 0 && (opt = ba_thread_field2(thread, thread->runtime->targets[i]->tree[j], "BROADCAST_OPTION")) != NULL && strcmp(opt, inp) == 0) {
				int k;
				for(k = 0; k < arrlen(thread->runtime->sprites); k++) {
					if(thread->runtime->sprites[k]->target == thread->runtime->targets[i]) {
						ba_thread_t* t = ba_thread_start(thread->runtime, thread->runtime->targets[i]->tree[j]);

						t->sprite    = thread->runtime->sprites[k];
						t->autoclean = ba_true;
					}
				}
			}

			if(opt != NULL) free(opt);
		}
	}

	free(inp);

	return ba_status_next;
}

static ba_bool event_broadcastandwait_check(ba_thread_t* thread) {
	int	      i;
	ba_thread_t** threads = thread->wait.arg;

	for(i = 0; i < arrlen(threads); i++) {
		if(!threads[i]->stopped) return ba_true;
	}

	return ba_false;
}

static void event_broadcastandwait_free_arg(void* arg) {
	int	      i;
	ba_thread_t** threads = arg;

	for(i = 0; i < arrlen(threads); i++) threads[i]->autoclean = ba_true;

	arrfree(threads);
}

static int event_broadcastandwait(ba_thread_t* thread) {
	int	      i;
	char*	      inp     = ba_thread_input(thread, "BROADCAST_INPUT");
	ba_thread_t** threads = NULL;

	if(inp == NULL) return ba_status_next;

	for(i = 0; i < arrlen(thread->runtime->targets); i++) {
		int j;
		for(j = 0; j < arrlen(thread->runtime->targets[i]->tree); j++) {
			char* opt = NULL;

			if(strcmp(thread->runtime->targets[i]->tree[j]->opcode, "event_whenbroadcastreceived") == 0 && (opt = ba_thread_field2(thread, thread->runtime->targets[i]->tree[j], "BROADCAST_OPTION")) != NULL && strcmp(opt, inp) == 0) {
				int k;
				for(k = 0; k < arrlen(thread->runtime->sprites); k++) {
					if(thread->runtime->sprites[k]->target == thread->runtime->targets[i]) {
						ba_thread_t* t = ba_thread_start(thread->runtime, thread->runtime->targets[i]->tree[j]);

						t->sprite = thread->runtime->sprites[k];

						arrput(threads, t);
					}
				}
			}

			if(opt != NULL) free(opt);
		}
	}

	free(inp);

	thread->wait.check    = event_broadcastandwait_check;
	thread->wait.arg      = threads;
	thread->wait.free_arg = event_broadcastandwait_free_arg;

	return ba_status_next;
}

void ba_block_event(ba_runtime_t* rt) {
	ba_runtime_block_handler(rt, "event_broadcast", event_broadcast);
	ba_runtime_block_handler(rt, "event_broadcastandwait", event_broadcastandwait);
}
