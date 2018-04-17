class Point
  attr_accessor :x, :y, :z

  def initialize(x, y, z)
    @x = x
    @y = y
    @z = z
  end

  def normalise!
    length = Math.sqrt(@x * @x + @y * @y + @z * @z)
    @x /= length
    @y /= length
    @z /= length
    self
  end

  def dot_product(other)
    @x * other.x + @y * other.y + @z * other.z
  end

  def cross_product(other)
    Point.new(@y * other.z - @z * other.y,
              @z * other.x - @x * other.z,
              @x * other.y - @y * other.x)
  end

  def minus(other)
    Point.new(@x - other.x, @y - other.y, @z - other.z)
  end
end
