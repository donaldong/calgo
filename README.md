Calgo
=====
[![Gem Version](https://badge.fury.io/rb/calgo.svg)](https://badge.fury.io/rb/calgo)

Common algorithms and data structures as Ruby C extensions

- [x] 💎 Elegant Ruby API
- [x] ⚡ Performant and reliable: A collection of quality C implementations 
- [x] 🥰 Actively maintained by Donald Dong

## Trees
### [K-d Tree](https://github.com/jtsiomb/kdtree)
```ruby
tree = Calgo::KdTree.new(dimension: 3)
a = Point.new(-5, -5, -5)
b = Point.new(1, 1, 1)
tree.insert(a)
tree.insert(b)

tree.nearest(Point.new(0, 0, 0)) # [b]
tree.nearest([0, 0, 0], range: 100) # [a, b]
```
