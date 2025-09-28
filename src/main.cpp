#include <iostream>
#include <memory>
#include "core/document.h"
#include "core/view.h"

int main() {
    auto controller = std::make_shared<DocumentController>();
    DocumentView view(controller);
    
    view.runDemo();
    
    return 0;
}