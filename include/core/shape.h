#pragma once

#include <memory>
#include <string>

// Базовый интерфейс для графических примитивов
class IShape {
public:
    virtual ~IShape() = default;
    virtual void draw() const = 0;
    virtual std::unique_ptr<IShape> clone() const = 0;
    virtual std::string getName() const = 0;
};

// реализация примитива - Круг
class Circle : public IShape {
public:
    Circle(double x, double y, double radius);
    void draw() const override;
    std::unique_ptr<IShape> clone() const override;
    std::string getName() const override;

private:
    double m_x;
    double m_y;
    double m_radius;
};

// реализация примитива - Прямоугольник
class Rectangle : public IShape {
public:
    Rectangle(double x, double y, double width, double height);
    void draw() const override;
    std::unique_ptr<IShape> clone() const override;
    std::string getName() const override;

private:
    double m_x;
    double m_y;
    double m_width;
    double m_height;
};