Calgo
=====
[![Gem Version](https://badge.fury.io/rb/calgo.svg)](https://badge.fury.io/rb/calgo)
[![Build Status](https://travis-ci.com/donaldong/calgo.svg?branch=main)](https://travis-ci.com/donaldong/calgo)

Common algorithms and data structures as Ruby C extensions

- [x] 💎 Elegant Ruby API
- [x] ⚡ Performant and reliable: A collection of quality C implementations 
- [x] 🥰 Actively maintained by Donald Dong

## Trees
### [AVL Tree](https://github.com/fragglet/c-algorithms/blob/master/src/avl-tree.h)
```ruby
tree = Calgo::AvlTree.new { |node| node.key }
Node = Struct.new(:key)
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
```
### [K-d Tree](https://github.com/jtsiomb/kdtree)
```ruby
tree = Calgo::KdTree.new(dimension: 3)
a = Point.new(-5, -5, -5) # Point#position => [-5, -5, -5]
b = Point.new(1, 1, 1) # Point#position => [1, 1, 1]
tree.insert(a)
tree.insert(b)

tree.nearest(Point.new(0, 0, 0)) # [b]
tree.nearest([0, 0, 0], range: 100) # [a, b]
```
