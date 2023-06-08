#include <iostream>
#include <random>
#include <set>
#include <cassert>
#include "red_black_tree.h"

void GenerateUniqueRandomValues(int* array, int size, int seed) {
    std::mt19937 rand(seed);
	constexpr int Coefficient = 5;
    std::uniform_int_distribution<> dist(0, size*Coefficient);

	std::set<int> set;
    for (int i = 0; i < size;) {
        int val = dist(rand);
        if (set.find(val) == set.end()) {
            set.insert(val);
            array[i++] = val;
        }
    }
}

void InsertRandom(RBTree& tree, int chunkStart, int chunkEnd, const int* values) {
	for (int i{ chunkStart }; i < chunkEnd; ++i) {
		tree.Add(values[i]);
	}
}
void Add_Test_Random() {
	constexpr int Num_Test_Itrs{ 30 };
	constexpr int Size{ 10000 };
	std::cout << "==================== Insert ====================" << std::endl;
	for (int i{}; i < Num_Test_Itrs; i++) {
		int seed = i + 1;
		int values[Size];
		GenerateUniqueRandomValues(values, Size, seed);
		std::cout << "Seed = " << seed << '\n';

		RBTree tree;

		for (int i = 0; i < Size; ++i) {
			tree.Add(values[i]);
			//tree.PrintTree();
		}

		//checks
		for (int i{}; i < Size; ++i) {
			assert(tree.Contains(values[i]));
		}
		std::wcout << "found all\n\n";
	}
}
void Add_Test_Multi_Threads() {
	std::cout << "====================multi-threaded Insert ====================" << std::endl;
	constexpr int Num_Tests{2};
	constexpr int Size{ 30 };
	constexpr int Num_Threads[]{ 1,2,4,8 };

	for (int num_threads:Num_Threads) {
		std::cout << "# threads = " << num_threads << '\n';
		for (int i{}; i < Num_Tests; i++) {
			int seed = i + 1;
			
			int values[Size];
			GenerateUniqueRandomValues(values, Size, seed);

			std::cout << "(Seed = " << seed << ")\n";

			RBTree tree;

			std::vector<std::thread> threads;
			int chunkSize = Size / num_threads;

			auto startMulti = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < num_threads; ++i) {
				if (i == num_threads - 1) {
					threads.push_back(std::thread(InsertRandom, std::ref(tree), chunkSize * i, std::max(chunkSize * (i + 1), Size), values));
				}
				else {
					threads.push_back(std::thread(InsertRandom, std::ref(tree), chunkSize * i, chunkSize * (i + 1), values));
				}
			}
			for (auto& th : threads) {
				th.join();
			}
			auto endMulti = std::chrono::high_resolution_clock::now();

			tree.PrintTree();

			//checks
			for (int i{}; i < Size; ++i) {
				assert(tree.Contains(values[i]));
			}

			std::chrono::duration<double> elapsedMulti = endMulti - startMulti;
			std::cout << "took " << elapsedMulti.count() << " seconds" << std::endl;

			std::cout << "found all ! \n\n";
		}
	}
}

int main() {
	//Add_Test_Random();
	Add_Test_Multi_Threads();
    return 0;
}
