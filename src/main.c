#include <ba_runtime.h>

int main(int argc, char** argv) {
	ba_runtime_t ze;

	if(argc < 2) return 1;

	ze.turbo = ba_false;

	ba_runtime_init(&ze);
	if(ba_runtime_load_path(&ze, argv[1]) != 0) {
		return 1;
	};

	ba_runtime_loop(&ze);
	ba_runtime_uninit(&ze);
}
