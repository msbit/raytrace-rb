class AxisAlignedBoundingBlock
  attr_accessor :origin, :extent

  def initialize(origin, extent)
    @origin = origin
    @extent = extent
  end

  def contains(test_x, test_y, test_z)
    return false if test_x < @origin.x
    return false if test_y < @origin.y
    return false if test_z < @origin.z
    return false if test_x > @extent.x
    return false if test_y > @extent.y
    return false if test_z > @extent.z
    true
  end

  def intersects(other)
    return false if other.extent.x < @origin.x
    return false if other.extent.y < @origin.y
    return false if other.extent.z < @origin.z
    return false if other.origin.x > @extent.x
    return false if other.origin.y > @extent.y
    return false if other.origin.z > @extent.z
    true
  end
end
