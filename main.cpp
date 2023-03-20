#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <unordered_map>
#include <memory>

using std::string;
using std::vector;

namespace {
    const vector<string> months = {"Январь", "Февраль", "Март", "Апрель", "Май", "Июнь",
                                             "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь"};
    const vector<string> headerTable = {"Месяц", "Баланс", "Приход", "Расход"};
    constexpr int ROWS = 12;
    constexpr int COLUMNS = 4;
    constexpr int MAX_RECEIPT = 100'000; 
}

class Node {
public:
    explicit Node(string _name, string _value = "") : name(_name), value(_value) {}

    /* Function for adding child to node
     * newNode - new child for node
     * return: nothing
     */
    void addNode(std::shared_ptr<Node> newNode) {
        newNode->depth = depth + 1;
        arrayNodes.push_back(newNode);
    }

    /* Function for getting child node by index
     * index - index array children
     * return: shared pointer to child node
     */
    std::shared_ptr<Node> getNode(size_t index) const {
        if (index >= arrayNodes.size()) {
            throw std::logic_error("Index out of array");
        }
        return arrayNodes[index];
    }

    /* Function for get count of children node
     * return: size of array children
     */
    size_t getSize() const {
        return arrayNodes.size();
    }

    /* Function for setting tag for node
     * nameTag - name tag node
     * valueTag - value this tag
     * return: nothing
     */
    void setTag(string nameTag, string valueTag) {
        tags[nameTag] = valueTag;
    }
    
    friend std::ostream& operator<<(std::ostream& fs, const Node& node);

private:
    string name;
    size_t depth = 0;
    string value = "";
    vector<std::shared_ptr<Node>> arrayNodes;

    std::unordered_map<string, string> tags;
};

/* Operator for output in file node
 * fs - file stream, where the information about the node is output
 * node - node that is being output
 * return: file stream
 */
std::ostream& operator<<(std::ostream& fs, const Node& node) {
    string tab(node.depth, '\t');

    fs << tab << "<" << node.name;

    for (const auto& [key, value]: node.tags) {
        fs << " " << key << "=\"" << value << "\"";
    }
    fs << ">\n";
    if (node.value != "") {
        fs << tab << "\t" << node.value << "\n";
    }
    for (const auto& child: node.arrayNodes) {
        fs << *child;
    }

    fs << tab << "</" << node.name << ">\n";

    return fs;
}

/* Function for creating value for report table
 * table - table that is filled with value
 * nInitBalance - elementary balance
 * return: nothing
 */
void CreateTable(vector<vector<int>>& table, int nInitBalance) {
    int prevBalance = nInitBalance, receipt = 0, expenditure = 0;
    
    srand(time(0));
    table.resize(ROWS, vector<int>(COLUMNS - 1));
    for (auto& row: table) {
        row[0] = prevBalance + receipt - expenditure;
        prevBalance = row[0];
        do {
            receipt = (rand() % MAX_RECEIPT) * (rand() % 2);
            expenditure = (rand() % row[0]) * (rand() % 2);
        } while (receipt == 0 && expenditure == 0);

        row[1] = receipt;
        row[2] = expenditure;
    }

}

/* Function for creating HTML tree for file
 * table - table with value
 * rootNode - root node with name <html>
 * return: nothing
 */
void CreateHTML(const vector<vector<int>>& table, std::shared_ptr<Node> rootNode) {
    rootNode->addNode(std::make_shared<Node>("head"));
    rootNode->getNode(0)->addNode(std::make_shared<Node>("title", "Report"));

    auto bodyNode = std::make_shared<Node>("body");
    rootNode->addNode(bodyNode);

    auto tableNode = std::make_shared<Node>("table");
    bodyNode->addNode(tableNode);
    tableNode->setTag("class", "report_table");

    tableNode->addNode(std::make_shared<Node>("thead"));
    tableNode->addNode(std::make_shared<Node>("tbody"));
    auto trHead = std::make_shared<Node>("tr");
    // tableNode[0] = thead
    tableNode->getNode(0)->addNode(trHead);
    for (size_t i = 0; i < COLUMNS; ++i) {
        trHead->addNode(std::make_shared<Node>("td", headerTable[i]));
    }
    // tableNode[1] = tbody
    for (size_t i = 0; i < ROWS; ++i) {
        tableNode->getNode(1)->addNode(std::make_shared<Node>("tr"));
        tableNode->getNode(1)->getNode(i)->addNode(std::make_shared<Node>("td", months[i]));
        for (size_t j = 0; j < COLUMNS - 1; ++j) {
            tableNode->getNode(1)->getNode(i)->addNode(std::make_shared<Node>("td", std::to_string(table[i][j + 1])));
        }
    }
}

/* Function for output HTML tree into file
 * rootNode - root node with name <html>
 * return: bool - true - file created successful
 *                false - file not created
 */
bool SaveHTMLToFile(std::shared_ptr<Node> rootNode) {
    std::fstream file("Report.htm");
    
    if (!file.is_open()) {
        return false;
    }

    file << "<!DOCTYPE html>\n" << *rootNode;

    file.close();
    return true; 
}

/* Function for build HTML file
 * Input parameters:
 * nInitBalance - elementary balance
 * return: bool - true - file created successful
 *                false - file not created
 */
bool BuildHtmlReport(int nInitBalance) {
    vector<vector<int>> table;

    CreateTable(table, nInitBalance);

    auto rootNode = std::make_shared<Node>("html");
    CreateHTML(table, rootNode);

    return SaveHTMLToFile(rootNode);
}

int main(int argc, char *argv[]) {
    
    if (BuildHtmlReport(100'000)) {
        std::cout << "File created!!!";
    } else {
        std::cout << "File not created!!!";
    }
    return 0;
}