#include <iostream>
#include "core/document.h"

void onNewDocument(DocumentController& controller) {
    controller.createNewDocument();
    std::cout << "New document created" << std::endl;
}

void onImportDocument(DocumentController& controller) {
    controller.importDocument("imported_document.xml");
}

void onExportDocument(DocumentController& controller) {
    controller.exportDocument("exported_document.xml");
}

void onCreateCircle(DocumentController& controller) {
    controller.createCircle(100, 100, 50);
}

void onCreateRectangle(DocumentController& controller) {
    controller.createRectangle(200, 200, 300, 150);
}

void onRemoveShape(DocumentController& controller) {
    controller.removeShape(0);
}

void onRenderDocument(DocumentController& controller) {
    controller.renderDocument();
}

int main() {
    
    DocumentController controller;
    
    std::cout << "/-- Graphics Editor Demo --/" << std::endl;
    
    onNewDocument(controller);
    
    onCreateCircle(controller);
    onCreateRectangle(controller);
    onRenderDocument(controller);
    
    onRemoveShape(controller);
    onRenderDocument(controller);
    
    onExportDocument(controller);
    onImportDocument(controller);
    
    onCreateCircle(controller);
    onRenderDocument(controller);
    
    std::cout << "/-- Demo Completed --/" << std::endl;
    
    return 0;
}