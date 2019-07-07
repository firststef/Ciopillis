#pragma once
#include <string.h>
#include <stdlib.h>

using namespace std;

namespace SString {
    class RChar {
        char* value;
    public:
        RChar() {}

        RChar(const char* charString) {
            if (strlen(charString) > 500)
                value = nullptr;
            
            value = (char*)malloc(strlen(charString) + 1);
            strcpy(value, charString);
        }

        RChar(SString::RChar &other) {
            value = (char*)malloc(strlen(other.value) + 1);
            strcpy(value, other.value);
        }

        auto operator= (const RChar & other) {
            delete value;
            value = (char*)malloc(strlen(other.value) + 1);
            strcpy(value, other.value);
        }

        char* GetText() {
            return value;
        }

        void Substitute(const SString::RChar &other) {
            delete value;
            value = (char*)malloc(strlen(other.value) + 1);
            strcpy(value, other.value);
        }

        ~RChar() {
            delete value;
        }
    };
}