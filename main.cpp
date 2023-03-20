#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

//shared_ptr
#include <memory>

// Rand
#include <cstdlib>
#include <ctime>

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

/**
 * @brief Class for node(element) to HTML
 **/
class Node {
public:
    explicit Node(const string& _name, bool _isEmptyNode = false, const string& _value = "") : name(_name), isEmptyNode(_isEmptyNode), value(_value) {}

    /**
     * @brief Add child to node
     * @param newNode - new child for node
     * @return void
     **/
    void addNode(std::shared_ptr<Node> newNode) {
        newNode->depth = depth + 1;
        arrayNodes.push_back(newNode);
    }

    /**
     * @brief Get child node by index
     * @param index - index array children
     * @return shared pointer to child node
     **/
    std::shared_ptr<Node> getNode(size_t index) const {
        if (index >= arrayNodes.size()) {
            throw std::logic_error("Index out of array");
        }
        return arrayNodes[index];
    }

    /**
     * @brief Get count of children node
     * @return size of array children
     **/
    size_t getSize() const {
        return arrayNodes.size();
    }

    /**
     * @brief Get depth node
     * @return size_t - depth node
     **/
    size_t getDepth() const {
        return depth;
    }

    /**
     * @brief setting attribute for node
     * @param nameAttr - name tag node
     * @param valueAttr - value this attribute
     * @return * void 
     *
     **/
    void setAttr(const string& nameAttr, const string& valueAttr) {
        attrs[nameAttr] = valueAttr;
    }

    /**
     * @brief Set the Value node
     * 
     * @param _value - value node
     * @return void 
     **/
    void setValue(const string& _value) {
        value = _value;
    }
    
    friend std::ostream& operator<<(std::ostream& fs, const Node& node);

private:
    string name;
    size_t depth = 0;
    bool isEmptyNode;
    string value = "";
    vector<std::shared_ptr<Node>> arrayNodes;

    std::unordered_map<string, string> attrs;
};

/**
 * @brief Operator for output in file node
 * 
 * @param fs - file stream, where the information about the node is output
 * @param node - node that is being output
 * @return std::ostream& - file stream
 */
std::ostream& operator<<(std::ostream& fs, const Node& node) {
    string tab(node.depth, '\t');

    fs << tab << "<" << node.name;

    for (const auto& [key, value]: node.attrs) {
        fs << " " << key << "=\"" << value << "\"";
    }
    fs << ">\n";

    if (!node.isEmptyNode) {
        if (node.value != "") {
            fs << tab << "\t" << node.value << "\n";
        }
        for (const auto& child: node.arrayNodes) {
            fs << *child;
        }

        fs << tab << "</" << node.name << ">\n";
    }
    return fs;
}

/**
 * @brief Create table for report
 * 
 * @param table - table that is filled with value
 * @param nInitBalance - elementary balance
 * @return void
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

/**
 * @brief create CSS style
 * 
 * @param styleNode - style element in HTML
 **/
void CreateCSSNode(std::shared_ptr<Node> styleNode) {
    string resultValue;
    size_t depth = styleNode->getDepth();
    string tab(depth + 1, '\t');

    std::unordered_map<string, std::unordered_map<string, std::unordered_set<string>>> styleCSS;

    styleCSS["td"]["border"].insert("1px");
    styleCSS["td"]["border"].insert("solid");

    styleCSS["table"]["width"].insert("60%");

    styleCSS["thead"]["background-color"].insert("lightgrey");
    styleCSS["thead"]["text-align"].insert("center");
    styleCSS["thead"]["font-weight"].insert("bold");

    styleCSS["td"]["padding-top"].insert("10px");
    styleCSS["td"]["padding-bottom"].insert("10px");
    styleCSS["td"]["padding-left"].insert("5px");
    styleCSS["td"]["padding-right"].insert("5px");

    styleCSS[".colInt"]["text-align"].insert("right");

    for (const auto& [elem, properties]: styleCSS) {
        resultValue += elem + " {\n";
        for (const auto& [name, values]: properties) {
            resultValue += tab + "\t" + name + ":";
            for (const auto& value: values) {
                resultValue += " " + value;
            }
            resultValue += ";\n";
        }
        resultValue += tab + "}\n" + tab;
    }

    styleNode->setValue(resultValue);
}

/**
 * @brief  Creating HTML tree for file
 * 
 * @param table - table with values
 * @param rootNode - root node with name <html>
 * @return void
 */
void CreateHTML(const vector<vector<int>>& table, std::shared_ptr<Node> rootNode) {
    auto headNode = std::make_shared<Node>("head");
    rootNode->addNode(headNode);

    headNode->addNode(std::make_shared<Node>("meta", true));
    headNode->getNode(0)->setAttr("charset", "utf-8");

    auto styleNode = std::make_shared<Node>("style");
    headNode->addNode(styleNode);

    styleNode->setAttr("type", "text/css");

    CreateCSSNode(styleNode);

    headNode->addNode(std::make_shared<Node>("title", false, "Report"));
    auto bodyNode = std::make_shared<Node>("body");
    rootNode->addNode(bodyNode);

    auto tableNode = std::make_shared<Node>("table");
    bodyNode->addNode(tableNode);
    tableNode->setAttr("class", "report_table");

    tableNode->addNode(std::make_shared<Node>("thead"));
    tableNode->addNode(std::make_shared<Node>("tbody"));
    auto trHead = std::make_shared<Node>("tr");
    // tableNode[0] = thead
    tableNode->getNode(0)->addNode(trHead);
    for (size_t i = 0; i < COLUMNS; ++i) {
        trHead->addNode(std::make_shared<Node>("td", false, headerTable[i]));
    }
    // tableNode[1] = tbody
    for (size_t i = 0; i < ROWS; ++i) {
        tableNode->getNode(1)->addNode(std::make_shared<Node>("tr"));
        tableNode->getNode(1)->getNode(i)->addNode(std::make_shared<Node>("td", false, months[i]));
        for (size_t j = 0; j < COLUMNS - 1; ++j) {
            tableNode->getNode(1)->getNode(i)->addNode(std::make_shared<Node>("td", false, std::to_string(table[i][j])));
            tableNode->getNode(1)->getNode(i)->getNode(j + 1)->setAttr("class", "colInt");
        }
    }
}

/**
 * @brief Output HTML tree into file
 *
 * @param rootNode - root node with name <html>
 * @return true - file created successful
 * @return false - file not created
 */
bool SaveHTMLToFile(std::shared_ptr<Node> rootNode) {
    std::fstream file("Report.htm", std::fstream::out | std::fstream::trunc);
    
    if (!file.is_open()) {
        return false;
    }

    file << "<!DOCTYPE html>\n" << *rootNode;

    file.close();
    return true; 
}

/**
 * @brief Build HTML file for report
 * 
 * @param nInitBalance  - elementary balance
 * @return true - file created successful
 * @return false - file not created
 */
bool BuildHtmlReport(int nInitBalance) {
    vector<vector<int>> table;

    CreateTable(table, nInitBalance);

    auto rootNode = std::make_shared<Node>("html");
    CreateHTML(table, rootNode);

    return SaveHTMLToFile(rootNode);
}

/**
 * @brief Main function
 * 
 * @param argc - count of argument (should be 2)
 * @param argv - arguments value (1 - name executable file,  2 - init balance)
 * @return int
 */
int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        std::cout << "Invalid input. Input example: " << argv[0] << " 100000\n";
        return -1;
    }

    int balance = 0;
    size_t pos = 0;
    while (argv[1][pos] != '\0') {
        if (std::isdigit(argv[1][pos])) {
            balance *= 10;
            balance += (argv[1][pos] - '0');
            ++pos;
        } else {
            std::cout << "Invalid input. Argument should be number!";
            return -1;
        }
    }

    if (BuildHtmlReport(balance)) {
        std::cout << "File created!!!\n";
    } else {
        std::cout << "File not created!!!\n";
    }
    return 0;
}