Calgo
=====
[![Gem Version](https://badge.fury.io/rb/calgo.svg)](https://badge.fury.io/rb/calgo)
[![Build Status](https://travis-ci.com/donaldong/calgo.svg?branch=main)](https://travis-ci.com/donaldong/calgo)

Common algorithms and data structures as Ruby C extensions

- [x] 💎 Elegant Ruby API
- [x] ⚡ Performant and reliable: A collection of quality C implementations 
- [x] 🥰 Actively maintained by Donald Dong

## Trees
### Balanced Binary Tree
- [AVL Tree](https://github.com/fragglet/c-algorithms/blob/master/src/avl-tree.h)
- [Red-black Tree](https://github.com/fragglet/c-algorithms/blob/master/src/rb-tree.h)
```ruby
Node = Struct.new(:key)
tree = Calgo::AvlTree.new
(0...11).each { |i| tree.insert(i, Node.new(i)) }
tree.each(:pre_order, lower_bound: 3, upper_bound: 8) do |key, node|
  #<struct Node key=3>
  #<struct Node key=7>
  #<struct Node key=5>
  #<struct Node key=4>
  #<struct Node key=6>
end
tree.delete(3)
tree[3] # []
tree[4] # [#<struct Node key=4>]
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
