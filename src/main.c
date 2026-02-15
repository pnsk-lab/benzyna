#include <ba_runtime.h>

static unsigned char* load_file(ba_runtime_t* rt, const char* path, int* size) {
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
	ba_runtime_t ze;

	if(argc < 2 || (f = fopen(argv[1], "r")) == NULL) return 1;

	fseek(f, 0, SEEK_END);
	sz = ftell(f);
	fseek(f, 0, SEEK_SET);

	buffer = malloc(sz);
	fread(buffer, 1, sz, f);

	fclose(f);

	ba_runtime_init(&ze);
	ze.load_file = load_file;
	ze.turbo     = ba_false;
	ba_runtime_load_project(&ze, buffer, sz);
	free(buffer);
	ba_runtime_loop(&ze);
	ba_runtime_uninit(&ze);
}
