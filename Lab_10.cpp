#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <typeinfo>

// Исключение для ошибок доступа
class AccessDeniedException : public std::runtime_error {
public:
    AccessDeniedException(const std::string& msg) : std::runtime_error(msg) {}
};

// Исключение для ошибок ввода данных
class InvalidInputException : public std::runtime_error {
public:
    InvalidInputException(const std::string& msg) : std::runtime_error(msg) {}
};

// Базовый класс пользователя
class User {
protected:
    std::string name;
    int id;
    int accessLevel; // 1 - студент, 2 - преподаватель, 3 - администратор

    void validate() const {
        if (name.empty()) {
            throw InvalidInputException("Имя пользователя не может быть пустым");
        }
        if (id <= 0) {
            throw InvalidInputException("ID пользователя должен быть положительным числом");
        }
        if (accessLevel < 1 || accessLevel > 3) {
            throw InvalidInputException("Уровень доступа должен быть от 1 до 3");
        }
    }

public:
    User(const std::string& n, int i, int al) : name(n), id(i), accessLevel(al) {
        validate();
    }

    virtual ~User() {}

    // Геттеры
    std::string getName() const { return name; }
    int getId() const { return id; }
    int getAccessLevel() const { return accessLevel; }

    // Сеттеры с проверкой
    void setName(const std::string& n) {
        if (n.empty()) throw InvalidInputException("Имя пользователя не может быть пустым");
        name = n;
    }

    void setId(int i) {
        if (i <= 0) throw InvalidInputException("ID пользователя должен быть положительным числом");
        id = i;
    }

    void setAccessLevel(int al) {
        if (al < 1 || al > 3) throw InvalidInputException("Уровень доступа должен быть от 1 до 3");
        accessLevel = al;
    }

    // Виртуальный метод для вывода информации
    virtual void displayInfo() const {
        std::cout << "ID: " << id << ", Имя: " << name 
                  << ", Уровень доступа: " << accessLevel;
    }

    // Виртуальный метод для сохранения в файл
    virtual void saveToFile(std::ofstream& out) const {
        out << typeid(*this).name() << "\n" << name << "\n" << id << "\n" << accessLevel << "\n";
    }

    // Виртуальный метод для загрузки из файла
    virtual void loadFromFile(std::ifstream& in) {
        std::getline(in, name);
        in >> id >> accessLevel;
        in.ignore();
        validate();
    }
};

// Класс студента
class Student : public User {
private:
    std::string group;

public:
    Student(const std::string& n, int i, const std::string& g) 
        : User(n, i, 1), group(g) {
        if (group.empty()) throw InvalidInputException("Группа не может быть пустой");
    }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << ", Тип: Студент, Группа: " << group << std::endl;
    }

    std::string getGroup() const { return group; }
    void setGroup(const std::string& g) {
        if (g.empty()) throw InvalidInputException("Группа не может быть пустой");
        group = g;
    }

    void saveToFile(std::ofstream& out) const override {
        User::saveToFile(out);
        out << group << "\n";
    }

    void loadFromFile(std::ifstream& in) override {
        User::loadFromFile(in);
        std::getline(in, group);
        if (group.empty()) throw InvalidInputException("Группа не может быть пустой");
    }
};

// Класс преподавателя
class Teacher : public User {
private:
    std::string department;

public:
    Teacher(const std::string& n, int i, const std::string& d) 
        : User(n, i, 2), department(d) {
        if (department.empty()) throw InvalidInputException("Кафедра не может быть пустой");
    }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << ", Тип: Преподаватель, Кафедра: " << department << std::endl;
    }

    std::string getDepartment() const { return department; }
    void setDepartment(const std::string& d) {
        if (d.empty()) throw InvalidInputException("Кафедра не может быть пустой");
        department = d;
    }

    void saveToFile(std::ofstream& out) const override {
        User::saveToFile(out);
        out << department << "\n";
    }

    void loadFromFile(std::ifstream& in) override {
        User::loadFromFile(in);
        std::getline(in, department);
        if (department.empty()) throw InvalidInputException("Кафедра не может быть пустой");
    }
};

// Класс администратора
class Administrator : public User {
private:
    std::string position;

public:
    Administrator(const std::string& n, int i, const std::string& p) 
        : User(n, i, 3), position(p) {
        if (position.empty()) throw InvalidInputException("Должность не может быть пустой");
    }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << ", Тип: Администратор, Должность: " << position << std::endl;
    }

    std::string getPosition() const { return position; }
    void setPosition(const std::string& p) {
        if (p.empty()) throw InvalidInputException("Должность не может быть пустой");
        position = p;
    }

    void saveToFile(std::ofstream& out) const override {
        User::saveToFile(out);
        out << position << "\n";
    }

    void loadFromFile(std::ifstream& in) override {
        User::loadFromFile(in);
        std::getline(in, position);
        if (position.empty()) throw InvalidInputException("Должность не может быть пустой");
    }
};

// Класс ресурса университета
class Resource {
private:
    std::string name;
    int requiredAccessLevel;

public:
    Resource(const std::string& n, int ral) : name(n), requiredAccessLevel(ral) {
        if (name.empty()) throw InvalidInputException("Название ресурса не может быть пустым");
        if (ral < 1 || ral > 3) throw InvalidInputException("Требуемый уровень доступа должен быть от 1 до 3");
    }

    std::string getName() const { return name; }
    int getRequiredAccessLevel() const { return requiredAccessLevel; }

    void setName(const std::string& n) {
        if (n.empty()) throw InvalidInputException("Название ресурса не может быть пустым");
        name = n;
    }

    void setRequiredAccessLevel(int ral) {
        if (ral < 1 || ral > 3) throw InvalidInputException("Требуемый уровень доступа должен быть от 1 до 3");
        requiredAccessLevel = ral;
    }

    void displayInfo() const {
        std::cout << "Ресурс: " << name << ", Требуемый уровень доступа: " 
                  << requiredAccessLevel << std::endl;
    }

    bool checkAccess(const User& user) const {
        return user.getAccessLevel() >= requiredAccessLevel;
    }

    void saveToFile(std::ofstream& out) const {
        out << name << "\n" << requiredAccessLevel << "\n";
    }

    void loadFromFile(std::ifstream& in) {
        std::getline(in, name);
        in >> requiredAccessLevel;
        in.ignore();
        if (name.empty()) throw InvalidInputException("Название ресурса не может быть пустым");
        if (requiredAccessLevel < 1 || requiredAccessLevel > 3) 
            throw InvalidInputException("Требуемый уровень доступа должен быть от 1 до 3");
    }
};

// Шаблонный класс системы контроля доступа
template<typename T>
class AccessControlSystem {
private:
    std::vector<std::unique_ptr<User>> users;
    std::vector<T> resources;

public:
    void addUser(std::unique_ptr<User> user) {
        users.push_back(std::move(user));
    }

    void addResource(const T& resource) {
        resources.push_back(resource);
    }

    void displayAllUsers() const {
        for (const auto& user : users) {
            user->displayInfo();
        }
    }

    void displayAllResources() const {
        for (const auto& resource : resources) {
            resource.displayInfo();
        }
    }

    bool checkAccess(int userId, const std::string& resourceName) const {
        auto userIt = std::find_if(users.begin(), users.end(), 
            [userId](const std::unique_ptr<User>& u) { return u->getId() == userId; });
        
        auto resourceIt = std::find_if(resources.begin(), resources.end(), 
            [resourceName](const T& r) { return r.getName() == resourceName; });

        if (userIt == users.end()) {
            throw std::runtime_error("Пользователь с ID " + std::to_string(userId) + " не найден");
        }

        if (resourceIt == resources.end()) {
            throw std::runtime_error("Ресурс с именем " + resourceName + " не найден");
        }

        if (!resourceIt->checkAccess(**userIt)) {
            throw AccessDeniedException("Доступ запрещен для пользователя " + (*userIt)->getName() + 
                                       " к ресурсу " + resourceName);
        }

        return true;
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream out(filename);
        if (!out) throw std::runtime_error("Не удалось открыть файл для записи");

        // Сохраняем пользователей
        out << users.size() << "\n";
        for (const auto& user : users) {
            user->saveToFile(out);
        }

        // Сохраняем ресурсы
        out << resources.size() << "\n";
        for (const auto& resource : resources) {
            resource.saveToFile(out);
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream in(filename);
        if (!in) throw std::runtime_error("Не удалось открыть файл для чтения");

        users.clear();
        resources.clear();

        // Загружаем пользователей
        int userCount;
        in >> userCount;
        in.ignore();

        for (int i = 0; i < userCount; ++i) {
            std::string type;
            std::getline(in, type);

            std::unique_ptr<User> user;
            if (type == typeid(Student).name()) {
                user = std::make_unique<Student>("", 0, "");
            } else if (type == typeid(Teacher).name()) {
                user = std::make_unique<Teacher>("", 0, "");
            } else if (type == typeid(Administrator).name()) {
                user = std::make_unique<Administrator>("", 0, "");
            } else {
                throw std::runtime_error("Неизвестный тип пользователя в файле");
            }

            user->loadFromFile(in);
            users.push_back(std::move(user));
        }

        // Загружаем ресурсы
        int resourceCount;
        in >> resourceCount;
        in.ignore();

        for (int i = 0; i < resourceCount; ++i) {
            T resource("", 0);
            resource.loadFromFile(in);
            resources.push_back(resource);
        }
    }

    void findUserByName(const std::string& name) const {
        bool found = false;
        for (const auto& user : users) {
            if (user->getName() == name) {
                user->displayInfo();
                found = true;
            }
        }
        if (!found) {
            std::cout << "Пользователь с именем " << name << " не найден" << std::endl;
        }
    }

    void findUserById(int id) const {
        bool found = false;
        for (const auto& user : users) {
            if (user->getId() == id) {
                user->displayInfo();
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "Пользователь с ID " << id << " не найден" << std::endl;
        }
    }

    void sortUsersByAccessLevel() {
        std::sort(users.begin(), users.end(), 
            [](const std::unique_ptr<User>& a, const std::unique_ptr<User>& b) {
                return a->getAccessLevel() < b->getAccessLevel();
            });
    }

    void sortUsersByName() {
        std::sort(users.begin(), users.end(), 
            [](const std::unique_ptr<User>& a, const std::unique_ptr<User>& b) {
                return a->getName() < b->getName();
            });
    }
};

// Функция для создания пользователя
std::unique_ptr<User> createUser() {
    std::cout << "Выберите тип пользователя:\n";
    std::cout << "1. Студент\n";
    std::cout << "2. Преподаватель\n";
    std::cout << "3. Администратор\n";
    std::cout << "Введите номер: ";

    int type;
    std::cin >> type;
    std::cin.ignore();

    std::string name;
    std::cout << "Введите имя: ";
    std::getline(std::cin, name);

    int id;
    std::cout << "Введите ID: ";
    std::cin >> id;
    std::cin.ignore();

    try {
        switch (type) {
            case 1: {
                std::string group;
                std::cout << "Введите группу: ";
                std::getline(std::cin, group);
                return std::make_unique<Student>(name, id, group);
            }
            case 2: {
                std::string department;
                std::cout << "Введите кафедру: ";
                std::getline(std::cin, department);
                return std::make_unique<Teacher>(name, id, department);
            }
            case 3: {
                std::string position;
                std::cout << "Введите должность: ";
                std::getline(std::cin, position);
                return std::make_unique<Administrator>(name, id, position);
            }
            default:
                throw InvalidInputException("Неверный тип пользователя");
        }
    } catch (const InvalidInputException& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return nullptr;
    }
}

// Функция для создания ресурса
Resource createResource() {
    std::string name;
    std::cout << "Введите название ресурса: ";
    std::getline(std::cin, name);

    int level;
    std::cout << "Введите требуемый уровень доступа (1-3): ";
    std::cin >> level;
    std::cin.ignore();

    try {
        return Resource(name, level);
    } catch (const InvalidInputException& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        throw;
    }
}

// Главное меню
void mainMenu(AccessControlSystem<Resource>& system) {
    while (true) {
        std::cout << "\n=== Система контроля доступа университета ===\n";
        std::cout << "1. Добавить пользователя\n";
        std::cout << "2. Добавить ресурс\n";
        std::cout << "3. Показать всех пользователей\n";
        std::cout << "4. Показать все ресурсы\n";
        std::cout << "5. Проверить доступ\n";
        std::cout << "6. Найти пользователя по имени\n";
        std::cout << "7. Найти пользователя по ID\n";
        std::cout << "8. Сортировать пользователей по уровню доступа\n";
        std::cout << "9. Сортировать пользователей по имени\n";
        std::cout << "10. Сохранить данные в файл\n";
        std::cout << "11. Загрузить данные из файла\n";
        std::cout << "0. Выход\n";
        std::cout << "Выберите действие: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        try {
            switch (choice) {
                case 1: {
                    auto user = createUser();
                    if (user) {
                        system.addUser(std::move(user));
                        std::cout << "Пользователь добавлен\n";
                    }
                    break;
                }
                case 2: {
                    try {
                        Resource resource = createResource();
                        system.addResource(resource);
                        std::cout << "Ресурс добавлен\n";
                    } catch (...) {
                        // Ошибка уже обработана в createResource
                    }
                    break;
                }
                case 3:
                    system.displayAllUsers();
                    break;
                case 4:
                    system.displayAllResources();
                    break;
                case 5: {
                    int userId;
                    std::cout << "Введите ID пользователя: ";
                    std::cin >> userId;
                    std::cin.ignore();

                    std::string resourceName;
                    std::cout << "Введите название ресурса: ";
                    std::getline(std::cin, resourceName);

                    try {
                        if (system.checkAccess(userId, resourceName)) {
                            std::cout << "Доступ разрешен\n";
                        }
                    } catch (const AccessDeniedException& e) {
                        std::cerr << "Ошибка доступа: " << e.what() << std::endl;
                    } catch (const std::runtime_error& e) {
                        std::cerr << "Ошибка: " << e.what() << std::endl;
                    }
                    break;
                }
                case 6: {
                    std::string name;
                    std::cout << "Введите имя для поиска: ";
                    std::getline(std::cin, name);
                    system.findUserByName(name);
                    break;
                }
                case 7: {
                    int id;
                    std::cout << "Введите ID для поиска: ";
                    std::cin >> id;
                    std::cin.ignore();
                    system.findUserById(id);
                    break;
                }
                case 8:
                    system.sortUsersByAccessLevel();
                    std::cout << "Пользователи отсортированы по уровню доступа\n";
                    break;
                case 9:
                    system.sortUsersByName();
                    std::cout << "Пользователи отсортированы по имени\n";
                    break;
                case 10: {
                    std::string filename;
                    std::cout << "Введите имя файла для сохранения: ";
                    std::getline(std::cin, filename);
                    system.saveToFile(filename);
                    std::cout << "Данные сохранены в файл " << filename << std::endl;
                    break;
                }
                case 11: {
                    std::string filename;
                    std::cout << "Введите имя файла для загрузки: ";
                    std::getline(std::cin, filename);
                    system.loadFromFile(filename);
                    std::cout << "Данные загружены из файла " << filename << std::endl;
                    break;
                }
                case 0:
                    return;
                default:
                    std::cout << "Неверный выбор\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
    }
}

int main() {
    AccessControlSystem<Resource> system;

    // Добавим несколько тестовых данных
    try {
        system.addUser(std::make_unique<Student>("Иванов Иван", 1, "ИТ-101"));
        system.addUser(std::make_unique<Teacher>("Петров Петр", 2, "Компьютерные науки"));
        system.addUser(std::make_unique<Administrator>("Сидорова Мария", 3, "Начальник отдела"));

        system.addResource(Resource("Лекционная аудитория 101", 1));
        system.addResource(Resource("Компьютерная лаборатория", 2));
        system.addResource(Resource("Серверная", 3));
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при инициализации тестовых данных: " << e.what() << std::endl;
    }

    mainMenu(system);

    return 0;
}