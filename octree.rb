require File.expand_path("./point.rb", __dir__)

class Octree
  attr_accessor :boundary
  attr_accessor :capacity, :subdivided, :blocks
  attr_accessor :left_top_back
  attr_accessor :left_bottom_back
  attr_accessor :left_top_forward
  attr_accessor :left_bottom_forward
  attr_accessor :right_top_back
  attr_accessor :right_bottom_back
  attr_accessor :right_top_forward
  attr_accessor :right_bottom_forward

  def initialize(boundary, capacity = 1)
    @boundary = boundary
    @capacity = capacity

    @subdivided = false
    @blocks = {}
  end

  def insert(block)
    return unless @boundary.intersects(block)
    if subdivided
      left_top_back.insert(block)
      left_bottom_back.insert(block)
      left_top_forward.insert(block)
      left_bottom_forward.insert(block)
      right_top_back.insert(block)
      right_bottom_back.insert(block)
      right_top_forward.insert(block)
      right_bottom_forward.insert(block)
    else
      blocks[block.id] = block
      if blocks.size >= capacity
        subdivide
      end
    end
  end

  def subdivide()
    width = (@boundary.extent.x - @boundary.origin.x) / 2
    height = (@boundary.extent.y - @boundary.origin.y) / 2
    depth = (@boundary.extent.z - @boundary.origin.z) / 2

    left_top_back_aabb = AxisAlignedBoundingBlock.new(@boundary.origin, Point.new(@boundary.origin.x + width, @boundary.origin.y + height, @boundary.origin.z + depth))
    @left_top_back = Octree.new(left_top_back_aabb, capacity)

    left_bottom_back_aabb = AxisAlignedBoundingBlock.new(Point.new(@boundary.origin.x, @boundary.origin.y + height, @boundary.origin.z), Point.new(@boundary.origin.x + width, @boundary.origin.y + height, @boundary.origin.z + depth))
    @left_bottom_back = Octree.new(left_bottom_back_aabb, capacity)

    left_top_forward_aabb = AxisAlignedBoundingBlock.new(Point.new(@boundary.origin.x, @boundary.origin.y, @boundary.origin.z + depth), Point.new(@boundary.origin.x + width, @boundary.origin.y + height, @boundary.origin.z + depth))
    @left_top_forward = Octree.new(left_top_forward_aabb, capacity)

    left_bottom_forward_aabb = AxisAlignedBoundingBlock.new(Point.new(@boundary.origin.x, @boundary.origin.y + height, @boundary.origin.z + depth), Point.new(@boundary.origin.x + width, @boundary.origin.y + height, @boundary.origin.z + depth))
    @left_bottom_forward = Octree.new(left_bottom_forward_aabb, capacity)

    right_top_back_aabb = AxisAlignedBoundingBlock.new(Point.new(@boundary.origin.x + width, @boundary.origin.y, @boundary.origin.z), Point.new(@boundary.origin.x + width, @boundary.origin.y + height, @boundary.origin.z + depth))
    @right_top_back = Octree.new(right_top_back_aabb, capacity)

    right_bottom_back_aabb = AxisAlignedBoundingBlock.new(Point.new(@boundary.origin.x + width, @boundary.origin.y + height, @boundary.origin.z), Point.new(@boundary.origin.x + width, @boundary.origin.y + height, @boundary.origin.z + depth))
    @right_bottom_back = Octree.new(right_bottom_back_aabb, capacity)

    right_top_forward_aabb = AxisAlignedBoundingBlock.new(Point.new(@boundary.origin.x + width, @boundary.origin.y, @boundary.origin.z + depth), Point.new(@boundary.origin.x + width, @boundary.origin.y + height, @boundary.origin.z + depth))
    @right_top_forward = Octree.new(right_top_forward_aabb, capacity)

    right_bottom_forward_aabb = AxisAlignedBoundingBlock.new(Point.new(@boundary.origin.x + width,  @boundary.origin.y + height, @boundary.origin.z + depth), @boundary.extent)
    @right_bottom_forward = Octree.new(right_bottom_forward_aabb, capacity)

    @subdivided = true
  end

  def query_range(boundary, result_blocks)
    return unless @blocks.any? && @boundary.intersects(boundary)

    @blocks.each do |_, block|
      result_blocks[block.id] = block if boundary.intersects(block)
    end

    if subdivided
      left_top_back.query_range(boundary, result_blocks)
      left_bottom_back.query_range(boundary, result_blocks)
      left_top_forward.query_range(boundary, result_blocks)
      left_bottom_forward.query_range(boundary, result_blocks)
      right_top_back.query_range(boundary, result_blocks)
      right_bottom_back.query_range(boundary, result_blocks)
      right_top_forward.query_range(boundary, result_blocks)
      right_bottom_forward.query_range(boundary, result_blocks)
    end
  end

  def speak(level = 0)
    preamble = "  " * level
    printf "%s(%.2f,%.2f,%.2f) => (%.2f,%.2f,%.2f)\n", preamble,
           @boundary.origin_x, @boundary.origin_y, @boundary.origin_z,
           @boundary.origin_x + @boundary.width, @boundary.origin_y + @boundary.height, @boundary.origin_z + @boundary.depth
    puts "#{preamble}#{@blocks.size}"
    puts
    if subdivided
      left_top_back.speak(level + 1)
      left_bottom_back.speak(level + 1)
      left_top_forward.speak(level + 1)
      left_bottom_forward.speak(level + 1)
      right_top_back.speak(level + 1)
      right_bottom_back.speak(level + 1)
      right_top_forward.speak(level + 1)
      right_bottom_forward.speak(level + 1)
    end
  end
end
