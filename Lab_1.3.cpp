#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <cstdlib>

class Entity {
protected:
    std::string name;
    int health;
    int attackPower;
    int defense;

public:
    Entity(const std::string& n, int h, int a, int d)
        : name(n), health(h), attackPower(a), defense(d) {
    }

    virtual ~Entity() {}

    // Виртуальный метод атаки
    virtual void attack(Entity& target) {
        int damage = attackPower - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName()
                << " for " << damage << " damage!" << std::endl;
        }
        else {
            std::cout << name << "'s attack has no effect on "
                << target.getName() << "!" << std::endl;
        }
    }

    // Виртуальный метод лечения
    virtual void heal(int amount) {
        health += amount;
        std::cout << name << " heals for " << amount
            << " HP. Current HP: " << health << std::endl;
    }

    // Виртуальный метод отображения информации
    virtual void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health
            << ", Attack: " << attackPower
            << ", Defense: " << defense;
    }

    // Методы для доступа к protected полям
    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getAttackPower() const { return attackPower; }
    int getDefense() const { return defense; }
    void takeDamage(int damage) { health -= damage; }
};

class Character : public Entity {
private:
    int criticalChance; // Шанс критического удара (%)

public:
    Character(const std::string& n, int h, int a, int d, int crit)
        : Entity(n, h, a, d), criticalChance(crit) {
    }

    // Переопределение метода атаки с возможностью критического удара
    void attack(Entity& target) override {
        bool isCritical = (rand() % 100) < criticalChance;
        int damage = attackPower - target.getDefense();

        if (isCritical) {
            damage *= 2;
            std::cout << "CRITICAL HIT! ";
        }

        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName()
                << " for " << damage << " damage!" << std::endl;
        }
        else {
            std::cout << name << "'s attack has no effect on "
                << target.getName() << "!" << std::endl;
        }
    }

    // Переопределение метода лечения
    void heal(int amount) override {
        int maxHealth = health + amount > 100 ? 100 : health + amount;
        std::cout << name << " uses healing potion! ";
        Entity::heal(maxHealth - health);
    }

    // Переопределение метода отображения информации
    void displayInfo() const override {
        Entity::displayInfo();
        std::cout << ", Critical: " << criticalChance << "%" << std::endl;
    }
};

class Monster : public Entity {
private:
    std::string monsterType;

public:
    Monster(const std::string& n, int h, int a, int d, const std::string& t)
        : Entity(n, h, a, d), monsterType(t) {
    }

    // Переопределение метода атаки
    void attack(Entity& target) override {
        std::cout << monsterType << " ";
        Entity::attack(target);
    }

    // Переопределение метода отображения информации
    void displayInfo() const override {
        Entity::displayInfo();
        std::cout << ", Type: " << monsterType << std::endl;
    }
};

class Boss : public Monster {
private:
    std::string specialAbility;

public:
    Boss(const std::string& n, int h, int a, int d,
        const std::string& t, const std::string& ability)
        : Monster(n, h, a, d, t), specialAbility(ability) {
    }

    // Переопределение метода атаки с использованием способности
    void attack(Entity& target) override {
        std::cout << name << " uses " << specialAbility << "! ";
        int damage = (attackPower * 2) - target.getDefense();

        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName()
                << " for " << damage << " damage!" << std::endl;
        }
        else {
            std::cout << name << "'s attack has no effect on "
                << target.getName() << "!" << std::endl;
        }
    }

    // Переопределение метода отображения информации
    void displayInfo() const override {
        Monster::displayInfo();
        std::cout << "Special Ability: " << specialAbility << std::endl;
    }
};

int main() {
    srand(time(0)); // Инициализация генератора случайных чисел

    // Создаем объекты разных типов
    Character hero("Hero", 100, 25, 10, 20); // 20% шанс крита
    Monster goblin("Goblin", 50, 15, 5, "Goblin");
    Boss dragon("Dragon", 200, 30, 20, "Ancient Dragon", "Fire Breath");

    // Создаем массив указателей на базовый класс
    std::vector<std::unique_ptr<Entity>> entities;
    entities.push_back(std::make_unique<Character>(hero));
    entities.push_back(std::make_unique<Monster>(goblin));
    entities.push_back(std::make_unique<Boss>(dragon));

    // Демонстрация полиморфизма
    std::cout << "=== Initial Status ===" << std::endl;
    for (const auto& entity : entities) {
        entity->displayInfo();
    }

    std::cout << "\n=== Combat ===" << std::endl;
    // Герой атакует голбина
    entities[0]->attack(*entities[1]);
    // Гоблин атакует героя
    entities[1]->attack(*entities[0]);
    // Дракон атакует героя
    entities[2]->attack(*entities[0]);

    std::cout << "\n=== After Combat ===" << std::endl;
    for (const auto& entity : entities) {
        entity->displayInfo();
    }

    std::cout << "\n=== Healing ===" << std::endl;
    // Герой лечится
    entities[0]->heal(30);

    std::cout << "\n=== Final Status ===" << std::endl;
    for (const auto& entity : entities) {
        entity->displayInfo();
    }

    return 0;
}