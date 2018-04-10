class AxisAlignedBoundingBlock
  attr_accessor :origin_x, :origin_y, :origin_z
  attr_accessor :width, :height, :depth

  def initialize(origin_x, origin_y, origin_z, width, height, depth)
    @origin_x = origin_x
    @origin_y = origin_y
    @origin_z = origin_z
    @width = width
    @height = height
    @depth = depth
  end

  def contains(test_x, test_y, test_z)
    return false if test_x < @origin_x
    return false if test_y < @origin_y
    return false if test_z < @origin_z
    return false if test_x > (@origin_x + @width)
    return false if test_y > (@origin_y + @height)
    return false if test_z > (@origin_z + @depth)
    true
  end

  def intersects(other)
    return false if (other.origin_x + other.width) < @origin_x
    return false if (other.origin_y + other.height) < @origin_y
    return false if (other.origin_z + other.depth) < @origin_z
    return false if other.origin_x > (@origin_x + @width)
    return false if other.origin_y > (@origin_y + @height)
    return false if other.origin_z > (@origin_z + @depth)
    true
  end
end
