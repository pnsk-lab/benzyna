#include <ba_runtime.h>

char* argv1;

static unsigned char* load_file(ba_runtime_t* rt, const char* path, int* size) {
	FILE*	       f;
	unsigned char* d;
	char*	       p = ba_string_concat(argv1, "/", path, NULL);

	if((f = fopen(p, "rb")) == NULL) {
		free(p);
		return NULL;
	}
	free(p);

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
	char*	     p;

	if(argc < 2) return 1;

	argv1 = argv[1];

	p = ba_string_concat(argv[1], "/project.json", NULL);
	if((f = fopen(p, "r")) == NULL) {
		free(p);
		return 1;
	}
	free(p);

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
