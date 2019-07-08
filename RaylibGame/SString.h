#pragma once
#include <string.h>
#include <stdlib.h>

using namespace std;

namespace SString {
    class SString {
        char* value;
    public:
        SString() {}

        SString(const char* charString) {
            if (strlen(charString) > 500)
                value = nullptr;
            
            value = (char*)malloc(strlen(charString) + 1);
            strcpy(value, charString);
        }

        SString(SString &other) {
            value = (char*)malloc(strlen(other.value) + 1);
            strcpy(value, other.value);
        }

        auto operator= (const SString & other) {
            delete value;
            value = (char*)malloc(strlen(other.value) + 1);
            strcpy(value, other.value);
        }

        char* GetText() {
            return value;
        }

        SString& Substitute(const SString &other) {
            delete value;
            value = (char*)malloc(strlen(other.value) + 1);
            strcpy(value, other.value);

            return *this;
        }

        ~SString() {
            delete value;
        }
    };
}