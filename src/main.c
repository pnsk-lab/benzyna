#include <ze_runtime.h>

static unsigned char* load_file(ze_runtime_t* rt, const char* path, int* size) {
	FILE*	       f;
	unsigned char* d;

	if((f = fopen(path, "rb")) == NULL) return NULL;

	fseek(f, 0, SEEK_END);
	*size = ftell(f);
	fseek(f, 0, SEEK_SET);

	d = malloc(*size);
	fread(d, 1, *size, f);

	fclose(f);

	return d;
}

int main(int argc, char** argv) {
	FILE*	     f;
	int	     sz = 0;
	char*	     buffer;
	ze_runtime_t ze;

	if(argc < 2 || (f = fopen(argv[1], "r")) == NULL) return 1;

	fseek(f, 0, SEEK_END);
	sz = ftell(f);
	fseek(f, 0, SEEK_SET);

	buffer = malloc(sz);
	fread(buffer, 1, sz, f);

	fclose(f);

	ze_runtime_init(&ze);
	ze.load_file = load_file;
	ze_runtime_load_project(&ze, buffer, sz);
	free(buffer);
	ze_runtime_loop(&ze);
	ze_runtime_uninit(&ze);
}
