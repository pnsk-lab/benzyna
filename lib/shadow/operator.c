#include <ba_runtime.h>

#define OP2(name, op) \
	static char* operator_##name(ba_thread_t* thread, ba_block_t* block) { \
		char* str1 = NULL; \
		char* str2 = NULL; \
		char* b	   = NULL; \
\
		if((str1 = ba_thread_input2(thread, block, "NUM1")) != NULL && (str2 = ba_thread_input2(thread, block, "NUM2")) != NULL) { \
			double n1 = atof(str1); \
			double n2 = atof(str2); \
\
			b = malloc(64); \
			sprintf(b, BA_FORMAT_DOUBLE, n1 op n2); \
		} \
\
		if(str1 != NULL) free(str1); \
		if(str2 != NULL) free(str2); \
\
		return b; \
	}

static char* operator_round(ba_thread_t* thread, ba_block_t* block) {
	char* str;
	char* b = NULL;

	if((str = ba_thread_input2(thread, block, "NUM")) != NULL) {
		b = malloc(64);
		sprintf(b, BA_FORMAT_DOUBLE, round(atof(str)));
		free(str);
	}

	return b;
}

OP2(add, +);
OP2(subtract, -);
OP2(multiply, *);
OP2(divide, /);

static char* operator_random(ba_thread_t* thread, ba_block_t* block) {
	char* str1 = NULL;
	char* str2 = NULL;
	char* b	   = NULL;

	if((str1 = ba_thread_input2(thread, block, "FROM")) != NULL && (str2 = ba_thread_input2(thread, block, "TO")) != NULL) {
		double n1   = atof(str1);
		double n2   = atof(str2);
		double low  = (n1 <= n2) ? n1 : n2;
		double high = (n1 <= n2) ? n2 : n1;
		double r    = (double)rand() / RAND_MAX;

		b = malloc(64);
		sprintf(b, BA_FORMAT_DOUBLE, low + (high - low) * r);
	}

	if(str1 != NULL) free(str1);
	if(str2 != NULL) free(str2);

	return b;
}
void ba_shadow_operator(ba_runtime_t* rt) {
	ba_runtime_shadow_handler(rt, "operator_round", operator_round);
	ba_runtime_shadow_handler(rt, "operator_add", operator_add);
	ba_runtime_shadow_handler(rt, "operator_subtract", operator_subtract);
	ba_runtime_shadow_handler(rt, "operator_multiply", operator_multiply);
	ba_runtime_shadow_handler(rt, "operator_divide", operator_divide);
	ba_runtime_shadow_handler(rt, "operator_random", operator_random);
}
