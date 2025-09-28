#pragma once

#include "core/document.h"
#include <memory>

class DocumentView {
public:
    DocumentView(std::shared_ptr<DocumentController> controller);
    void runDemo();

private:
    std::shared_ptr<DocumentController> m_controller;
};