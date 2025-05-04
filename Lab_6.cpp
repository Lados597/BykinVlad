#include <iostream>
#include <vector>
#include <stdexcept>
#include <queue>

using namespace std;

// Базовый класс Entity для примера
class Entity {
public:
    Entity(const string& name, int health) : name(name), health(health) {}
    virtual ~Entity() {}
    
    int getHealth() const { return health; }
    string getName() const { return name; }
    
private:
    string name;
    int health;
};

// Класс Player для примера
class Player : public Entity {
public:
    Player(const string& name, int health, int level) 
        : Entity(name, health), level(level) {}
    
    int getLevel() const { return level; }
    
private:
    int level;
};

// Класс GameManager с обработкой исключений
template <typename T>
class GameManager {
public:
    void addEntity(const T& entity) {
        if (entity->getHealth() <= 0) {
            throw invalid_argument("Entity has invalid health");
        }
        entities.push_back(entity);
    }
    
private:
    vector<T> entities;
};

// Класс Queue с обработкой исключений
template <typename T>
class Queue {
public:
    void push(const T& value) {
        data.push(value);
    }
    
    T pop() {
        if (data.empty()) {
            throw runtime_error("Cannot pop from empty queue");
        }
        T value = data.front();
        data.pop();
        return value;
    }
    
    bool empty() const {
        return data.empty();
    }
    
private:
    queue<T> data;
};

int main() {
    // Тестирование GameManager
    try {
        GameManager<Entity*> manager;
        manager.addEntity(new Player("Hero", -100, 0)); // Вызовет исключение
    } catch (const invalid_argument& e) {
        cerr << "GameManager error: " << e.what() << endl;
    }
    
    // Тестирование Queue
    try {
        Queue<int> intQueue;
        intQueue.push(10);
        cout << "Popped: " << intQueue.pop() << endl;
        cout << "Popped: " << intQueue.pop() << endl; // Вызовет исключение
    } catch (const runtime_error& e) {
        cerr << "Queue error: " << e.what() << endl;
    }
    
    // Тест с корректными данными
    try {
        GameManager<Entity*> manager;
        manager.addEntity(new Player("Valid Hero", 100, 1));
        cout << "Entity added successfully" << endl;
        
        Queue<string> stringQueue;
        stringQueue.push("Hello");
        cout << "Popped: " << stringQueue.pop() << endl;
    } catch (const exception& e) {
        cerr << "Unexpected error: " << e.what() << endl;
    }

    return 0;
}