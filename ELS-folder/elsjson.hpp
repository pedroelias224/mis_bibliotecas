#ifndef ELSJSON_HPP
#define ELSJSON_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <variant>
#include <memory>
#include <sstream>
#include <iomanip>

namespace json {

    class ELS {
    public:
        using ELSPtr = std::shared_ptr<ELS>;
        std::map<std::string, std::variant<int, std::string, ELSPtr>> data;

        // Constructor vacío
        ELS() {}

        // Proxy para operador ()
        class KeyProxy {
            std::string key;
            ELS& parent;
        public:
            KeyProxy(const std::string& k, ELS& p) : key(k), parent(p) {}

            void operator>>(int value) { parent.data[key] = value; }
            void operator>>(const std::string& value) { parent.data[key] = value; }
            void operator>>(ELS value) { parent.data[key] = std::make_shared<ELS>(value); }
        };

        KeyProxy operator()(const std::string& key) {
            return KeyProxy(key, *this);
        }

        // Dump a consola o stream
        void dump(int indent = 0) const {
            dump_to_stream(std::cout, indent);
            if (indent == 0) std::cout << "\n";
        }

        void dump_to_stream(std::ostream& os, int indent = 0) const {
            std::string padding(indent, ' ');
            os << "{\n";
            for (auto it = data.begin(); it != data.end(); ++it) {
                os << padding << "  \"" << it->first << "\": ";
                if (std::holds_alternative<int>(it->second))
                    os << std::get<int>(it->second);
                else if (std::holds_alternative<std::string>(it->second))
                    os << "\"" << std::get<std::string>(it->second) << "\"";
                else if (std::holds_alternative<ELSPtr>(it->second))
                    std::get<ELSPtr>(it->second)->dump_to_stream(os, indent + 2);
                if (std::next(it) != data.end()) os << ",";
                os << "\n";
            }
            os << padding << "}";
        }

        // Guardar en archivo
        void save(const std::string& filename) const {
            std::ofstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error al abrir archivo para guardar: " << filename << "\n";
                return;
            }
            std::ostringstream oss;
            dump_to_stream(oss, 0);
            file << oss.str();
            file.close();
        }

        // Leer archivo y mostrar contenido
        void read_and_validate(const std::string& filename) {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error al abrir archivo: " << filename << "\n";
                return;
            }
            std::ostringstream oss;
            oss << file.rdbuf();
            std::string content = oss.str();
            file.close();

            // Mostrar contenido
            std::cout << "Contenido del archivo:\n" << content << "\n";

            // Validación básica
            std::string trimmed;
            for(char c : content) if(!isspace(c)) trimmed += c;

            if (!trimmed.empty() && trimmed.front() == '{' && trimmed.back() == '}')
                std::cout << "JSON válido ✅\n";
            else
                std::cout << "JSON inválido ❌\n";
        }
    };

} // namespace json

// --- Alias opcional para un nombre por defecto ---
using ELS_json = json::ELS;

#endif // ELSJSON_HPP
