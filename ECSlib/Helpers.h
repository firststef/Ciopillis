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
    : id(id), objectClass(objectClass) 
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

struct ShapeContainer
{
	Rectangle& position;
	float& rotation;

	const Rectangle original_position;
	const float original_rotation;

	struct ShapeHolder
    {
        Shape shape;

        Vector2 offset;
        float initialRotation;
        Vector2 currentOrientation;
        
        bool radians;
    };

    std::vector<ShapeHolder> holders;

	ShapeContainer(Rectangle& position, float& rotation)
		:position(position), rotation(rotation), original_position(position), original_rotation(rotation)
	{

	}

	ShapeHolder& AddShape(Shape& shape, Vector2 offset, Vector2 orientation, bool radians = false)
    {
        if (offset.x == 0)
            offset.x = 0.0001f;
        if (offset.y == 0)
            offset.y = 0.0001f;

        Vector2 newPos = offset + Vector2{position.x, position.y};

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

		return holders.back();
    }

	void Update(bool force = false) {

        for (auto& holder : holders)
        {
            float radius = sqrt(pow(holder.offset.x, 2) + pow(holder.offset.y, 2));

			auto new_rot = rotation - original_rotation;

			holder.shape.rotation = holder.initialRotation + new_rot;

            if (holder.shape.type == Shape::CIRCLE)
            {
                holder.shape.circle.x = position.x + cos(new_rot) * radius;
                holder.shape.circle.y = position.y + sin(new_rot) * radius;
            } 
            else if (holder.shape.type == Shape::RECTANGLE)
            {
				holder.shape.rectangle.rec.x = position.x + cos(new_rot) * radius;
				holder.shape.rectangle.rec.y = position.y + sin(new_rot) * radius;
            }
            else if (holder.shape.type == Shape::TRIANGLE)
            {
                //to be implemented
            }
			else if (holder.shape.type == Shape::POLYGON)
			{
				holder.shape.poly.center.x = position.x + cos(new_rot) * radius;
				holder.shape.poly.center.y = position.y + sin(new_rot) * radius;
			}
        }
    }

	void Draw()
    {
        for (auto& holder : holders)
        {
            holder.shape.Draw();
        }
    }
	
};