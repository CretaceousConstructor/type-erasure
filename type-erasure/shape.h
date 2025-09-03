// Type Erasure Sample Code.
//
// Implementation of Klaus Iglberger's C++ Type Erasure Design Pattern.
//
// References:
// - Breaking Dependencies: Type Erasure - A Design Analysis,
//   by Klaus Iglberger, CppCon 2021.
//   - Video: https://www.youtube.com/watch?v=4eeESJQk-mw
//   - Slides: https://meetingcpp.com/mcpp/slides/2021/Type%20Erasure%20-%20A%20Design%20Analysis9268.pdf

#pragma once

#include <concepts>
#include <iostream>
#include <memory>

// High Level Summary of the Design
// - `class Shape` and global functions (`serialize()`, `draw()`, etc.)
//   - The external client facing interface.
//   - Holds a pointer to `ShapeConcept` internally.
// - `class ShapeConcept`
//   - The internal interface of the Bridge Design Pattern.
//   - It is needed to hide the template parameter of `ShapeModel<T>`.
// - `class ShapeModel<T>`
//   - The templated implementation of `ShapeConcept`.
//   - Routes virtual functions to global functions.

template <typename T>
void serialize(const T&);

template <typename T>
void draw(const T&);

#ifdef __clang__
// CAUTION: Workaround for clang.
// The following forward declarations of explicit specialization of
// `serialize()` and `draw()` prevent Clang from complaining about redefintion
// errors when seeing the definitions later.
class Shape;

template <>
void serialize(const Shape& shape);

template <>
void draw(const Shape& shape);

#endif  // __clang__

template <typename T>
concept IsShape = requires(T t)
{
	serialize(t);
	draw(t);
	{
		std::declval<std::ostream&>() << t
	} -> std::same_as<std::ostream&>;
};

class Shape
{
// NOTE: Definition of the explicit specialization has to appear separately
// later outside of class `Shape`, otherwise it results in error such as:
//
// ```
// error: defining explicit specialization 'serialize<Shape>' in friend declaration
// ```
//
// Reference: https://en.cppreference.com/w/cpp/language/friend
	friend void serialize<>(const Shape& shape);
	friend void draw<>(const Shape& shape);

	friend std::ostream& operator<<(std::ostream& os, const Shape& shape)
	{
		return os << *shape.pimpl_;
	}

	// The External Polymorphism Design Pattern
	class ShapeConcept
	{
	public:
		virtual ~ShapeConcept()
		{
		}
		virtual void serialize() const = 0;
		virtual void draw() const = 0;
		virtual void print(std::ostream& os) const = 0;

		// The Prototype Design Pattern
		virtual std::unique_ptr<ShapeConcept> clone() const = 0;

		friend std::ostream& operator<<(
			std::ostream& os, const ShapeConcept& shape)
		{
			shape.print(os);
			return os;
		}
	};

	template <typename T>
	class ShapeModel : public ShapeConcept
	{
		T object_;

	public:
		ShapeModel(const T& value)
			: object_{ value }
		{
		}

		void serialize() const override
		{
// CAUTION: The using declaration tells the compiler to look up the free
// serialize() function rather than the member function.
//
// Reference: https://stackoverflow.com/a/32091297/4475887
			using ::serialize;

			serialize(object_);
		}

		void draw() const override
		{
			using ::draw;

			draw(object_);
		}

		void print(std::ostream& os) const override
		{
			os << object_;
		}

		// The Prototype Design Pattern
		std::unique_ptr<ShapeConcept> clone() const override
		{
			return std::make_unique<ShapeModel>(*this);
		}
	};



	template < typename ShapeT
		, typename DrawStrategy>
	class ExtendedModel : public ShapeConcept
	{
		ShapeT shape_;
		DrawStrategy drawer_;
	public:
		explicit ExtendedModel(
			ShapeT shape,
			DrawStrategy drawer
		) :
			shape_(std::move(shape)),
			drawer_(std::move(drawer))
		{
		}

		void serialize() const override
		{
			using ::serialize;
			serialize(shape_);
		}

		void draw() const override
		{
			drawer_(shape_);
		}

		void print(std::ostream& os) const override
		{
			os << shape_;
		}

		// The Prototype Design Pattern
		std::unique_ptr<ShapeConcept> clone() const override
		{
			return std::make_unique<ExtendedModel>(*this);
		}

	};

	// The Bridge Design Pattern
	std::unique_ptr<ShapeConcept> pimpl_;

public:
 // A constructor template to create a bridge.
	template <IsShape T>
	Shape(const T& x)
		//: pimpl_{ new ShapeModel<T>(x) }
		: pimpl_{ std::make_unique<ShapeModel<T>>(x) }
	{
	}

	template <typename ShapeT, typename DrawStrategy>
	Shape(ShapeT shape, DrawStrategy drawer)    //This is the point of dependency injection.
		: pimpl_{ std::make_unique<ExtendedModel<ShapeT,DrawStrategy>>(std::move(shape),std::move(drawer)) }
	{
	}

	Shape(const Shape& s)
		: pimpl_{ s.pimpl_->clone() }
	{
	}

	Shape(Shape&& s)
		: pimpl_{ std::move(s.pimpl_) }
	{
	}

	Shape& operator=(const Shape& s)
	{
		pimpl_ = s.pimpl_->clone();
		return *this;
	}

	Shape& operator=(Shape&& s)
	{
		pimpl_ = std::move(s.pimpl_);
		return *this;
	}
};

template <>
void draw(const Shape& shape)
{
	shape.pimpl_->draw();
}

template <>
void serialize(const Shape& shape)
{
	shape.pimpl_->serialize();
}



