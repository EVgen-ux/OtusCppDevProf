#include "core/shape.h"
#include <iostream>

Circle::Circle(double x, double y, double radius)
    : m_x(x), m_y(y), m_radius(radius) {}

void Circle::draw() const {
    std::cout << "Drawing Circle at (" << m_x << ", " << m_y 
              << ") with radius " << m_radius << std::endl;
}

std::unique_ptr<IShape> Circle::clone() const {
    return std::make_unique<Circle>(*this);
}

std::string Circle::getName() const {
    return "Circle";
}

Rectangle::Rectangle(double x, double y, double width, double height)
    : m_x(x), m_y(y), m_width(width), m_height(height) {}

void Rectangle::draw() const {
    std::cout << "Drawing Rectangle at (" << m_x << ", " << m_y 
              << ") with size " << m_width << "x" << m_height << std::endl;
}

std::unique_ptr<IShape> Rectangle::clone() const {
    return std::make_unique<Rectangle>(*this);
}

std::string Rectangle::getName() const {
    return "Rectangle";
}