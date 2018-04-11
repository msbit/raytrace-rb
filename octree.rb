require File.expand_path('./point.rb', __dir__)

class Octree
  attr_accessor :boundary
  attr_accessor :capacity, :subdivided, :blocks
  attr_accessor :ltb
  attr_accessor :lbb
  attr_accessor :ltf
  attr_accessor :lbf
  attr_accessor :rtb
  attr_accessor :rbb
  attr_accessor :rtf
  attr_accessor :rbf

  def initialize(boundary, capacity = 1)
    @boundary = boundary
    @capacity = capacity

    @subdivided = false
    @blocks = {}
  end

  def insert(block)
    return unless @boundary.intersects(block)
    if subdivided
      ltb.insert(block)
      lbb.insert(block)
      ltf.insert(block)
      lbf.insert(block)
      rtb.insert(block)
      rbb.insert(block)
      rtf.insert(block)
      rbf.insert(block)
    else
      blocks[block.id] = block
      subdivide if blocks.size >= capacity
    end
  end

  def subdivide
    width = (@boundary.extent.x - @boundary.origin.x) / 2
    height = (@boundary.extent.y - @boundary.origin.y) / 2
    depth = (@boundary.extent.z - @boundary.origin.z) / 2

    ltb_extent = Point.new(@boundary.origin.x + width,
                           @boundary.origin.y + height,
                           @boundary.origin.z + depth)
    ltb_aabb = AxisAlignedBoundingBlock.new(@boundary.origin, ltb_extent)
    @ltb = Octree.new(ltb_aabb, capacity)

    lbb_origin = Point.new(@boundary.origin.x,
                           @boundary.origin.y + height,
                           @boundary.origin.z)
    lbb_extent = Point.new(@boundary.origin.x + width,
                           @boundary.origin.y + height,
                           @boundary.origin.z + depth)
    lbb_aabb = AxisAlignedBoundingBlock.new(lbb_origin, lbb_extent)
    @lbb = Octree.new(lbb_aabb, capacity)

    ltf_origin = Point.new(@boundary.origin.x,
                           @boundary.origin.y,
                           @boundary.origin.z + depth)
    ltf_extent = Point.new(@boundary.origin.x + width,
                           @boundary.origin.y + height,
                           @boundary.origin.z + depth)
    ltf_aabb = AxisAlignedBoundingBlock.new(ltf_origin, ltf_extent)
    @ltf = Octree.new(ltf_aabb, capacity)

    lbf_origin = Point.new(@boundary.origin.x,
                           @boundary.origin.y + height,
                           @boundary.origin.z + depth)
    lbf_extent = Point.new(@boundary.origin.x + width,
                           @boundary.origin.y + height,
                           @boundary.origin.z + depth)
    lbf_aabb = AxisAlignedBoundingBlock.new(lbf_origin, lbf_extent)
    @lbf = Octree.new(lbf_aabb, capacity)

    rtb_origin = Point.new(@boundary.origin.x + width,
                           @boundary.origin.y,
                           @boundary.origin.z)
    rtb_extent = Point.new(@boundary.origin.x + width,
                           @boundary.origin.y + height,
                           @boundary.origin.z + depth)
    rtb_aabb = AxisAlignedBoundingBlock.new(rtb_origin, rtb_extent)
    @rtb = Octree.new(rtb_aabb, capacity)

    rbb_origin = Point.new(@boundary.origin.x + width,
                           @boundary.origin.y + height,
                           @boundary.origin.z)
    rbb_extent = Point.new(@boundary.origin.x + width,
                           @boundary.origin.y + height,
                           @boundary.origin.z + depth)
    rbb_aabb = AxisAlignedBoundingBlock.new(rbb_origin, rbb_extent)
    @rbb = Octree.new(rbb_aabb, capacity)

    rtf_origin = Point.new(@boundary.origin.x + width,
                           @boundary.origin.y,
                           @boundary.origin.z + depth)
    rtf_extent = Point.new(@boundary.origin.x + width,
                           @boundary.origin.y + height,
                           @boundary.origin.z + depth)
    rtf_aabb = AxisAlignedBoundingBlock.new(rtf_origin, rtf_extent)
    @rtf = Octree.new(rtf_aabb, capacity)

    rbf_origin = Point.new(@boundary.origin.x + width,
                           @boundary.origin.y + height,
                           @boundary.origin.z + depth)
    rbf_aabb = AxisAlignedBoundingBlock.new(rbf_origin, @boundary.extent)
    @rbf = Octree.new(rbf_aabb, capacity)

    @subdivided = true
  end

  def query_range(boundary, result_blocks)
    return unless @blocks.any? && @boundary.intersects(boundary)

    @blocks.each_value do |block|
      result_blocks[block.id] = block if boundary.intersects(block)
    end

    return unless subdivided

    ltb.query_range(boundary, result_blocks)
    lbb.query_range(boundary, result_blocks)
    ltf.query_range(boundary, result_blocks)
    lbf.query_range(boundary, result_blocks)
    rtb.query_range(boundary, result_blocks)
    rbb.query_range(boundary, result_blocks)
    rtf.query_range(boundary, result_blocks)
    rbf.query_range(boundary, result_blocks)
  end

  def speak(level = 0)
    preamble = '  ' * level
    printf "%s(%.2f,%.2f,%.2f) => (%.2f,%.2f,%.2f)\n", preamble,
           @boundary.origin.x, @boundary.origin.y, @boundary.origin.z,
           @boundary.extent.x, @boundary.extent.y, @boundary.extent.z
    puts "#{preamble}#{@blocks.size}"
    puts

    return unless subdivided

    ltb.speak(level + 1)
    lbb.speak(level + 1)
    ltf.speak(level + 1)
    lbf.speak(level + 1)
    rtb.speak(level + 1)
    rbb.speak(level + 1)
    rtf.speak(level + 1)
    rbf.speak(level + 1)
  end
end
