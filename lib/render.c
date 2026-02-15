#include <ze_runtime.h>

void ze_render(ze_runtime_t* rt) {
	glfwMakeContextCurrent(rt->window);
	glfwSwapInterval(1);

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glfwSwapBuffers(rt->window);
}
