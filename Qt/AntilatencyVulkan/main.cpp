#include <QGuiApplication>
#include <QWindow>
#include <QSurface>

#include <QDebug>


#include "../../src/AntilatencyVulkan/AntilatencyVulkan.h"

VulkanInstanceFactory factory;
VulkanInstance<VulkanInstanceFactory> instance(factory);


class VulkanWindow : public QWindow{
public:
	VulkanWindow() {
		setSurfaceType(VulkanSurface);
	}

	void exposeEvent(QExposeEvent *) {
		if (isExposed()) {
			if (!_initialized) {
				_initialized = true;
				// initialize device, swapchain, etc.
				//QVulkanInstance *inst = vulkanInstance();
				//QVulkanFunctions *f = inst->functions();


				//setVulkanInstance(QVulkanInstance() instance.instance);
				//uint32_t devCount = 0;
				//f->vkEnumeratePhysicalDevices(inst->vkInstance(), &devCount, nullptr);

				//auto surface = surfaceHandle();

				// build the first frame
				render();
			}
		}
	}

	bool event(QEvent *e) {
		if (e->type() == QEvent::UpdateRequest)
			render();
		return QWindow::event(e);
	}

	void render() {
	   qDebug()<<"render";
	   requestUpdate(); // render continuously
	}

private:
	bool _initialized = false;
};



int main(int argc, char *argv[]){

	QGuiApplication a(argc, argv);

	VulkanWindow window;

	window.create();
	window.show();


	//window.setSurfaceType(QSurface::VulkanSurface);
	//window.isExposed()

	return a.exec();
}
