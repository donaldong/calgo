require 'calgo'

RSpec.describe Calgo::KdTree do
  class Point
    attr_accessor :x, :y, :z

    def initialize(x, y, z)
      @x = x
      @y = y
      @z = z
    end

    def position
      [x, y, z]
    end
  end

  it 'supports basic usage' do
    expect {
      Calgo::KdTree.new
    }.to raise_error(ArgumentError)

    tree = Calgo::KdTree.new(dimension: 3)

    a = Point.new(-1, -1, -1)
    b = Point.new(1, 1, 1)
    tree.insert(a)
    tree.insert(b)
    tree.insert(Point.new(100, 100, 100))

    nearest = tree.nearest(Point.new(0, 0, 0), range: 5.0)
    expect(nearest.size).to eq(2)
    expect(nearest.include?(a)).to be(true)
    expect(nearest.include?(b)).to be(true)

    tree.clear

    expect(
      tree.nearest(Point.new(0, 0, 0), range: 5.0).size
    ).to eq(0)
    tree.insert([0, 0, 0])
    tree.insert([100, 100, 100])
    expect(tree.nearest(a)).to eq([[0, 0, 0]])
  end
end
