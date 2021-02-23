require 'calgo'

RSpec.describe Calgo::AvlTree do
  Node = Struct.new(:key)

  it 'supports basic usage' do
    tree = Calgo::AvlTree.new do |node|
      node.key
    end

    nodes = 3.times.map { |i| Node.new(i) }
    nodes.each { |node| tree.insert(node.key, node) }

    expect(tree.size).to eq(3)

    tree.clear
    expect(tree.size).to eq(0)

    nodes.each { |node| tree.insert(node.key, node) }
    expect(tree.size).to eq(3)

    nodes.each do |node|
      expect(tree.include?(node.key)).to be(true)
    end
    expect(tree.include?(4)).to be(false)

    expect(tree.delete(nodes.last.key)).to be(true)
    expect(tree.delete(4)).to be(false)
    expect(tree.size).to eq(2)

    a, b, c = *nodes
    tree.clear
    tree.insert(b.key, b)
    tree.insert(a.key, a)
    tree.insert(c.key, c)
    expect(tree.to_a(:pre_order).map(&:last).flatten).to eq([b, a, c])
    expect(tree.to_a(:in_order).map(&:last).flatten).to eq([a, b, c])
    expect(tree.to_a(:post_order).map(&:last).flatten).to eq([a, c, b])

    expect(tree.to_a(:in_order, lower_bound: 2)).to eq([[2, c]])
    expect(tree.to_a(:in_order, upper_bound: 2)).to eq([[0, a], [1, b]])
    expect(tree.to_a(:in_order, lower_bound: 1, upper_bound: 2)).to eq([[1, b]])
    expect(tree.to_a(:in_order, lower_bound: 5)).to eq([])
  end

  it 'supports more simple use cases' do
    tree = Calgo::AvlTree.new
    (0...11).each { |i| tree.insert(i, Node.new(i)) }
    tree.each(:pre_order, lower_bound: 3, upper_bound: 8) do |key, node|
      #<struct Node key=3>
      #<struct Node key=7>
      #<struct Node key=5>
      #<struct Node key=4>
      #<struct Node key=6>
      expect(key).to eq(node.key)
    end
    tree.delete(3)
    expect(tree.size).to eq(10)

    a = 'a'
    tree.clear
    tree.insert(1, a);
    tree.insert(1, 'b');
    expect(tree.size).to eq(2)
    expect(tree[1]).to eq(['a', 'b'])

    tree.delete(1)
    expect(tree[1]).to eq(['b'])
  end
end
