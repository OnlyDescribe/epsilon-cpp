#ifndef DECOMPOSITION_TREE_H
#define DECOMPOSITION_TREE_H

#include <iostream>
#include <queue>
#include <string>
#include <utility>

#include <boost/container/flat_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/property_map/property_map.hpp>
// #include <boost/iterator/iterator_adaptor.hpp>

#include "canonicalform.h"
#include "convert.h"
#include "epsilon.h"
// #include "triser.h"

namespace epsilon {

template<typename Tree>
const typename boost::graph_traits<Tree>::vertex_descriptor
get_vertex(Tree& tree, const typename Tree::out_edge_iterator& iter)
{
  return iter->m_target;
}

template<typename Tree>
const typename boost::graph_traits<Tree>::vertex_descriptor
get_vertex(Tree& tree, const typename Tree::vertex_iterator& iter)
{
  return *iter;
}

// 支持 *, ++, == 的迭代器
template<typename Tree, typename IteratorType>
class TreeIter
  : public boost::iterator_facade<TreeIter<Tree, IteratorType>,
                                  typename Tree::vertex_property_type,
                                  boost::forward_traversal_tag>
{
public:
  using other_type = TreeIter<Tree, IteratorType>;
  using vertex_t = typename boost::graph_traits<Tree>::vertex_descriptor;
  using vertex_property_t = typename Tree::vertex_property_type;

private:
  friend class boost::iterator_core_access; // used by iterator_facade

  // iterator_core_access 会调用以下成员函数
  // 同时本类继承 iterator_facade, 通过 iterator_core_access 来定义 *, ++, ==
  [[nodiscard]] vertex_property_t& dereference() const
  {
    return tree[get_vertex(tree, iter)];
  }

  void increment() { ++iter; }

  [[nodiscard]] bool equal(other_type const& other) const
  {
    return iter == other.iter;
  }

public:
  TreeIter(Tree& tree, IteratorType iter)
    : tree(tree)
    , iter(iter)
  {
  }

  other_type& operator=(other_type const& a)
  {
    tree = a.tree, iter = a.iter;
    return *this;
  };

  vertex_t vertex() { return get_vertex(tree, iter); }

private:
  Tree& tree;
  IteratorType iter;
};

// 对 pair<迭代器,迭代器> 的封装, 以支持 begin(),end()
template<typename IterType> // proxy container
class IterPair
{
public:
  using iterator_pair_t = std::pair<IterType, IterType>;

  IterPair(iterator_pair_t pair)
    : pair(std::move(pair))
  {
  }
  // 定义begin(),end() 以支持for(x:IterPair)遍历
  IterType begin() { return pair.first; }
  IterType end() { return pair.second; }

private:
  iterator_pair_t pair;
};

template<typename VertexProperty>
class BGTree
{
public:
  using tree_t = boost::
    adjacency_list<boost::setS, boost::vecS, boost::directedS, VertexProperty>;

  using vertex_t = typename boost::graph_traits<tree_t>::vertex_descriptor;
  using edge_t = typename boost::graph_traits<tree_t>::edge_descriptor;

  using edge_iterator = typename tree_t::edge_iterator;
  using vertex_iterator = typename tree_t::vertex_iterator;
  using out_edge_iterator = typename tree_t::out_edge_iterator;

  using vertex_tree_iterator = TreeIter<tree_t, vertex_iterator>;
  using vertex_pair_t = IterPair<vertex_tree_iterator>;

  using out_tree_iterator = TreeIter<tree_t, out_edge_iterator>;
  using out_pair_t = IterPair<out_tree_iterator>;

public:
  BGTree() = delete;

  BGTree(const VertexProperty& root)
    : tree_()
    , root_(boost::add_vertex(root, tree_))
  {
  }

  ~BGTree() { tree_.clear(); }

  BGTree(const BGTree&) = delete;
  BGTree operator=(const BGTree&) = delete;

  VertexProperty& operator[](vertex_t at) { return tree_[at]; }

  [[nodiscard]] const tree_t& get_tree() const { return tree_; }
  [[nodiscard]] vertex_t& get_root() { return root_; }

  vertex_t add_child(const VertexProperty& item, vertex_t where)
  {
    auto temp = boost::add_vertex(item, tree_);
    boost::add_edge(where, temp, tree_);
    return temp;
  }

  [[nodiscard]] vertex_t get_parent(vertex_t from) const
  {
    return boost::in_edges(from, tree_).first->m_source;
  }

  // auto get_bundle() { return boost::get(boost::vertex_bundle, tree_); }

  auto get_vertices()
  {
    return IterPair<vertex_iterator>(boost::vertices(tree_));
  }
  auto get_vertex_children(vertex_t pos)
  {
    return out_pair_t(boost::out_edges(pos, tree_));
  }
  auto get_edges() { return IterPair<edge_iterator>(boost::edges(tree_)); }

  // get children
  auto get_children(vertex_t pos = 0)
  {
    auto out_edges = boost::out_edges(pos, tree_);

    return out_pair_t(
      std::make_pair(out_tree_iterator(tree_, out_edges.first),
                     out_tree_iterator(tree_, out_edges.first)));
  }

  // get sub tree
  auto breadth_first(vertex_t pos = 0)
  {
    auto vertices = boost::vertices(tree_);

    return vertex_pair_t(
      std::make_pair(vertex_tree_iterator(tree_, vertices.first),
                     vertex_tree_iterator(tree_, vertices.second)));
  }

private:
  tree_t tree_;
  vertex_t root_;
};

struct VertexProperty
{
  using polynomial_set_t = CFSet;
  using polynomial_system_t = std::tuple<polynomial_set_t, polynomial_set_t>;

  int consistent; // -1 unknown. 1 consistent, 0 inconsistent
  int level;
  polynomial_system_t ps;
};

class DecompositionTree : public BGTree<VertexProperty>
{
  // 把节点的&放在一个字典里 然后查找那些label
public:
  using polynomial_set_t = CFSet;
  using polynomial_system_t = std::tuple<polynomial_set_t, polynomial_set_t>;

  DecompositionTree() = delete;

  DecompositionTree(const VertexProperty& root)
    : BGTree(root)
  {
    current_vetex_ = get_root();
    vertex_unused_.insert(current_vetex_);
  };

  vertex_t begin() { return *vertex_unused_.begin(); }

  bool empty() { return vertex_unused_.empty(); }

  void insert(const polynomial_system_t& poly_sys, int level)
  {
    auto t = add_child({ -1, level, poly_sys }, current_vetex_);
    vertex_unused_.insert(t);
  }
  void insert(std::tuple<const polynomial_system_t&, int> poly_sys_level)
  {
    insert(std::get<0>(poly_sys_level), std::get<1>(poly_sys_level));
  }

  void erase(vertex_t vertex)
  {
    vertex_unused_.erase(vertex);
    // TODO return
  }

  void set_current_vetex(vertex_t vertex) { current_vetex_ = vertex; }

  VertexProperty& get_current_vertex_property()
  {
    return (*this)[current_vetex_];
  }

public:
  void write_graphviz(std::ostream& os)
  {
    os << "digraph G {\n";
    os << "node[shape = box]\n";
    for (auto it : (*this).get_vertices())
      os << it << "[label=\"" << (*this)[it].ps << ","
         << "level: " << (*this)[it].level
         << ", consistent: " << (*this)[it].consistent << "\"];\n";
    for (auto it : (*this).get_edges())
      os << it.m_source << "->" << it.m_target << ";\n";
    os << "}\n";
  }

  void print(std::ostream& out) const
  {
    boost::write_graphviz(
      out,
      get_tree(),
      make_label_writer(get(&VertexProperty::ps, get_tree())),
      boost::default_writer{},
      boost::default_writer{} // get(&VertexData::ps, getTree())
    );
  }

private:
  vertex_t current_vetex_;
  std::set<vertex_t> vertex_unused_;
  // std::queue<vertex_t> vertex_unused_;
};

}

#endif
