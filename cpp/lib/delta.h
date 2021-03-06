// DeltaExpr is a linear expression where each term is a tensor product of residuals of
// two variables.
//
// Example:
//   -2 (x1 - x2)*(x1 - x2)*(x1 - x2)
//    + (x1 - x2)*(x2 - x3)*(x1 - x4)
//
// The expression is always normalized so that in each residual the variable with smaller
// index goes first. The order of variables does not affect the sign: (x2 - x1) == (x1 - x2).
// If any factor contains two equal variable or contains at least one variable that is
// equal to infinity, the entire term is discarded.

#pragma once

#include <algorithm>
#include <limits>

#include "absl/strings/str_cat.h"

#include "check.h"
#include "coalgebra.h"
#include "format.h"
#include "linear.h"
#include "pvector.h"
#include "util.h"
#include "x.h"


// TODO: Implement integratability criterion

// Represents (x_i - x_j).
class Delta {
public:
  Delta() {}
  Delta(X a, X b) {
    if (a == Inf || b == Inf) {
      a_ = 0;
      b_ = 0;
    } else {
      a_ = a.var();
      b_ = b.var();
      CHECK_GE(a_, 1);
      CHECK_GE(b_, 1);
      sort_two(a_, b_);
    }
  }

  int a() const { return a_; }
  int b() const { return b_; }

  bool is_nil() const { return a_ == b_; }

  bool contains(int point) const { return point == a_ || point == b_; }
  int other_point(int point) const;

  bool operator==(const Delta& other) const { return as_pair() == other.as_pair(); }
  bool operator!=(const Delta& other) const { return as_pair() != other.as_pair(); }
  bool operator< (const Delta& other) const { return as_pair() <  other.as_pair(); }
  bool operator<=(const Delta& other) const { return as_pair() <= other.as_pair(); }
  bool operator> (const Delta& other) const { return as_pair() >  other.as_pair(); }
  bool operator>=(const Delta& other) const { return as_pair() >= other.as_pair(); }

  // Put larger point first to synchronize ordering with DeltaAlphabetMapping.
  std::pair<int, int> as_pair() const { return {b_, a_}; }

  template <typename H>
  friend H AbslHashValue(H h, const Delta& delta) {
    return H::combine(std::move(h), delta.as_pair());
  }

private:
  int a_ = 0;
  int b_ = 0;
};

inline std::string to_string(const Delta& d) {
  // return fmt::parens(fmt::diff(fmt::var(d.a()), fmt::var(d.b())));
  return fmt::brackets(absl::StrCat(d.a(), ",", d.b()));
}

inline int Delta::other_point(int point) const {
  if (point == a_) {
    return b_;
  } else if (point == b_) {
    return a_;
  } else {
    FATAL(absl::StrCat("Point ", point, " not found in ", to_string(*this)));
  }
}

namespace internal {
using DeltaDiffT = unsigned char;
}  // namespace internal


class DeltaAlphabetMapping {
public:
  static constexpr int kMaxDimension = X::kMaxVariableIndex;

  DeltaAlphabetMapping() {
    static constexpr int kAlphabetSize = kMaxDimension * (kMaxDimension - 1) / 2;
    static_assert(kAlphabetSize <= std::numeric_limits<internal::DeltaDiffT>::max() + 1);
    deltas_.resize(kAlphabetSize);
    for (int b : range_incl(1, kMaxDimension)) {
      for (int a : range(1, b)) {
        Delta d(a, b);
        deltas_.at(to_alphabet(d)) = d;
      }
    }
  }

  int to_alphabet(const Delta& d) const {
    CHECK(!d.is_nil());
    CHECK_LE(d.b(), kMaxDimension);
    const int za = d.a() - 1;
    const int zb = d.b() - 1;
    return zb*(zb-1)/2 + za;
  }

  Delta from_alphabet(int ch) const {
    return deltas_.at(ch);
  }

private:
  std::vector<Delta> deltas_;
};

// Idea: replace with PArray<uint4_t, 2>
extern DeltaAlphabetMapping delta_alphabet_mapping;

namespace internal {
struct DeltaExprParam {
  using ObjectT = std::vector<Delta>;
#if DISABLE_PACKING
  IDENTITY_STORAGE_FORM
#else
  using StorageT = PVector<DeltaDiffT, 10>;
  static StorageT object_to_key(const ObjectT& obj) {
    return mapped_to_pvector<StorageT>(obj, [](const Delta& d) -> DeltaDiffT {
      return delta_alphabet_mapping.to_alphabet(d);
    });
  }
  static ObjectT key_to_object(const StorageT& key) {
    return mapped(key, [](int ch) {
      return delta_alphabet_mapping.from_alphabet(ch);
    });
  }
#endif
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_DEFAULT
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::tensor_prod());
  }
  static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs) {
    return concat(lhs, rhs);
  }
  static int object_to_weight(const ObjectT& obj) {
    return obj.size();
  }
};

struct DeltaCoExprParam {
  using ObjectT = std::vector<std::vector<Delta>>;
#if DISABLE_PACKING
  IDENTITY_STORAGE_FORM
#else
  using PartStorageT = DeltaExprParam::StorageT;
  using StorageT = PVector<PartStorageT, 2>;
  static StorageT object_to_key(const ObjectT& obj) {
    return mapped_to_pvector<StorageT>(obj, DeltaExprParam::object_to_key);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return mapped(key, DeltaExprParam::key_to_object);
  }
#endif
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_LENGTH_FIRST
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_lie(), DeltaExprParam::object_to_string);
  }
  static int object_to_weight(const ObjectT& obj) {
    return sum(mapped(obj, [](const auto& part) { return part.size(); }));
  }
  static constexpr bool coproduct_is_lie_algebra = true;
};
}  // namespace internal


using DeltaExpr = Linear<internal::DeltaExprParam>;
using DeltaCoExpr = Linear<internal::DeltaCoExprParam>;
template<> struct CoExprForExpr<DeltaExpr> { using type = DeltaCoExpr; };

inline DeltaExpr D(X a, X b) {
  Delta d(a, b);
  return d.is_nil() ? DeltaExpr() : DeltaExpr::single({d});
}


DeltaExpr substitute_variables(const DeltaExpr& expr, const XArgs& new_points);

// Expects: points.size() == 6
// Eliminates terms (x5-x6), (x4-x6), (x2-x6) using involution x1<->x4, x2<->x5, x3<->x6.
DeltaExpr involute(const DeltaExpr& expr, const std::vector<int>& points);

DeltaExpr sort_term_multiples(const DeltaExpr& expr);
DeltaExpr terms_with_unique_muptiples(const DeltaExpr& expr);
DeltaExpr terms_with_nonunique_muptiples(const DeltaExpr& expr);

DeltaExpr terms_with_num_distinct_variables(const DeltaExpr& expr, int num_distinct);
DeltaExpr terms_with_min_distinct_variables(const DeltaExpr& expr, int min_distinct);
DeltaExpr terms_containing_only_variables(const DeltaExpr& expr, const std::vector<int>& indices);
DeltaExpr terms_without_variables(const DeltaExpr& expr, const std::vector<int>& indices);

DeltaExpr terms_with_connected_variable_graph(const DeltaExpr& expr);

// For using together with `DeltaExpr::filter`
inline int count_var(const DeltaExpr::ObjectT& term, int var) {
  return absl::c_count_if(term, [&](const Delta& d) { return d.contains(var); });
};

void print_sorted_by_num_distinct_variables(std::ostream& os, const DeltaExpr& expr);
