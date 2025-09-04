#pragma once

#include <memory>
#include <vector>
#include <string>
#include "core/shape.h"

class Document {
public:
    Document() = default;

    void addShape(std::unique_ptr<IShape> shape);
    void removeShape(size_t index);
    void clear();
    
    const std::vector<std::unique_ptr<IShape>>& getShapes() const;
    size_t getShapeCount() const;
    void drawAll() const;

private:
    std::vector<std::unique_ptr<IShape>> m_shapes;
};

class DocumentController {
public:
    DocumentController();
    void createNewDocument();
    bool importDocument(const std::string& filename);
    bool exportDocument(const std::string& filename);
    void createCircle(double x, double y, double radius);
    void createRectangle(double x, double y, double width, double height);
    void removeShape(size_t index);
    const Document& getDocument() const;
    void renderDocument();

private:
    std::unique_ptr<Document> m_currentDocument;
};