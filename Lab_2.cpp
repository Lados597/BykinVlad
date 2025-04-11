#include <iostream>
#include <string>
#include <vector>

class Weapon {
private:
    std::string name;
    int damage;
    float weight;

public:
    // Конструктор
    Weapon(const std::string& n, int d, float w)
        : name(n), damage(d), weight(w) {
        std::cout << "Weapon " << name << " created!\n";
    }

    // Деструктор
    ~Weapon() {
        std::cout << "Weapon " << name << " destroyed!\n";
    }

    // Метод для отображения информации
    void displayInfo() const {
        std::cout << "Weapon: " << name << ", Damage: " << damage
            << ", Weight: " << weight << " kg\n";
    }
};

class Character {
private:
    std::string name;
    int health;
    int attack;
    int defense;
    Weapon* weapon; // Указатель на оружие

public:
    // Конструктор
    Character(const std::string& n, int h, int a, int d, Weapon* w = nullptr)
        : name(n), health(h), attack(a), defense(d), weapon(w) {
        std::cout << "Character " << name << " created!\n";
    }

    // Деструктор
    ~Character() {
        std::cout << "Character " << name << " destroyed!\n";
        if (weapon) {
            delete weapon;
        }
    }

    // Метод для установки оружия
    void equipWeapon(Weapon* w) {
        if (weapon) {
            delete weapon;
        }
        weapon = w;
    }

    void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense;
        if (weapon) {
            std::cout << "\n  ";
            weapon->displayInfo();
        }
        else {
            std::cout << ", No weapon equipped\n";
        }
    }
};

class Monster {
private:
    std::string name;
    int health;
    int attack;
    int defense;
    std::string type;

public:
    // Конструктор
    Monster(const std::string& n, int h, int a, int d, const std::string& t)
        : name(n), health(h), attack(a), defense(d), type(t) {
        std::cout << "Monster " << name << " (" << type << ") created!\n";
    }

    // Деструктор
    ~Monster() {
        std::cout << "Monster " << name << " (" << type << ") destroyed!\n";
    }

    void displayInfo() const {
        std::cout << "Name: " << name << ", Type: " << type
            << ", HP: " << health << ", Attack: " << attack
            << ", Defense: " << defense << std::endl;
    }
};

int main() {
    // Создаем оружие
    std::cout << "=== Creating weapons ===\n";
    Weapon* sword = new Weapon("Steel Sword", 15, 3.2);
    Weapon* axe = new Weapon("Battle Axe", 20, 5.1);
    Weapon* bow = new Weapon("Long Bow", 12, 1.8);

    // Демонстрация работы оружия
    std::cout << "\n=== Weapons info ===\n";
    sword->displayInfo();
    axe->displayInfo();
    bow->displayInfo();

    // Создаем персонажей и монстров
    std::cout << "\n=== Creating characters ===\n";
    Character hero("Hero", 100, 25, 10, sword);
    Character archer("Archer", 80, 20, 8, bow);

    std::cout << "\n=== Creating monsters ===\n";
    Monster goblin("Goblin", 50, 15, 5, "Goblin");
    Monster orc("Orc", 80, 25, 12, "Orc");

    // Отображаем информацию
    std::cout << "\n=== Characters info ===\n";
    hero.displayInfo();
    archer.displayInfo();

    std::cout << "\n=== Monsters info ===\n";
    goblin.displayInfo();
    orc.displayInfo();

    // Меняем оружие
    std::cout << "\n=== Changing weapon ===\n";
    archer.equipWeapon(axe);
    archer.displayInfo();

    // Объекты будут автоматически уничтожены при выходе из области видимости
    std::cout << "\n=== End of scope - objects will be destroyed ===\n";

    return 0;
}