#include "Common.h"

using namespace std;

// Этот файл сдаётся на проверку
// Здесь напишите реализацию необходимых классов-потомков `IShape`
template <class T>
Size GetSizeOf(const T& item) {
  return item.GetSize();
}

Size GetSizeOf(const Image& item) {
  Size s;
  s.width = item.empty() ? 0 : item.front().size();
  s.height = item.size();
  return s;
}

template <class T>
bool IsPointIn(Point p, const T& item) {
  return IsPointIn(p, GetSizeOf(item));
}

bool IsPointIn(Point p, const Size& item) {
  return (p.x < item.width) && (p.x >= 0) && (p.y < item.height) && (p.y >= 0);
}

class BasicShape : public IShape {
 public:
  void SetPosition(Point p) override { point = p; }
  Point GetPosition() const override { return point; }

  void SetSize(Size s) override { size = s; }
  Size GetSize() const override { return size; }

  void SetTexture(std::shared_ptr<ITexture> t) override { texture = move(t); }
  ITexture* GetTexture() const override { return texture.get(); }

  void Draw(Image& image) const override {
    const char pixel = '.';
    for (auto y = 0; y < size.height; ++y)
      for (auto x = 0; x < size.width; ++x)
        if (IsPointInShape({x, y})) {
          if (Point img_point = {point.x + x, point.y + y};
              IsPointIn(img_point, image))
            image[img_point.y][img_point.x] =
                (texture && IsPointIn({x, y}, *texture))
                    ? texture->GetImage()[y][x]
                    : pixel;
        }
  }

 private:
  Point point = {0, 0};
  Size size = {0, 0};
  shared_ptr<ITexture> texture;
  virtual bool IsPointInShape(Point p) const = 0;
};

class Rectangle : public BasicShape {
 public:
  unique_ptr<IShape> Clone() const override {
    return make_unique<Rectangle>(*this);
  }

 private:
  bool IsPointInShape(Point p) const override { return true; }
};

class Ellipse : public BasicShape {
 public:
  unique_ptr<IShape> Clone() const override {
    return make_unique<Ellipse>(*this);
  }

 private:
  bool IsPointInShape(Point p) const override {
    return IsPointInEllipse(p, GetSize());
  }
};

// Напишите реализацию функции
unique_ptr<IShape> MakeShape(ShapeType shape_type) {
  switch (shape_type) {
    case ShapeType::Rectangle:
      return make_unique<Rectangle>();
    case ShapeType::Ellipse:
      return make_unique<Ellipse>();
    default:
    return nullptr;
  }
}