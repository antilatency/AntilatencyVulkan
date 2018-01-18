#pragma once

#include <QWindow>
#include <QSurface>
#include <QDebug>

class VulkanWindow : public QWindow{
public:
    VulkanWindow() {
        setSurfaceType(QSurface::VulkanSurface);
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
