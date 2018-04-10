class Octree
  attr_accessor :axis_aligned_bounding_block
  attr_accessor :capacity, :subdivided, :blocks
  attr_accessor :left_top_back
  attr_accessor :left_bottom_back
  attr_accessor :left_top_forward
  attr_accessor :left_bottom_forward
  attr_accessor :right_top_back
  attr_accessor :right_bottom_back
  attr_accessor :right_top_forward
  attr_accessor :right_bottom_forward

  def initialize(axis_aligned_bounding_block, capacity = 1)
    @axis_aligned_bounding_block = axis_aligned_bounding_block
    @capacity = capacity

    @subdivided = false
    @blocks = {}
  end

  def insert(block)
    return unless @axis_aligned_bounding_block.intersects(block)
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
    width = @axis_aligned_bounding_block.width / 2
    height = @axis_aligned_bounding_block.height / 2
    depth = @axis_aligned_bounding_block.depth / 2
    x = @axis_aligned_bounding_block.origin_x
    y = @axis_aligned_bounding_block.origin_y
    z = @axis_aligned_bounding_block.origin_z

    left_top_back_aabb = AxisAlignedBoundingBlock.new(x, y, z, width, height, depth)
    @left_top_back = Octree.new(left_top_back_aabb, capacity)

    left_bottom_back_aabb = AxisAlignedBoundingBlock.new(x, y + height, z, width, height, depth)
    @left_bottom_back = Octree.new(left_bottom_back_aabb, capacity)

    left_top_forward_aabb = AxisAlignedBoundingBlock.new(x, y, z + depth, width, height, depth)
    @left_top_forward = Octree.new(left_top_forward_aabb, capacity)

    left_bottom_forward_aabb = AxisAlignedBoundingBlock.new(x, y + height, z + depth, width, height, depth)
    @left_bottom_forward = Octree.new(left_bottom_forward_aabb, capacity)

    right_top_back_aabb = AxisAlignedBoundingBlock.new(x + width, y, z, width, height, depth)
    @right_top_back = Octree.new(right_top_back_aabb, capacity)

    right_bottom_back_aabb = AxisAlignedBoundingBlock.new(x + width, y + height, z, width, height, depth)
    @right_bottom_back = Octree.new(right_bottom_back_aabb, capacity)

    right_top_forward_aabb = AxisAlignedBoundingBlock.new(x + width, y, z + depth, width, height, depth)
    @right_top_forward = Octree.new(right_top_forward_aabb, capacity)

    right_bottom_forward_aabb = AxisAlignedBoundingBlock.new(x + width,  y + height, z + depth, width, height, depth)
    @right_bottom_forward = Octree.new(right_bottom_forward_aabb, capacity)

    @subdivided = true
  end

  def query_range(axis_aligned_bounding_block, result_blocks)
    return unless @axis_aligned_bounding_block.intersects(axis_aligned_bounding_block)

    blocks.each do |_, block|
      result_blocks[block.id] = block if axis_aligned_bounding_block.intersects(block)
    end

    if subdivided
      left_top_back.query_range(axis_aligned_bounding_block, result_blocks)
      left_bottom_back.query_range(axis_aligned_bounding_block, result_blocks)
      left_top_forward.query_range(axis_aligned_bounding_block, result_blocks)
      left_bottom_forward.query_range(axis_aligned_bounding_block, result_blocks)
      right_top_back.query_range(axis_aligned_bounding_block, result_blocks)
      right_bottom_back.query_range(axis_aligned_bounding_block, result_blocks)
      right_top_forward.query_range(axis_aligned_bounding_block, result_blocks)
      right_bottom_forward.query_range(axis_aligned_bounding_block, result_blocks)
    end
  end

  def speak(level = 0)
    preamble = "  " * level
    printf "%s(%.2f,%.2f,%.2f) => (%.2f,%.2f,%.2f)\n", preamble,
           @axis_aligned_bounding_block.origin_x, @axis_aligned_bounding_block.origin_y, @axis_aligned_bounding_block.origin_z,
           @axis_aligned_bounding_block.origin_x + @axis_aligned_bounding_block.width, @axis_aligned_bounding_block.origin_y + @axis_aligned_bounding_block.height, @axis_aligned_bounding_block.origin_z + @axis_aligned_bounding_block.depth
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
