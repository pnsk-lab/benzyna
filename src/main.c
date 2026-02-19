#include <ba_runtime.h>

#include <GLFW/glfw3.h>

static char*	   argv1 = NULL;
static GLFWwindow* window;

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
	ba_runtime_t ba;
	int	     i;

	ba.param.turbo = ba_false;
	ba.param.fps   = 30;

	for(i = 1; i < argc; i++) {
		if(strcmp(argv[i], "--turbo") == 0) {
			ba.param.turbo = ba_true;
		} else if(strcmp(argv[i], "--vsync") == 0) {
			ba.param.fps = 0;
		} else if(strcmp(argv[i], "--fps") == 0) {
			if(argv[i + 1] == NULL) {
				fprintf(stderr, "%s: --fps needs argument\n", argv[0]);
				return 1;
			}
			ba.param.fps = atof(argv[i + 1]);
		} else {
			argv1 = argv[i];
		}
	}

	if(argv1 == NULL) return 1;

	glfwInit();

	window = glfwCreateWindow(BA_WIDTH * 2, BA_HEIGHT * 2, "Benzyna Scratch Runtime", NULL, NULL);

	ba.param.make_current  = make_current;
	ba.param.swap_buffer   = swap_buffer;
	ba.param.swap_interval = swap_interval;

	if(!ba_runtime_init(&ba)) {
		return 1;
	}

	if(!ba_runtime_load_path(&ba, argv1)) {
		ba_runtime_uninit(&ba);
		return 1;
	}

	while(1) {
		glfwPollEvents();

		ba_runtime_step(&ba);
	}
	ba_runtime_uninit(&ba);

	glfwDestroyWindow(window);
}
