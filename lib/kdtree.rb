# frozen_string_literal: true

require 'calgo/kdtree'
require 'set'

module Calgo
  class KdTree
    def initialize(dimension:)
      @dimension = dimension
      # mimic rb_gc_mark by holding node & poistion refs on this object
      @hold_refs = Set.new
      _initialize(dimension)
    end

    def insert(position)
      node, position = extract_position!(position)
      @hold_refs << node
      @hold_refs << position
      _insert(node, position.map(&:to_f))
    end

    def nearest(position, range: nil)
      _, position = extract_position!(position)
      _nearest(range, position)
    end

    private

    def extract_position!(position)
      unless position.is_a?(Array) || position.respond_to?(:position)
        raise ArgumentError, "#{position} does not have `position` defined"
      end

      node = position
      position = node.position if position.respond_to?(:position)

      unless position.all? { |n| n.is_a?(Numeric) }
        raise ArgumentError, 'coordinates must be numeric'
      end

      unless position.size == @dimension
        raise ArgumentError, "position must be #{@dimension}-dimensional"
      end

      [node, position]
    end
  end
end
