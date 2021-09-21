#include "doctest.h"
#include "random_container.h"

#include <random>
#include <cstddef>
#include <utility>
#include <iostream>
#include <vector>
#include <iterator>

using test_task::random_container;

namespace {

struct test_randomizer {
	std::size_t operator()(std::size_t left_bound,
		                 std::size_t right_bound) const {
		static std::mt19937_64 rnd(4);
    	return left_bound + (static_cast<std::size_t>(rnd() % (right_bound - left_bound)));
	}
};

struct myhash {
	size_t operator()(const std::pair<int, int> &p) const {
		return std::hash<int>{}(p.first) ^ std::hash<int>{}(p.second);
	}
};


template<typename T, typename ...Ts> 
bool is_in_set(T first, Ts ...other) {
    return ((first == other) || ...);
}// CHECK(x == 3 || x == 5) doesn't work, so I wrote this

}

TEST_CASE("Full template parametrs") {
	random_container<int, test_randomizer, 
	                std::hash<int>, 
	                std::hash<std::size_t>> rc{test_randomizer(), std::hash<int>()};
	CHECK(rc.empty());
	rc.insert(5);
	rc.insert(4);
	rc.insert(3);
	rc.insert(std::move(3));
	rc.erase(3);
	rc.erase(4);
	CHECK(is_in_set(rc.random_element(), 3, 5));
}

TEST_CASE("Small cases") {
	{
		random_container<int> rc;
		rc.insert(3);
		rc.insert(3);
		rc.erase(3);
		CHECK(!rc.empty());
		CHECK(rc.random_element() == 3);
		rc.erase(3);
		CHECK(rc.empty());
	} 
	{
		random_container<int> rc;
		rc.insert(3);
		rc.insert(4);
		rc.insert(5);
		rc.insert(6);
		rc.insert(7);
		rc.erase(4);
		rc.erase(5);
		CHECK(is_in_set(std::move(rc).random_element(), 3, 6, 7));
		CHECK(rc.size() == 3);
	} 
	{
		random_container<int> rc;
		rc.insert(3);
		rc.insert(7);
		rc.insert(7);
		rc.insert(7);
		rc.erase(3);
		CHECK(rc.random_element() == 7);
		rc.erase(7);
		rc.erase(7);
		rc.erase(7);
		CHECK(rc.empty());
	}
}

TEST_CASE("A lot of inserts") {
	random_container<std::size_t> rc;
	const int STEPS = 100;
	for (std::size_t i = 0; i < STEPS; ++i) {
		rc.insert(i);
		CHECK(rc.random_element() <= i);
	}
}

TEST_CASE("Insert from vector") {
	random_container<std::size_t> rc;
	const int N = 100;

	std::vector <std::size_t> numbers(N);
	std::iota(numbers.begin(), numbers.end(), 1);
	rc.insert(numbers.begin(), numbers.end());
	
	CHECK(rc.size() == N);
	
	for (std::size_t i = N; i >= 1; --i) {
		CHECK(rc.random_element() <= i);
		rc.erase(i);
	}
}

TEST_CASE("Insert with duplicates") {
	random_container<std::size_t> rc;
	const int N = 100;
	std::vector <std::size_t> numbers(N);
	std::iota(numbers.begin(), numbers.end(), 1);
	
	rc.insert(numbers.begin(), numbers.end());
	rc.insert(numbers.begin(), numbers.end());
	CHECK(rc.size() == N * 2);
	
	for (std::size_t i = N; i >= 1; --i) {
		rc.erase(i);
		CHECK(rc.random_element() <= i);
		rc.erase(i);
		if (i > 1) {
			CHECK(rc.random_element() < i);
		}
	}
}

TEST_CASE("approximately probability") {
	random_container<std::size_t> rc;
	const int N = 100;
	for (int i = 1; i <= N; ++i) {
		rc.insert(i);
	}
	std::size_t count_1 = 0;
	std::size_t STEPS = 10'000;
	for (std::size_t i = 0; i < STEPS; ++i) {
		if (rc.random_element() == 1) {
			++count_1;
		}
	}
	bool is_in_good_segment = count_1 >= 50 && count_1 <= 300; 
	WARN(is_in_good_segment);
}

TEST_CASE("iterator") {
	random_container<std::size_t> rc;
	rc.insert(3);
	rc.insert(4);
	rc.insert(5);
	CHECK(is_in_set(static_cast<int>(*rc.begin()), 3, 4 ,5));
	CHECK(*(++rc.begin()) == 4);
	CHECK(*(rc.begin()++) == 3);
	random_container<std::pair<int, int>, test_randomizer, myhash> rc1;
	rc1.insert(std::make_pair(5, 4));
	CHECK(rc1.begin()->first == 5);
}

TEST_CASE("iterator range-based-for") {
	random_container<std::size_t> rc;
	rc.insert(3);
	rc.insert(4);
	rc.insert(5);
	for (std::size_t x : rc) {
		CHECK(is_in_set(static_cast<int>(x), 3, 4, 5));
	}
	rc.erase(3);
	rc.erase(4);
	for (std::size_t x : rc) {
		CHECK(x == 5);
	}
	rc.erase(5);
	for ([[maybe_unused]]std::size_t x : rc) {
		CHECK(false);
	}
}

TEST_CASE("constructors") {
	{
		random_container<int> rc;
		rc.insert(3);
		rc.insert(4);
		rc.insert(5);
		const random_container<int> rc1 = rc;
		CHECK(is_in_set(rc1.random_element(), 3, 4 ,5));
	}
	{
		random_container<int> rc;
		rc.insert(3);
		rc.insert(4);
		rc.insert(5);
		random_container<int> rc1 = std::move(rc);
		CHECK(is_in_set(rc1.random_element(), 3, 4 ,5));
		rc.clear();
		rc.insert(6);
		rc.insert(7);
		rc.insert(8);
		CHECK(is_in_set(rc.random_element(), 6, 7 ,8));
	}
}

TEST_CASE("operators=") {
	{
		random_container<int> rc;
		rc.insert(3);
		rc.insert(4);
		rc.insert(5);
		rc.insert(6);
		rc.insert(7);
		random_container<int> rc1;
		rc1 = rc;
		CHECK(is_in_set(rc1.random_element(), 3, 4 ,5, 6, 7));
		CHECK(rc.size() == 5);
	}
	{
		random_container<int> rc;
		rc.insert(3);
		rc.insert(4);
		rc.insert(5);
		random_container<int> rc1;
		rc1 = std::move(rc);
		CHECK(is_in_set(rc1.random_element(), 3, 4 ,5));
		rc.clear();
		rc.insert(6);
		rc.insert(7);
		rc.insert(8);
		CHECK(is_in_set(rc.random_element(), 6, 7 ,8));
	}
}

