/**
 * @file set.hpp
 * 
 * @brief Header file for the templated Set class.
 * 
 * Declaration/Definition of the templated Set class.
*/

#ifndef SET_HPP
#define SET_HPP

#include <iostream>
#include <algorithm> // std::swap
#include <ostream> // std::ostream
#include <stdexcept> // std::out_of_range
#include <iterator> // std::forward_iterator_tag
#include <cstddef> // std::ptrdiff_t, size_t
#include <fstream> // std::ofstream

/**
 * @brief Set Class
 * 
 * Generic Set of type T elements. The Set ensures that each element is unique, 
 * disallowing duplicates. 
 * The Set uses a dynamic array to store its elements. This array is 
 * dynamically resized to accommodate the changing number of elements: it is
 * doubled in size when full and halved when only one quarter or less of its
 * capacity is being used.
 * 
 * @tparam T Type of the elements in the Set.
 * @tparam Equal Functor used for comparing two elements for equality. Returns 
 * true if the elements passed are equal, false otherwhise.
*/
template <typename T, typename Equal>
class Set {
private:
  T* _array; ///< Pointer to the array
  size_t _size; ///< Capacity of the array (at a given moment)
  size_t _num_elements; ///< Number of elements currently in the Set
  Equal _equal; ///< Instance of the Equal functor;

  /**
   * @brief Resizes the dynamic array used by the Set.
   *
   * This function resizes the internal array of the Set. The resizing operation
   * takes care to copy the existing elements to the new array and to free up
   * the old array's memory.
   *
   * @param increase A boolean indicating whether to increase (true) or 
   * decrease (false) the array size.
   * 
   * @throw Allocation exception.
   */
  void resize(bool increase) {
    size_t new_size;
    if (increase) {
      new_size = _size > 0 ? _size * 2 : 1;
    } else {
      new_size = _size / 2;
    }

    T* new_array = nullptr;

    try {
      new_array = new T[new_size];

      for (size_t i = 0; i < _num_elements; ++i) {
        new_array[i] = _array[i];
      }

      delete[] _array;
      _array = new_array;
      _size = new_size;
    } catch(const std::exception& e) {
      std::cerr << "Exception caught in resize: " << e.what() << '\n';
      delete[] new_array; // Clean up new array in case of exception
      throw; // Re-throw the exception
    }
  }

public:
  /**
   * @brief Default constructor.
   * 
   * Initializes a new, empty Set.
   * 
   * @post _array == nullptr
   * @post _size == 0
   * @post _num_elements == 0
  */
  Set() : _array(nullptr), _size(0), _num_elements(0) {}

  /**
   * @brief Copy constructor.
   * 
   * Creates a new Set by copying the elements from another Set.
   * 
   * @param other Set from which to copy the elements.
   * 
   * @throw Allocation exception.
  */
  Set(const Set& other) : _array(nullptr), _num_elements(0), _size(0) {
    try {
      _array = new T[other._size];
      _size = other._size;

      for (size_t i = 0; i < other._num_elements; ++i) {
        _array[i] = other._array[i];
        ++_num_elements;
      }
    } catch(const std::exception& e) {
      std::cerr << "Exception caught in copy constructor: " << e.what() << '\n';
      empty(); // Clean up array in case of exception
      throw;
    }
  }

  /**
   * @brief Assignment operator.
   * 
   * Assigns the content of the specified 'other' Set to this Set. It creates a
   * copy of the 'other' Set and then swaps its contents with this Set.
   *
   * @param other The Set object to be copied.
   * 
   * @return A reference to this Set after the assignment.
   * 
   * @throw Allocation exception.
  */
  Set& operator=(const Set& other) {
    if (&other != this) {
      Set tmp(other);
      this->swap(tmp);
    }
    return *this;
  }

  /**
   * @brief Destructor.
   * 
   * Safely deallocates the dynamic memory used by the Set. Utilizes the empty()
   * function to do so.
   * 
   * @post The internal array memory has been deallocated.
   * @post _array == nullptr
   * @post _num_elements = 0
   * @post _size = 0
  */
  ~Set() {
    empty();
  }

  /**
   * @brief Empties the Set.
   * 
   * Safely deallocates the dynamic memory used by the Set.
   * 
   * @post The internal array memory has been deallocated.
   * @post _array == nullptr
   * @post _num_elements = 0
   * @post _size = 0
  */
  void empty(void) {
    delete[] _array;
    _array = nullptr;
    _num_elements = 0;
    _size = 0;
  }

  /**
   * @brief Swap function.
   *
   * Swaps the state between the current instance of Set and the instance
   * provided as a parameter.
   *
   * @param other The Set instance to swap states with the current instance.
  */
  void swap(Set &other) {
    std::swap(_num_elements, other._num_elements);
    std::swap(_size, other._size);
    std::swap(_array, other._array);
  }

  /**
   * @brief Adds a new element to the Set.
   * 
   * Inserts the value into the Set if it is not already present. 
   * If the Set gets full, the _array gets resized (doubled in size).
   * 
   * @param value The element of type T to be added to the Set.
   * 
   * @return true if the element was added, false if it is already contained.
   * 
   * @note If an exception is thrown during resizing, the state of Set hasn't 
   * been changed yet, mantaining the Set in a consistent state.
  */
  bool add(const T& value) {
    // Check if the element already exists
    if(this->contains(value)) {
      return false;
    }

    // If the array is full, resize it
    if (_num_elements == _size) {
      resize(true);
    }

    // Add the new element at the end of the used part of the array
    _array[_num_elements] = value;
    ++_num_elements;
    return true;
  }


  /**
   * @brief Removes an element from the Set.
   * 
   * If the value is present in the Set, it is removed. If the element is not
   * found, the Set remains unchanged. The Set is resized if it becomes
   * significantly underutilized as a result of the removal.
   * 
   * @param value The element of type T to be removed from the Set.
   * 
   * @return true if the element was removed, false if it is not contained.
   * 
   * @note If an exception is thrown during resizing, the element will still be
   * removed, but the internal array may not be resized. By doing this, the Set 
   * will mantain a consistent state.
  */
  bool remove(const T& value) {
    for (size_t i = 0; i < _num_elements; ++i) {
      if (_equal(_array[i], value)) {
        // Overwrite the removed element with the last element in the array
        _array[i] = _array[_num_elements - 1];
        --_num_elements;

        if (_num_elements <= _size / 4) {
          resize(false);
        }

        return true; // Element found and removed
      }
    }
    return false; // Element not found (and therefore not removed)
  }

  /**
   * @brief Accesses the element at the specified index.
   * 
   * Provides read-only access to the element at the given index.
   *
   * @param index The index of the element to access.
   * 
   * @return A const reference to the element at the specified index.
   * 
   * @throw std::out_of_range If the index is out of the bounds of the Set. 
   * The 'index' should be within the range of the Set (0 to number of elements
   * - 1), or an exception is thrown.
   */
  const T& operator[](int index) const {
    if (index < 0 || index >= _num_elements) {
      throw std::out_of_range("Index out of range");
    }
    return _array[index];
  }

  /**
   * @brief Checks if the Set contains a specific element.
   * 
   * Iterates through the Set to determine if it contains the specified value.
   * The comparison is performed using the custom equality functor.
   *
   * @param value The element to search for in the Set.
   * 
   * @return true if the element is found in the Set, false otherwise.
   */
  bool contains(const T& value) const {
    for (size_t i = 0; i < _num_elements; ++i) {
      if (_equal(_array[i], value)) {
        return true; // Element found
      }
    }
    return false; // Element not found
  }

  /**
   * Returns the number of elements stored inside of the Set
   * 
   * @return number of elements stored inside of the Set
  */
  size_t getNumElements() const {
    return _num_elements;
  }

  /**
   * @brief Constant forward iterator for the Set class.
   * 
   * This iterator provides read-only access to the elements of the Set.
   * It supports both prefix and postfix increment operations and can be used
   * to compare two iterators for equality or inequality.
  */
  class const_iterator {
  public:
    typedef std::forward_iterator_tag iterator_category; ///< Category of the iterator
    typedef T value_type; ///< Type of elements pointed to by the iterator
    typedef ptrdiff_t difference_type; ///< Type to represent the difference between two iterators
    typedef const T* pointer; ///< Pointer to the constant element type
    typedef const T& reference; ///< Reference to the constant element type

    /**
     * @brief Default constructor.
     * 
     * Initializes the iterator to a null pointer.
    */
    const_iterator() : _ptr(nullptr) {}

    /**
     * @brief Copy constructor.
     * 
     * @param other Another const_iterator to be copied.
    */
    const_iterator(const const_iterator &other) : _ptr(other._ptr) {}

    /**
     * @brief Assignment operator.
     * 
     * @param other Another const_iterator to be assigned from.
     * 
     * @return Reference to the updated iterator.
    */
    const_iterator& operator=(const const_iterator &other) {
      _ptr = other._ptr;
      return *this;
    }

    /**
     * @brief Destructor.
    */
    ~const_iterator() {}

    /**
     * @brief Dereference operator.
     * 
     * @return A reference to the element pointed to by the iterator.
    */
    reference operator*() const { return *_ptr; }

    /**
     * @brief Arrow operator.
     * 
     * @return A pointer to the element pointed to by the iterator.
    */
    pointer operator->() const { return _ptr; }

    /**
     * @brief Prefix increment operator.
     * 
     * Advances the iterator to the next element.
     * 
     * @return Reference to the updated iterator.
    */
    const_iterator& operator++() {
      ++_ptr;
      return *this;
    }

    /**
     * @brief Postfix increment operator.
     * 
     * Advances the iterator to the next element.
     * 
     * @return Copy of the original iterator.
    */
    const_iterator operator++(int) {
      const_iterator temp = *this;
      ++(*this);
      return temp;
    }

    /**
     * @brief Equality comparison operator.
     * 
     * @param other Another const_iterator to compare with.
     * 
     * @return True if both iterators point to same element, false otherwise.
    */
    bool operator==(const const_iterator &other) const {
      return _ptr == other._ptr;
    }

    /**
     * @brief Inequality comparison operator.
     * 
     * @param other Another const_iterator to compare with.
     * 
     * @return True if iterators point to different element, false otherwise.
    */
    bool operator!=(const const_iterator &other) const {
      return _ptr != other._ptr;
    }

  private:
    pointer _ptr; ///< Pointer to the current element in the Set.

    friend class Set; ///< Allow Set class to access private constructor.

    /**
     * @brief Constructor for internal use by the Set class.
     * 
     * @param ptr Pointer to the current element in the Set.
    */
    const_iterator(pointer ptr) : _ptr(ptr) {}

  }; //const_iterator class

  /**
   * @brief Returns an iterator to the beginning of the Set.
   * 
   * @return A const_iterator to the first element of the Set.
  */
  const_iterator begin() const {
    return const_iterator(_array);
  }

  /**
   * @brief Returns an iterator to the end of the Set.
   * 
   * @return A const_iterator to the element following the last element of the
   * Set.
  */
  const_iterator end() const {
    return const_iterator(_array + _num_elements);
  }

  /**
   * Constructor that creates a Set from a range defined by two iterators.
   * 
   * @param begin Iterator pointing to the beginning of the range.
   * @param end Iterator pointing to the end of the range.
  */
  template <typename IteratorQ>
  Set(IteratorQ begin, IteratorQ end) : _array(nullptr), _size(0), _num_elements(0) {
    try {
      for (IteratorQ it = begin; it != end; ++it) {
        add(*it);
      }
    } catch (const std::exception& e) {
      empty();
      std::cerr << "Exception caught in range constructor: " << e.what() << '\n';
      throw;
    }
  }

  /**
   * @brief Stream operator for the Set class.
   * 
   * This operator overloads the stream operator (<<) for the Set class.
   * 
   * @param os The output stream to which the Set data will be sent.
   * @param set The Set object to be output.
   * 
   * @return std::ostream& The modified output stream with the Set data.
  */
  inline friend std::ostream& operator<<(std::ostream& os, const Set& set) {
    os << set._num_elements;
    for (size_t i = 0; i < set._num_elements; ++i) {
      os << " (" << set._array[i] << ")";
    }
    return os;
  }

  /**
   * @brief Equality operator for Set.
   * 
   * Compares this Set with another Set for equality. Two Sets are considered
   * equal if they contain the same elements.
   * This method first checks if the number of elements in both sets is the
   * same. Then, it verifies that each element of this set is present in the
   * other set.
   *
   * @param other The Set to compare with.
   * 
   * @return True if the Sets contain the same elements, false otherwise.
   * 
   * @note The order in which the elements are saved in the Set is not relevant 
   * for the comparison.
  */
  bool operator==(const Set& other) const {
    if (_num_elements != other._num_elements) return false;

    for (size_t i = 0; i < _num_elements; ++i) {
      if (!this->contains(other._array[i])) return false;
    }

    return true;
  }
};

/**
 * @brief Filters elements of a set, based on a predicate.
 * 
 * This function creates a new Set containing elements from the original Set
 * that satisfy the given predicate.
 *
 * @tparam T The type of elements stored in the Set.
 * @tparam Equal A functor or function for comparing two elements of type T for 
 *         equality.
 * @tparam Predicate A functor or function that takes an element of type T and 
 *         returns a boolean.
 * 
 * @param S The original Set from which elements are filtered.
 * @param P The predicate function that decides whether an element should be 
 *          included in the new Set.
 * 
 * @return Set<T, Equal> A new Set containing elements that satisfy the 
 * predicate P.
*/
template <typename T, typename Equal, typename Predicate>
Set<T, Equal> filter_out(const Set<T, Equal>& S, Predicate P) {
  Set<T, Equal> new_set;
  try {
    for (typename Set<T, Equal>::const_iterator it = S.begin(); it != S.end(); ++it) {
      if (P(*it)) {
        new_set.add(*it);
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "Exception caught in filter_out: " << e.what() << '\n';
    new_set.empty();
    throw;
  }
  return new_set;
}

/**
 * @brief Overloads the addition operator to concatenate two sets.
 * 
 * Creates a new Set that represents the union of two sets, 'a' and 'b'. 
 * The resulting Set contains all the elements from both 'a' and 'b', ensuring 
 * uniqueness of the elements (no duplicates).
 * 
 * @tparam T The type of elements stored in the Set.
 * @tparam Equal A functor or function for comparing two elements of type T for 
 * equality.
 * 
 * @param a The first Set to be concatenated.
 * @param b The second Set to be concatenated.
 * 
 * @return Set<T, Equal> A new Set containing all elements from both 'a' and 
 * 'b', with duplicates removed.
*/
template <typename T, typename Equal>
Set<T, Equal> operator+(const Set<T, Equal>& a, const Set<T, Equal>& b) {
  Set<T, Equal> new_set = a;
  try {
    for (typename Set<T, Equal>::const_iterator it = b.begin(); it != b.end(); ++it) {
      new_set.add(*it);
    }
  } catch (const std::exception& e) {
    std::cerr << "Exception caught in operator+: " << e.what() << '\n';
    new_set.empty();
    throw;
  }
  return new_set;
}

/**
 * @brief Overloads the subtraction operator to calculate the intersection of 
 * two sets.
 * 
 * This function creates a new Set representing the intersection between Sets 
 * 'a' and 'b'. The intersection contains all elements that are present in both 
 * 'a' and 'b'.
 * 
 * @tparam T The type of elements stored in the Set.
 * @tparam Equal A functor or function for comparing two elements of type T for 
 * equality.
 * 
 * @param a The first Set to intersect.
 * @param b The second Set to intersect.
 * 
 * @return Set<T, Equal> A new Set containing the intersection of 'a' and 'b'.
*/
template <typename T, typename Equal>
Set<T, Equal> operator-(const Set<T, Equal>& a, const Set<T, Equal>& b) {
  Set<T, Equal> new_set;
  try {
    for (typename Set<T, Equal>::const_iterator it = a.begin(); it != a.end(); ++it) {
      if(b.contains(*it)) {
        new_set.add(*it);
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "Exception caught in operator-: " << e.what() << '\n';
    new_set.empty();
    throw;
  }
  return new_set;
}

/**
 * @brief Saves the contents of a Set to a file.
 * 
 * This function writes the contents of a given Set to a file specified by 
 * 'filename'. The Set is output using the overriden 'operator<<'.
 *
 * @tparam Equal A functor or function for comparing two elements of type T for 
 * equality.
 * 
 * @param set The Set to be saved to the file.
 * @param filename The name of the file to which the Set's contents will be
 *                 saved.
 * 
 * @note The function does not return a value or throw exceptions, but it 
 * reports to stderr if the file cannot be opened.
*/
template <typename Equal>
void save(const Set<std::string, Equal>& set, const std::string& filename) {
  std::ofstream outFile(filename);

  if (!outFile.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return;
  }

  outFile << set;

  outFile.close();
}

#endif // SET_HPP
