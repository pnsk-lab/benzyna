#include <ba_runtime.h>

#include <GLFW/glfw3.h>

static char*	   argv1;
static GLFWwindow* window;

static unsigned char* load_file(ba_runtime_t* rt, const char* path, int* siba) {
	FILE*	       f;
	unsigned char* d;
	char*	       p = ba_string_concat(argv1, "/", path, NULL);

	if((f = fopen(p, "rb")) == NULL) {
		free(p);
		return NULL;
	}
	free(p);

	fseek(f, 0, SEEK_END);
	*siba = ftell(f);
	fseek(f, 0, SEEK_SET);

	d = malloc(*siba);
	fread(d, 1, *siba, f);

	fclose(f);

	return d;
}

static void make_current(ba_runtime_t* rt) {
	glfwMakeContextCurrent(window);
}

static void swap_buffer(ba_runtime_t* rt) {
	glfwSwapBuffers(window);
}

static void swap_interval(ba_runtime_t* rt, int interval) {
	glfwSwapInterval(interval);
}

int main(int argc, char** argv) {
	FILE*	     f;
	int	     sz = 0;
	char*	     buffer;
	ba_runtime_t ba;
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

	glfwInit();

	window = glfwCreateWindow(BA_WIDTH * 2, BA_HEIGHT * 2, "Benzyna Scratch Runtime", NULL, NULL);

	ba.param.load_file     = load_file;
	ba.param.make_current  = make_current;
	ba.param.swap_buffer   = swap_buffer;
	ba.param.swap_interval = swap_interval;
	ba.param.turbo	       = ba_false;
	ba_runtime_init(&ba);
	ba_runtime_load_project(&ba, buffer, sz);
	free(buffer);
	while(1) {
		ba_runtime_step(&ba);
	}
	ba_runtime_uninit(&ba);

	glfwDestroyWindow(window);
}
