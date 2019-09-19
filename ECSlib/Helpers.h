#pragma once
#include <raylib.h>
#include <vector>

inline Rectangle operator+(Rectangle &a, Rectangle b)
{
    return Rectangle { a.x + b.x,a.y + b.y,a.width + b.width,a.height + b.height };
}

inline Rectangle operator-(Rectangle &a, Rectangle b)
{
    return Rectangle{ a.x - b.x,a.y - b.y,a.width - b.width,a.height - b.height };
}

inline Vector2 operator+(Vector2 &a, Vector2 b)
{
    return Vector2{ a.x + b.x,a.y + b.y};
}

inline Vector2 operator-(Vector2 &a, Vector2 b)
{
    return Vector2{ a.x - b.x,a.y - b.y };
}

inline bool operator==(Texture2D& a, Texture2D& b)
{
    return a.id == b.id;
}

inline bool operator==(const Vector2 a, const Vector2 b)
{
    return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const Vector2 a, const Vector2 b)
{
    return a.x != b.x || a.y != b.y;
}

inline bool operator==(Rectangle &a, Rectangle b)
{
    return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
}

inline bool operator!=(Rectangle &a, Rectangle b)
{
    return !(a == b);
}

inline bool operator<(const Vector2& a, const Vector2& b)
{
    return a.x < b.x && a.y < b.y;
}

inline bool operator>(const Vector2& a, const Vector2& b)
{
    return !(a.x < b.x && a.y < b.y);
}

inline bool operator<=(const Vector2& a, const Vector2& b)
{
    return a.x <= b.x && a.y <= b.y;
}

inline bool operator>=(const Vector2& a, const Vector2& b)
{
    return a.x >= b.x && a.y >= b.y;
}

struct Shape
{
    enum ShapeType
    {
        NONE,
        CIRCLE,
        RECTANGLE,
        TRIANGLE,
        POLYGON
    } type = NONE;

    union
    {
        struct
        {
            int x;
            int y; 
            float radius;
        } circle;

        struct
        {
            Rectangle rec;
        } rectangle;

        struct
        {
            Vector2 a;
            Vector2 b;
            Vector2 c;
        } triangle;

        struct
        {
            Vector2 center;
            int sides;
            float radius;
        } poly;
    };

    Color color;
    float rotation;

    void SetCircle(int x, int y, float radius, Color color = RED)
    {
        type = CIRCLE;
        circle.x = x;
        circle.y = y;
        circle.radius = radius;
        this->color = color;
    }
    void SetRectangle(Rectangle rec, float rotation, Color color = RED)
    {
        type = RECTANGLE;
        rectangle.rec = rec;
        this->rotation = rotation;
        this->color = color;
    }
    void SetTriangle(Vector2 a, Vector2 b, Vector2 c, float rotation, Color color = RED)
    {
        type = TRIANGLE;
        triangle.a = a;
        triangle.b = b;
        triangle.c = c;
        this->rotation = rotation;
        this->color = color;
    }
    void SetPoly(Vector2 center, int sides, float radius, float rotation, Color color = RED)
    {
        type = POLYGON;
        poly.center = center;
        poly.sides = sides;
        poly.radius = radius;
        this->rotation = rotation;
        this->color = color;
    }

    void static Draw(Shape& shape)
    {
        if (shape.type == CIRCLE)
        {
            DrawCircle(shape.circle.x, shape.circle.y, shape.circle.radius, shape.color);
        }
        else if (shape.type == RECTANGLE)
        {
            DrawRectanglePro(shape.rectangle.rec, Vector2{ 0,0 }, shape.rotation, shape.color);
        }
        else if (shape.type == TRIANGLE)
        {
            //DrawTriangle(shape.triangle.a, shape.triangle.b, shape.triangle.c, shape.color);
        }
        else if (shape.type == POLYGON)
        {
            DrawPoly(shape.poly.center, shape.poly.sides, shape.poly.radius, shape.rotation, shape.color);
        }
    }
};

constexpr float radiansToDegrees(float rad) { return rad / (2 * PI) * 360; }
constexpr float degreesToRadians(float deg) { return deg / 360 * (2 * PI); }

class ShapeContainer
{
public:

    struct ShapeHolder
    {
        std::string identifier;

        Vector2 offset;
        float initialRotation;
        Shape shape;
        
        bool radians;
    };

    std::vector<ShapeHolder> holders;

    Vector2 lastPosition;
    float lastRotation;

    Vector2& position;
    float& rotation;
    bool radians;
    //INFO: la physac rotatia este in radiani, pe cand in raylib este in grade

    ShapeContainer(Vector2& position, float& rotation, bool radians = true)
        :lastPosition(position), lastRotation(rotation), position(position), rotation(rotation), radians(radians)
    {}

    void AddShape(std::string identifier, Vector2 offset, Shape& shape, bool radians = false)
    {
        Vector2 newPos = position + offset;

        if (shape.type == Shape::CIRCLE)
        {
            shape.circle.x = int(newPos.x);
            shape.circle.y = int(newPos.y);
        }
        else if (shape.type == Shape::RECTANGLE)
        {
            shape.rectangle.rec.x = newPos.x;
            shape.rectangle.rec.y = newPos.y;
        }
        else if (shape.type == Shape::TRIANGLE)
        {
            //to be implemented
        }
        else if (shape.type == Shape::POLYGON)
        {
            shape.poly.center.x = newPos.x;
            shape.poly.center.y = newPos.y;
        }

        holders.push_back(ShapeHolder{ std::move(identifier), offset, shape.rotation, shape, radians});
    }

    void Update() {
        if (lastPosition == position && lastRotation == rotation)
            return;

        for (auto& holder : holders)
        {
            float radius = sqrt(pow(holder.offset.x, 2) + pow(holder.offset.y, 2));
            float initialAngle = atan(holder.offset.x / holder.offset.y);

            if (holder.shape.type == Shape::CIRCLE)
            {
                holder.shape.circle.x = position.x + cos(initialAngle + rotation) * radius;
                holder.shape.circle.y = position.y + sin(initialAngle + rotation) * radius;
            } 
            else if (holder.shape.type == Shape::RECTANGLE)
            {
                holder.shape.rectangle.rec.x = position.x + cos(initialAngle + rotation) * radius;
                holder.shape.rectangle.rec.y = position.y + sin(initialAngle + rotation) * radius;
            }
            else if (holder.shape.type == Shape::TRIANGLE)
            {
                //to be implemented
            }
            else if (holder.shape.type == Shape::POLYGON)
            {
                holder.shape.poly.center.x = position.x + cos(initialAngle + rotation) * radius;
                holder.shape.poly.center.y = position.y + sin(initialAngle + rotation) * radius;
            }

            if (radians && !holder.radians)
                holder.shape.rotation = radiansToDegrees(rotation + holder.initialRotation);
            else if (!radians && holder.radians)
                holder.shape.rotation = degreesToRadians(rotation) + holder.initialRotation;
            else
                holder.shape.rotation = rotation + holder.initialRotation;
        }

        lastPosition = position;
        lastRotation = rotation;
    }

    void static Draw(ShapeContainer& cont)
    {
        for (auto& holder : cont.holders)
        {
            Shape::Draw(holder.shape);
        }
    }
};