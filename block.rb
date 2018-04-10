class Block
  attr_accessor :x, :y, :z
  attr_accessor :width, :height, :depth
  attr_accessor :r, :g, :b

  def contains(test_x, test_y, test_z)
    if test_x < x 
      return false
    end 
    if test_y < y 
      return false
    end 
    if test_z < z 
      return false
    end 
    if test_x > (x + width)
      return false
    end 
    if test_y > (y + height)
      return false
    end 
    if test_z > (z + depth)
      return false
    end 
    return true
  end 
end
