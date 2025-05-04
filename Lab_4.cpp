#include <iostream>
#include <memory>
#include <string>
#include <utility>

class Inventory {
private:
    std::unique_ptr<std::string[]> items; // Умный указатель на массив строк
    size_t capacity;                      // Емкость инвентаря
    size_t size;                          // Текущее количество предметов

    // Увеличиваем емкость инвентаря при необходимости
    void resize() {
        size_t newCapacity = capacity == 0 ? 2 : capacity * 2;
        std::unique_ptr<std::string[]> newItems(new std::string[newCapacity]);
        
        // Копируем существующие предметы
        for (size_t i = 0; i < size; ++i) {
            newItems[i] = std::move(items[i]);
        }
        
        items = std::move(newItems);
        capacity = newCapacity;
    }

public:
    // Конструктор
    Inventory() : items(nullptr), capacity(0), size(0) {}

    // Добавление предмета в инвентарь
    void addItem(const std::string& item) {
        if (size >= capacity) {
            resize();
        }
        items[size++] = item;
        std::cout << "Added item: " << item << std::endl;
    }

    // Отображение содержимого инвентаря
    void displayInventory() const {
        if (size == 0) {
            std::cout << "Inventory is empty." << std::endl;
            return;
        }
        
        std::cout << "Inventory contents:" << std::endl;
        for (size_t i = 0; i < size; ++i) {
            std::cout << "- " << items[i] << std::endl;
        }
    }
};

int main() {
    // Создаем инвентарь
    Inventory inventory;

    // Добавляем предметы
    inventory.addItem("Sword");
    inventory.addItem("Health Potion");
    inventory.addItem("Magic Ring");
    inventory.addItem("Leather Armor");

    // Отображаем инвентарь
    inventory.displayInventory();

    // Добавляем еще предметов (проверка увеличения емкости)
    inventory.addItem("Bow");
    inventory.addItem("Arrows");
    inventory.addItem("Mana Potion");

    // Снова отображаем инвентарь
    inventory.displayInventory();

    return 0;
}