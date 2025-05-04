#include <iostream>
#include <vector>
#include <string>

// Шаблонный класс Queue (очередь)
template <typename T>
class Queue {
private:
    std::vector<T> elements;

public:
    // Добавление элемента в очередь
    void push(const T& item) {
        elements.push_back(item);
    }

    // Удаление элемента из очереди
    void pop() {
        if (!elements.empty()) {
            elements.erase(elements.begin());
        }
    }

    // Просмотр первого элемента
    const T& front() const {
        return elements.front();
    }

    // Проверка на пустоту
    bool empty() const {
        return elements.empty();
    }

    // Размер очереди
    size_t size() const {
        return elements.size();
    }

    // Вывод всех элементов (для демонстрации)
    void displayAll() const {
        for (const auto& item : elements) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
};

// Базовый класс для игровых объектов
class Entity {
public:
    virtual void displayInfo() const = 0;
    virtual ~Entity() {}
};

// Класс игрока
class Player : public Entity {
    std::string name;
    int health;
    int level;
public:
    Player(const std::string& n, int h, int l) : name(n), health(h), level(l) {}
    void displayInfo() const override {
        std::cout << "Player: " << name << ", Health: " << health << ", Level: " << level << std::endl;
    }
};

// Класс врага
class Enemy : public Entity {
    std::string name;
    int health;
    std::string type;
public:
    Enemy(const std::string& n, int h, const std::string& t) : name(n), health(h), type(t) {}
    void displayInfo() const override {
        std::cout << "Enemy: " << name << ", Health: " << health << ", Type: " << type << std::endl;
    }
};

// Шаблонный класс GameManager
template <typename T>
class GameManager {
private:
    std::vector<T> entities;

public:
    void addEntity(const T& entity) {
        entities.push_back(entity);
    }

    void displayAll() const {
        for (const auto& entity : entities) {
            entity->displayInfo();
        }
    }

    ~GameManager() {
        for (auto& entity : entities) {
            delete entity;
        }
    }
};

int main() {
    // Тестирование шаблонной очереди с числами
    Queue<int> intQueue;
    intQueue.push(10);
    intQueue.push(20);
    intQueue.push(30);
    std::cout << "Integer Queue: ";
    intQueue.displayAll();
    intQueue.pop();
    std::cout << "After pop: ";
    intQueue.displayAll();

    // Тестирование шаблонной очереди со строками
    Queue<std::string> stringQueue;
    stringQueue.push("Hello");
    stringQueue.push("World");
    stringQueue.push("!");
    std::cout << "String Queue: ";
    stringQueue.displayAll();
    stringQueue.pop();
    std::cout << "After pop: ";
    stringQueue.displayAll();

    // Тестирование GameManager с игровыми объектами
    GameManager<Entity*> manager;
    manager.addEntity(new Player("Hero", 100, 1));
    manager.addEntity(new Enemy("Goblin", 50, "Goblin"));
    manager.addEntity(new Player("Mage", 80, 2));
    std::cout << "\nGame Entities:\n";
    manager.displayAll();

    return 0;
}