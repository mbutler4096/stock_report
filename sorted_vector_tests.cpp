#include "CppUnitTest.h"
#include "..\sorted_vector.h"
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define MSG(msg) [&]{ std::wstringstream _s; _s << msg; return _s.str(); }().c_str()

namespace sorted_vector_tests
{		
	TEST_CLASS(verify_sorted_vector)
	{
	public:
		//basic, complex data structure
		struct data_struct
		{
			std::string key;
			int value;
		};

		//compare function for object-2-object, object-2-key & key-2-object comparisons
		struct Compare
		{
			bool operator()(const data_struct& lhs, const data_struct& rhs) const
			{
				return lhs.key < rhs.key;
			}

			template<typename T>
			bool operator()(const data_struct& lhs, const T& key) const
			{
				return lhs.key < key;
			}

			template<typename T>
			bool operator()(const T& key, const data_struct& rhs) const
			{
				return key < rhs.key;
			}

			using is_transparent = int;
		};

		//tests to make sure that we got a valid iterator on a find for a complex type
		bool is_object_valid(std::vector<data_struct>::const_iterator it, std::vector<data_struct>::const_iterator end)
		{
			if (it == end) return false;

			return true;
		}

		//tests to make sure that we got a valid iterator on a find for a built-it type
		bool is_object_valid(std::vector<int>::const_iterator it, std::vector<int>::const_iterator end)
		{
			if (it == end) return false;

			return true;
		}

		//populate the vector for a default built-in type
		void populate_vector(sorted_vector<int>& vec)
		{
			for (int i = 0; i < 20; ++i)
				vec.insert(i);
		}

		//populate the vector for complex types that use an object as the key
		void populate_vector(sorted_vector<data_struct, data_struct, Compare>& vec)
		{
			for (int i = 0; i < 20; ++i)
			{
				data_struct obj;
				obj.key = "key " + std::to_string(i);
				obj.value = i;

				vec.insert(obj);
			}
		}

		//populate the vector for complex types that use std::string as the key
		void populate_vector(sorted_vector<data_struct, std::string, Compare>& vec)
		{
			for (int i = 0; i < 20; ++i)
			{
				data_struct obj;
				obj.key = "key " + std::to_string(i);
				obj.value = i;

				vec.insert(obj);
			}
		}


		TEST_METHOD(sorted_vector_verify_initial_size)
		{
			sorted_vector<int> vec;
			Assert::AreEqual((int)vec.size(), 0, L"Empty vector is not empty (should be)", LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_build_in_type)
		{
			sorted_vector<int> vec;
			vec.insert(5);

			Assert::AreEqual(vec[0], 5, L"Use of built-in type failed", LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_complex_type)
		{
			sorted_vector<data_struct, data_struct, Compare> vec;
			data_struct obj;
			obj.key = "key 1";
			vec.insert(obj);

			Assert::AreEqual(vec[0].key, std::string("key 1"), L"Use of complex type failed", LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_updated_size)
		{
			sorted_vector<int> vec;
			vec.insert(5);

			Assert::AreEqual((int)vec.size(), 1, L"Empty vector size is not 1 (should be)", LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_find_by_built_in_type)
		{
			sorted_vector<int> vec;
			populate_vector(vec);

			auto obj_ret = vec.find(5);

			Assert::AreEqual(is_object_valid(obj_ret, vec.end()), true, L"Find by string failed", LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_find_complex_type_by_string)
		{
			sorted_vector<data_struct, std::string, Compare> vec;
			populate_vector(vec);

			auto obj_ret = vec.find("key 12");

			Assert::AreEqual(is_object_valid(obj_ret, vec.end()), true, L"Find by string failed", LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_find_complex_type_by_object)
		{
			sorted_vector<data_struct, data_struct, Compare> vec;
			populate_vector(vec);

			data_struct obj;
			obj.key = "key 12";
			auto obj_ret = vec.find(obj);

			Assert::AreEqual(obj_ret->key, obj.key, L"Find by object failed", LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_value_stored)
		{
			sorted_vector<data_struct, data_struct, Compare> vec;

			data_struct obj;
			obj.key = "key 1";
			obj.value = 5;

			vec.insert(obj);

			auto obj_ret = vec.find(obj);

			Assert::AreEqual(obj_ret->key, obj.key, L"Stored value not found", LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_built_in_values_stored)
		{
			sorted_vector<int> vec;
			populate_vector(vec);

			for (int i = 0; i < 20; ++i)
			{
				auto it = vec.find(i);

				Assert::AreEqual(*it, i, L"Stored value not found", LINE_INFO());
			}
		}

		TEST_METHOD(sorted_vector_verify_complex_values_stored)
		{
			sorted_vector<data_struct, std::string, Compare> vec;
			populate_vector(vec);

			for (int i = 0; i < 20; ++i)
			{
				std::string key = "key " + std::to_string(i);
				auto obj_ret = vec.find(key);

				Assert::AreEqual(obj_ret->key, key, L"Stored value not found", LINE_INFO());
			}
		}

		TEST_METHOD(sorted_vector_verify_value_not_found)
		{
			sorted_vector<data_struct, std::string, Compare> vec;
			populate_vector(vec);

			auto obj_ret = vec.find(std::string("key 99"));

			Assert::AreEqual(is_object_valid(obj_ret, vec.end()), false, L"Non-stored value found", LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_value_erased)
		{
			sorted_vector<data_struct, std::string, Compare> vec;
			populate_vector(vec);

			std::string key = "key 12";
			vec.erase(vec.find(key));
			auto obj_ret = vec.find(key);

			Assert::AreEqual(is_object_valid(obj_ret, vec.end()), false, L"Non-stored value found", LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_all_values_erased_by_iterator)
		{
			sorted_vector<data_struct, std::string, Compare> vec;
			populate_vector(vec);

			for (int i = 0; i < 20; ++i)
			{
				std::string key = "key " + std::to_string(i);
				vec.erase(vec.find(key));
			}

			Assert::AreEqual((int)vec.size(), 0, L"Empty vector is not empty (should be)", LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_all_values_erased_by_key)
		{
			sorted_vector<data_struct, std::string, Compare> vec;
			populate_vector(vec);

			for (int i = 0; i < 20; ++i)
			{
				std::string key = "key " + std::to_string(i);
				vec.erase(key);
			}

			Assert::AreEqual((int)vec.size(), 0, L"Empty vector is not empty (should be)", LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_all_values_block_erased)
		{
			sorted_vector<data_struct, std::string, Compare> vec;
			populate_vector(vec);

			vec.erase(vec.begin(), vec.end());

			Assert::AreEqual((int)vec.size(), 0, L"Empty vector is not empty (should be)", LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_random_access)
		{
			sorted_vector<data_struct, std::string, Compare> vec;
			populate_vector(vec);

			int loc = 12;
			Assert::AreEqual(vec[loc].key, std::string("key 2"), MSG("Failed to find correct key at " << loc), LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_copy_constructor)
		{
			sorted_vector<data_struct, std::string, Compare> vec;
			populate_vector(vec);

			auto vec_copy(vec);

			int loc = 12;
			Assert::AreEqual(vec_copy[loc].key, vec[loc].key, MSG("Failed to find correct key at " << loc), LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_move_constructor)
		{
			sorted_vector<data_struct, std::string, Compare> vec;
			populate_vector(vec);

			auto vec_copy(std::move(vec));

			int loc = 12;
			Assert::AreEqual(vec_copy[loc].key, std::string("key 2"), MSG("Failed to find correct key at " << loc), LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_operator_equals)
		{
			sorted_vector<data_struct, std::string, Compare> vec;
			populate_vector(vec);

			auto vec_copy = vec;

			int loc = 12;
			Assert::AreEqual(vec_copy[loc].key, vec[loc].key, MSG("Failed to find correct key at " << loc), LINE_INFO());
		}

		TEST_METHOD(sorted_vector_verify_move_operator_equals)
		{
			sorted_vector<data_struct, std::string, Compare> vec;
			populate_vector(vec);

			auto vec_copy = std::move(vec);

			int loc = 12;
			Assert::AreEqual(vec_copy[loc].key, std::string("key 2"), MSG("Failed to find correct key at " << loc), LINE_INFO());
		}
	};
}