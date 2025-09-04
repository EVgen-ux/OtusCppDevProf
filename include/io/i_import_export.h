#pragma once

#include <memory>
#include <string>

class Document;

// Интерфейс для импорта документов
class IImport {
public:
    virtual ~IImport() = default;
    virtual std::unique_ptr<Document> importFromFile(const std::string& filename) = 0;
};

// Интерфейс для экспорта документов
class IExport {
public:
    virtual ~IExport() = default;
    virtual bool exportToFile(const Document& document, const std::string& filename) = 0;
};