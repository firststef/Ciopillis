#pragma once
#include <ostream>

using namespace std;

namespace Types {

    class  SString {
    protected:
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

        SString(SString *other) {
            value = (char*)malloc(strlen(other->value) + 1);
            strcpy(value, other->value);
        }

        auto operator= (const SString & other) {
            delete value;
            value = (char*)malloc(strlen(other.value) + 1);
            strcpy(value, other.value);
        }

        friend ostream & operator << (ostream &out, const SString &c);

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
            free(value);
        }
    };

    ostream & operator << (ostream &out, const Types::SString &c)
    {
        out << c.value;
        return out;
    }
}

