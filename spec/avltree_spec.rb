require 'calgo'

RSpec.describe Calgo::AvlTree do
  Node = Struct.new(:key)

  it 'supports basic usage' do
    tree = Calgo::AvlTree.new do |node|
      node.key
    end

    nodes = 3.times.map { |i| Node.new(i) }
    nodes.each { |node| tree.insert(node) }

    expect(tree.size).to eq(3)

    tree.clear
    expect(tree.size).to eq(0)

    nodes.each { |node| tree.insert(node) }
    expect(tree.size).to eq(3)

    nodes.each do |node|
      expect(tree.include?(node)).to be(true)
    end
    expect(tree.include?(Node.new(4))).to be(false)

    expect(tree.delete(nodes.last)).to be(true)
    expect(tree.delete(Node.new(4))).to be(false)
    expect(tree.size).to eq(2)

    a, b, c = *nodes
    tree.clear
    tree.insert(b)
    tree.insert(a)
    tree.insert(c)
    expect(tree.to_a(order: :pre)).to eq([b, a, c])
    expect(tree.to_a(order: :in)).to eq([a, b, c])
    expect(tree.to_a(order: :post)).to eq([a, c, b])

    expect(tree.to_a(order: :in, lower_bound: 2)).to eq([c])
    expect(tree.to_a(order: :in, upper_bound: 2)).to eq([a, b])
    expect(tree.to_a(order: :in, lower_bound: 1, upper_bound: 2)).to eq([b])
    expect(tree.to_a(order: :in, lower_bound: 5)).to eq([])
  end

  it 'can use key as nodes' do
    tree = Calgo::AvlTree.new
    tree.insert(2)
    tree.insert(1)
    tree.insert(3)
    expect(tree.to_a(order: :pre)).to eq([2, 1, 3])
  end

  it 'supports more simple use cases' do
    tree = Calgo::AvlTree.new { |node| node.key }
    (0...11).each { |i| tree.insert(Node.new(i)) }
    tree.each(order: :pre, lower_bound: 3, upper_bound: 8) do |node|
      #<struct Node key=3>
      #<struct Node key=7>
      #<struct Node key=5>
      #<struct Node key=4>
      #<struct Node key=6>
      node
    end
    tree.delete(Node.new(3))
    tree.size # 10
  end
end
