#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

// Базовый класс Entity
class Entity {
protected:
    std::string name;
    int health;
    int level;

public:
    Entity(const std::string& name, int health, int level) 
        : name(name), health(health), level(level) {}

    virtual ~Entity() {}

    virtual void display() const {
        std::cout << "Name: " << name << ", Health: " << health 
                  << ", Level: " << level << std::endl;
    }

    // Методы для сериализации
    virtual std::string serialize() const {
        return name + "," + std::to_string(health) + "," + std::to_string(level);
    }

    virtual void deserialize(const std::string& data) {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1 + 1);
        
        if (pos1 == std::string::npos || pos2 == std::string::npos) {
            throw std::runtime_error("Invalid data format for Entity");
        }
        
        name = data.substr(0, pos1);
        health = std::stoi(data.substr(pos1 + 1, pos2 - pos1 - 1));
        level = std::stoi(data.substr(pos2 + 1));
    }

    // Геттеры
    const std::string& getName() const { return name; }
    int getHealth() const { return health; }
    int getLevel() const { return level; }
};

// Класс Player
class Player : public Entity {
private:
    int experience;

public:
    Player(const std::string& name, int health, int level, int exp = 0) 
        : Entity(name, health, level), experience(exp) {}

    void display() const override {
        std::cout << "Player: " << name << ", Health: " << health 
                  << ", Level: " << level << ", Exp: " << experience << std::endl;
    }

    std::string serialize() const override {
        return Entity::serialize() + "," + std::to_string(experience) + ",Player";
    }

    void deserialize(const std::string& data) override {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1 + 1);
        size_t pos3 = data.find(',', pos2 + 1);
        size_t pos4 = data.find(',', pos3 + 1);
        
        if (pos1 == std::string::npos || pos2 == std::string::npos || 
            pos3 == std::string::npos || pos4 == std::string::npos) {
            throw std::runtime_error("Invalid data format for Player");
        }
        
        Entity::deserialize(data.substr(0, pos3));
        experience = std::stoi(data.substr(pos3 + 1, pos4 - pos3 - 1));
    }
};

// Класс GameManager
template <typename T>
class GameManager {
private:
    std::vector<T> entities;

public:
    void addEntity(T entity) {
        entities.push_back(entity);
    }

    void displayAll() const {
        for (const auto& entity : entities) {
            entity->display();
        }
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Failed to open file for writing.");
        }

        for (const auto& entity : entities) {
            file << entity->serialize() << std::endl;
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Failed to open file for reading.");
        }

        // Очищаем текущие entities
        for (auto& entity : entities) {
            delete entity;
        }
        entities.clear();

        std::string line;
        while (std::getline(file, line)) {
            // Определяем тип сущности (последнее поле в строке)
            size_t lastComma = line.rfind(',');
            if (lastComma == std::string::npos) {
                throw std::runtime_error("Invalid data format in file");
            }

            std::string type = line.substr(lastComma + 1);
            Entity* entity = nullptr;

            if (type == "Player") {
                entity = new Player("", 0, 0, 0);
            } else {
                entity = new Entity("", 0, 0);
            }

            entity->deserialize(line);
            entities.push_back(entity);
        }
    }

    ~GameManager() {
        for (auto& entity : entities) {
            delete entity;
        }
    }
};

int main() {
    try {
        // Создаем менеджер и добавляем персонажей
        GameManager<Entity*> manager;
        manager.addEntity(new Player("Hero", 100, 1, 50));
        manager.addEntity(new Player("Mage", 80, 3, 150));
        manager.addEntity(new Entity("NPC", 50, 1));

        std::cout << "Original characters:" << std::endl;
        manager.displayAll();

        // Сохраняем в файл
        manager.saveToFile("game_save.txt");
        std::cout << "\nCharacters saved to file." << std::endl;

        // Загружаем из файла в новый менеджер
        GameManager<Entity*> loadedManager;
        loadedManager.loadFromFile("game_save.txt");
        std::cout << "\nLoaded characters:" << std::endl;
        loadedManager.displayAll();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}