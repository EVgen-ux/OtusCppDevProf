#include "core/document.h"
#include <iostream>

void Document::addShape(std::unique_ptr<IShape> shape) {
    m_shapes.push_back(std::move(shape));
}

void Document::removeShape(size_t index) {
    if (index < m_shapes.size()) {
        m_shapes.erase(m_shapes.begin() + index);
    }
}

void Document::clear() {
    m_shapes.clear();
}

const std::vector<std::unique_ptr<IShape>>& Document::getShapes() const {
    return m_shapes;
}

size_t Document::getShapeCount() const {
    return m_shapes.size();
}

void Document::drawAll() const {
    for (const auto& shape : m_shapes) {
        shape->draw();
    }
}

DocumentController::DocumentController() {
    createNewDocument();
}

void DocumentController::createNewDocument() {
    m_currentDocument = std::make_unique<Document>();
}

bool DocumentController::importDocument(const std::string& filename) {
    std::cout << "Importing document from: " << filename << std::endl;
    createNewDocument();
    return true;
}

bool DocumentController::exportDocument(const std::string& filename) {
    std::cout << "Exporting document to: " << filename << std::endl;
    return true;
}

void DocumentController::createCircle(double x, double y, double radius) {
    m_currentDocument->addShape(std::make_unique<Circle>(x, y, radius));
}

void DocumentController::createRectangle(double x, double y, double width, double height) {
    m_currentDocument->addShape(std::make_unique<Rectangle>(x, y, width, height));
}

void DocumentController::removeShape(size_t index) {
    m_currentDocument->removeShape(index);
    std::cout << "Shape at index " << index << " removed" << std::endl;
}

const Document& DocumentController::getDocument() const {
    return *m_currentDocument;
}

void DocumentController::renderDocument() {
    std::cout << "/-- Document Content --/" << std::endl;
    m_currentDocument->drawAll();
    std::cout << "------------------------" << std::endl;
}