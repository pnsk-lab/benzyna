#include <ba_runtime.h>

#include <GLFW/glfw3.h>

static char*	   argv1;
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

	if(argc < 2) return 1;

	glfwInit();

	window = glfwCreateWindow(BA_WIDTH * 2, BA_HEIGHT * 2, "Benzyna Scratch Runtime", NULL, NULL);

	ba.param.make_current  = make_current;
	ba.param.swap_buffer   = swap_buffer;
	ba.param.swap_interval = swap_interval;
	ba.param.turbo	       = ba_false;
	ba_runtime_init(&ba);

	if(ba_runtime_load_path(&ba, argv[1]) != 0) {
		return 1;
	};

	while(1) {
		ba_runtime_step(&ba);
	}
	ba_runtime_uninit(&ba);

	glfwDestroyWindow(window);
}
