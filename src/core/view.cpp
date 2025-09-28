#include "core/view.h"
#include <iostream>

DocumentView::DocumentView(std::shared_ptr<DocumentController> controller)
    : m_controller(controller) {}

void DocumentView::runDemo() {
    std::cout << "/-- Graphics Editor Demo --/" << std::endl;
    
    m_controller->createNewDocument();
    std::cout << "New document created" << std::endl;
    
    m_controller->createCircle(100, 100, 50);
    m_controller->createRectangle(200, 200, 300, 150);
    m_controller->renderDocument();
    
    m_controller->removeShape(0);
    m_controller->renderDocument();
    
    m_controller->exportDocument("exported_document.xml");
    m_controller->importDocument("imported_document.xml");
    
    m_controller->createCircle(100, 100, 50);
    m_controller->renderDocument();
    
    std::cout << "/-- Demo Completed --/" << std::endl;
}