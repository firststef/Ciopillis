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

constexpr float radiansToDegrees(float rad) { return rad / (2 * PI) * 360; }
constexpr float degreesToRadians(float deg) { return deg / 360 * (2 * PI); }

struct Shape
{
	enum ShapeType
	{
		NONE,
		CIRCLE,
		RECTANGLE,
		TRIANGLE,
		POLYGON
	}
	type = NONE;
	
	float rotation;

	union
	{
		struct { Vector2 center; float radius; } circle;
		struct { Vector2 center; float width, height; } rectangle;
		struct { Vector2 a; Vector2 b; Vector2 c; } triangle;
		struct { Vector2 center; int sides; float radius; } poly;
	};

	float GetCenterX()
	{
		switch (type)
		{
		case Shape::ShapeType::CIRCLE: return circle.center.x;
		case Shape::ShapeType::RECTANGLE: return rectangle.center.x;
		case Shape::ShapeType::TRIANGLE: return (triangle.a.x + triangle.b.x + triangle.c.x) / 3;
		case Shape::ShapeType::POLYGON: return poly.center.x;
		}
	}

	float GetCenterY()
	{
		switch (type)
		{
		case Shape::ShapeType::CIRCLE: return circle.center.y;
		case Shape::ShapeType::RECTANGLE: return rectangle.center.y;
		case Shape::ShapeType::TRIANGLE: return (triangle.a.y + triangle.b.y + triangle.c.y) / 3;
		case Shape::ShapeType::POLYGON: return poly.center.y;
		}
	}

	void SetCenterX(float newX)
	{
		//float change = newX - GetCenterX();
		switch (type)
		{
		case Shape::ShapeType::CIRCLE:
			circle.center.x = newX;
			break;
		case Shape::ShapeType::RECTANGLE:
			rectangle.center.x = newX;
			break;
		case Shape::ShapeType::TRIANGLE:
			//triangle.a.x += change;
			//triangle.b.x += change;
			//triangle.c.x += change;
			break;
		case Shape::ShapeType::POLYGON:
			poly.center.x = newX;
			break;
		}
	}

	void SetCenterY(float newY)
	{
		//float change = newY - GetCenterY(); not useful
		switch (type)
		{
		case Shape::ShapeType::CIRCLE:
			circle.center.y = newY;
			break;
		case Shape::ShapeType::RECTANGLE:
			rectangle.center.y = newY;
			break;
		case Shape::ShapeType::TRIANGLE:
			//triangle.a.y += change;
			//triangle.b.y += change;
			//triangle.c.y += change;
			break;
		case Shape::ShapeType::POLYGON:
			poly.center.y = newY;
			break;
		}
	}

	void Draw()
	{
		if (type == CIRCLE)
		{
			DrawCircle(circle.center.x, circle.center.y, circle.radius, Fade(BLUE, 0.3f));
		}
		else if (type == RECTANGLE)
		{
			DrawRectanglePro(Rectangle{ rectangle.center.x, rectangle.center.y, rectangle.width,rectangle.height }, Vector2{ 0,0 }, rotation, Fade(RED, 0.3f));
		}
		else if (type == TRIANGLE)
		{
			//DrawTriangle(shape.triangle.a, shape.triangle.b, shape.triangle.c, shape.color);
		}
		else if (type == POLYGON)
		{
			DrawPoly(poly.center, poly.sides, poly.radius, rotation, RED);
		}
	}
};

struct AttachedShape : Shape
{
	Vector2 mainBodyCenter;

	void SetMainBodyCenter(Vector2 mainBodyCenter)
	{
		this->mainBodyCenter.x = mainBodyCenter.x;
		this->mainBodyCenter.y = mainBodyCenter.y;
	}

	void SetMainBodyCenter(Shape mainBody)
	{
		mainBodyCenter.x = mainBody.GetCenterX();
		mainBodyCenter.y = mainBody.GetCenterY();
	}

	float GetDistanceFromMainX()
	{
		return abs(GetCenterX() - mainBodyCenter.x);
	}

	float GetDistanceFromMainY()
	{
		return abs(GetCenterY() - mainBodyCenter.y);
	}
};

struct ShapeContainer
{
	float last_x;
	float last_y;
	float origin_orientation;

	Shape origin_position;

	std::vector<AttachedShape> shapes;

	ShapeContainer(Shape mainBody, float mainBodyOrientation) : origin_position(mainBody), origin_orientation(mainBodyOrientation)
	{

	}

	void AddShape(AttachedShape s)
	{
		shapes.push_back(s);
		last_x = s.GetCenterX();
		last_y = s.GetCenterY();
	}

	void Update()
	{
		for (std::vector<AttachedShape>::iterator it = shapes.begin(); it != shapes.end(); ++it) {

			Vector2 newCenter;

			newCenter.x = origin_position.GetCenterX() - it->mainBodyCenter.x;
			newCenter.y = origin_position.GetCenterY() - it->mainBodyCenter.y;

			it->mainBodyCenter.x = origin_position.GetCenterX();
			it->mainBodyCenter.y = origin_position.GetCenterY();

			it->SetCenterX(it->GetCenterX() + newCenter.x);
			it->SetCenterY(it->GetCenterY() + newCenter.y);

			if (origin_position.rotation != 0.00f && origin_position.rotation != it->rotation)
			{
				float NX = it->mainBodyCenter.x + (it->GetCenterX() - it->mainBodyCenter.x) * cos(origin_position.rotation) - (it->GetCenterY() - it->mainBodyCenter.y) * sin(origin_position.rotation);
				float NY = it->mainBodyCenter.y + (it->GetCenterX() - it->mainBodyCenter.x) * sin(origin_position.rotation) + (it->GetCenterY() - it->mainBodyCenter.y) * cos(origin_position.rotation);

				it->SetCenterX(NX);
				it->SetCenterY(NY);

				it->rotation = origin_position.rotation;
			}
		}
	}

	void Mirror(Vector2 orientation)
	{
		// In this method the entire structure is mirrored 
	}

	void Draw()
    {
		origin_position.Draw();
        for (auto& shape : shapes)
        {
            shape.Draw();
        }
    }
	
};