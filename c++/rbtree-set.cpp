#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <set>

template<typename Key, size_t NKeys>
class RBTreeSet
{
  static_assert(NKeys > 1);

  typedef RBTreeSet* RBTreeSetPtr;

  enum class Color
  {
    red,
    black
  };

  Color mColor;
  Key mKey;
  RBTreeSetPtr mLeft;
  RBTreeSetPtr mRight;

  void insert(RBTreeSetPtr parent, Key const& key)
  {
    if (key == mKey) {
      return;
    }

    if (key < mKey) {
      if (mLeft) {
        mLeft->insert(this, key);
        return;
      }

      // Red-black trees lean left.
      assert(!mRight);
      mLeft = new RBTreeSet(Color::black, key);
      return;
    }

    if (mRight) {
      mRight->insert(this, key);
      return;
    }

    if (mLeft) {
      mRight = new RBTreeSet(Color::black, key);
      return;
    }

    // We must keep the tree leaning left.
    // XXX
  }

  void erase(RBTreeSetPtr parent, Key const& key)
  {
    if (key == mKey) {
      // XXX
      return;
    }

    if (key < mKey) {
      if (mLeft) {
        mLeft->erase(this, key);
      }
      return;
    }

    if (mRight) {
      mRight->erase(this, key);
    }
  }

  RBTreeSet(Color color)
    : mColor(color)
  {}

  RBTreeSet(Color color, Key const& key)
    : mColor(color)
    , mKey(key)
  {}

public:
  RBTreeSet()
    : RBTreeSet(Color::black)
  {}

  ~RBTreeSet()
  {
    delete mLeft;
    delete mRight;
  }

  bool contains(Key const& key) const
  {
    if (key == mKey) {
      return true;
    }

    if (key < mKey) {
      if (mLeft) {
        return mLeft->contains(key);
      }
      return false;
    }

    if (mRight) {
      return mRight->contains(key);
    }

    return false;
  }

  void insert(Key const& key) { insert(nullptr, key); }

  void erase(Key const& key) { erase(nullptr, key); }

  size_t maxDepth() const
  {
    size_t leftDepth = mLeft ? 1 + mLeft->maxDepth() : 0;
    size_t rightDepth = mRight ? 1 + mRight->maxDepth() : 0;

    return std::max(leftDepth, rightDepth);
  }

  size_t minDepth() const
  {
    size_t leftDepth = mLeft ? 1 + mLeft->minDepth() : 0;
    size_t rightDepth = mRight ? 1 + mRight->minDepth() : 0;

    return std::min(leftDepth, rightDepth);
  }

  size_t maxBlackDepth() const
  {
    size_t leftDepth =
      mLeft ? (mLeft->mColor == Color::black ? 1 + mLeft->maxDepth()
                                             : mLeft->maxBlackDepth())
            : 0;
    size_t rightDepth =
      mRight ? (mRight->mColor == Color::black ? 1 + mRight->maxDepth()
                                               : mRight->maxBlackDepth())
             : 0;

    return std::max(leftDepth, rightDepth);
  }

  size_t minBlackDepth() const
  {
    size_t leftDepth =
      mLeft ? (mLeft->mColor == Color::black ? 1 + mLeft->minDepth()
                                             : mLeft->minBlackDepth())
            : 0;
    size_t rightDepth =
      mRight ? (mRight->mColor == Color::black ? 1 + mRight->minDepth()
                                               : mRight->minBlackDepth())
             : 0;

    return std::min(leftDepth, rightDepth);
  }
};

void
testRBTreeSet()
{
  using Key = uint64_t;
  size_t constexpr NKeys = 3;
  using TestSet = RBTreeSet<Key, NKeys>;

  TestSet testSet;

  std::set<Key> expectedSet;

  std::default_random_engine generator;

  std::uniform_int_distribution<uint> opDistribution(1, 3);
  auto opGenerator = std::bind(opDistribution, generator);

  size_t constexpr keyRange = 3'000'000ULL;
  std::uniform_int_distribution<Key> distribution(1, keyRange);
  auto keyGenerator = std::bind(distribution, generator);

  size_t constexpr numIterations = 1'000'000ULL;

  size_t containsTrue = 0;
  size_t containsFalse = 0;
  size_t insert = 0;
  size_t erase = 0;

  for (size_t iteration = 0; iteration < numIterations; ++iteration) {
    auto key = keyGenerator();

    switch (opGenerator()) {
      case 1:
        if (expectedSet.contains(key)) {
          assert(testSet.contains(key));
          ++containsTrue;
        } else {
          assert(!testSet.contains(key));
          ++containsFalse;
        }
        break;
      case 2:
        expectedSet.insert(key);
        testSet.insert(key);
        ++insert;
        break;
      case 3:
        auto found = expectedSet.find(key);
        if (found != expectedSet.end()) {
          expectedSet.erase(found);
          assert(testSet.contains(key));
          testSet.erase(key);
          ++erase;
        } else {
          ++containsFalse;
        }
        assert(!testSet.contains(key));

        break;
    }
  }

  for (Key key = 1; key <= keyRange; ++key) {
    assert(testSet.contains(key) == expectedSet.contains(key));
  }

  std::cout << "RBTreeSet test passed" << std::endl;
  std::cout << "Depth: max=" << testSet.maxDepth()
            << "; min=" << testSet.minDepth() << std::endl;
  std::cout << "Black depth: max=" << testSet.maxBlackDepth()
            << "; min=" << testSet.minBlackDepth() << std::endl;
  std::cout << "#containsTrue=" << containsTrue << std::endl;
  std::cout << "#containsFalse=" << containsFalse << std::endl;
  std::cout << "#insert=" << insert << std::endl;
}

int
main(void)
{
  testRBTreeSet();
  return 0;
}
