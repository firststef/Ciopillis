#pragma once
#include <raylib.h>
#include <utility>
#include <vector>
#include <functional>
#include <cmath>

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

inline Vector2 operator* (Vector2 &a, Vector2 b)
{
    return Vector2{ a.x * b.x,a.y * b.y };
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

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

struct Shape
{
    typedef std::string Identifier;

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

    Identifier id;
    Identifier objectClass;

    Color color;
    float rotation;

    Shape(Identifier id, Identifier objectClass) 
    : id(std::move(id)), objectClass(objectClass) 
    {}

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

    void Draw()
    {
        if (type == CIRCLE)
        {
            DrawCircle(circle.x, circle.y, circle.radius, color);
        }
        else if (type == RECTANGLE)
        {
            DrawRectanglePro(rectangle.rec, Vector2{ 0,0 }, rotation, color);
        }
        else if (type == TRIANGLE)
        {
            //DrawTriangle(shape.triangle.a, shape.triangle.b, shape.triangle.c, shape.color);
        }
        else if (type == POLYGON)
        {
            DrawPoly(poly.center, poly.sides, poly.radius, rotation, color);
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
        Shape shape;

        Vector2 offset;
        float initialRotation;
        Vector2 currentOrientation;
        
        bool radians;
    };

    std::vector<ShapeHolder> holders;

    Vector2 lastPosition;
    float lastRotation;

    Vector2* position;
    float* rotation;

    std::function<Vector2(void*)> positionGetter;
    void* posContext;

    std::function<float(void*)> rotationGetter;
    void* rotContext;

    bool radians;
    //INFO: la physac rotatia este in radiani, pe cand in raylib este in grade

    ShapeContainer(Vector2* position, float* rotation, bool radians = true)
        :lastPosition(*position), lastRotation(*rotation), position(position), rotation(rotation), radians(radians)
    {}

    ShapeContainer(std::function<Vector2(void*)> positionGetter, void* posContext,
        std::function<float(void*)> rotationGetter, void* rotContext, bool radians = true)
        :position(nullptr), rotation(nullptr),
        positionGetter(positionGetter), posContext(posContext),
        rotationGetter(rotationGetter), rotContext(rotContext),
        radians(radians)
    {
        lastPosition = positionGetter(posContext);
        lastRotation = rotationGetter(rotContext);
    }

    void AddShape(Shape& shape, Vector2 offset, Vector2 orientation, bool radians = false)
    {
        if (offset.x == 0)
            offset.x = 0.0001f;
        if (offset.y == 0)
            offset.y = 0.0001f;

        Vector2 newPos;
        if (position)
            newPos = *position + offset;
        else
            newPos = offset + positionGetter(posContext);

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

        holders.push_back(ShapeHolder{ 
            shape,
            offset,
            shape.rotation,
            orientation,
            radians });
    }

    void Update(bool force = false) {
        Vector2 pos_x;
        float rot_x;

        if (!force) {
            if (position && rotation) {

                pos_x = *position;
                rot_x = *rotation;

                if ((lastPosition == pos_x && lastRotation == rot_x))
                    return;
            }
            else
            {
                pos_x = positionGetter(posContext);
                rot_x = rotationGetter(rotContext);

                if (lastPosition == pos_x && lastRotation == rot_x)
                    return;
            }
        }
        else
            return;

        for (auto& holder : holders)
        {
            float radius = sqrt(pow(holder.offset.x, 2) + pow(holder.offset.y, 2));
            float initialAngle = atan(holder.offset.y / holder.offset.x);
            if (holder.offset.x < 0)
                initialAngle += radians ? PI : 180;

            if (holder.shape.type == Shape::CIRCLE)
            {
                holder.shape.circle.x = pos_x.x + cos(initialAngle + rot_x) * radius;
                holder.shape.circle.y = pos_x.y + sin(initialAngle + rot_x) * radius;
            } 
            else if (holder.shape.type == Shape::RECTANGLE)
            {
                holder.shape.rectangle.rec.x = pos_x.x + cos(initialAngle + rot_x) * radius;
                holder.shape.rectangle.rec.y = pos_x.y + sin(initialAngle + rot_x) * radius;
            }
            else if (holder.shape.type == Shape::TRIANGLE)
            {
                //to be implemented
            }
            else if (holder.shape.type == Shape::POLYGON)
            {
                holder.shape.poly.center.x = pos_x.x + cos(initialAngle + rot_x) * radius;
                holder.shape.poly.center.y = pos_x.y + sin(initialAngle + rot_x) * radius;
            }

            if (radians && !holder.radians)
                holder.shape.rotation = radiansToDegrees(rot_x + holder.initialRotation);
            else if (!radians && holder.radians)
                holder.shape.rotation = degreesToRadians(rot_x) + holder.initialRotation;
            else
                holder.shape.rotation = rot_x + holder.initialRotation;
        }

        lastPosition = pos_x;
        lastRotation = rot_x;
    }

    void Draw()
    {
        for (auto& holder : holders)
        {
            holder.shape.Draw();
        }
    }

    void Mirror(Vector2 orientation)
    {
        for (auto& holder : holders)
        {
            if (holder.currentOrientation != orientation)
            {
                printf("%s\n", "change");

                holder.offset = holder.offset * Vector2{ float(sgn<float>(orientation.x)), float(sgn<float>(orientation.y)) };

                if (holder.shape.type == Shape::RECTANGLE)
                {
                    holder.shape.rectangle.rec.x *= float(sgn<float>(orientation.x));
                    holder.shape.rectangle.rec.y *= float(sgn<float>(orientation.y));
                }

                holder.currentOrientation = orientation;
            }
        }
    }
};