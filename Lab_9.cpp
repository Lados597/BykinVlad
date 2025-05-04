#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <typeinfo>
#include <ctime>
#include <sstream>

// Шаблонный класс Logger для записи логов
template<typename T>
class Logger {
private:
    std::ofstream logFile;
public:
    Logger(const std::string& filename) {
        logFile.open(filename, std::ios::app);
        if (!logFile.is_open()) {
            throw std::runtime_error("Unable to open log file");
        }
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void log(const T& message) {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        logFile << "[" << 1900 + ltm->tm_year << "-" 
                << 1 + ltm->tm_mon << "-" << ltm->tm_mday << " "
                << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "] "
                << message << std::endl;
    }
};

// Класс предмета в инвентаре
class Item {
protected:
    std::string name;
    std::string description;
public:
    Item(const std::string& n, const std::string& desc) : name(n), description(desc) {}
    virtual ~Item() {}

    virtual void use(class Character& character) = 0;
    virtual std::string getInfo() const {
        return name + ": " + description;
    }

    std::string getName() const { return name; }
};

// Оружие
class Weapon : public Item {
private:
    int attackBonus;
public:
    Weapon(const std::string& n, const std::string& desc, int atk) 
        : Item(n, desc), attackBonus(atk) {}

    void use(Character& character) override;
    std::string getInfo() const override {
        return Item::getInfo() + ", Attack +" + std::to_string(attackBonus);
    }
};

// Зелье здоровья
class HealthPotion : public Item {
private:
    int healAmount;
public:
    HealthPotion(const std::string& n, const std::string& desc, int heal) 
        : Item(n, desc), healAmount(heal) {}

    void use(Character& character) override;
    std::string getInfo() const override {
        return Item::getInfo() + ", Heals +" + std::to_string(healAmount) + " HP";
    }
};

// Класс инвентаря
class Inventory {
private:
    std::vector<std::unique_ptr<Item>> items;
    Logger<std::string> logger;
public:
    Inventory() : logger("inventory_log.txt") {}

    void addItem(std::unique_ptr<Item> item) {
        items.push_back(std::move(item));
        logger.log("Added item: " + items.back()->getName());
    }

    void removeItem(int index) {
        if (index >= 0 && index < items.size()) {
            logger.log("Removed item: " + items[index]->getName());
            items.erase(items.begin() + index);
        }
    }

    void useItem(int index, Character& character) {
        if (index >= 0 && index < items.size()) {
            items[index]->use(character);
            if (dynamic_cast<HealthPotion*>(items[index].get())) {
                removeItem(index); // Зелья удаляются после использования
            }
        }
    }

    void display() const {
        std::cout << "Inventory:\n";
        if (items.empty()) {
            std::cout << "Empty\n";
            return;
        }
        for (size_t i = 0; i < items.size(); ++i) {
            std::cout << i + 1 << ". " << items[i]->getInfo() << "\n";
        }
    }

    size_t size() const { return items.size(); }
};

// Базовый класс монстра
class Monster {
protected:
    std::string name;
    int health;
    int attack;
    int defense;
    Logger<std::string> logger;
public:
    Monster(const std::string& n, int h, int a, int d) 
        : name(n), health(h), attack(a), defense(d), logger("monster_log.txt") {}
    virtual ~Monster() {}

    virtual void attackTarget(class Character& target);
    virtual void takeDamage(int damage) {
        health -= damage;
        logger.log(name + " takes " + std::to_string(damage) + " damage. Remaining HP: " + std::to_string(health));
        if (health <= 0) {
            logger.log(name + " has been defeated!");
        }
    }

    bool isAlive() const { return health > 0; }
    virtual std::string getInfo() const {
        return name + " (HP: " + std::to_string(health) + 
               ", ATK: " + std::to_string(attack) + 
               ", DEF: " + std::to_string(defense) + ")";
    }

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
};

// Классы конкретных монстров
class Goblin : public Monster {
public:
    Goblin() : Monster("Goblin", 30, 8, 3) {}
};

class Dragon : public Monster {
public:
    Dragon() : Monster("Dragon", 100, 20, 15) {}
};

class Skeleton : public Monster {
private:
    bool isResurrected;
public:
    Skeleton() : Monster("Skeleton", 40, 10, 5), isResurrected(false) {}

    void takeDamage(int damage) override {
        if (!isResurrected && health - damage <= 0) {
            health = 30; // Воскрешение с 30 HP
            isResurrected = true;
            logger.log(name + " has resurrected with 30 HP!");
        } else {
            Monster::takeDamage(damage);
        }
    }

    std::string getInfo() const override {
        return Monster::getInfo() + (isResurrected ? " [Resurrected]" : "");
    }
};

// Класс персонажа
class Character {
private:
    std::string name;
    int health;
    int maxHealth;
    int attack;
    int defense;
    int level;
    int experience;
    Inventory inventory;
    Logger<std::string> logger;
public:
    Character(const std::string& n, int h, int a, int d) 
        : name(n), health(h), maxHealth(h), attack(a), defense(d), 
          level(1), experience(0), logger("character_log.txt") {
        logger.log("Character " + name + " created");
    }

    void attackEnemy(Monster& enemy) {
        int damage = attack - enemy.getDefense();
        if (damage > 0) {
            enemy.takeDamage(damage);
            logger.log(name + " attacks " + enemy.getName() + " for " + std::to_string(damage) + " damage!");
            std::cout << name << " attacks " << enemy.getName() << " for " << damage << " damage!" << std::endl;
            
            if (!enemy.isAlive()) {
                gainExperience(30);
            }
        } else {
            logger.log(name + " attacks " + enemy.getName() + ", but it has no effect!");
            std::cout << name << " attacks " << enemy.getName() << ", but it has no effect!" << std::endl;
        }
    }

    void takeDamage(int damage) {
        health -= damage;
        logger.log(name + " takes " + std::to_string(damage) + " damage. Remaining HP: " + std::to_string(health));
        if (health <= 0) {
            logger.log(name + " has been defeated!");
            throw std::runtime_error(name + " has been defeated!");
        }
    }

    void heal(int amount) {
        health += amount;
        if (health > maxHealth) health = maxHealth;
        logger.log(name + " heals for " + std::to_string(amount) + " HP. Current HP: " + std::to_string(health));
        std::cout << name << " heals for " << amount << " HP!" << std::endl;
    }

    void gainExperience(int exp) {
        experience += exp;
        logger.log(name + " gains " + std::to_string(exp) + " experience. Total: " + std::to_string(experience));
        if (experience >= 100) {
            levelUp();
        }
    }

    void levelUp() {
        level++;
        experience -= 100;
        maxHealth += 20;
        health = maxHealth;
        attack += 5;
        defense += 3;
        logger.log(name + " leveled up to level " + std::to_string(level) + "!");
        std::cout << name << " leveled up to level " << level << "!" << std::endl;
        std::cout << "Stats improved: HP +20, ATK +5, DEF +3" << std::endl;
    }

    void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health << "/" << maxHealth
                  << ", ATK: " << attack << ", DEF: " << defense
                  << ", Level: " << level << ", EXP: " << experience << "/100" << std::endl;
    }

    void addToInventory(std::unique_ptr<Item> item) {
        inventory.addItem(std::move(item));
    }

    void useItem(int index) {
        if (index >= 0 && index < inventory.size()) {
            inventory.useItem(index, *this);
        }
    }

    void showInventory() const {
        inventory.display();
    }

    void saveGame(const std::string& filename) {
        std::ofstream out(filename);
        if (!out) {
            throw std::runtime_error("Unable to save game");
        }
        
        out << name << "\n" << health << "\n" << maxHealth << "\n" 
            << attack << "\n" << defense << "\n" 
            << level << "\n" << experience << "\n";
        
        logger.log("Game saved for character " + name);
    }

    void loadGame(const std::string& filename) {
        std::ifstream in(filename);
        if (!in) {
            throw std::runtime_error("Unable to load game");
        }
        
        in >> name >> health >> maxHealth >> attack >> defense >> level >> experience;
        
        logger.log("Game loaded for character " + name);
    }

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getLevel() const { return level; }
    int getExperience() const { return experience; }
};

// Реализация методов использования предметов
void Weapon::use(Character& character) {
    // В этой реализации оружие не расходуется
    std::cout << name << " is already equipped and being used!" << std::endl;
}

void HealthPotion::use(Character& character) {
    character.heal(healAmount);
    std::cout << character.getName() << " used " << name << " and healed " 
              << healAmount << " HP!" << std::endl;
}

// Реализация метода атаки монстра
void Monster::attackTarget(Character& target) {
    int damage = attack - target.getDefense();
    if (damage > 0) {
        target.takeDamage(damage);
        logger.log(name + " attacks " + target.getName() + " for " + std::to_string(damage) + " damage!");
        std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!" << std::endl;
    } else {
        logger.log(name + " attacks " + target.getName() + ", but it has no effect!");
        std::cout << name << " attacks " << target.getName() << ", but it has no effect!" << std::endl;
    }
}

// Класс игры
class Game {
private:
    std::unique_ptr<Character> player;
    Logger<std::string> logger;
public:
    Game() : logger("game_log.txt") {
        logger.log("Game started");
    }

    void start() {
        std::cout << "Welcome to Text RPG Game!\n";
        std::string name;
        std::cout << "Enter your character name: ";
        std::getline(std::cin, name);
        
        player = std::make_unique<Character>(name, 100, 15, 10);
        
        // Добавляем начальные предметы
        player->addToInventory(std::make_unique<Weapon>("Iron Sword", "A basic iron sword", 10));
        player->addToInventory(std::make_unique<HealthPotion>("Small Health Potion", "Restores 30 HP", 30));
        
        logger.log("New game started with character " + name);
        
        mainMenu();
    }

    void mainMenu() {
        while (true) {
            std::cout << "\n=== Main Menu ===\n";
            std::cout << "1. Explore\n";
            std::cout << "2. Show character info\n";
            std::cout << "3. Show inventory\n";
            std::cout << "4. Save game\n";
            std::cout << "5. Load game\n";
            std::cout << "6. Exit\n";
            std::cout << "Choose an option: ";
            
            int choice;
            std::cin >> choice;
            std::cin.ignore();
            
            try {
                switch (choice) {
                    case 1: explore(); break;
                    case 2: player->displayInfo(); break;
                    case 3: 
                        player->showInventory(); 
                        if (player->getHealth() < player->getMaxHealth() && 
                            dynamic_cast<HealthPotion*>(&(*player->getInventory().getItem(0)))) {
                            std::cout << "Would you like to use a health potion? (y/n): ";
                            char use;
                            std::cin >> use;
                            if (use == 'y' || use == 'Y') {
                                player->useItem(0);
                            }
                        }
                        break;
                    case 4: 
                        player->saveGame("save.txt"); 
                        std::cout << "Game saved!\n";
                        break;
                    case 5: 
                        player->loadGame("save.txt"); 
                        std::cout << "Game loaded!\n";
                        break;
                    case 6: return;
                    default: std::cout << "Invalid choice!\n";
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << "\n";
                if (player->getHealth() <= 0) {
                    std::cout << "Game Over!\n";
                    return;
                }
            }
        }
    }

    void explore() {
        std::cout << "\nYou're exploring the area...\n";
        
        // Случайный выбор монстра
        int monsterType = rand() % 3;
        std::unique_ptr<Monster> monster;
        
        switch (monsterType) {
            case 0: monster = std::make_unique<Goblin>(); break;
            case 1: monster = std::make_unique<Dragon>(); break;
            case 2: monster = std::make_unique<Skeleton>(); break;
        }
        
        std::cout << "A wild " << monster->getName() << " appears!\n";
        std::cout << monster->getInfo() << "\n";
        
        battle(*monster);
    }

    void battle(Monster& monster) {
        logger.log("Battle started between " + player->getName() + " and " + monster.getName());
        
        while (player->getHealth() > 0 && monster.isAlive()) {
            std::cout << "\n=== Battle ===\n";
            player->displayInfo();
            std::cout << monster.getInfo() << "\n";
            
            std::cout << "1. Attack\n";
            std::cout << "2. Use item\n";
            std::cout << "3. Try to flee\n";
            std::cout << "Choose an action: ";
            
            int choice;
            std::cin >> choice;
            std::cin.ignore();
            
            try {
                switch (choice) {
                    case 1:
                        player->attackEnemy(monster);
                        if (monster.isAlive()) {
                            monster.attackTarget(*player);
                        }
                        break;
                    case 2:
                        player->showInventory();
                        if (player->getInventory().size() > 0) {
                            std::cout << "Enter item number to use (0 to cancel): ";
                            int itemChoice;
                            std::cin >> itemChoice;
                            std::cin.ignore();
                            if (itemChoice > 0 && itemChoice <= player->getInventory().size()) {
                                player->useItem(itemChoice - 1);
                                monster.attackTarget(*player);
                            }
                        } else {
                            std::cout << "Inventory is empty!\n";
                        }
                        break;
                    case 3:
                        if (rand() % 2 == 0) { // 50% шанс убежать
                            std::cout << "You successfully fled from battle!\n";
                            logger.log(player->getName() + " fled from battle");
                            return;
                        } else {
                            std::cout << "You failed to flee!\n";
                            monster.attackTarget(*player);
                        }
                        break;
                    default:
                        std::cout << "Invalid choice!\n";
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << "\n";
                if (player->getHealth() <= 0) {
                    std::cout << "Game Over!\n";
                    return;
                }
            }
        }
        
        if (player->getHealth() > 0) {
            std::cout << "You defeated the " << monster.getName() << "!\n";
            logger.log(player->getName() + " defeated " + monster.getName());
        }
    }
};

int main() {
    srand(time(0)); // Инициализация генератора случайных чисел
    
    try {
        Game game;
        game.start();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}