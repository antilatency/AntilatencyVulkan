#include <QCoreApplication>
#include <QWindow>
#include <QSurface>

#include <QDebug>

#include <VulkanInstanceFactory.h>

int main(int argc, char *argv[]) {

    auto instanceFactory = VulkanInstanceFactory::create();

    auto instanceExtensions = instanceFactory->enumerateExtensionProperties();
    qDebug() << "Instance extensions: ";
    for (const auto ie : instanceExtensions) {
        qDebug() << ie.extensionName;
    }


    QCoreApplication a(argc, argv);

    //VulkanWindow window;

    //window.create();
    //window.show();

	//window.setSurfaceType(QSurface::VulkanSurface);
	//window.isExposed()

	return a.exec();
}
