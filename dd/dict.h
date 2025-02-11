#include <iostream>
#include <map>

class dict {
private:
    std::map<int, int> elements;  // 存储元素及其频数

public:
    uint32_t size() const {
        // sizeof(key) + sizeof(value) + 3 * sizeof(pointer) + sizeof(color) = 4 + 4 + 3 * 8 + 1 = 33 BYTE
        // alignment: 40 BYTE
        uint32_t DICT_NODE_SIZE = 40;
        uint32_t numNodes = elements.size();
        return DICT_NODE_SIZE * numNodes;
    }

    uint32_t total_items() const {
        return elements.size();
    }

    void insert(int x) {
        elements[x]++;
    }

    uint32_t count(int x) const {
        auto it = elements.find(x);
        if (it != elements.end()) {
            return it->second;
        }
        return 0;
    }

    void uniformCollapse() {
        // printf("uniform collapse");
        std::map<int, int> newMap;

        for (const auto& pair : elements) {
            int newKey = static_cast<int>(std::ceil(pair.first / 2.0));
            newMap[newKey] += pair.second;
        }

        elements = newMap;
    }

    void mergeMinToSecondMin() {
        if (elements.size() < 2) {
            // std::cerr << "Error: Not enough elements to merge." << std::endl;
            return;
        }

        auto it = elements.begin();
        int minElement = it->first;
        int minCount = it->second;

        it++;
        int secondMinElement = it->first;
        int secondMinCount = it->second;

        elements[secondMinElement] += minCount;
        elements.erase(minElement);
    }

    void print() const {
        for (const auto& pair : elements) {
            std::cout << "Element: " << pair.first << ", Count: " << pair.second << std::endl;
        }
    }

    std::map<int, int>::const_iterator begin() const {
        return elements.cbegin();
    }

    std::map<int, int>::const_iterator end() const {
        return elements.cend();
    }
};