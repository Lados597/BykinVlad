#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <random>
#include <string>

using namespace std;

// Класс существа (базовый класс для персонажа и монстров)
class Creature {
protected:
    string name;
    int health;
    int maxHealth;
    int attack;
    int defense;
    
public:
    Creature(string n, int h, int a, int d) 
        : name(n), health(h), maxHealth(h), attack(a), defense(d) {}
    
    virtual ~Creature() = default;
    
    void takeDamage(int damage) {
        int actualDamage = max(1, damage - defense);
        health -= actualDamage;
        if (health < 0) health = 0;
    }
    
    bool isAlive() const {
        return health > 0;
    }
    
    int getAttack() const {
        return attack;
    }
    
    string getName() const {
        return name;
    }
    
    int getHealth() const {
        return health;
    }
    
    int getMaxHealth() const {
        return maxHealth;
    }
    
    virtual void displayInfo() const {
        cout << name << " (HP: " << health << "/" << maxHealth 
             << ", ATK: " << attack << ", DEF: " << defense << ")";
    }
    
    void heal(int amount) {
        health = min(maxHealth, health + amount);
    }
};

// Класс персонажа
class Character : public Creature {
public:
    Character(string n, int h, int a, int d) : Creature(n, h, a, d) {}
    
    void displayInfo() const override {
        cout << "=== Ваш персонаж ===" << endl;
        Creature::displayInfo();
        cout << endl;
    }
};

// Класс монстра
class Monster : public Creature {
public:
    Monster(string n, int h, int a, int d) : Creature(n, h, a, d) {}
    
    static Monster generateRandomMonster() {
        static vector<string> names = {"Гоблин", "Орк", "Скелет", "Зомби", "Паук", "Волк"};
        static random_device rd;
        static mt19937 gen(rd());
        uniform_int_distribution<> nameDist(0, names.size() - 1);
        uniform_int_distribution<> healthDist(30, 80);
        uniform_int_distribution<> attackDist(5, 25);
        uniform_int_distribution<> defenseDist(0, 15);
        
        string name = names[nameDist(gen)];
        int health = healthDist(gen);
        int attack = attackDist(gen);
        int defense = defenseDist(gen);
        
        return Monster(name, health, attack, defense);
    }
    
    void displayInfo() const override {
        Creature::displayInfo();
        cout << endl;
    }
};

// Глобальные переменные
vector<Monster> monsters;
mutex monstersMutex;
bool gameRunning = true;
mutex gameMutex;

// Функция генерации монстров
void generateMonsters() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> timeDist(2, 5);
    
    while (true) {
        {
            lock_guard<mutex> gameLock(gameMutex);
            if (!gameRunning) break;
        }
        
        int sleepTime = timeDist(gen);
        this_thread::sleep_for(chrono::seconds(sleepTime));
        
        {
            lock_guard<mutex> lock(monstersMutex);
            monsters.push_back(Monster::generateRandomMonster());
            cout << "\nПоявился новый монстр: ";
            monsters.back().displayInfo();
        }
    }
}

// Функция боя между персонажем и монстром
void battle(Character& hero, Monster& monster) {
    cout << "\n=== НАЧАЛО БОЯ ===" << endl;
    cout << hero.getName() << " vs " << monster.getName() << endl;
    
    while (hero.isAlive() && monster.isAlive()) {
        // Персонаж атакует
        cout << hero.getName() << " атакует " << monster.getName() << "!" << endl;
        monster.takeDamage(hero.getAttack());
        monster.displayInfo();
        if (!monster.isAlive()) break;
        
        // Монстр атакует
        cout << monster.getName() << " атакует " << hero.getName() << "!" << endl;
        hero.takeDamage(monster.getAttack());
        hero.displayInfo();
        if (!hero.isAlive()) break;
        
        this_thread::sleep_for(chrono::seconds(1));
    }
    
    if (hero.isAlive()) {
        cout << hero.getName() << " победил " << monster.getName() << "!" << endl;
        // Лечение после боя
        hero.heal(hero.getMaxHealth() / 4);
        cout << hero.getName() << " восстановил немного здоровья." << endl;
        hero.displayInfo();
    } else {
        cout << hero.getName() << " был побежден " << monster.getName() << "!" << endl;
        {
            lock_guard<mutex> gameLock(gameMutex);
            gameRunning = false;
        }
    }
}

// Основной игровой цикл
void gameLoop(Character& hero) {
    while (true) {
        {
            lock_guard<mutex> gameLock(gameMutex);
            if (!gameRunning) break;
        }
        
        // Показать меню
        cout << "\n=== МЕНЮ ===" << endl;
        cout << "1. Показать информацию о персонаже" << endl;
        cout << "2. Показать список монстров" << endl;
        cout << "3. Атаковать монстра" << endl;
        cout << "4. Выйти из игры" << endl;
        cout << "Выберите действие: ";
        
        int choice;
        cin >> choice;
        
        if (choice == 1) {
            hero.displayInfo();
        } else if (choice == 2) {
            lock_guard<mutex> lock(monstersMutex);
            if (monsters.empty()) {
                cout << "Монстров нет поблизости." << endl;
            } else {
                cout << "\n=== СПИСОК МОНСТРОВ ===" << endl;
                for (size_t i = 0; i < monsters.size(); ++i) {
                    cout << i + 1 << ". ";
                    monsters[i].displayInfo();
                }
            }
        } else if (choice == 3) {
            lock_guard<mutex> lock(monstersMutex);
            if (monsters.empty()) {
                cout << "Нет монстров для атаки." << endl;
            } else {
                cout << "Выберите монстра для атаки (1-" << monsters.size() << "): ";
                int monsterChoice;
                cin >> monsterChoice;
                
                if (monsterChoice >= 1 && monsterChoice <= static_cast<int>(monsters.size())) {
                    Monster& selectedMonster = monsters[monsterChoice - 1];
                    battle(hero, selectedMonster);
                    
                    // Удаляем побежденного монстра
                    monsters.erase(monsters.begin() + monsterChoice - 1);
                } else {
                    cout << "Неверный выбор." << endl;
                }
            }
        } else if (choice == 4) {
            lock_guard<mutex> gameLock(gameMutex);
            gameRunning = false;
            break;
        } else {
            cout << "Неверный выбор." << endl;
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    
    // Создаем персонажа
    Character hero("Герой", 100, 20, 10);
    
    // Запускаем генератор монстров в отдельном потоке
    thread monsterGenerator(generateMonsters);
    
    // Запускаем игровой цикл
    gameLoop(hero);
    
    // Ожидаем завершения потока генератора монстров
    monsterGenerator.join();
    
    cout << "\nИгра завершена. ";
    if (hero.isAlive()) {
        cout << "Вы выжили!";
    } else {
        cout << "Вы погибли...";
    }
    cout << endl;
    
    return 0;
}