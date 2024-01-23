/**
 * @file main.cpp
 * 
 * @brief File used to test the templated Set class.
*/

#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>
#include <vector>
#include "set.hpp"

class Person {
public:
  std::string name;
  int age;

  Person() : name(""), age(-1) {}
  Person(std::string name, int age) : name(name), age(age) {}

  friend std::ostream& operator<<(std::ostream& os, const Person& person);
};

std::ostream& operator<<(std::ostream& os, const Person& person) {
  os << "Name: " << person.name << ", Age: " << person.age;
  return os;
}

struct EqualPerson {
  bool operator()(const Person& a, const Person& b) const {
    return a.age == b.age && a.name == b.name;
  }
};

typedef Set<int, std::equal_to<int>> intSet;
typedef Set<std::string, std::equal_to<std::string>> stringSet;
typedef Set<Person, EqualPerson> personSet;

void testCopyConstructorInt() {
  intSet originalSet;
  originalSet.add(1);
  originalSet.add(2);
  originalSet.add(3);

  intSet copiedSet(originalSet);

  assert(copiedSet.getNumElements() == originalSet.getNumElements());
  assert(copiedSet.contains(1));
  assert(copiedSet.contains(2));
  assert(copiedSet.contains(3));

  std::cout << "testCopyConstructorInt() passed" << std::endl;
}

void testAssignmentOperatorInt() {
  intSet set1;
  set1.add(4);
  set1.add(5);

  intSet set2;
  set2 = set1;

  assert(set2.getNumElements() == 2);
  assert(set2.contains(4));
  assert(set2.contains(5));

  std::cout << "testAssignmentOperatorInt() passed" << std::endl;
}

void testDestructorInt() {
  intSet* dynamicSet = new intSet();
  dynamicSet->add(10);
  delete dynamicSet; // Check for mem leaks w/ valgrind

  std::cout << "testDestructorInt() passed" << std::endl;
}

void testEmptyInt() {
  intSet set;
  set.add(6);
  set.add(7);
  set.empty();

  assert(set.getNumElements() == 0);
  assert(!set.contains(6));
  assert(!set.contains(7));

  std::cout << "testEmptyInt() passed" << std::endl;
}

void testSwapInt() {
  intSet set1;
  set1.add(8);
  intSet set2;
  set2.add(9);

  set1.swap(set2);

  assert(set1.getNumElements() == 1);
  assert(set1.contains(9));
  assert(set2.contains(8));

  std::cout << "testSwapInt() passed" << std::endl;
}

void testAddInt() {
  intSet set;

  assert(set.add(1));
  assert(set.contains(1) && set.getNumElements() == 1);

  assert(set.add(2));
  assert(set.add(3));
  assert(set.contains(2) && set.contains(3) && set.getNumElements() == 3);

  assert(!set.add(2)); // Duplicate, should not change size
  assert(set.getNumElements() == 3);

  assert(set.add(-1));
  assert(set.contains(-1) && set.getNumElements() == 4);

  assert(set.add(0));
  assert(set.contains(0) && set.getNumElements() == 5);

  assert(set.add(100000));
  assert(set.contains(100000) && set.getNumElements() == 6);

  assert(set.add(-100000));
  assert(set.contains(-100000) && set.getNumElements() == 7);

  std::cout << "testAddInt() passed" << std::endl;
}

void testAddString() {
  stringSet set;

  assert(set.add("Deleits"));
  assert(set.contains("Deleits") && set.getNumElements() == 1);

  assert(set.add("Aidds"));
  assert(set.add("Cuncatenaits"));
  assert(set.contains("Aidds") && set.contains("Cuncatenaits") && set.getNumElements() == 3);

  assert(!set.add("Aidds")); // Duplicate, should not change size
  assert(set.getNumElements() == 3);

  std::cout << "testAddString() passed" << std::endl;
}

void testAddPerson() {
  personSet set;

  assert(set.add(Person("Ruben", 30)));
  assert(set.contains(Person("Ruben", 30)) && set.getNumElements() == 1);

  assert(set.add(Person("Youness", 25)));
  assert(set.add(Person("Quack", 35)));
  assert(set.contains(Person("Youness", 25)) && set.contains(Person("Quack", 35)) && set.getNumElements() == 3);

  assert(!set.add(Person("Youness", 25))); // Duplicate, should not change size
  assert(set.getNumElements() == 3);

  std::cout << "testAddPerson() passed" << std::endl;
}

void testRemoveInt() {
  intSet set;

  set.add(1);
  set.add(2);
  set.add(3);

  assert(set.remove(2));
  assert(!set.contains(2));
  assert(set.getNumElements() == 2);

  assert(!set.remove(4));
  assert(set.getNumElements() == 2);

  assert(set.remove(1));
  assert(set.remove(3));
  assert(set.getNumElements() == 0);

  std::cout << "testRemoveInt() passed" << std::endl;
}

void testRemoveString() {
  stringSet set;

  set.add("Deleits");
  set.add("Aidds");
  set.add("Cuncatenaits");

  assert(set.remove("Aidds"));
  assert(!set.contains("Aidds") && set.getNumElements() == 2);

  assert(!set.remove("Soubtracktss"));
  assert(set.getNumElements() == 2);

  assert(set.remove("Deleits"));
  assert(set.remove("Cuncatenaits"));
  assert(set.getNumElements() == 0);

  std::cout << "testRemoveString() passed" << std::endl;
}

void testRemovePerson() {
  personSet set;

  set.add(Person("Ruben", 30));
  set.add(Person("Youness", 25));
  set.add(Person("Quack", 35));

  assert(set.remove(Person("Youness", 25)));
  assert(!set.contains(Person("Youness", 25)) && set.getNumElements() == 2);

  assert(!set.remove(Person("Wasabi", 40)));
  assert(set.getNumElements() == 2);

  assert(set.remove(Person("Ruben", 30)));
  assert(set.remove(Person("Quack", 35)));
  assert(set.getNumElements() == 0);

  std::cout << "testRemovePerson() passed" << std::endl;
}

void testBracketOperatorInt() {
  intSet set;

  set.add(10);
  set.add(30);
  set.add(20);

  assert(set[0] == 10);
  assert(set[1] == 30);
  assert(set[2] == 20);

  try {
    int value = set[3];
    assert(false);
  } catch (const std::out_of_range& e) {
    // Expected exception
  }

  try {
    int value = set[-7654];
    assert(false);
  } catch (const std::out_of_range& e) {
    // Expected exception
  }

  std::cout << "testBracketOperatorInt() passed" << std::endl;
}

void testBracketOperatorString() {
  stringSet set;

  set.add("Deleits");
  set.add("Multiplicaits");
  set.add("Aidds");

  assert(set[0] == "Deleits");
  assert(set[1] == "Multiplicaits");
  assert(set[2] == "Aidds");

  try {
    std::string value = set[3];
    assert(false);
  } catch (const std::out_of_range& e) {
    // Expected exception
  }

  try {
    std::string value = set[-1];
    assert(false);
  } catch (const std::out_of_range& e) {
    // Expected exception
  }

  std::cout << "testBracketOperatorString() passed" << std::endl;
}

void testBracketOperatorPerson() {
  personSet set;

  Person ruben("Ruben", 32);
  Person quack("Quack", 21);
  Person youness("Youness", 25);

  set.add(ruben);
  set.add(quack);
  set.add(youness);
  
  assert(set[0].name == "Ruben" && set[0].age == 32);
  assert(set[1].name == "Quack" && set[1].age == 21);
  assert(set[2].name == "Youness" && set[2].age == 25);

  try {
    Person p = set[3];
    assert(false);
  } catch (const std::out_of_range& e) {
    // Expected exception
  }

  try {
    Person p = set[-3];
    assert(false);
  } catch (const std::out_of_range& e) {
    // Expected exception
  }

  std::cout << "testBracketOperatorPerson() passed" << std::endl;
}

void testContainsInt() {
  intSet set;

  assert(!set.contains(1));
  assert(!set.contains(2));

  set.add(1);
  set.add(2);

  assert(set.contains(1));
  assert(set.contains(2));

  assert(!set.contains(3));

  std::cout << "testContainsInt() passed" << std::endl;
}

void testContainsString() {
  stringSet set;

  assert(!set.contains("Deleits"));
  assert(!set.contains("Aidds"));

  set.add("Deleits");
  set.add("Aidds");

  assert(set.contains("Deleits"));
  assert(set.contains("Aidds"));

  assert(!set.contains("Cuncatenaits"));

  std::cout << "testContainsString() passed" << std::endl;
}

void testContainsPerson() {
  personSet set;

  Person ruben("Ruben", 32);
  Person quack("Quack", 21);
  Person youness("Youness", 25);

  assert(!set.contains(ruben));
  assert(!set.contains(youness));

  set.add(ruben);
  set.add(youness);

  assert(set.contains(ruben));
  assert(set.contains(youness));

  assert(!set.contains(quack));

  std::cout << "testContainsPerson() passed" << std::endl;
}

void testConstIteratorInt() {
  intSet set;

  set.add(10);
  set.add(30);
  set.add(20);

  int expectedOrder[] = {10, 30, 20};
  int index = 0;
  for (intSet::const_iterator it = set.begin(); it != set.end(); ++it) {
    assert(*it == expectedOrder[index++]);
  }

  std::cout << "testConstIteratorInt() passed" << std::endl;
}

void testConstIteratorString() {
  stringSet set;

  set.add("Deleits");
  set.add("Multiplicaits");
  set.add("Aidds");

  std::string expectedOrder[] = {"Deleits", "Multiplicaits", "Aidds"};
  int index = 0;
  for (stringSet::const_iterator it = set.begin(); it != set.end(); ++it) {
    assert(*it == expectedOrder[index++]);
  }

  std::cout << "testConstIteratorString() passed" << std::endl;
}

void testConstIteratorPerson() {
  personSet set;

  Person ruben("Ruben", 56);
  Person quack("Quack", 75);
  Person youness("Youness", 33);

  set.add(ruben);
  set.add(quack);
  set.add(youness);

  Person expectedOrder[] = {ruben, quack, youness};
  int index = 0;
  for (personSet::const_iterator it = set.begin(); it != set.end(); ++it) {
    assert(it->name == expectedOrder[index].name && it->age == expectedOrder[index].age);
    index++;
  }

  std::cout << "testConstIteratorPerson() passed" << std::endl;
}

void testIteratorConstructorInt() {
  std::vector<int> testData;

  testData.push_back(1);
  testData.push_back(4);
  testData.push_back(2);
  testData.push_back(7);

  intSet mySetFromIterators(testData.begin(), testData.end());

  for (size_t i = 0; i < testData.size(); i++) {
    assert(mySetFromIterators.contains(testData[i]));
  }

  std::cout << "testIteratorConstructorInt() passed" << std::endl;
}

void testIteratorConstructorString() {
  std::vector<std::string> testData;

  testData.push_back("Deleits");
  testData.push_back("Aidds");
  testData.push_back("Cuncatenaits");
  testData.push_back("Divaids");

  stringSet mySetFromIterators(testData.begin(), testData.end());

  for (size_t i = 0; i < testData.size(); i++) {
    assert(mySetFromIterators.contains(testData[i]));
  }

  std::cout << "testIteratorConstructorString() passed" << std::endl;
}

void testIteratorConstructorPerson() {
  std::vector<Person> testData;

  testData.push_back(Person("Ruben", 99));
  testData.push_back(Person("Youness", 13));
  testData.push_back(Person("Quack", 64));

  personSet mySetFromIterators(testData.begin(), testData.end());

  for (size_t i = 0; i < testData.size(); i++) {
    assert(mySetFromIterators.contains(testData[i]));
  }

  std::cout << "testIteratorConstructorPerson() passed" << std::endl;
}

void testPrintOperatorInt() {
  intSet set;
  set.add(1);
  set.add(2);
  set.add(3);

  std::stringstream buffer;
  std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
  std::cout.rdbuf(buffer.rdbuf());

  std::cout << set;
  
  std::cout.rdbuf(oldCoutStreamBuf);

  std::string expectedOutput = "3 (1) (2) (3)";
  assert(buffer.str() == expectedOutput);

  std::cout << "testPrintSetInt() passed" << std::endl;
}

void testPrintOperatorString() {
  stringSet set;
  set.add("Hello");
  set.add("World");
  set.add("Test");

  std::stringstream buffer;
  std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
  std::cout.rdbuf(buffer.rdbuf());

  std::cout << set;
  
  std::cout.rdbuf(oldCoutStreamBuf);

  std::string expectedOutput = "3 (Hello) (World) (Test)";
  assert(buffer.str() == expectedOutput);
  std::cout << "testPrintSetString() passed" << std::endl;
}

void testPrintOperatorPerson() {
  personSet set;
  set.add(Person("Johnny", 12));
  set.add(Person("Daky", 6));
  set.add(Person("Fortuna", 8));
  set.add(Person("Wasabi", 5));

  std::stringstream buffer;
  std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
  std::cout.rdbuf(buffer.rdbuf());

  std::cout << set;
  
  std::cout.rdbuf(oldCoutStreamBuf);

  std::string expectedOutput = "4 (Name: Johnny, Age: 12) (Name: Daky, Age: 6) (Name: Fortuna, Age: 8) (Name: Wasabi, Age: 5)";
  assert(buffer.str() == expectedOutput);
  std::cout << "testPrintSetPerson() passed" << std::endl;
}

void testEqualityOperatorInt() {
  intSet set1;
  set1.add(1);
  set1.add(2);
  set1.add(3);

  intSet set2;
  set2.add(3);
  set2.add(2);
  set2.add(1);

  assert(set1 == set2);
  assert(set2 == set1);

  set2.add(4);
  assert(!(set1 == set2));
  set2.remove(4);

  intSet emptySet;
  assert(!(set1 == emptySet));

  set2.remove(3);
  assert(!(set1 == set2));

  std::cout << "testEqualityOperatorInt() passed" << std::endl;
}

void testEqualityOperatorString() {
  stringSet set1;
  set1.add("Deleits");
  set1.add("Aidds");
  set1.add("Cuncatenaits");

  stringSet set2;
  set2.add("Cuncatenaits");
  set2.add("Aidds");
  set2.add("Deleits");

  assert(set1 == set2);
  assert(set2 == set1);

  set2.add("Divaids");
  assert(!(set1 == set2));
  set2.remove("Divaids");

  stringSet emptySet;
  assert(!(set1 == emptySet));

  set2.remove("Cuncatenaits");
  assert(!(set1 == set2));

  std::cout << "testEqualityOperatorString() passed" << std::endl;
}

void testEqualityOperatorPerson() {
  personSet set1;
  set1.add(Person("Mami", 61));
  set1.add(Person("Papi", 57));

  personSet set2;
  set2.add(Person("Mami", 61));
  set2.add(Person("Papi", 57));

  assert(set1 == set2);
  assert(set2 == set1);

  set2.add(Person("Gu", 31));
  assert(!(set1 == set2));
  set2.remove(Person("Gu", 31));

  personSet emptySet;
  assert(!(set1 == emptySet));

  set2.remove(Person("Mami", 61));
  set2.add(Person("Mami", 70));
  assert(!(set1 == set2));

  std::cout << "testEqualityOperatorPerson() passed" << std::endl;
}

void testFilterOutInt() {
  intSet set;
  for (int i = 1; i <= 5; ++i) {
    set.add(i);
  }

  auto isEven = [](int x) { return x % 2 == 0; };
  intSet filteredSet = filter_out(set, isEven);

  for (int i = 1; i <= 5; ++i) {
    if (i % 2 == 0) {
      assert(filteredSet.contains(i));
    } else {
      assert(!filteredSet.contains(i));
    }
  }

  std::cout << "testFilterOutInt() passed" << std::endl;
}

void testFilterOutString() {
  stringSet set;
  set.add("Deleits");
  set.add("Emre");
  set.add("Aidds");
  set.add("Cya");

  auto lengthGreaterThanFour = [](const std::string& s) { return s.length() > 4; };
  stringSet filteredSet = filter_out(set, lengthGreaterThanFour);

  assert(filteredSet.contains("Deleits"));
  assert(!filteredSet.contains("Emre"));
  assert(filteredSet.contains("Aidds"));
  assert(!filteredSet.contains("Cya"));

  std::cout << "testFilterOutString() passed" << std::endl;
}

void testFilterOutPerson() {
  personSet set;
  set.add(Person("Adoro", 17));
  set.add(Person("Nico", 20));
  set.add(Person("TTS bot", 16));
  set.add(Person("emremrmermermem", 22));

  auto isMinor = [](const Person& p) { return p.age < 18; };
  personSet filteredSet = filter_out(set, isMinor);

  assert(filteredSet.contains(Person("Adoro", 17)));
  assert(!filteredSet.contains(Person("Nico", 20)));
  assert(filteredSet.contains(Person("TTS bot", 16)));
  assert(!filteredSet.contains(Person("emremrmermermem", 22)));

  std::cout << "testFilterOutPerson() passed" << std::endl;
}

void testConcatenationOperatorInt() {
  intSet set1;
  set1.add(1);
  set1.add(2);
  set1.add(3);

  intSet set2;
  set2.add(3);
  set2.add(4);
  set2.add(5);

  intSet resultSet = set1 + set2;

  assert(resultSet.contains(1));
  assert(resultSet.contains(2));
  assert(resultSet.contains(3));
  assert(resultSet.contains(4));
  assert(resultSet.contains(5));
  assert(resultSet.getNumElements() == 5);

  std::stringstream buffer;
  std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
  std::cout.rdbuf(buffer.rdbuf());

  std::cout << resultSet;
  
  std::cout.rdbuf(oldCoutStreamBuf);

  std::string expectedOutput = "5 (1) (2) (3) (4) (5)";
  assert(buffer.str() == expectedOutput);

  std::cout << "testOperatorPlusInt() passed" << std::endl;
}

void testConcatenationOperatorString() {
  stringSet set1;
  set1.add("Deleits");
  set1.add("Aidds");

  stringSet set2;
  set2.add("Cuncatenaits");
  set2.add("Aidds");

  stringSet resultSet = set1 + set2;

  assert(resultSet.contains("Deleits"));
  assert(resultSet.contains("Aidds"));
  assert(resultSet.contains("Cuncatenaits"));
  assert(resultSet.getNumElements() == 3);

  std::stringstream buffer;
  std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
  std::cout.rdbuf(buffer.rdbuf());

  std::cout << resultSet;
  
  std::cout.rdbuf(oldCoutStreamBuf);

  std::string expectedOutput = "3 (Deleits) (Aidds) (Cuncatenaits)";
  assert(buffer.str() == expectedOutput);

  std::cout << "testConcatenationOperatorString() passed" << std::endl;
}

void testConcatenationOperatorPerson() {
  personSet set1;
  set1.add(Person("ericdoa", 21));
  set1.add(Person("glaive", 25));

  personSet set2;
  set2.add(Person("zeynep bastik", 35));
  set2.add(Person("ericdoa", 21));

  personSet resultSet = set1 + set2;

  assert(resultSet.contains(Person("ericdoa", 21)));
  assert(resultSet.contains(Person("glaive", 25)));
  assert(resultSet.contains(Person("zeynep bastik", 35)));
  assert(resultSet.getNumElements() == 3);

  std::stringstream buffer;
  std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
  std::cout.rdbuf(buffer.rdbuf());

  std::cout << resultSet;
  
  std::cout.rdbuf(oldCoutStreamBuf);

  std::string expectedOutput = "3 (Name: ericdoa, Age: 21) (Name: glaive, Age: 25) (Name: zeynep bastik, Age: 35)";
  assert(buffer.str() == expectedOutput);

  std::cout << "testConcatenationOperatorPerson() passed" << std::endl;
}

void testDifferenceOperatorInt() {
  intSet set1;
  set1.add(1);
  set1.add(2);
  set1.add(3);

  intSet set2;
  set2.add(3);
  set2.add(4);
  set2.add(5);

  intSet resultSet = set1 - set2;

  assert(!resultSet.contains(1));
  assert(!resultSet.contains(2));
  assert(resultSet.contains(3));
  assert(!resultSet.contains(4));
  assert(!resultSet.contains(5));
  assert(resultSet.getNumElements() == 1);

  std::stringstream buffer;
  std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
  std::cout.rdbuf(buffer.rdbuf());

  std::cout << resultSet;

  std::cout.rdbuf(oldCoutStreamBuf);

  std::string expectedOutput = "1 (3)";
  assert(buffer.str() == expectedOutput);

  std::cout << "testDifferenceOperatorInt() passed" << std::endl;
}

void testDifferenceOperatorString() {
  stringSet set1;
  set1.add("Hello");
  set1.add("World");
  set1.add("Test");
  set1.add("bhoo");

  stringSet set2;
  set2.add("Test");
  set2.add("bhoo");

  stringSet resultSet = set1 - set2;

  assert(!resultSet.contains("Hello"));
  assert(!resultSet.contains("World"));
  assert(resultSet.contains("Test"));
  assert(resultSet.contains("bhoo"));
  assert(resultSet.getNumElements() == 2);

  std::stringstream buffer;
  std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
  std::cout.rdbuf(buffer.rdbuf());

  std::cout << resultSet;

  std::cout.rdbuf(oldCoutStreamBuf);

  std::string expectedOutput = "2 (Test) (bhoo)";
  assert(buffer.str() == expectedOutput);

  std::cout << "testDifferenceOperatorString() passed" << std::endl;
}

void testDifferenceOperatorPerson() {
  personSet set1;
  set1.add(Person("tomcbumpz", 30));
  set1.add(Person("cosmo pyke", 25));

  personSet set2;
  set2.add(Person("rex orange county", 35));
  set2.add(Person("tomcbumpz", 30));

  personSet resultSet = set1 - set2;

  assert(resultSet.contains(Person("tomcbumpz", 30)));
  assert(!resultSet.contains(Person("cosmo pyke", 25)));
  assert(!resultSet.contains(Person("rex orange county", 35)));
  assert(resultSet.getNumElements() == 1);

  std::stringstream buffer;
  std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
  std::cout.rdbuf(buffer.rdbuf());

  std::cout << resultSet;

  std::cout.rdbuf(oldCoutStreamBuf);

  std::string expectedOutput = "1 (Name: tomcbumpz, Age: 30)";
  assert(buffer.str() == expectedOutput);

  std::cout << "testDifferenceOperatorPerson() passed" << std::endl;
}

void testSaveFunction() {
  stringSet set;
  set.add("Hello");
  set.add("World");
  set.add("Test");

  std::string filename = "test_save.txt";
  save(set, filename);

  std::ifstream inFile(filename);
  assert(inFile.is_open());

  std::stringstream fileContents;
  fileContents << inFile.rdbuf();
  inFile.close();

  std::string expectedOutput = "3 (Hello) (World) (Test)";

  assert(fileContents.str() == expectedOutput);

  std::remove(filename.c_str());

  std::cout << "testSaveFunction() passed" << std::endl;
}

int main() {
  // copy constructor
  testCopyConstructorInt();

  // assignment operator
  testAssignmentOperatorInt();

  // destructor
  testDestructorInt();

  // empty
  testEmptyInt();

  // swap
  testSwapInt();

  // tests add
  testAddInt();
  testAddString();
  testAddPerson();

  // tests remove
  testRemoveInt();
  testRemoveString();
  testRemovePerson();

  // tests operator[]
  testBracketOperatorInt();
  testBracketOperatorString();
  testBracketOperatorPerson();

  // tests contains
  testContainsInt();
  testContainsString();
  testContainsPerson();

  // tests const iterator
  testConstIteratorInt();
  testConstIteratorString();
  testConstIteratorPerson();

  // tests constructor with iterators
  testIteratorConstructorInt();
  testIteratorConstructorString();
  testIteratorConstructorPerson();

  // tests operator<<
  testPrintOperatorInt();
  testPrintOperatorString();
  testPrintOperatorPerson();

  // tests operator==
  testEqualityOperatorInt();
  testEqualityOperatorString();
  testEqualityOperatorPerson();

  // tests filter_out
  testFilterOutInt();
  testFilterOutString();
  testFilterOutPerson();

  // tests operator+
  testConcatenationOperatorInt();
  testConcatenationOperatorString();
  testConcatenationOperatorPerson();

  // tests operator-
  testDifferenceOperatorInt();
  testDifferenceOperatorString();
  testDifferenceOperatorPerson();

  // tests save
  testSaveFunction();

  std::cout << "All tests passed!" << std::endl;
  return 0;
}
