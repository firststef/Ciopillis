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
		struct { Vector2 startPosition; float width, height; } rectangle;
		struct { Vector2 a; Vector2 b; Vector2 c; } triangle;
		struct { Vector2 center; int sides; float radius; } poly;
	};

	float GetCenterX()
	{
		switch (type)
		{
		case Shape::ShapeType::CIRCLE: return circle.center.x;
		case Shape::ShapeType::RECTANGLE: return rectangle.startPosition.x + rectangle.width / 2;
		case Shape::ShapeType::TRIANGLE: return (triangle.a.x + triangle.b.x + triangle.c.x) / 3;
		case Shape::ShapeType::POLYGON: return poly.center.x;
		}
	}

	float GetCenterY()
	{
		switch (type)
		{
		case Shape::ShapeType::CIRCLE: return circle.center.y;
		case Shape::ShapeType::RECTANGLE: return rectangle.startPosition.y + rectangle.height / 2;
		case Shape::ShapeType::TRIANGLE: return (triangle.a.y + triangle.b.y + triangle.c.y) / 3;
		case Shape::ShapeType::POLYGON: return poly.center.y;
		}
	}

	void SetCenterX(float newX)
	{
		float change = newX - GetCenterX();
		switch (type)
		{
		case Shape::ShapeType::CIRCLE:
			circle.center.x = newX;
			break;
		case Shape::ShapeType::RECTANGLE:
			rectangle.startPosition.x = newX - rectangle.width / 2;
			break;
		case Shape::ShapeType::TRIANGLE:
			triangle.a.x += change;
			triangle.b.x += change;
			triangle.c.x += change;

			break;
		case Shape::ShapeType::POLYGON:
			poly.center.x = newX;
			break;
		}
	}

	void SetCenterY(float newY)
	{
		float change = newY - GetCenterY();
		switch (type)
		{
		case Shape::ShapeType::CIRCLE:
			circle.center.y = newY;
			break;
		case Shape::ShapeType::RECTANGLE:
			rectangle.startPosition.y = newY - rectangle.height / 2;
			break;
		case Shape::ShapeType::TRIANGLE:
			triangle.a.y += change;
			triangle.b.y += change;
			triangle.c.y += change;
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
			DrawRectanglePro(Rectangle{ rectangle.startPosition.x, rectangle.startPosition.y, rectangle.width, rectangle.height }, Vector2{ 0, 0 }, rotation * (180.0 / 3.141592653589793238463), Fade(RED, 0.3f));
			DrawCircle(rectangle.startPosition.x, rectangle.startPosition.y, 10, BLACK);
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
	float angleFromMainBody; // in radians

	void SetMainBodyCenter(Vector2 mainBodyCenter)
	{
		this->mainBodyCenter.x = mainBodyCenter.x;
		this->mainBodyCenter.y = mainBodyCenter.y;

		UpdateAngleFromMainBody();
	}

	void SetMainBodyCenter(Shape mainBody)
	{
		mainBodyCenter.x = mainBody.GetCenterX();
		mainBodyCenter.y = mainBody.GetCenterY();

		UpdateAngleFromMainBody();
	}

	void SetMainBodyCenterWithoutUpdate(Shape mainBody)
	{
		mainBodyCenter.x = mainBody.GetCenterX();
		mainBodyCenter.y = mainBody.GetCenterY();
	}

	void UpdateAngleFromMainBody()
	{
		float delta_x = GetCenterX() - mainBodyCenter.x;
		float delta_y = GetCenterY() - mainBodyCenter.y;
		angleFromMainBody = atan2(delta_y, delta_x);
	}

	float GetDistanceFromMainX()
	{
		return abs(GetCenterX() - mainBodyCenter.x);
	}

	float GetDistanceFromMainY()
	{
		return abs(GetCenterY() - mainBodyCenter.y);
	}

	float GetDistanceFromMain()
	{
		return sqrtf(powf(GetDistanceFromMainX(), 2) + powf(GetDistanceFromMainY(), 2));
	}
};

struct ShapeContainer
{
	std::string name;
	
	Shape origin_position;
	Vector2 origin_orientation;

	std::vector<AttachedShape> shapes;

	ShapeContainer(std::string name, Shape mainBody, Vector2 orientation)
	: name(std::move(name)), origin_position(mainBody),origin_orientation(orientation)
	{

	}

	void AddShape(AttachedShape s)
	{
		shapes.push_back(s);
	}

	void Update()
	{
		for (auto& shape : shapes)
		{
			Vector2 newCenter;

			newCenter.x = origin_position.GetCenterX() - shape.mainBodyCenter.x;
			newCenter.y = origin_position.GetCenterY() - shape.mainBodyCenter.y;

			shape.SetMainBodyCenterWithoutUpdate(origin_position);

			shape.SetCenterX(shape.GetCenterX() + newCenter.x);
			shape.SetCenterY(shape.GetCenterY() + newCenter.y);

			if (origin_position.rotation != 0.00f && origin_position.rotation != shape.rotation)
			{

				float NX = shape.mainBodyCenter.x + shape.GetDistanceFromMain() * cosf(shape.angleFromMainBody + origin_position.rotation);
				float NY = shape.mainBodyCenter.y + shape.GetDistanceFromMain() * sinf(shape.angleFromMainBody + origin_position.rotation);

				shape.SetCenterX(NX);
				shape.SetCenterY(NY);

				shape.rotation = origin_position.rotation;
			}
		}
	}

	//Mirror by the x or y axis
	void Mirror(Vector2 orientation)
	{
		if (bool(orientation.x) != bool(origin_orientation.x))
		{
			for (auto& shape : shapes)
			{
				float NX = shape.mainBodyCenter.x + (bool(orientation.x) ? -1 : 1) * shape.GetDistanceFromMain() * cosf(shape.angleFromMainBody + origin_position.rotation);

				shape.SetCenterX(NX);
			}

			origin_orientation.x = bool(orientation.x);
		}
		
		if (bool(orientation.y) != bool(origin_orientation.y))
		{
			for (auto& shape : shapes)
			{
				float NY = shape.mainBodyCenter.y + (bool(orientation.y) ? -1 : 1) * shape.GetDistanceFromMain() * sinf(shape.angleFromMainBody + origin_position.rotation);

				shape.SetCenterY(NY);
			}

			origin_orientation.y = bool(orientation.y);
		}
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